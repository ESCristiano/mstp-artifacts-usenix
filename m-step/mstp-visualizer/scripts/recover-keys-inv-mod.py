#!/usr/bin/env python3

import argparse
import re
import sys
import os

def my_algorithm(SHIFTS_u, SHIFTS_v, SUBS):
    # Step 1
    l = len(SUBS)  # total number of steps
    u = 0
    v = 1
    # Steps 2 & 3: loop from l down to 2
    for i in range(l-1, -1, -1):
        if SUBS[i]:
            u = u + v 
        else:
            v = u + v
        u = u << SHIFTS_u[i]
        v = v << SHIFTS_v[i]
    # Step 5
    return u, v


def parse_array_from_line(line):
    """Parse array from a line like 'SHIFTS_u = [1, 2, 3, ...]'"""
    # Find the content between brackets
    match = re.search(r'\[(.*?)\]', line)
    if not match:
        raise ValueError(f"Could not parse array from line: {line}")
    
    # Split by comma and convert to integers
    array_str = match.group(1)
    return [int(x.strip()) for x in array_str.split(',')]


def read_patterns_file(filename):
    """Read SHIFTS_u, SHIFTS_v, and SUBS from file"""
    if not os.path.exists(filename):
        raise FileNotFoundError(f"Patterns file not found: {filename}")
    
    with open(filename, 'r') as f:
        content = f.read()
    
    SHIFTS_u = None
    SHIFTS_v = None
    SUBS = None
    
    for line in content.split('\n'):
        line = line.strip()
        if line.startswith('SHIFTS_u'):
            SHIFTS_u = parse_array_from_line(line)
        elif line.startswith('SHIFTS_v'):
            SHIFTS_v = parse_array_from_line(line)
        elif line.startswith('SUBS'):
            SUBS = parse_array_from_line(line)
    
    if SHIFTS_u is None or SHIFTS_v is None or SUBS is None:
        raise ValueError("Could not find all required arrays (SHIFTS_u, SHIFTS_v, SUBS) in patterns file")
    
    return SHIFTS_u, SHIFTS_v, SUBS


def read_keys_file(filename):
    """Read p, q, and optionally N from private keys file"""
    if not os.path.exists(filename):
        raise FileNotFoundError(f"Keys file not found: {filename}")
    
    with open(filename, 'r') as f:
        content = f.read()
    
    p = None
    q = None
    
    for line in content.split('\n'):
        line = line.strip()
        if line.startswith('p =') or line.startswith('p='):
            # Extract hex value after '='
            hex_match = re.search(r'=\s*0?x?([0-9a-fA-F]+)', line)
            if hex_match:
                p = int(hex_match.group(1), 16)
        elif line.startswith('q =') or line.startswith('q='):
            # Extract hex value after '='
            hex_match = re.search(r'=\s*0?x?([0-9a-fA-F]+)', line)
            if hex_match:
                q = int(hex_match.group(1), 16)
    
    return p, q


def main():
    parser = argparse.ArgumentParser(description='RSA Key Recovery Tool')
    parser.add_argument('-p', '--patterns', required=True, 
                       help='File containing SHIFTS_u, SHIFTS_v, and SUBS arrays')
    parser.add_argument('-k', '--keys', required=True,
                       help='File containing original p, q values or N for verification')
    
    args = parser.parse_args()
    
    try:
        # Read patterns from file
        SHIFTS_u, SHIFTS_v, SUBS = read_patterns_file(args.patterns)
        
        # Read keys from file
        original_p, original_q = read_keys_file(args.keys)
        
        # Run the algorithm
        u, v = my_algorithm(SHIFTS_u, SHIFTS_v, SUBS)
        
        p = v 
        q = u  
                
        # Recover the keys
        if original_p and original_q:
            # If we have original p and q check if they match w/ hte recovered values
            
            # Verify the results 
            if p != original_p or q != original_q:
                print(f"Warning: Recovered keys don't match original keys", file=sys.stderr)
    
        
        # Output only the recovered p and q
        print(f"p = 0x{p:x}")
        print(f"q = 0x{q:x}")
        
    except Exception as e:
        print(f"Error: {e}", file=sys.stderr)
        sys.exit(1)


if __name__ == "__main__":
    main()