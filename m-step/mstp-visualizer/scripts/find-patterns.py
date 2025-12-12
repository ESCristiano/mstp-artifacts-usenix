#!/usr/bin/env python3
"""
Find patterns script - Search for timing patterns in trace files.

Usage:
    python find-patterns.py -t trace.txt -p pattern.txt
    python find-patterns.py --trace trace.txt --pattern pattern.txt
    python find-patterns.py -t trace.txt -p pattern.txt -w 5
    python find-patterns.py -t trace.txt -ps pattern_start.txt -pe pattern_end.txt
    python find-patterns.py --trace trace.txt --pattern-start pattern_start.txt --pattern-end pattern_end.txt
    python find-patterns.py -t trace.txt -p pattern.txt -pl
"""

import argparse
import sys
import re
import os
from collections import defaultdict

def parse_trace_file(filename):
    """Parse trace file and return list of (prefix, time, line_id) tuples where prefix contains everything before timing"""
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
                match = re.match(r'^(.+)\s+(\d+)$', line)
                if match:
                    prefix = match.group(1).strip()  # Everything before the last number
                    time = int(match.group(2))       # The timing value
                    
                    # Extract line ID if present (for numbered traces)
                    line_id = None
                    numbered_match = re.match(r'^(\d+)\s+(.+)$', prefix)
                    if numbered_match:
                        line_id = int(numbered_match.group(1))
                        addr_part = numbered_match.group(2)
                    else:
                        # For non-numbered traces, use the file line number as ID
                        line_id = line_num
                        addr_part = prefix
                    
                    trace_data.append((prefix, time, line_id))
                else:
                    print(f"Warning: Invalid format in trace file at line {line_num}: {line}")
    
    except FileNotFoundError:
        print(f"Error: Trace file '{filename}' not found")
        sys.exit(1)
    except Exception as e:
        print(f"Error reading trace file '{filename}': {e}")
        sys.exit(1)
    
    return trace_data

def parse_pattern_file(filename):
    """Parse pattern file and return dictionary of {pattern_name: {'timings': [timings], 'label': label}}"""
    patterns = {}
    
    try:
        with open(filename, 'r') as f:
            for line_num, line in enumerate(f, 1):
                line = line.strip()
                if not line or line.startswith('#'):
                    continue
                
                # Split line into parts
                parts = line.split()
                if len(parts) < 2:
                    print(f"Warning: Invalid format in pattern file at line {line_num}: {line}")
                    continue
                
                pattern_name = parts[0]
                
                # Check if this is the new format with labels (at least 3 parts: name, label, timing(s))
                # or old format (name, timing(s))
                if len(parts) >= 3:
                    # Try to parse as new format: pattern_name label timing1 timing2 ...
                    try:
                        # Check if the third element onwards are all numbers (timings)
                        test_timings = [int(timing) for timing in parts[2:]]
                        # If we get here, it's likely the new format
                        label = parts[1]
                        timings = test_timings
                        patterns[pattern_name] = {'timings': timings, 'label': label}
                        continue
                    except ValueError:
                        # Fall back to old format parsing
                        pass
                
                # Parse as old format: pattern_name timing1 timing2 ...
                try:
                    timings = [int(timing) for timing in parts[1:]]
                    patterns[pattern_name] = {'timings': timings, 'label': None}
                except ValueError as e:
                    print(f"Warning: Invalid timing values in pattern file at line {line_num}: {line}")
                    continue
    
    except FileNotFoundError:
        print(f"Error: Pattern file '{filename}' not found")
        sys.exit(1)
    except Exception as e:
        print(f"Error reading pattern file '{filename}': {e}")
        sys.exit(1)
    
    return patterns

def parse_single_pattern_file(filename):
    """Parse a single pattern file and return (pattern_name, [timings]) tuple"""
    patterns = parse_pattern_file(filename)
    
    if len(patterns) == 0:
        print(f"Error: No patterns found in '{filename}'")
        sys.exit(1)
    elif len(patterns) > 1:
        print(f"Warning: Multiple patterns found in '{filename}', using first one")
    
    # Return first (and should be only) pattern
    pattern_name, pattern_data = next(iter(patterns.items()))
    return pattern_name, pattern_data['timings']

