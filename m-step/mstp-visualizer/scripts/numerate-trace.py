#!/usr/bin/env python3
"""
Numerate trace script - Add line numbers to trace files.

Usage:
    python numerate.py -t trace.txt -o trace_num.txt
    python numerate.py --trace trace.txt --output trace_num.txt
"""

import argparse
import sys
import os

def numerate_trace(input_file, output_file):
    """Add line numbers to each line in the trace file"""
    try:
        with open(input_file, 'r') as infile, open(output_file, 'w') as outfile:
            line_number = 1
            
            for line in infile:
                line = line.strip()
                
                # Skip empty lines and comments, but still process them
                if not line:
                    outfile.write('\n')
                    continue
                    
                if line.startswith('#'):
                    # Keep comments as-is without numbering
                    outfile.write(line + '\n')
                    continue
                
                # Add line number to the beginning of the line
                numbered_line = f"{line_number} {line}\n"
                outfile.write(numbered_line)
                line_number += 1
                
    except FileNotFoundError:
        print(f"Error: Input file '{input_file}' not found")
        sys.exit(1)
    except PermissionError:
        print(f"Error: Permission denied when writing to '{output_file}'")
        sys.exit(1)
    except Exception as e:
        print(f"Error processing files: {e}")
        sys.exit(1)

def main():
    parser = argparse.ArgumentParser(
        description="Add line numbers to trace files",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  %(prog)s -t trace.txt -o trace_num.txt
  %(prog)s --trace trace.txt --output trace_num.txt

Input format:
  0x0c02c1be 02
  0x0c02c1c0 02
  0x0c02c1c2 01

Output format:
  1 0x0c02c1be 02
  2 0x0c02c1c0 02
  3 0x0c02c1c2 01
        """
    )
    
    parser.add_argument('-t', '--trace', required=True,
                       help='Input trace file')
    parser.add_argument('-o', '--output', required=True,
                       help='Output numbered trace file')
    
    args = parser.parse_args()
    
    # Check if input file exists
    if not os.path.exists(args.trace):
        print(f"Error: Input file '{args.trace}' does not exist")
        sys.exit(1)
    
    # Check if output directory exists, create if needed
    output_dir = os.path.dirname(args.output)
    if output_dir and not os.path.exists(output_dir):
        try:
            os.makedirs(output_dir)
        except Exception as e:
            print(f"Error: Could not create output directory '{output_dir}': {e}")
            sys.exit(1)
    
    print(f"Processing trace file: {args.trace}")
    print(f"Output file: {args.output}")
    
    # Process the file
    numerate_trace(args.trace, args.output)
    
    # Count lines for summary
    try:
        with open(args.trace, 'r') as f:
            total_lines = sum(1 for line in f if line.strip() and not line.strip().startswith('#'))
        
        with open(args.output, 'r') as f:
            output_lines = sum(1 for line in f)
        
        print(f"Successfully processed {total_lines} trace entries")
        print(f"Output file contains {output_lines} total lines")
        print("Done!")
        
    except Exception as e:
        print(f"Warning: Could not count lines: {e}")
        print("File processing completed, but line count unavailable")

if __name__ == "__main__":
    main()