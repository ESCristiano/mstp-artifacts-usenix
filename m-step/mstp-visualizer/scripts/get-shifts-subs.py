#!/usr/bin/env python3
"""
Pattern Match Processor Script
Processes pattern match files and extracts shift/sub sequences
"""

import argparse
import os
import sys
from pathlib import Path
from typing import List, Tuple, Dict


class PatternProcessor:
    def __init__(self, input_dir: str, output_dir: str, verbosity: int):
        self.input_dir = Path(input_dir)
        self.output_dir = Path(output_dir)
        self.verbosity = verbosity
        self.valid_labels = {'shift_u', 'shift_v', 'sub_u', 'sub_v'}
        
    def log(self, message: str, level: int = 1):
        """Print message if verbosity level allows"""
        if self.verbosity >= level:
            print(message)
    
    def parse_pattern_file(self, filepath: Path, label: str) -> List[Tuple[int, str]]:
        """Parse a single pattern file and extract IDs with labels"""
        results = []
        
        try:
            with open(filepath, 'r') as f:
                lines = f.readlines()
            
            for i, line in enumerate(lines):
                line = line.strip()
                
                # Check if this is a match marker
                if line.startswith('# Match'):
                    # Get the next non-comment line
                    j = i + 1
                    while j < len(lines):
                        next_line = lines[j].strip()
                        if next_line and not next_line.startswith('#'):
                            # Extract the ID (first number)
                            parts = next_line.split()
                            if parts:
                                try:
                                    id_num = int(parts[0])
                                    results.append((id_num, label))
                                except ValueError:
                                    self.log(f"Warning: Could not parse ID from line: {next_line}", 0)
                            break
                        j += 1
            
            return results
            
        except FileNotFoundError:
            self.log(f"Error: File not found: {filepath}", 0)
            return []
        except Exception as e:
            self.log(f"Error processing file {filepath}: {str(e)}", 0)
            return []
    
    def process_step1(self, patterns_file: Path) -> Dict[str, List[Tuple[int, str]]]:
        """Step 1: Process all files listed in patterns file"""
        self.log("Step 1: Processing individual pattern files...", 1)
        
        tmp_files = {}
        
        try:
            with open(patterns_file, 'r') as f:
                for line in f:
                    line = line.strip()
                    if not line:
                        continue
                    
                    parts = line.split()
                    if len(parts) != 2:
                        self.log(f"Warning: Invalid line in patterns file: {line}", 0)
                        continue
                    
                    filename, label = parts
                    
                    if label not in self.valid_labels:
                        self.log(f"Error: Invalid label '{label}' for file {filename}. Valid labels are: {self.valid_labels}", 0)
                        continue
                    
                    filepath = self.input_dir / filename
                    self.log(f"  Processing {filename} with label '{label}'...", 1)
                    
                    # Parse the file
                    results = self.parse_pattern_file(filepath, label)
                    
                    if results:
                        # Write temporary file
                        tmp_filename = f"{Path(filename).stem}-tmp.txt"
                        tmp_filepath = self.output_dir / tmp_filename
                        
                        with open(tmp_filepath, 'w') as tmp_f:
                            for id_num, lbl in results:
                                tmp_f.write(f"{id_num} {lbl}\n")
                        
                        tmp_files[tmp_filename] = results
                        self.log(f"    Created {tmp_filename} with {len(results)} entries", 1)
                    
        except FileNotFoundError:
            self.log(f"Error: Patterns file not found: {patterns_file}", 0)
            sys.exit(1)
        except Exception as e:
            self.log(f"Error reading patterns file: {str(e)}", 0)
            sys.exit(1)
        
        return tmp_files
    
    def process_step2(self, tmp_files: Dict[str, List[Tuple[int, str]]]) -> List[Tuple[int, str]]:
        """Step 2: Combine all tmp files and sort by ID"""
        self.log("Step 2: Combining temporary files...", 1)
        
        # Combine all entries
        all_entries = []
        for entries in tmp_files.values():
            all_entries.extend(entries)
        
        # Sort by ID
        all_entries.sort(key=lambda x: x[0])
        
        # Write combined file
        combined_filepath = self.output_dir / "combined-trace-tmp.txt"
        with open(combined_filepath, 'w') as f:
            for id_num, label in all_entries:
                f.write(f"{id_num} {label}\n")
        
        self.log(f"  Created combined-trace-tmp.txt with {len(all_entries)} entries", 1)
        
        return all_entries
    
    def process_step3(self, entries: List[Tuple[int, str]], patterns_filename: str) -> Tuple[List[int], List[int], List[int]]:
        """Step 3: Count shift and sub sequences"""
        self.log("Step 3: Analyzing shift and sub sequences...", 1)
        
        shifts_u = []
        shifts_v = []
        subs = []
        
        current_shift_u_count = 0
        current_shift_v_count = 0
        
        for i, (id_num, label) in enumerate(entries):
            if label == 'shift_u':
                if current_shift_v_count > 0:
                    self.log(f"Error: shift_u following shift_v at ID {id_num} without sub in between!", 0)
                current_shift_u_count += 1
                
            elif label == 'shift_v':
                if current_shift_u_count > 0:
                    self.log(f"Error: shift_v following shift_u at ID {id_num} without sub in between!", 0)
                current_shift_v_count += 1
                
            elif label == 'sub_u':
                # Record current shift counts
                shifts_u.append(current_shift_u_count)
                shifts_v.append(current_shift_v_count)
                subs.append(1)
                
                # Reset counters
                current_shift_u_count = 0
                current_shift_v_count = 0
                
            elif label == 'sub_v':
                # Record current shift counts
                shifts_u.append(current_shift_u_count)
                shifts_v.append(current_shift_v_count)
                subs.append(0)
                
                # Reset counters
                current_shift_u_count = 0
                current_shift_v_count = 0
        
        # Check if there are remaining shifts without a sub
        if current_shift_u_count > 0 or current_shift_v_count > 0:
            self.log("Warning: File ends with shift operations without a following sub operation", 0)
        
        # Verify all vectors have the same length
        if not (len(shifts_u) == len(shifts_v) == len(subs)):
            self.log(f"Error: Vector length mismatch! shifts_u: {len(shifts_u)}, shifts_v: {len(shifts_v)}, subs: {len(subs)}", 0)
        
        # Write results to file
        results_filename = f"{Path(patterns_filename).stem}-results.txt"
        results_filepath = self.output_dir / results_filename
        
        with open(results_filepath, 'w') as f:
            f.write(f"SHIFTS_u = {shifts_u}\n")
            f.write(f"SHIFTS_v = {shifts_v}\n")
            f.write(f"SUBS = {subs}\n")
        
        self.log(f"  Results written to {results_filename}", 1)
        
        return shifts_u, shifts_v, subs
    
    def run(self, patterns_file: Path):
        """Run the complete processing pipeline"""
        # Create output directory if it doesn't exist
        self.output_dir.mkdir(parents=True, exist_ok=True)
        
        # Step 1: Process individual files
        tmp_files = self.process_step1(patterns_file)
        
        if not tmp_files:
            self.log("Error: No files were successfully processed", 0)
            return
        
        # Step 2: Combine and sort
        all_entries = self.process_step2(tmp_files)
        
        # Step 3: Analyze sequences
        shifts_u, shifts_v, subs = self.process_step3(all_entries, patterns_file.name)
        
        # Print final results
        print(f"\nFinal Results:")
        print(f"SHIFTS_u = {shifts_u}")
        print(f"SHIFTS_v = {shifts_v}")
        print(f"SUBS = {subs}")


def main():
    parser = argparse.ArgumentParser(
        description='Process pattern match files and extract shift/sub sequences'
    )
    
    parser.add_argument(
        '-p', '--patterns',
        required=True,
        help='Path to the patterns file listing files and labels'
    )
    
    parser.add_argument(
        '-i', '--input-dir',
        default='.',
        help='Input directory containing the pattern files (default: current directory)'
    )
    
    parser.add_argument(
        '-o', '--output-dir',
        default='.',
        help='Output directory for generated files (default: current directory)'
    )
    
    parser.add_argument(
        '-v', '--verbosity',
        type=int,
        choices=[0, 1],
        default=0,
        help='Verbosity level: 0 = only results and errors, 1 = show steps (default: 0)'
    )
    
    args = parser.parse_args()
    
    # Create processor and run
    processor = PatternProcessor(args.input_dir, args.output_dir, args.verbosity)
    patterns_path = Path(args.patterns)
    
    if not patterns_path.exists():
        print(f"Error: Patterns file not found: {args.patterns}")
        sys.exit(1)
    
    processor.run(patterns_path)


if __name__ == '__main__':
    main()