def find_pattern_matches(trace_data, pattern_timings):
    """Find all matches of a timing pattern in trace data"""
    matches = []
    trace_timings = [time for prefix, time, line_id in trace_data]
    pattern_length = len(pattern_timings)
    
    # Search for pattern in trace
    for i in range(len(trace_timings) - pattern_length + 1):
        # Check if pattern matches at this position
        if trace_timings[i:i + pattern_length] == pattern_timings:
            # Store the start index and length for later context extraction
            matches.append((i, pattern_length))
    
    return matches

def find_start_end_pattern_matches(trace_data, start_pattern_timings, end_pattern_timings):
    """Find all matches between start and end patterns in trace data"""
    matches = []
    trace_timings = [time for prefix, time, line_id in trace_data]
    start_pattern_length = len(start_pattern_timings)
    end_pattern_length = len(end_pattern_timings)
    
    i = 0
    while i < len(trace_timings) - start_pattern_length + 1:
        # Look for start pattern
        if trace_timings[i:i + start_pattern_length] == start_pattern_timings:
            start_match_idx = i
            start_match_end = i + start_pattern_length
            
            # Now look for end pattern starting from after the start pattern
            j = start_match_end
            while j < len(trace_timings) - end_pattern_length + 1:
                if trace_timings[j:j + end_pattern_length] == end_pattern_timings:
                    end_match_idx = j
                    end_match_end = j + end_pattern_length
                    
                    # Found a complete start-end match
                    matches.append({
                        'start_idx': start_match_idx,
                        'start_length': start_pattern_length,
                        'end_idx': end_match_idx,
                        'end_length': end_pattern_length,
                        'full_start': start_match_idx,
                        'full_end': end_match_end,
                        'full_length': end_match_end - start_match_idx
                    })
                    
                    # Continue searching from after this end pattern
                    i = end_match_end
                    break
                j += 1
            else:
                # No matching end pattern found, move to next potential start
                i += 1
        else:
            i += 1
    
    return matches

def extract_match_with_context(trace_data, match_start, pattern_length, context_window=0):
    """Extract match with optional context window"""
    # Calculate context boundaries
    context_start = max(0, match_start - context_window)
    context_end = min(len(trace_data), match_start + pattern_length + context_window)
    
    # Extract all entries in the context window
    context_entries = trace_data[context_start:context_end]
    
    # Identify which entries are context vs pattern
    before_context = trace_data[context_start:match_start] if context_window > 0 else []
    pattern_entries = trace_data[match_start:match_start + pattern_length]
    after_context = trace_data[match_start + pattern_length:context_end] if context_window > 0 else []
    
    return {
        'before_context': before_context,
        'pattern_entries': pattern_entries,
        'after_context': after_context,
        'full_context': context_entries
    }

def extract_start_end_match(trace_data, match_info):
    """Extract the full start-end match including everything between"""
    start_idx = match_info['full_start']
    end_idx = match_info['full_end']
    
    # Extract all entries from start to end (inclusive)
    full_match_entries = trace_data[start_idx:end_idx]
    
    # Separate into components
    start_entries = trace_data[start_idx:start_idx + match_info['start_length']]
    end_entries = trace_data[match_info['end_idx']:match_info['end_idx'] + match_info['end_length']]
    
    # Calculate middle entries (between start and end patterns)
    middle_start = start_idx + match_info['start_length']
    middle_end = match_info['end_idx']
    middle_entries = trace_data[middle_start:middle_end] if middle_end > middle_start else []
    
    return {
        'start_entries': start_entries,
        'middle_entries': middle_entries,
        'end_entries': end_entries,
        'full_entries': full_match_entries,
        'match_info': match_info
    }

