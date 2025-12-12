#!/usr/bin/env python3
"""
Prune trace script - Extract patterns from trace files based on start/end addresses.

Usage:
    python prune-trace.py trace.txt -s 0x12345678 -e 0x87654321 [-c 5]
    python prune-trace.py trace.txt --start 0x12345678 --end 0x87654321 [--count 5]
"""

import argparse
import sys
import re
import os

def parse_trace_file(filename):
    """Parse trace file and return list of (original_line, address, time) tuples"""
    trace_data = []
    
    try:
        with open(filename, 'r') as f:
            for line_num, line in enumerate(f, 1):
                line = line.strip()
                if not line or line.startswith('#'):
                    continue
                
                # Match both formats:
                # Format 1: 0xXXXXXXXX XX
                # Format 2: N 0xXXXXXXXX XX (where N is line number)
                
                # First try to extract address and timing from the line
                # Look for pattern: (optional prefix) 0xXXXXXXXX XX
                match = re.match(r'^(.*?)\s*(0x[0-9a-fA-F]+)\s+(\d+)$', line)
                if match:
                    prefix_part = match.group(1).strip()  # Everything before address (could be empty or line number)
                    addr_str = match.group(2)             # The 0xXXXXXXXX part
                    time_str = match.group(3)             # The timing value
                    
                    try:
                        addr = int(addr_str, 16)
                        time = int(time_str)
                        
                        # Store the original line format for output preservation
                        original_line = f"{prefix_part} {addr_str}" if prefix_part else addr_str
                            
                        trace_data.append((original_line, addr, time))
                    except ValueError:
                        print(f"Warning: Invalid address or timing at line {line_num}: {line}")
                else:
                    print(f"Warning: Invalid format at line {line_num}: {line}")
    
    except FileNotFoundError:
        print(f"Error: File '{filename}' not found")
        sys.exit(1)
    except Exception as e:
        print(f"Error reading file '{filename}': {e}")
        sys.exit(1)
    
    return trace_data

def find_patterns(trace_data, start_addr, end_addr, max_count=None):
    """Find patterns in trace data that start with start_addr and end with end_addr"""
    patterns = []
    current_pattern = []
    pattern_active = False
    
    for original_line, addr, time in trace_data:
        if addr == start_addr and not pattern_active:
            # Start of a new pattern
            pattern_active = True
            current_pattern = [(original_line, addr, time)]
        elif pattern_active:
            # We're inside a pattern
            current_pattern.append((original_line, addr, time))
            
            if addr == end_addr:
                # End of pattern found
                patterns.append(current_pattern.copy())
                pattern_active = False
                current_pattern = []
                
                # Check if we've found enough patterns
                if max_count and len(patterns) >= max_count:
                    break
    
    return patterns

def write_pruned_trace(patterns, start_addr, end_addr, count_param, output_filename):
    """Write found patterns to output file"""
    try:
        with open(output_filename, 'w') as f:
            f.write(f"# Pruned trace: start=0x{start_addr:08x}, end=0x{end_addr:08x}, count={count_param}\n")
            f.write(f"# Found {len(patterns)} pattern(s)\n")
            f.write("#\n")
            
            for pattern_num, pattern in enumerate(patterns, 1):
                f.write(f"# Pattern {pattern_num} (length: {len(pattern)} instructions)\n")
                for original_line, addr, time in pattern:
                    f.write(f"{original_line} {time:02d}\n")
                f.write("#\n")
    
    except Exception as e:
        print(f"Error writing to file '{output_filename}': {e}")
        sys.exit(1)

def main():
    parser = argparse.ArgumentParser(
        description="Extract patterns from trace files based on start/end addresses",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  %(prog)s trace.txt -s 0x0c01e194 -e 0x0c01e1a4
  %(prog)s trace.txt --start 0x12345678 --end 0x87654321 --count 3
  %(prog)s trace.txt -s 0xc01e194 -e 0xc01e1a4 -c 1

Supported trace formats:
  Format 1: 0x0c01e194 03
           0x0c01e196 02
           
  Format 2: 1 0x0c01e194 03
           2 0x0c01e196 02
        """
    )
    
    parser.add_argument('trace_file', help='Input trace file')
    parser.add_argument('-s', '--start', required=True, 
                       help='Start address (0xXXXXXXXX format)')
    parser.add_argument('-e', '--end', required=True,
                       help='End address (0xXXXXXXXX format)')
    parser.add_argument('-c', '--count', type=int,
                       help='Maximum number of patterns to extract (optional)')
    
    args = parser.parse_args()
    
    # Validate and parse addresses
    try:
        start_addr = int(args.start, 16)
        end_addr = int(args.end, 16)
    except ValueError:
        print("Error: Start and end addresses must be in hexadecimal format (0xXXXXXXXX)")
        sys.exit(1)
    
    # Validate 32-bit addresses
    if not (0 <= start_addr <= 0xFFFFFFFF) or not (0 <= end_addr <= 0xFFFFFFFF):
        print("Error: Addresses must be valid 32-bit values (0x00000000 to 0xFFFFFFFF)")
        sys.exit(1)
    
    print(f"Processing trace file: {args.trace_file}")
    print(f"Start address: 0x{start_addr:08x}")
    print(f"End address: 0x{end_addr:08x}")
    print(f"Max patterns: {args.count if args.count else 'all'}")
    print()
    
    # Parse trace file
    trace_data = parse_trace_file(args.trace_file)
    print(f"Loaded {len(trace_data)} trace entries")
    
    # Show detected format
    if trace_data:
        sample_line = trace_data[0][0]
        if re.match(r'^\d+\s+0x[0-9a-fA-F]+$', sample_line):
            print("Detected format: Numbered trace (N 0xXXXXXXXX XX)")
        elif re.match(r'^0x[0-9a-fA-F]+$', sample_line):
            print("Detected format: Standard trace (0xXXXXXXXX XX)")
        else:
            print("Detected format: Custom trace format")
    print()
    
    if not trace_data:
        print("Error: No valid trace data found")
        sys.exit(1)
    
    # Find patterns
    patterns = find_patterns(trace_data, start_addr, end_addr, args.count)
    
    print(f"Found {len(patterns)} pattern(s)")
    
    if not patterns:
        print("No patterns found matching the criteria")
        return
    
    # Generate output filename
    count_str = str(args.count) if args.count else "all"
    output_filename = f"pruned-trace-0x{start_addr:08x}-0x{end_addr:08x}-{count_str}.txt"
    
    # Write output
    write_pruned_trace(patterns, start_addr, end_addr, count_str, output_filename)
    
    print(f"Pruned trace written to: {output_filename}")
    
    # Print summary
    total_instructions = sum(len(pattern) for pattern in patterns)
    print(f"\nSummary:")
    print(f"  Patterns found: {len(patterns)}")
    print(f"  Total instructions: {total_instructions}")
    if patterns:
        pattern_lengths = [len(pattern) for pattern in patterns]
        print(f"  Pattern lengths: min={min(pattern_lengths)}, max={max(pattern_lengths)}, avg={sum(pattern_lengths)/len(pattern_lengths):.1f}")

if __name__ == "__main__":
    main()