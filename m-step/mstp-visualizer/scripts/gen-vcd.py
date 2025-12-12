import datetime
import sys
import re
import subprocess
import os

# Basic VCD header template
header = """$date
    {date}
$end
$version
    {generator}
$end
$timescale
    1ns
$end
$scope module top $end
{vars}$upscope $end
$enddefinitions $end
$dumpvars
"""

class BaseSignal:
    def __init__(self, name, size, identifier):
        self.name = name
        self.size = size
        self.identifier = identifier

    def declaration(self):
        if self.size == 1:
            return f"$var wire {self.size} {self.identifier} {self.name} $end\n"
        else:
            return f"$var wire {self.size} {self.identifier} {self.name} $end\n"


class ClockSignal(BaseSignal):
    def __init__(self, max_clock):
        super().__init__("clk", 1, "#")
        self.max_clock = max_clock


class Signal(BaseSignal):
    def __init__(self, name, size, identifier, values):
        super().__init__(name, size, identifier)
        self.values = values

    def value_print(self, i):
        if self.size == 1:
            return f"{self.values[i]}{self.identifier}\n"
        else:
            # Ensure proper binary formatting with correct bit width
            binary = format(self.values[i], f'0{self.size}b')
            return f"b{binary} {self.identifier}\n"

def retire_signal_from_latencies(latencies: list[int]) -> Signal:
    name = "exec_done"
    size = 1
    identifier = "E"  # Single character identifier
    values = {0: 0}
    i = latencies[0] - 1
    for lat in latencies[1:]:
        values[i * 2] = 1
        if lat > 1:
            values[(i + 1) * 2] = 0
        i += lat
    values[i * 2] = 1
    values[(i + 1) * 2] = 0

    return Signal(name, size, identifier, values)

def int_signal_from_latencies(latencies: list[int]) -> Signal:
    name = "inst_len"
    size = 32
    identifier = "L"  # Single character identifier
    values = {0: latencies[0]}
    i = latencies[0]
    for lat in latencies[1:]:
        values[i * 2] = lat
        i += lat
    values[i * 2] = 0

    return Signal(name, size, identifier, values)

def addr_signal_from_trace(trace_data: list[tuple[int, int]]) -> Signal:
    name = "instr_addr"
    size = 32
    identifier = "A"  # Single character identifier
    values = {}
    
    total_cycles = 0
    for i, (addr, latency) in enumerate(trace_data):
        values[total_cycles * 2] = addr
        total_cycles += latency
    
    return Signal(name, size, identifier, values)

def parse_trace_file(filename):
    trace_data = []
    pattern = re.compile(r'0x([0-9a-f]+)\s+(\d+)')
    
    with open(filename, 'r') as f:
        for line in f:
            match = pattern.search(line)
            if match:
                addr = int(match.group(1), 16)
                latency = int(match.group(2))
                trace_data.append((addr, latency))
    
    return trace_data

def create_vcd(clock: ClockSignal, signals: list[Signal], output_dir: str, vcd_filename: str):
    # Ensure output directory exists
    os.makedirs(output_dir, exist_ok=True)
    
    vcd_path = os.path.join(output_dir, vcd_filename)
    
    with open(vcd_path, 'w') as f:
        f.write(header.format(
            date=datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S"),
            generator="M-Step Trace Visualizer",
            vars="".join([var.declaration() for var in signals + [clock]]),
        ))

        # Write initial values for signals only (not clock)
        for var in signals:
            if 0 in var.values:
                f.write(var.value_print(0))
            elif var.size == 1:
                f.write(f"0{var.identifier}\n")
            else:
                f.write(f"b{'0' * var.size} {var.identifier}\n")
        
        # Write initial clock value
        f.write(f"0{clock.identifier}\n")
        f.write("$end\n")

        # Write time changes
        for i in range(1, clock.max_clock * 2):
            f.write(f"#{i * 5}\n")
            if i % 2 == 0:
                f.write(f"0{clock.identifier}\n")
            else:
                f.write(f"1{clock.identifier}\n")

            for var in signals:
                if i in var.values:
                    f.write(var.value_print(i))
    
    return vcd_path