def find_repeated_matches(all_pattern_matches):
    """Find line IDs that appear in multiple pattern matches"""
    line_id_to_patterns = defaultdict(list)
    
    # Build mapping of line_id -> [(pattern_name, match_num, entry_info)]
    for pattern_name, matches in all_pattern_matches.items():
        for match_num, match_data in enumerate(matches, 1):
            # Extract pattern entries from match data
            pattern_entries = match_data['pattern_entries']
            for prefix, time, line_id in pattern_entries:
                line_id_to_patterns[line_id].append({
                    'pattern_name': pattern_name,
                    'match_num': match_num,
                    'prefix': prefix,
                    'time': time,
                    'line_id': line_id
                })
    
    # Find repeated matches (line_ids that appear in multiple patterns)
    repeated_matches = {}
    for line_id, occurrences in line_id_to_patterns.items():
        if len(occurrences) > 1:
            # Group by pattern to avoid counting multiple occurrences within same pattern
            patterns_involved = set(occ['pattern_name'] for occ in occurrences)
            if len(patterns_involved) > 1:
                repeated_matches[line_id] = occurrences
    
    return repeated_matches

def write_pattern_matches(pattern_name, matches, context_window=0, output_dir=".", verbosity=1):
    """Write pattern matches to file with optional context"""
    if not matches:
        return
    
    # Ensure output directory exists
    os.makedirs(output_dir, exist_ok=True)
    
    filename = os.path.join(output_dir, f"pattern-match-{pattern_name}.txt")
    
    try:
        with open(filename, 'w') as f:
            f.write(f"# Pattern matches for: {pattern_name}\n")
            f.write(f"# Found {len(matches)} match(es)\n")
            if context_window > 0:
                f.write(f"# Context window: {context_window} lines before/after\n")
            f.write("#\n")
            
            for match_num, match_data in enumerate(matches, 1):
                f.write(f"# Match {match_num}\n")
                
                if context_window > 0:
                    # Write with context sections
                    if match_data['before_context']:
                        f.write("# -------- Context --------\n")
                        for prefix, time, line_id in match_data['before_context']:
                            f.write(f"{prefix} {time:02d}\n")
                    
                    f.write("# -------- Pattern --------\n")
                    for prefix, time, line_id in match_data['pattern_entries']:
                        f.write(f"{prefix} {time:02d}\n")
                    
                    if match_data['after_context']:
                        f.write("# -------- Context --------\n")
                        for prefix, time, line_id in match_data['after_context']:
                            f.write(f"{prefix} {time:02d}\n")
                else:
                    # Write pattern only (original behavior)
                    for prefix, time, line_id in match_data['pattern_entries']:
                        f.write(f"{prefix} {time:02d}\n")
                
                f.write("#\n")
        
        if verbosity >= 1:
            print(f"  Wrote matches to: {filename}")
    
    except Exception as e:
        print(f"Error writing pattern matches for '{pattern_name}': {e}")

def write_start_end_pattern_matches(start_pattern_name, matches, output_dir=".", verbosity=1):
    """Write start-end pattern matches to file"""
    if not matches:
        return
    
    # Ensure output directory exists
    os.makedirs(output_dir, exist_ok=True)
    
    filename = os.path.join(output_dir, f"pattern-match-{start_pattern_name}.txt")
    
    try:
        with open(filename, 'w') as f:
            f.write(f"# Start-End Pattern matches for: {start_pattern_name}\n")
            f.write(f"# Found {len(matches)} match(es)\n")
            f.write("#\n")
            
            for match_num, match_data in enumerate(matches, 1):
                f.write(f"# Match {match_num}\n")
                f.write("# -------- Start Pattern --------\n")
                for prefix, time, line_id in match_data['start_entries']:
                    f.write(f"{prefix} {time:02d}\n")
                
                if match_data['middle_entries']:
                    f.write("# -------- Middle Section --------\n")
                    for prefix, time, line_id in match_data['middle_entries']:
                        f.write(f"{prefix} {time:02d}\n")
                
                f.write("# -------- End Pattern --------\n")
                for prefix, time, line_id in match_data['end_entries']:
                    f.write(f"{prefix} {time:02d}\n")
                
                f.write("#\n")
        
        if verbosity >= 1:
            print(f"  Wrote start-end matches to: {filename}")
    
    except Exception as e:
        print(f"Error writing start-end pattern matches for '{start_pattern_name}': {e}")

