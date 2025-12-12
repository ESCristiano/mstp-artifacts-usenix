#!/usr/bin/env python3
"""
Pattern Matrix Visualization Generator
Generates clean visual pattern template matrices with optional pruning

Usage:
    python pattern_matrix_gen.py -p patterns.txt
    python pattern_matrix_gen.py -p patterns.txt --pruned --pruned-start 10 --pruned-final 50
    python pattern_matrix_gen.py -p patterns.txt -pr -prs 15 -prf 45 -v 1
"""

import argparse
import sys
import os
from pathlib import Path
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
import numpy as np
from typing import List, Tuple, Dict, Optional
import matplotlib.gridspec as gridspec


class PatternVisualizer:
    """Class to generate pattern matrix visualizations"""
    
    # Color gradient definitions (lighter to darker)
    COLOR_GRADIENTS = {
        'blue': ['#ffffff', '#e0f2fe', '#bae6fd', '#7dd3fc', '#38bdf8', '#0ea5e9', '#0284c7', '#0369a1', '#075985', '#0c4a6e'],
        'blue_dark': ['#ffffff', '#bae6fd', '#7dd3fc', '#38bdf8', '#0ea5e9', '#0284c7', '#0369a1', '#075985', '#0c4a6e', '#082f49'],
        'green': ['#ffffff', '#dcfce7', '#bbf7d0', '#86efac', '#4ade80', '#22c55e', '#16a34a', '#15803d', '#14532d', '#052e16'],
        'red': ['#ffffff', '#fee2e2', '#fecaca', '#fca5a5', '#f87171', '#ef4444', '#dc2626', '#b91c1c', '#991b1b', '#7f1d1d'],
        'orange': ['#ffffff', '#fed7aa', '#fdba74', '#fb923c', '#f97316', '#ea580c', '#c2410c', '#9a3412', '#7c2d12', '#431407'],
        'purple': ['#ffffff', '#f3e8ff', '#e9d5ff', '#d8b4fe', '#c084fc', '#a855f7', '#9333ea', '#7e22ce', '#6b21a8', '#581c87'],
        'gray': ['#ffffff', '#f3f4f6', '#e5e7eb', '#d1d5db', '#9ca3af', '#6b7280', '#4b5563', '#374151', '#1f2937', '#111827'],
    }
    
    def __init__(self, input_dir: str, output_dir: str, verbosity: int):
        self.input_dir = Path(input_dir)
        self.output_dir = Path(output_dir)
        self.verbosity = verbosity
        self.patterns = []
        
    def log(self, message: str, level: int = 1):
        """Print message if verbosity level allows"""
        if self.verbosity >= level:
            print(message)
    
    def parse_pattern_file(self, filepath: Path) -> List[Dict]:
        """Parse pattern file and extract pattern definitions"""
        patterns = []
        
        try:
            with open(filepath, 'r') as f:
                for line_num, line in enumerate(f, 1):
                    line = line.strip()
                    if not line or line.startswith('#'):
                        continue
                    
                    parts = line.split()
                    if len(parts) < 4:
                        self.log(f"Warning: Invalid pattern format at line {line_num}: {line}", 0)
                        continue
                    
                    color = parts[0].lower()
                    name = parts[1]
                    label = parts[2]
                    
                    # Parse timing values
                    try:
                        timings = [int(t) for t in parts[3:]]
                    except ValueError:
                        self.log(f"Warning: Invalid timing values at line {line_num}", 0)
                        continue
                    
                    if color not in self.COLOR_GRADIENTS:
                        self.log(f"Warning: Unknown color '{color}' at line {line_num}, using 'gray'", 0)
                        color = 'gray'
                    
                    patterns.append({
                        'color': color,
                        'name': name,
                        'label': label,
                        'timings': timings,
                        'length': len(timings)
                    })
                    
        except FileNotFoundError:
            self.log(f"Error: Pattern file not found: {filepath}", 0)
            sys.exit(1)
        except Exception as e:
            self.log(f"Error reading pattern file: {str(e)}", 0)
            sys.exit(1)
        
        return patterns
    
    def find_differences(self, patterns: List[Dict]) -> Dict[int, bool]:
        """Find indices where patterns differ and return a dict of {index: differs}"""
        if len(patterns) < 2:
            return {}
        
        # Find minimum length
        min_length = min(p['length'] for p in patterns)
        differences = {}
        
        # Check each position
        for i in range(min_length):
            values = [p['timings'][i] for p in patterns if i < p['length']]
            # Mark as different if not all values are the same
            differences[i] = len(set(values)) > 1
        
        # Mark positions beyond min_length as different
        max_length = max(p['length'] for p in patterns)
        for i in range(min_length, max_length):
            differences[i] = True
        
        return differences
    
    def auto_detect_pruning_range(self, patterns: List[Dict]) -> Tuple[int, int]:
        """Automatically detect the range to prune based on pattern differences"""
        if len(patterns) < 2:
            max_length = max(p['length'] for p in patterns) if patterns else 0
            return 0, max_length
        
        differences = self.find_differences(patterns)
        min_length = min(p['length'] for p in patterns)
        
        # Find first difference from start
        start_diff = 0
        for i in range(min_length):
            if differences.get(i, False):
                start_diff = max(0, i - 3)  # Include some context before difference
                break
        
        # Find last difference from end
        end_diff = min_length
        for i in range(min_length - 1, -1, -1):
            if differences.get(i, False):
                end_diff = min(min_length, i + 4)  # Include some context after difference
                break
        
        return start_diff, end_diff
    
    def create_full_matrix(self, patterns: List[Dict], output_file: str):
        """Create visualization with complete pattern traces"""
        self.log("Creating full pattern matrix visualization...", 1)
        
        if not patterns:
            self.log("Error: No patterns to visualize", 0)
            return
        
        # Setup figure with square aspect ratio for cells
        max_length = max(p['length'] for p in patterns)
        cell_size = 0.3  # Size of each square cell in inches
        fig_width = min(20, max(8, max_length * cell_size + 3))
        fig_height = len(patterns) * cell_size + 2
        
        fig, ax = plt.subplots(figsize=(fig_width, fig_height))
        
        # Draw background grid
        self._draw_grid(ax, max_length, len(patterns))
        
        # Fill empty cells with light gray
        self._fill_empty_cells(ax, patterns, max_length)
        
        # Plot each pattern
        for row_idx, pattern in enumerate(patterns):
            color_gradient = self.COLOR_GRADIENTS[pattern['color']]
            
            # Draw pattern samples
            for col_idx, timing in enumerate(pattern['timings']):
                # Map timing value to color intensity (0-8 range)
                color_idx = min(max(0, timing), len(color_gradient) - 1)
                color = color_gradient[color_idx]
                
                # Create square rectangle
                rect = mpatches.Rectangle((col_idx, row_idx), 1, 1,
                                         facecolor=color, edgecolor='white', linewidth=0.5)
                ax.add_patch(rect)
        
        self._configure_axes(ax, patterns, max_length, mode='full')
        
        # Adjust layout and save
        plt.tight_layout()
        plt.savefig(output_file, dpi=300, bbox_inches='tight', facecolor='white')
        plt.close()
        
        self.log(f"  Saved full matrix to: {output_file}", 1)
    
    def create_pruned_matrix(self, patterns: List[Dict], output_file: str, 
                            pruned_start: int, pruned_final: int):
        """Create visualization with pruned traces showing discontinuity"""
        self.log(f"Creating pruned pattern matrix (hiding samples {pruned_start}-{pruned_final})...", 1)
        
        if not patterns:
            self.log("Error: No patterns to visualize", 0)
            return
        
        max_length = max(p['length'] for p in patterns)
        
        # Validate pruning parameters
        if pruned_start < 0:
            pruned_start = 0
        if pruned_final > max_length:
            pruned_final = max_length
        if pruned_start >= pruned_final:
            self.log("Warning: Invalid pruning range, showing full trace", 1)
            return self.create_full_matrix(patterns, output_file)
        
        # Calculate display dimensions
        gap_cells = 3  # Width of discontinuity indicator
        prefix_length = pruned_start
        suffix_length = max_length - pruned_final
        total_display_cols = prefix_length + gap_cells + suffix_length
        
        # Setup figure
        cell_size = 0.3
        fig_width = min(20, max(8, total_display_cols * cell_size + 3))
        fig_height = len(patterns) * cell_size + 2
        
        fig, ax = plt.subplots(figsize=(fig_width, fig_height))
        
        # Draw background grid
        self._draw_grid(ax, total_display_cols, len(patterns))
        
        # Fill empty cells in gap area with light gray (for visual consistency)
        empty_color = '#f3f4f6'
        gap_start = pruned_start
        for row_idx in range(len(patterns)):
            for i in range(gap_cells):
                rect = mpatches.Rectangle((gap_start + i, row_idx), 1, 1,
                                         facecolor=empty_color, edgecolor='white', linewidth=0.5)
                ax.add_patch(rect)
        
        # Plot each pattern
        for row_idx, pattern in enumerate(patterns):
            color_gradient = self.COLOR_GRADIENTS[pattern['color']]
            display_col = 0
            
            # Draw prefix (before pruned section)
            for col_idx in range(min(pruned_start, pattern['length'])):
                timing = pattern['timings'][col_idx]
                color_idx = min(max(0, timing), len(color_gradient) - 1)
                color = color_gradient[color_idx]
                
                rect = mpatches.Rectangle((display_col, row_idx), 1, 1,
                                         facecolor=color, edgecolor='white', linewidth=0.5)
                ax.add_patch(rect)
                display_col += 1
            
            # Draw discontinuity indicator (on top of the light gray background)
            gap_start = display_col
            
            # Draw ellipsis dots to indicate discontinuity
            for i in range(3):
                circle = plt.Circle((gap_start + 0.5 + i*0.8, row_idx + 0.5), 0.08, 
                                  color='#666666', alpha=0.8)
                ax.add_patch(circle)
            
            display_col = gap_start + gap_cells
            
            # Draw suffix (after pruned section)
            for col_idx in range(pruned_final, pattern['length']):
                timing = pattern['timings'][col_idx]
                color_idx = min(max(0, timing), len(color_gradient) - 1)
                color = color_gradient[color_idx]
                
                rect = mpatches.Rectangle((display_col, row_idx), 1, 1,
                                         facecolor=color, edgecolor='white', linewidth=0.5)
                ax.add_patch(rect)
                display_col += 1
        
        # Configure axes for pruned view
        self._configure_axes_pruned(ax, patterns, total_display_cols, 
                                   pruned_start, pruned_final, gap_cells)
        
        # Adjust layout and save
        plt.tight_layout()
        plt.savefig(output_file, dpi=300, bbox_inches='tight', facecolor='white')
        plt.close()
        
        self.log(f"  Saved pruned matrix to: {output_file}", 1)
    
    def _draw_grid(self, ax, cols: int, rows: int):
        """Draw a white grid aligned with the pattern matrix"""
        grid_color = 'white'
        
        # Draw vertical grid lines (between columns)
        for col in range(cols + 1):
            ax.axvline(x=col, color=grid_color, linewidth=1, zorder=0)
        
        # Draw horizontal grid lines (between rows)
        for row in range(rows + 1):
            ax.axhline(y=row, color=grid_color, linewidth=1, zorder=0)
    
    def _fill_empty_cells(self, ax, patterns: List[Dict], max_cols: int):
        """Fill cells without samples with light gray"""
        empty_color = '#f3f4f6'  # Light gray for empty cells
        
        for row_idx, pattern in enumerate(patterns):
            # Fill empty cells beyond pattern length
            for col_idx in range(pattern['length'], max_cols):
                rect = mpatches.Rectangle((col_idx, row_idx), 1, 1,
                                         facecolor=empty_color, edgecolor='white', linewidth=0.5)
                ax.add_patch(rect)
    
    def _configure_axes(self, ax, patterns: List[Dict], max_length: int, mode: str = 'full'):
        """Configure axes for full matrix view"""
        # Configure axes
        ax.set_xlim(0, max_length)
        ax.set_ylim(0, len(patterns))
        ax.set_aspect('equal')  # Make cells square
        
        # Set labels - use the pattern names as row labels
        ax.set_yticks([i + 0.5 for i in range(len(patterns))])
        ax.set_yticklabels([p['name'] for p in patterns], fontsize=12)
        
        # Set x-axis ticks - show sample indices at reasonable intervals
        if max_length <= 20:
            tick_interval = 2
        elif max_length <= 50:
            tick_interval = 5
        elif max_length <= 100:
            tick_interval = 10
        elif max_length <= 200:
            tick_interval = 20
        else:
            tick_interval = 50
        
        # Show x-axis ticks at the bottom
        x_ticks = list(range(0, max_length + 1, tick_interval))
        ax.set_xticks(x_ticks)
        ax.set_xticklabels([str(i) for i in x_ticks], fontsize=10)
        
        # Position x-axis at bottom
        ax.xaxis.tick_bottom()
        # ax.set_xlabel('Sample Index', fontsize=12)
        
        # Remove all spines for cleaner look
        for spine in ax.spines.values():
            spine.set_visible(False)
        
        # Remove tick marks but keep labels
        ax.tick_params(left=False, bottom=False, top=False, right=False)
        
        # Invert y-axis to match example (first pattern at top)
        ax.invert_yaxis()
    
    def _configure_axes_pruned(self, ax, patterns: List[Dict], total_cols: int,
                              pruned_start: int, pruned_final: int, gap_cells: int):
        """Configure axes for pruned matrix view"""
        # Configure basic axes
        ax.set_xlim(0, total_cols)
        ax.set_ylim(0, len(patterns))
        ax.set_aspect('equal')
        
        # Set row labels
        ax.set_yticks([i + 0.5 for i in range(len(patterns))])
        ax.set_yticklabels([p['name'] for p in patterns], fontsize=12)
        
        # Custom x-axis labels showing actual sample indices
        x_labels = []
        x_positions = []
        
        # Add labels for prefix section
        prefix_length = pruned_start
        if prefix_length > 0:
            step = max(1, prefix_length // 5)  # Show ~5 labels in prefix
            for i in range(0, prefix_length, step):
                x_labels.append(str(i))
                x_positions.append(i)
        
        # Add labels for suffix section
        suffix_start_display = prefix_length + gap_cells
        max_length = max(p['length'] for p in patterns)
        suffix_length = max_length - pruned_final
        
        if suffix_length > 0:
            step = max(1, suffix_length // 5)  # Show ~5 labels in suffix
            for i in range(pruned_final, max_length, step):
                display_pos = suffix_start_display + (i - pruned_final)
                if display_pos < total_cols:
                    x_labels.append(str(i))
                    x_positions.append(display_pos)
        
        # Set the custom ticks
        if x_positions:
            ax.set_xticks(x_positions)
            ax.set_xticklabels(x_labels, fontsize=10)
        
        ax.set_xlabel(f'Sample Index (samples {pruned_start}-{pruned_final-1} hidden)', fontsize=12)
        
        # Remove all spines
        for spine in ax.spines.values():
            spine.set_visible(False)
        
        ax.tick_params(left=False, bottom=False, top=False, right=False)
        ax.invert_yaxis()
    
    def generate_text_matrix(self, patterns: List[Dict], output_file: str):
        """Generate text-based matrix representation"""
        self.log("Generating text matrix representation...", 1)
        
        try:
            with open(output_file, 'w') as f:
                f.write("# Pattern Template Matrix (Text Format)\n")
                f.write("# Format: Pattern_Name Label Timing_Sequence\n")
                f.write("#\n\n")
                
                # Find max name length for alignment
                max_name_len = max(len(p['name']) for p in patterns) if patterns else 0
                max_label_len = max(len(p['label']) for p in patterns) if patterns else 0
                
                for pattern in patterns:
                    # Format timing sequence with spaces
                    timing_str = ' '.join(f"{t:02d}" for t in pattern['timings'])
                    
                    # Write formatted line
                    f.write(f"{pattern['name']:<{max_name_len}} "
                           f"{pattern['label']:<{max_label_len}} "
                           f"{timing_str}\n")
                
                f.write(f"\n# Total patterns: {len(patterns)}\n")
                
                # Add statistics
                if patterns:
                    f.write("\n# Pattern Statistics:\n")
                    for pattern in patterns:
                        f.write(f"#   {pattern['name']}: {pattern['length']} samples\n")
            
            self.log(f"  Saved text matrix to: {output_file}", 1)
            
        except Exception as e:
            self.log(f"Error writing text matrix: {str(e)}", 0)
    
    def run(self, pattern_file: Path, pruned: bool = False, 
            pruned_start: Optional[int] = None, pruned_final: Optional[int] = None):
        """Run the visualization generation"""
        # Create output directory if it doesn't exist
        self.output_dir.mkdir(parents=True, exist_ok=True)
        
        self.log(f"Processing pattern file: {pattern_file}", 1)
        
        # Parse patterns
        patterns = self.parse_pattern_file(pattern_file)
        
        if not patterns:
            self.log("Error: No valid patterns found", 0)
            return
        
        self.log(f"Loaded {len(patterns)} patterns", 1)
        
        # Generate visualization
        base_name = pattern_file.stem
        
        if pruned:
            # Handle auto-detection of pruning range if not specified
            if pruned_start is None or pruned_final is None:
                auto_start, auto_final = self.auto_detect_pruning_range(patterns)
                if pruned_start is None:
                    pruned_start = auto_start
                if pruned_final is None:
                    pruned_final = auto_final
                self.log(f"Auto-detected pruning range: {pruned_start} to {pruned_final}", 1)
            
            output_file = self.output_dir / f"{base_name}_matrix_pruned.png"
            self.create_pruned_matrix(patterns, output_file, pruned_start, pruned_final)
        else:
            output_file = self.output_dir / f"{base_name}_matrix.png"
            self.create_full_matrix(patterns, output_file)
        
        # Generate text matrix
        text_output = self.output_dir / f"{base_name}_matrix.txt"
        self.generate_text_matrix(patterns, text_output)
        
        # Report summary
        max_length = max(p['length'] for p in patterns)
        
        print(f"\nPattern Analysis:")
        print(f"  Total patterns: {len(patterns)}")
        print(f"  Total samples: {max_length}")
        
        if pruned:
            hidden_samples = pruned_final - pruned_start
            print(f"  Hidden samples: {hidden_samples} (from {pruned_start} to {pruned_final-1})")
            print(f"  Shown samples: {max_length - hidden_samples}")
        
        self.log("\nVisualization generation complete!", 1)


def main():
    parser = argparse.ArgumentParser(
        description='Generate pattern template matrix visualizations with pruning options',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  # Full trace visualization
  %(prog)s -p patterns.txt
  
  # Pruned trace with auto-detection of different regions
  %(prog)s -p patterns.txt --pruned
  
  # Pruned trace hiding samples 10-50
  %(prog)s -p patterns.txt --pruned --pruned-start 10 --pruned-final 50
  
  # Short form
  %(prog)s -p patterns.txt -pr -prs 15 -prf 45
  
  # With custom directories and verbose output
  %(prog)s -p patterns.txt -pr -prs 10 -prf 40 -i ./traces -o ./output -v 1

Pattern file format:
  color pattern_name label timing_sequence
  
  Example:
    blue key shift_key 02 02 01 01 01 01 01 02 03 01 02 01 01 01 03 ...
    blue longpress shift_long 02 02 01 01 01 01 01 02 03 01 02 01 01 01 03 ...
    blue swipe shift_swipe 02 02 01 01 01 01 01 01 02 03 01 02 01 01 01 ...
    blue tap shift_tap 02 02 01 01 01 01 01 01 02 03 01 02 01 01 01 ...
    blue text shift_text 02 02 01 01 01 01 01 01 02 03 01 02 01 01 01 ...

Available colors:
  blue, green, red, orange, purple, gray

Pruning modes:
  - Full trace: Shows complete pattern sequences
  - Pruned trace: Hides specified middle section, shows discontinuity with dots
  - Auto-pruned: Automatically detects and hides similar regions between patterns

Visual features:
  - Clean square cells with pattern-specific color gradients
  - White borders between cells
  - Pattern names as row labels
  - Sample indices on x-axis
  - Discontinuity indicator (dots) for pruned sections

Output files:
  <pattern_name>_matrix.png        - Full matrix visualization
  <pattern_name>_matrix_pruned.png - Pruned matrix visualization  
  <pattern_name>_matrix.txt        - Text representation
        """
    )
    
    parser.add_argument('-p', '--patterns', required=True,
                       help='Pattern definition file')
    parser.add_argument('-i', '--input-dir', default='.',
                       help='Input directory (default: current directory)')
    parser.add_argument('-o', '--output-dir', default='.',
                       help='Output directory (default: current directory)')
    parser.add_argument('-v', '--verbosity', type=int, choices=[0, 1], default=0,
                       help='Verbosity level: 0=quiet, 1=verbose (default: 0)')
    
    # Pruning options
    parser.add_argument('-pr', '--pruned', action='store_true',
                       help='Create pruned visualization hiding middle section')
    parser.add_argument('-prs', '--pruned-start', type=int,
                       help='Start index of section to hide (auto-detect if not specified)')
    parser.add_argument('-prf', '--pruned-final', type=int,
                       help='Final index of section to hide (auto-detect if not specified)')
    
    args = parser.parse_args()
    
    # Validate pruning arguments
    if (args.pruned_start is not None or args.pruned_final is not None) and not args.pruned:
        print("Warning: Pruning start/final specified but --pruned not set. Enabling pruned mode.")
        args.pruned = True
    
    if args.pruned and args.pruned_start is not None and args.pruned_final is not None:
        if args.pruned_start >= args.pruned_final:
            print("Error: pruned-start must be less than pruned-final")
            sys.exit(1)
    
    # Create visualizer
    visualizer = PatternVisualizer(args.input_dir, args.output_dir, args.verbosity)
    
    # Check if pattern file exists
    pattern_path = Path(args.patterns)
    if not pattern_path.exists():
        print(f"Error: Pattern file not found: {args.patterns}")
        sys.exit(1)
    
    # Run visualization
    visualizer.run(pattern_path, args.pruned, args.pruned_start, args.pruned_final)
    
    # Print summary
    print(f"\nSummary:")
    print(f"  Pattern file: {args.patterns}")
    print(f"  Output directory: {args.output_dir}")
    if args.pruned:
        if args.pruned_start is not None and args.pruned_final is not None:
            print(f"  Pruning mode: Manual (hiding samples {args.pruned_start}-{args.pruned_final-1})")
        else:
            print(f"  Pruning mode: Auto-detection")
    else:
        print(f"  Visualization mode: Full trace")
    print("\nDone!")


if __name__ == '__main__':
    main()