def disassemble_elf(elf_file):
    """Disassemble the ELF file and extract address->instruction mappings using arm-none-eabi-objdump"""
    addr_to_instr = {}
    addr_to_function = {}
    
    if not os.path.exists(elf_file):
        print(f"Error: ELF file {elf_file} not found")
        return addr_to_instr, addr_to_function
    
    try:
        print(f"Running arm-none-eabi-objdump on {elf_file}")
        result = subprocess.run(['arm-none-eabi-objdump', '-d', elf_file], 
                               capture_output=True, text=True, check=True)
        
        # Parse the output to extract address->instruction mappings
        current_function = ""
        for line in result.stdout.splitlines():
            line = line.strip()
            
            # Track function names - format: "0c02efc2 <mbedtls_gcm_auth_decrypt>:"
            if '<' in line and '>:' in line:
                fn_match = re.search(r'<([^>]+)>:', line)
                if fn_match:
                    current_function = fn_match.group(1).split('(')[0].split('@')[0]
                continue
            
            # Extract instruction info - format: " c02efc2:       b5f0            push    {r4, r5, r6, r7, lr}"
            if ':' in line and len(line) > 0:
                parts = line.split(':')
                if len(parts) >= 2:
                    try:
                        # Extract address (remove leading/trailing whitespace)
                        addr_str = parts[0].strip()
                        addr = int(addr_str, 16)
                        
                        # Store function name for this address
                        addr_to_function[addr] = current_function
                        
                        # Extract instruction part after the colon
                        instr_part = parts[1].strip()
                        
                        # The format is: "b5f0            push    {r4, r5, r6, r7, lr}"
                        # We need to skip the hex bytes and get only the instruction
                        
                        # Find the first tab or multiple spaces (separates hex from instruction)
                        if '\t' in instr_part:
                            # Split by tab - instruction starts after the tab
                            instruction_parts = instr_part.split('\t', 1)
                            if len(instruction_parts) > 1:
                                instruction = instruction_parts[1].strip()
                            else:
                                continue
                        else:
                            # Split by multiple spaces - look for the instruction part
                            # Pattern: hex_bytes followed by spaces, then instruction
                            # Use regex to find where hex ends and instruction begins
                            match = re.match(r'^([0-9a-f]+)\s+(.+)$', instr_part)
                            if match:
                                instruction = match.group(2).strip()
                            else:
                                # Fallback: split by whitespace and take everything after first element
                                parts = instr_part.split()
                                if len(parts) > 1:
                                    instruction = ' '.join(parts[1:])
                                else:
                                    continue
                        
                        addr_to_instr[addr] = instruction
                            
                    except (ValueError, IndexError):
                        continue
                        
    except subprocess.CalledProcessError as e:
        print(f"Error: Failed to run arm-none-eabi-objdump on {elf_file}")
        print(f"Make sure arm-none-eabi-objdump is installed and in your PATH")
        print(f"Error details: {e}")
        return addr_to_instr, addr_to_function
    except FileNotFoundError:
        print("Error: arm-none-eabi-objdump not found")
        print("Please install the ARM GNU Toolchain and ensure arm-none-eabi-objdump is in your PATH")
        print("You can install it with:")
        print("  Ubuntu/Debian: sudo apt-get install gcc-arm-none-eabi")
        print("  macOS: brew install arm-none-eabi-gcc")
        return addr_to_instr, addr_to_function
    
    print(f"Successfully extracted {len(addr_to_instr)} instructions and {len(set(addr_to_function.values()))} functions")
    return addr_to_instr, addr_to_function

def parse_instruction(instruction):
    """Parse instruction into mnemonic and operands"""
    if not instruction:
        return "", ""
    
    # Split instruction into parts
    parts = instruction.split(None, 1)  # Split on first whitespace
    if len(parts) == 1:
        return parts[0], ""
    else:
        return parts[0], parts[1]