def write_repeated_matches(repeated_matches, output_dir=".", verbosity=1):
    """Write repeated matches analysis to file"""
    if not repeated_matches:
        return
    
    # Ensure output directory exists
    os.makedirs(output_dir, exist_ok=True)
    
    filename = os.path.join(output_dir, "repeated-matches-analysis.txt")
    
    try:
        with open(filename, 'w') as f:
            f.write("# Repeated Matches Analysis\n")
            f.write(f"# Found {len(repeated_matches)} line(s) that appear in multiple patterns\n")
            f.write("#\n")
            f.write("# Format: Line_ID | Address | Timing | Patterns_Involved\n")
            f.write("#\n")
            
            for line_id in sorted(repeated_matches.keys()):
                occurrences = repeated_matches[line_id]
                
                # Get the trace entry info (should be same for all occurrences)
                first_occ = occurrences[0]
                prefix = first_occ['prefix']
                time = first_occ['time']
                
                # Get all patterns involved
                patterns_involved = sorted(list(set(occ['pattern_name'] for occ in occurrences)))
                
                f.write(f"Line {line_id}: {prefix} {time:02d}\n")
                f.write(f"  Appears in patterns: {', '.join(patterns_involved)}\n")
                
                # Show detailed occurrences
                for occ in occurrences:
                    f.write(f"    - {occ['pattern_name']} (match #{occ['match_num']})\n")
                
                f.write("\n")
        
        if verbosity >= 1:
            print(f"  Wrote repeated matches analysis to: {filename}")
    
    except Exception as e:
        print(f"Error writing repeated matches analysis: {e}")

def write_pattern_list(pattern_results, output_dir=".", verbosity=1):
    """Write pattern list file with pattern file names and their labels"""
    # Ensure output directory exists
    os.makedirs(output_dir, exist_ok=True)
    
    pattern_list_file = os.path.join(output_dir, "patterns-list.txt")
    
    try:
        # Check if file exists to determine if we should append
        file_exists = os.path.exists(pattern_list_file)
        mode = 'a' if file_exists else 'w'
        
        with open(pattern_list_file, mode) as f:
            if not file_exists:
                f.write("# Pattern List File\n")
                f.write("# Format: pattern_file_name label\n")
                f.write("#\n")
            
            for pattern_name, pattern_info in pattern_results.items():
                if pattern_info['matches'] > 0:  # Only write patterns that have matches
                    pattern_filename = f"pattern-match-{pattern_name}.txt"
                    label = pattern_info['label'] if pattern_info['label'] is not None else "unknown"
                    f.write(f"{pattern_filename} {label}\n")
        
        if verbosity >= 1:
            action = "Appended to" if file_exists else "Created"
            print(f"  {action} pattern list: {pattern_list_file}")
    
    except Exception as e:
        print(f"Error writing pattern list file: {e}")