def create_translation_tables(trace_data, addr_to_instr, addr_to_function, output_dir: str, table_prefix: str = "inst_addr"):
    """Create multiple GTKWave translation tables with different levels of detail"""
    
    # Get unique addresses from trace data
    unique_addrs = set(addr for addr, _ in trace_data)
    
    # Ensure output directory exists
    os.makedirs(output_dir, exist_ok=True)
    
    # File 1: Full instruction with registers (mnemonic + operands)
    full_path = os.path.join(output_dir, f"{table_prefix}_full.tbl")
    with open(full_path, 'w') as f:
        for addr in sorted(unique_addrs):
            if addr in addr_to_instr:
                # Clean up the instruction to match your desired format
                instruction = addr_to_instr[addr]
                # Remove extra whitespace and normalize spacing
                cleaned_instr = ' '.join(instruction.split())
                f.write(f"{addr:08x} {cleaned_instr}\n")
            else:
                f.write(f"{addr:08x} UNKNOWN\n")
    
    # File 2: Raw instruction without comments/annotations
    raw_path = os.path.join(output_dir, f"{table_prefix}_raw.tbl")
    with open(raw_path, 'w') as f:
        for addr in sorted(unique_addrs):
            if addr in addr_to_instr:
                instruction = addr_to_instr[addr]
                # Remove comments (everything after @) and function references (everything in < >)
                # First remove @ comments
                if '@' in instruction:
                    instruction = instruction.split('@')[0].strip()
                # Then remove function references in < >
                instruction = re.sub(r'<[^>]*>', '', instruction).strip()
                # Clean up extra whitespace
                cleaned_instr = ' '.join(instruction.split())
                f.write(f"{addr:08x} {cleaned_instr}\n")
            else:
                f.write(f"{addr:08x} UNKNOWN\n")
    
    # File 3: Instruction mnemonic only
    mnemonic_path = os.path.join(output_dir, f"{table_prefix}_mnemonic.tbl")
    with open(mnemonic_path, 'w') as f:
        for addr in sorted(unique_addrs):
            if addr in addr_to_instr:
                mnemonic, _ = parse_instruction(addr_to_instr[addr])
                f.write(f"{addr:08x} {mnemonic}\n")
            else:
                f.write(f"{addr:08x} UNKNOWN\n")
    
    # File 4: Raw operands only (without comments/annotations)
    operands_path = os.path.join(output_dir, f"{table_prefix}_operands.tbl")
    with open(operands_path, 'w') as f:
        for addr in sorted(unique_addrs):
            if addr in addr_to_instr:
                _, operands = parse_instruction(addr_to_instr[addr])
                if operands:
                    # Remove comments (everything after @) and function references (everything in < >)
                    # First remove @ comments
                    if '@' in operands:
                        operands = operands.split('@')[0].strip()
                    # Then remove function references in < >
                    operands = re.sub(r'<[^>]*>', '', operands).strip()
                    # Clean up extra whitespace
                    cleaned_operands = ' '.join(operands.split())
                    f.write(f"{addr:08x} {cleaned_operands}\n")
                else:
                    f.write(f"{addr:08x}\n")  # Empty line for no operands
            else:
                f.write(f"{addr:08x} UNKNOWN\n")
    
    # File 5: Function name only
    function_path = os.path.join(output_dir, f"{table_prefix}_function.tbl")
    with open(function_path, 'w') as f:
        for addr in sorted(unique_addrs):
            if addr in addr_to_function:
                f.write(f"{addr:08x} {addr_to_function[addr]}\n")
            else:
                f.write(f"{addr:08x} UNKNOWN_FUNC\n")
    
    print(f"Created translation tables in {output_dir}:")
    print(f"  1. {table_prefix}_full.tbl - Full instructions with operands and comments")
    print(f"  2. {table_prefix}_raw.tbl - Raw instructions without comments/annotations")
    print(f"  3. {table_prefix}_mnemonic.tbl - Instruction mnemonics only")
    print(f"  4. {table_prefix}_operands.tbl - Raw operands only (without comments)")
    print(f"  5. {table_prefix}_function.tbl - Function names only")
    print(f"Total entries: {len(unique_addrs)}")
    print(f"Instructions found: {len([a for a in unique_addrs if a in addr_to_instr])}")
    print(f"Functions found: {len([a for a in unique_addrs if a in addr_to_function])}")
    
    return {
        'full': full_path,
        'raw': raw_path,
        'mnemonic': mnemonic_path,
        'operands': operands_path,
        'function': function_path
    }

def main():
    if len(sys.argv) < 2:
        print(f"Usage: {sys.argv[0]} <trace_file> [elf_file] [--output-dir DIR] [--tables-dir DIR] [--vcd-name NAME] [--table-prefix PREFIX]")
        print("  trace_file: File containing address/latency pairs")
        print("  elf_file:   ELF binary for instruction disassembly (optional)")
        print("  --output-dir DIR: Output directory for VCD file (default: ./outputs/)")
        print("  --tables-dir DIR: Output directory for translation tables (default: ./gtkw_confs/)")
        print("  --vcd-name NAME: VCD filename (default: trace.vcd)")
        print("  --table-prefix PREFIX: Prefix for translation table files (default: inst_addr)")
        print("\nExample:")
        print(f"  {sys.argv[0]} trace.txt firmware.elf --output-dir ./my_results --tables-dir ./my_tables --vcd-name my_trace.vcd --table-prefix my_inst")
        sys.exit(1)
    
    # Parse command line arguments
    trace_file = sys.argv[1]
    elf_file = None
    output_dir = "./outputs/"
    tables_dir = "./gtkw_confs/"
    vcd_filename = "trace.vcd"
    table_prefix = "inst_addr"
    
    args = sys.argv[2:]
    i = 0
    while i < len(args):
        if args[i] == "--output-dir" and i + 1 < len(args):
            output_dir = args[i + 1]
            i += 2
        elif args[i] == "--tables-dir" and i + 1 < len(args):
            tables_dir = args[i + 1]
            i += 2
        elif args[i] == "--vcd-name" and i + 1 < len(args):
            vcd_filename = args[i + 1]
            i += 2
        elif args[i] == "--table-prefix" and i + 1 < len(args):
            table_prefix = args[i + 1]
            i += 2
        elif args[i].startswith("--"):
            print(f"Unknown option: {args[i]}")
            sys.exit(1)
        else:
            # Assume it's the ELF file if no option is specified
            if elf_file is None:
                elf_file = args[i]
            i += 1
    
    print(f"Configuration:")
    print(f"  Trace file: {trace_file}")
    print(f"  ELF file: {elf_file if elf_file else 'None'}")
    print(f"  Output directory: {output_dir}")
    print(f"  Tables directory: {tables_dir}")
    print(f"  VCD filename: {vcd_filename}")
    print(f"  Table prefix: {table_prefix}")
    print()
    
    trace_data = parse_trace_file(trace_file)
    
    if not trace_data:
        print(f"No valid trace data found in {trace_file}")
        sys.exit(1)
    
    # Extract latencies from trace data
    latencies = [lat for _, lat in trace_data]
    
    # Get instruction information from ELF file if provided
    addr_to_instr = {}
    addr_to_function = {}
    translation_files = {}
    
    if elf_file:
        addr_to_instr, addr_to_function = disassemble_elf(elf_file)
        print(f"Extracted {len(addr_to_instr)} instructions from {elf_file}")
        
        # Create multiple GTKWave translation tables
        translation_files = create_translation_tables(trace_data, addr_to_instr, addr_to_function, tables_dir, table_prefix)
        
    else:
        print("No ELF file provided, skipping instruction disassembly")
    
    # Calculate total cycles needed
    total_cycles = sum(latencies) + 10
    
    clock = ClockSignal(total_cycles)
    
    # Create signals
    retires = retire_signal_from_latencies(latencies)
    instr_lens = int_signal_from_latencies(latencies)
    addrs = addr_signal_from_trace(trace_data)
    
    # Create cycle counter
    cycle_counter = Signal("cycle", 32, "C", {})
    total = 0
    for i in range(total_cycles):
        cycle_counter.values[i*2] = total
        total += 1
    
    signals = [
        cycle_counter,
        addrs,
        instr_lens,
        retires,
    ]
    
    vcd_path = create_vcd(clock, signals, output_dir, vcd_filename)
    
    print(f"\nCreated VCD file: {vcd_path}")
    print(f"Total instructions: {len(latencies)}")
    print(f"Total cycles: {sum(latencies)}")
    
    if translation_files:
        print(f"\nTo use translation tables in GTKWave:")
        print("1. Right-click on 'instr_addr' signal")
        print("2. Select 'Data Format' -> 'Translate Filter File' -> 'Enable and Select'")
        print("3. Choose one of the translation files:")
        for table_type, file_path in translation_files.items():
            print(f"   - {os.path.basename(file_path)} for {table_type} format")

if __name__ == "__main__":
    main()