def main():
    parser = argparse.ArgumentParser(
        description="Search for timing patterns in trace files",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  %(prog)s -t trace.txt -p patterns.txt
  %(prog)s --trace trace.txt --pattern patterns.txt
  %(prog)s -t trace.txt -p pattern.txt -w 5
  %(prog)s -t trace.txt -ps pattern_start.txt -pe pattern_end.txt
  %(prog)s --trace trace.txt --pattern-start pattern_start.txt --pattern-end pattern_end.txt
  %(prog)s -t trace.txt -p patterns.txt -v 0
  %(prog)s -t trace.txt -p patterns.txt -v 1
  %(prog)s -t trace.txt -p patterns.txt -pl

Pattern file formats:
  Old format:
    pattern_name_1 01 02 03 04 01
    pattern_name_2 02 02 01 04 01
  
  New format (with labels):
    pattern_name_1 shift_u 01 02 03 04 01
    pattern_name_2 sub_u 02 02 01 04 01
  
Start/End pattern files format (one pattern each):
  start_pattern 01 02 03
  end_pattern 04 05 06
  
Trace file formats supported:
  Format 1: 0x0c01e194 03
           0x0c01e196 02
           
  Format 2: 1 0x0c01e194 03
           2 0x0c01e196 02

The script supports two modes:
1. Regular pattern matching (-p/--pattern): Find specific timing patterns
2. Start-End pattern matching (-ps/-pe/--pattern-start/--pattern-end): 
   Find sections between start and end patterns

Pattern List Generation:
  Use -pl/--pattern-list to generate/append to patterns-list.txt file
  This file contains: pattern_file_name label
  Only patterns with matches are included

Verbosity levels:
  -v 0: Only show summary (quiet mode)
  -v 1: Show all processing details (default)

Context window (-w/--window) adds trace lines before and after each match
for better analysis context (only applies to regular pattern matching).
        """
    )
    
    parser.add_argument('-t', '--trace', required=True,
                       help='Input trace file')
    
    # Regular pattern matching
    parser.add_argument('-p', '--pattern',
                       help='Pattern definition file (for regular pattern matching)')
    
    # Start-End pattern matching
    parser.add_argument('-ps', '--pattern-start',
                       help='Start pattern file (for start-end pattern matching)')
    parser.add_argument('-pe', '--pattern-end',
                       help='End pattern file (for start-end pattern matching)')
    
    parser.add_argument('-o', '--output-dir', default='.',
                       help='Output directory for match files (default: current directory)')
    parser.add_argument('-w', '--window', type=int, default=0,
                       help='Context window size - number of trace lines to include before/after each match (default: 0, only applies to regular pattern matching)')
    parser.add_argument('-v', '--verbosity', type=int, choices=[0, 1], default=1,
                       help='Verbosity level: 0 = only summary, 1 = full output (default: 1)')
    parser.add_argument('-pl', '--pattern-list', action='store_true',
                       help='Generate/append to patterns-list.txt file with pattern files and labels')
    
    args = parser.parse_args()
    
    # Validate arguments
    regular_pattern_mode = args.pattern is not None
    start_end_pattern_mode = args.pattern_start is not None and args.pattern_end is not None
    
    if not regular_pattern_mode and not start_end_pattern_mode:
        print("Error: Must specify either regular pattern file (-p) or both start and end pattern files (-ps and -pe)")
        sys.exit(1)
    
    if regular_pattern_mode and start_end_pattern_mode:
        print("Error: Cannot specify both regular pattern file (-p) and start-end pattern files (-ps/-pe) at the same time")
        sys.exit(1)
    
    if start_end_pattern_mode and (args.pattern_start is None or args.pattern_end is None):
        print("Error: Both start pattern file (-ps) and end pattern file (-pe) must be specified for start-end pattern matching")
        sys.exit(1)
    
    # Verbosity level 1: Show processing details
    if args.verbosity >= 1:
        print(f"Processing trace file: {args.trace}")
        if regular_pattern_mode:
            print(f"Processing pattern file: {args.pattern}")
            print("Mode: Regular pattern matching")
        else:
            print(f"Processing start pattern file: {args.pattern_start}")
            print(f"Processing end pattern file: {args.pattern_end}")
            print("Mode: Start-End pattern matching")
        
        print(f"Output directory: {args.output_dir}")
        if regular_pattern_mode and args.window > 0:
            print(f"Context window: {args.window} lines before/after each match")
        if args.pattern_list:
            print("Pattern list generation: Enabled")
        print()
    
    # Parse input files
    trace_data = parse_trace_file(args.trace)
    
    if args.verbosity >= 1:
        print(f"Loaded {len(trace_data)} trace entries")
        
        # Show detected format
        if trace_data:
            sample_prefix = trace_data[0][0]
            if re.match(r'^\d+\s+0x[0-9a-fA-F]+$', sample_prefix):
                print("Detected format: Numbered trace (N 0xXXXXXXXX XX)")
            elif re.match(r'^0x[0-9a-fA-F]+$', sample_prefix):
                print("Detected format: Standard trace (0xXXXXXXXX XX)")
            else:
                print("Detected format: Custom trace format")
    
    if not trace_data:
        print("Error: No valid trace data found")
        sys.exit(1)
    
    if regular_pattern_mode:
        # Regular pattern matching mode
        patterns = parse_pattern_file(args.pattern)
        if args.verbosity >= 1:
            print(f"Loaded {len(patterns)} pattern(s)")
            
            # Show format detection
            has_labels = any(pattern_data['label'] is not None for pattern_data in patterns.values())
            if has_labels:
                print("Detected pattern format: New format (with labels)")
            else:
                print("Detected pattern format: Old format (no labels)")
            print()
        
        if not patterns:
            print("Error: No valid patterns found")
            sys.exit(1)
        
        # Search for each pattern and collect all matches
        total_matches = 0
        pattern_results = {}
        all_pattern_matches = {}
        
        for pattern_name, pattern_data in patterns.items():
            pattern_timings = pattern_data['timings']
            pattern_label = pattern_data['label']
            
            if args.verbosity >= 1:
                print(f"Searching for pattern '{pattern_name}'")
                if pattern_label:
                    print(f"  Label: {pattern_label}")
            
            # Find raw matches (indices and lengths)
            raw_matches = find_pattern_matches(trace_data, pattern_timings)
            
            # Extract matches with context
            matches_with_context = []
            for match_start, pattern_length in raw_matches:
                match_data = extract_match_with_context(trace_data, match_start, pattern_length, args.window)
                matches_with_context.append(match_data)
            
            pattern_results[pattern_name] = {
                'matches': len(matches_with_context),
                'label': pattern_label
            }
            all_pattern_matches[pattern_name] = matches_with_context
            total_matches += len(matches_with_context)
            
            if args.verbosity >= 1:
                print(f"  Found {len(matches_with_context)} match(es)")
            
            # Write matches to file
            if matches_with_context:
                write_pattern_matches(pattern_name, matches_with_context, args.window, args.output_dir, args.verbosity)
            
            if args.verbosity >= 1:
                print()
        
        # Generate pattern list file if requested
        if args.pattern_list:
            if args.verbosity >= 1:
                print("Generating pattern list file...")
            write_pattern_list(pattern_results, args.output_dir, args.verbosity)
            if args.verbosity >= 1:
                print()
        
        # Find and analyze repeated matches
        if args.verbosity >= 1:
            print("Analyzing repeated matches...")
        repeated_matches = find_repeated_matches(all_pattern_matches)
        
        if repeated_matches:
            if args.verbosity >= 1:
                print(f"Found {len(repeated_matches)} line(s) that appear in multiple patterns")
            write_repeated_matches(repeated_matches, args.output_dir, args.verbosity)
        else:
            if args.verbosity >= 1:
                print("No repeated matches found")
        
        if args.verbosity >= 1:
            print()
        
        # Print summary
        print("="*50)
        print("SUMMARY")
        print("="*50)
        for pattern_name, pattern_info in pattern_results.items():
            label_info = f" (label: {pattern_info['label']})" if pattern_info['label'] else ""
            print(f"{pattern_name}{label_info}: {pattern_info['matches']} matches")
        print(f"\nTotal matches found: {total_matches}")
        
        if repeated_matches:
            print(f"Repeated matches: {len(repeated_matches)} lines appear in multiple patterns")
        
        if total_matches > 0:
            print(f"\nMatch files written to: {args.output_dir}")
            if args.window > 0:
                print(f"Context window: {args.window} lines before/after each match")
        
        if args.pattern_list:
            print(f"Pattern list file: {os.path.join(args.output_dir, 'patterns-list.txt')}")
    
    else:
        # Start-End pattern matching mode
        start_pattern_name, start_pattern_timings = parse_single_pattern_file(args.pattern_start)
        end_pattern_name, end_pattern_timings = parse_single_pattern_file(args.pattern_end)
        
        if args.verbosity >= 1:
            print(f"Loaded start pattern '{start_pattern_name}': {start_pattern_timings}")
            print(f"Loaded end pattern '{end_pattern_name}': {end_pattern_timings}")
            print()
            
            print(f"Searching for matches between '{start_pattern_name}' and '{end_pattern_name}'")
        
        # Find start-end matches
        raw_matches = find_start_end_pattern_matches(trace_data, start_pattern_timings, end_pattern_timings)
        
        # Extract full matches
        matches_with_data = []
        for match_info in raw_matches:
            match_data = extract_start_end_match(trace_data, match_info)
            matches_with_data.append(match_data)
        
        if args.verbosity >= 1:
            print(f"  Found {len(matches_with_data)} start-end match(es)")
        
        # Write matches to file
        if matches_with_data:
            write_start_end_pattern_matches(start_pattern_name, matches_with_data, args.output_dir, args.verbosity)
        
        if args.verbosity >= 1:
            print()
        
        # Print summary
        print("="*50)
        print("SUMMARY")
        print("="*50)
        print(f"Start-End Matches: {len(matches_with_data)} matches")
        
        if matches_with_data:
            print(f"\nMatch files written to: {args.output_dir}")

if __name__ == "__main__":
    main()