#!/usr/bin/env python3
"""
Enhanced Pattern Matrix Visualization Generator
Generates clean visual pattern template matrices with custom color gradients

Usage:
    python pattern_matrix_gen.py -p patterns.txt
    python pattern_matrix_gen.py -p patterns.txt --pruned --pruned-start 10 --pruned-final 50
    python pattern_matrix_gen.py -p patterns.txt -pr -prs 15 -prf 45 -v 1
"""

import argparse
import sys
import os
import re
from pathlib import Path
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
import matplotlib.colors as mcolors
import matplotlib.cm as cm
import numpy as np
from typing import List, Tuple, Dict, Optional
import matplotlib.gridspec as gridspec

# Try to import seaborn for additional palettes
try:
    import seaborn as sns
    HAS_SEABORN = True
except ImportError:
    HAS_SEABORN = False


class PatternVisualizer:
    """Class to generate pattern matrix visualizations"""
    
    # Default color gradient (orange)
    DEFAULT_COLOR_GRADIENT = {
        'min_color': '#431407',
        'max_color': '#fed7aa',
        'steps': 10
    }
    
    def __init__(self, input_dir: str, output_dir: str, verbosity: int):
        self.input_dir = Path(input_dir)
        self.output_dir = Path(output_dir)
        self.verbosity = verbosity
        self.patterns = []
        self.x_labels = None
        self.y_labels = None
        self.color_config = None
        
        # Generate color gradients automatically
        self.COLOR_GRADIENTS = self._generate_color_gradients()
    
    def _generate_color_gradients(self) -> Dict[str, List[str]]:
        """Generate color gradients automatically using matplotlib and seaborn"""
        gradients = {}
        
        # Helper function to convert colormap to hex list
        def cmap_to_hex(cmap, n_colors=10):
            colors = [cmap(i / (n_colors - 1)) for i in range(n_colors)]
            return ['#{:02x}{:02x}{:02x}'.format(int(r*255), int(g*255), int(b*255)) for r, g, b, a in colors]
        
        # Basic gradients using matplotlib colormaps (new API)
        try:
            # Use new matplotlib colormap API
            mpl_colormaps = {
                'blue': plt.colormaps['Blues'].resampled(10),
                'blue_dark': plt.colormaps['Blues_r'].resampled(10),
                'green': plt.colormaps['Greens'].resampled(10),
                'red': plt.colormaps['Reds'].resampled(10),
                'purple': plt.colormaps['Purples'].resampled(10),
                'gray': plt.colormaps['Greys'].resampled(10),
                'orange': plt.colormaps['Oranges'].resampled(10),
                'thermal': plt.colormaps['viridis'].resampled(11),
                'inferno': plt.colormaps['inferno'].resampled(10),
                'magma': plt.colormaps['magma'].resampled(11),
                'plasma': plt.colormaps['plasma'].resampled(10),
            }
        except AttributeError:
            # Fallback for older matplotlib versions
            mpl_colormaps = {
                'blue': cm.get_cmap('Blues', 10),
                'blue_dark': cm.get_cmap('Blues_r', 10),
                'green': cm.get_cmap('Greens', 10),
                'red': cm.get_cmap('Reds', 10),
                'purple': cm.get_cmap('Purples', 10),
                'gray': cm.get_cmap('Greys', 10),
                'orange': cm.get_cmap('Oranges', 10),
                'thermal': cm.get_cmap('viridis', 11),
                'inferno': cm.get_cmap('inferno', 10),
                'magma': cm.get_cmap('magma', 11),
                'plasma': cm.get_cmap('plasma', 10),
            }
        
        # Generate standard gradients
        for name, cmap in mpl_colormaps.items():
            gradients[name] = cmap_to_hex(cmap, 10 if name not in ['thermal', 'magma'] else 11)
        
        # Add seaborn rocket if available
        if HAS_SEABORN:
            try:
                rocket_palette = sns.color_palette("rocket", 11)
                gradients['rocket'] = ['#{:02x}{:02x}{:02x}'.format(int(r*255), int(g*255), int(b*255)) for r, g, b in rocket_palette]
            except Exception:
                # Fallback rocket gradient if seaborn fails
                gradients['rocket'] = ['#03051a', '#161339', '#2d1e3e', '#472a3c', '#623635', '#7e412b', '#9a4c1a', '#b75700', '#d46200', '#f16e00', '#ff7a00']
        else:
            # Fallback rocket gradient when seaborn is not available
            gradients['rocket'] = ['#03051a', '#161339', '#2d1e3e', '#472a3c', '#623635', '#7e412b', '#9a4c1a', '#b75700', '#d46200', '#f16e00', '#ff7a00']
        
        # Create orange variants
        gradients['orange_white'] = ['#ffffff'] + gradients['orange'][1:]
        
        # Generate inverted gradients
        inverted_gradients = {}
        for name, colors in gradients.items():
            inverted_gradients[f'{name}_inv'] = list(reversed(colors))
        
        # Combine all gradients
        gradients.update(inverted_gradients)
        
        return gradients
    
    def log(self, message: str, level: int = 1):
        """Print message if verbosity level allows"""
        if self.verbosity >= level:
            print(message)
    
    def hex_to_rgb(self, hex_color: str) -> Tuple[float, float, float]:
        """Convert hex color to RGB tuple (0-1 range)"""
        # Clean up the hex color string
        hex_color = hex_color.strip().lstrip('#')
        
        # Check for 'auto' keyword
        if 'auto' in hex_color.lower():
            # Use default color
            hex_color = self.DEFAULT_COLOR_GRADIENT['min_color'].lstrip('#')
        
        # Validate hex color format
        if not hex_color or len(hex_color) != 6:
            self.log(f"Warning: Invalid hex color '{hex_color}', using default", 0)
            hex_color = self.DEFAULT_COLOR_GRADIENT['min_color'].lstrip('#')
        
        try:
            return tuple(int(hex_color[i:i+2], 16) / 255.0 for i in (0, 2, 4))
        except ValueError:
            self.log(f"Warning: Could not parse hex color '{hex_color}', using default", 0)
            default_hex = self.DEFAULT_COLOR_GRADIENT['min_color'].lstrip('#')
            return tuple(int(default_hex[i:i+2], 16) / 255.0 for i in (0, 2, 4))
    
    def rgb_to_hex(self, rgb: Tuple[float, float, float]) -> str:
        """Convert RGB tuple (0-1 range) to hex color"""
        return '#{:02x}{:02x}{:02x}'.format(
            int(rgb[0] * 255),
            int(rgb[1] * 255),
            int(rgb[2] * 255)
        )
    
    def create_color_gradient(self, min_color: str, max_color: str, steps: int) -> List[str]:
        """Create a color gradient between two colors"""
        min_rgb = self.hex_to_rgb(min_color)
        max_rgb = self.hex_to_rgb(max_color)
        
        gradient = []
        for i in range(steps):
            # Linear interpolation
            t = i / (steps - 1) if steps > 1 else 0
            r = min_rgb[0] + (max_rgb[0] - min_rgb[0]) * t
            g = min_rgb[1] + (max_rgb[1] - min_rgb[1]) * t
            b = min_rgb[2] + (max_rgb[2] - min_rgb[2]) * t
            gradient.append(self.rgb_to_hex((r, g, b)))
        
        return gradient
    
    def interpolate_gradient(self, gradient: List[str], target_steps: int) -> List[str]:
        """Interpolate a gradient to have a specific number of steps"""
        if len(gradient) == target_steps:
            return gradient
        
        if len(gradient) > target_steps:
            # Downsample: select evenly spaced colors
            indices = np.linspace(0, len(gradient) - 1, target_steps, dtype=int)
            return [gradient[i] for i in indices]
        
        # Upsample: interpolate between existing colors
        new_gradient = []
        
        # Convert all colors to RGB
        rgb_colors = [self.hex_to_rgb(color) for color in gradient]
        
        # Calculate how many colors we need between each pair
        segments = len(gradient) - 1
        colors_per_segment = (target_steps - 1) / segments
        
        for i in range(len(gradient) - 1):
            start_rgb = rgb_colors[i]
            end_rgb = rgb_colors[i + 1]
            
            # Calculate number of steps for this segment
            segment_start = int(i * colors_per_segment)
            segment_end = int((i + 1) * colors_per_segment)
            segment_steps = segment_end - segment_start
            
            # Interpolate colors for this segment
            for j in range(segment_steps):
                t = j / segment_steps if segment_steps > 0 else 0
                r = start_rgb[0] + (end_rgb[0] - start_rgb[0]) * t
                g = start_rgb[1] + (end_rgb[1] - start_rgb[1]) * t
                b = start_rgb[2] + (end_rgb[2] - start_rgb[2]) * t
                new_gradient.append(self.rgb_to_hex((r, g, b)))
        
        # Add the last color
        new_gradient.append(gradient[-1])
        
        # Ensure we have exactly target_steps colors
        while len(new_gradient) < target_steps:
            new_gradient.append(gradient[-1])
        while len(new_gradient) > target_steps:
            new_gradient.pop()
        
        return new_gradient
    
    def parse_pattern_file(self, filepath: Path) -> Tuple[List[List[float]], Dict]:
        """Parse pattern file and extract configuration and data matrix"""
        data_matrix = []
        config = {
            'color_type': 'integer',  # Default
            'min_color': self.DEFAULT_COLOR_GRADIENT['min_color'],
            'max_color': self.DEFAULT_COLOR_GRADIENT['max_color'],
            'min_number_of_colors': self.DEFAULT_COLOR_GRADIENT['steps'],
            'gradient': None,  # Custom gradient if provided
            'gradient_name': None,  # Predefined gradient name
            'x_labels': None,
            'y_labels': None
        }
        
        try:
            with open(filepath, 'r') as f:
                for line_num, line in enumerate(f, 1):
                    line = line.strip()
                    
                    # Skip empty lines and comments
                    if not line or line.startswith('#'):
                        continue
                    
                    # Check for configuration lines
                    if line.startswith('Color_type:'):
                        value = line.split(':', 1)[1].strip().split('#')[0].strip().lower()
                        config['color_type'] = value
                        self.log(f"Color type: {value}", 1)
                        continue
                    
                    elif line.startswith('Gradient:') or line.startswith('gradient:'):
                        # Parse custom gradient or predefined gradient name
                        value_part = line.split(':', 1)[1].strip()
                        
                        # Check if it's a predefined gradient name
                        gradient_name = value_part.strip().lower()
                        if gradient_name in self.COLOR_GRADIENTS:
                            config['gradient'] = self.COLOR_GRADIENTS[gradient_name].copy()
                            config['gradient_name'] = gradient_name
                            self.log(f"Using predefined gradient: {gradient_name}", 1)
                        # Check if it's a list of colors
                        elif '[' in value_part and ']' in value_part:
                            # Extract the list of colors
                            import ast
                            try:
                                # Find the list within the string
                                start_idx = value_part.index('[')
                                end_idx = value_part.rindex(']') + 1
                                list_str = value_part[start_idx:end_idx]
                                
                                # Parse the list
                                gradient_list = ast.literal_eval(list_str)
                                if isinstance(gradient_list, list) and all(isinstance(c, str) for c in gradient_list):
                                    config['gradient'] = gradient_list
                                    self.log(f"Custom gradient with {len(gradient_list)} colors", 1)
                                else:
                                    self.log(f"Warning: Invalid gradient format", 0)
                            except (ValueError, SyntaxError) as e:
                                self.log(f"Warning: Could not parse gradient: {e}", 0)
                        continue
                    
                    elif line.startswith('Min_color:'):
                        # Get everything after the colon
                        value_part = line.split(':', 1)[1].strip()
                        
                        # Check if it's 'auto' (with or without #)
                        if 'auto' in value_part.lower():
                            # Keep default color
                            self.log(f"Min color: auto (using default {config['min_color']})", 1)
                        else:
                            # Look for hex color code
                            hex_match = re.search(r'#[0-9a-fA-F]{6}', value_part)
                            if hex_match:
                                config['min_color'] = hex_match.group()
                                self.log(f"Min color: {config['min_color']}", 1)
                            else:
                                self.log(f"Warning: Invalid min_color value: {value_part}", 0)
                        continue
                    
                    elif line.startswith('Max_color:'):
                        # Get everything after the colon
                        value_part = line.split(':', 1)[1].strip()
                        
                        # Check if it's 'auto' (with or without #)
                        if 'auto' in value_part.lower():
                            # Keep default color
                            self.log(f"Max color: auto (using default {config['max_color']})", 1)
                        else:
                            # Look for hex color code
                            hex_match = re.search(r'#[0-9a-fA-F]{6}', value_part)
                            if hex_match:
                                config['max_color'] = hex_match.group()
                                self.log(f"Max color: {config['max_color']}", 1)
                            else:
                                self.log(f"Warning: Invalid max_color value: {value_part}", 0)
                        continue
                    
                    elif line.startswith('Min_number_of_colors:'):
                        value = line.split(':', 1)[1].strip().split('#')[0].strip()
                        try:
                            # Extract just the number, handling formats like "100" or "100]"
                            number_match = re.search(r'\d+', value)
                            if number_match:
                                config['min_number_of_colors'] = int(number_match.group())
                                self.log(f"Min number of colors: {config['min_number_of_colors']}", 1)
                            else:
                                self.log(f"Warning: Could not parse min_number_of_colors value: {value}", 0)
                        except ValueError:
                            self.log(f"Warning: Invalid min_number_of_colors value: {value}", 0)
                        continue
                    
                    elif line.startswith('X:'):
                        labels = line.split(':', 1)[1].strip().split()
                        config['x_labels'] = labels
                        self.log(f"X labels: {labels}", 1)
                        continue
                    
                    elif line.startswith('Y:'):
                        labels = line.split(':', 1)[1].strip().split()
                        config['y_labels'] = labels
                        self.log(f"Y labels: {labels}", 1)
                        continue
                    
                    # Parse data line
                    parts = line.split()
                    
                    # Check if first element is a color name (optional)
                    if parts[0].lower() in ['blue', 'green', 'red', 'orange', 'purple', 'gray']:
                        # Skip color label and use the rest as data
                        data_values = parts[1:]
                    else:
                        # No color label, all are data values
                        data_values = parts
                    
                    # Convert to float values
                    try:
                        row_data = [float(v) for v in data_values]
                        data_matrix.append(row_data)
                    except ValueError as e:
                        self.log(f"Warning: Invalid data values at line {line_num}: {e}", 0)
                        continue
                    
        except FileNotFoundError:
            self.log(f"Error: Pattern file not found: {filepath}", 0)
            sys.exit(1)
        except Exception as e:
            self.log(f"Error reading pattern file: {str(e)}", 0)
            sys.exit(1)
        
        return data_matrix, config
    
    def calculate_color_mapping(self, data_matrix: List[List[float]], config: Dict) -> Tuple[List[str], Dict]:
        """Calculate color gradient and value-to-color mapping"""
        # Check if a custom gradient was provided
        if config.get('gradient'):
            # Use the provided gradient
            base_gradient = config['gradient']
            
            if config['color_type'] == 'probability':
                # For probability, always use min_number_of_colors
                num_colors = config['min_number_of_colors']
                min_val = 0.0
                max_val = 1.0
            else:
                # For integer, find actual min/max in data
                all_values = [val for row in data_matrix for val in row]
                if not all_values:
                    min_val = 0
                    max_val = 1
                else:
                    min_val = min(all_values)
                    max_val = max(all_values)
                
                # Determine number of colors
                unique_values = len(set(all_values)) if all_values else 1
                num_colors = max(unique_values, config['min_number_of_colors'])
            
            # Interpolate gradient if needed
            if len(base_gradient) != num_colors:
                gradient = self.interpolate_gradient(base_gradient, num_colors)
                self.log(f"Interpolated gradient from {len(base_gradient)} to {num_colors} colors", 1)
            else:
                gradient = base_gradient
            
            self.log(f"Using {'predefined' if config.get('gradient_name') else 'custom'} gradient with {len(gradient)} colors", 1)
        
        else:
            # Use min_color and max_color to create gradient
            min_color = config['min_color']
            max_color = config['max_color']
            
            # If colors contain 'auto', use defaults
            if 'auto' in min_color.lower():
                min_color = self.DEFAULT_COLOR_GRADIENT['min_color']
            if 'auto' in max_color.lower():
                max_color = self.DEFAULT_COLOR_GRADIENT['max_color']
            
            if config['color_type'] == 'probability':
                # For probability, always use 0 to 1 range
                min_val = 0.0
                max_val = 1.0
                num_colors = config['min_number_of_colors']
            else:
                # For integer, find actual min/max in data
                all_values = [val for row in data_matrix for val in row]
                if not all_values:
                    min_val = 0
                    max_val = 1
                else:
                    min_val = min(all_values)
                    max_val = max(all_values)
                
                # Determine number of colors based on unique values or minimum specified
                unique_values = len(set(all_values)) if all_values else 1
                
                # For integer mode with specified min_number_of_colors
                if config['min_number_of_colors'] and config['min_number_of_colors'] > unique_values:
                    num_colors = config['min_number_of_colors']
                else:
                    # Use at least the number of unique values
                    num_colors = max(unique_values, config['min_number_of_colors'])
            
            # Create color gradient with cleaned color values
            gradient = self.create_color_gradient(
                min_color,
                max_color,
                num_colors
            )
            
            self.log(f"Created gradient from {min_color} to {max_color}", 1)
        
        # Create value-to-color index mapping
        value_mapping = {
            'min': min_val,
            'max': max_val,
            'gradient': gradient,
            'num_colors': num_colors
        }
        
        self.log(f"Color mapping: min={min_val}, max={max_val}, colors={num_colors}", 1)
        
        return gradient, value_mapping
    
    def get_color_for_value(self, value: float, mapping: Dict) -> str:
        """Get color for a specific value based on mapping"""
        min_val = mapping['min']
        max_val = mapping['max']
        gradient = mapping['gradient']
        
        if max_val == min_val:
            # All values are the same, use middle color
            return gradient[len(gradient) // 2] if gradient else '#808080'
        
        # Normalize value to 0-1 range
        normalized = (value - min_val) / (max_val - min_val)
        normalized = max(0, min(1, normalized))  # Clamp to [0, 1]
        
        # Map to color index
        # For a gradient of N colors, map normalized value to index 0 to N-1
        color_idx = int(normalized * (len(gradient) - 1))
        color_idx = min(color_idx, len(gradient) - 1)
        
        # Debug logging for first few calls
        if hasattr(self, '_debug_count'):
            if self._debug_count < 5:
                self.log(f"  Value {value} -> normalized {normalized:.2f} -> color index {color_idx}/{len(gradient)-1}", 2)
                self._debug_count += 1
        else:
            self._debug_count = 0
        
        return gradient[color_idx]
    
    def create_full_matrix(self, data_matrix: List[List[float]], config: Dict, 
                          output_file: str):
        """Create visualization with complete pattern matrix"""
        self.log("Creating full pattern matrix visualization...", 1)
        
        if not data_matrix:
            self.log("Error: No data to visualize", 0)
            return
        
        # Calculate color mapping
        gradient, value_mapping = self.calculate_color_mapping(data_matrix, config)
        
        # Setup figure
        num_rows = len(data_matrix)
        num_cols = max(len(row) for row in data_matrix) if data_matrix else 0
        
        cell_size = 0.3  # Size of each square cell in inches
        fig_width = min(20, max(8, num_cols * cell_size + 3))
        fig_height = num_rows * cell_size + 2
        
        fig, ax = plt.subplots(figsize=(fig_width, fig_height))
        
        # Draw background grid
        self._draw_grid(ax, num_cols, num_rows)
        
        # Fill empty cells with light gray
        empty_color = '#f3f4f6'
        for row_idx, row in enumerate(data_matrix):
            for col_idx in range(len(row), num_cols):
                rect = mpatches.Rectangle((col_idx, row_idx), 1, 1,
                                         facecolor=empty_color, edgecolor='white', linewidth=0.5)
                ax.add_patch(rect)
        
        # Plot each cell
        for row_idx, row in enumerate(data_matrix):
            for col_idx, value in enumerate(row):
                color = self.get_color_for_value(value, value_mapping)
                rect = mpatches.Rectangle((col_idx, row_idx), 1, 1,
                                         facecolor=color, edgecolor='white', linewidth=0.5)
                ax.add_patch(rect)
        
        # Configure axes
        self._configure_axes_new(ax, num_rows, num_cols, config)
        
        # Add color bar
        self._add_colorbar(fig, ax, value_mapping, config)
        
        # Adjust layout and save
        plt.tight_layout()
        plt.savefig(output_file, dpi=300, bbox_inches='tight', facecolor='white')
        plt.close()
        
        self.log(f"  Saved full matrix to: {output_file}", 1)
    
    def create_pruned_matrix(self, data_matrix: List[List[float]], config: Dict,
                            output_file: str, pruned_start: int, pruned_final: int):
        """Create visualization with pruned matrix showing discontinuity"""
        self.log(f"Creating pruned pattern matrix (hiding columns {pruned_start}-{pruned_final})...", 1)
        
        if not data_matrix:
            self.log("Error: No data to visualize", 0)
            return
        
        # Calculate color mapping
        gradient, value_mapping = self.calculate_color_mapping(data_matrix, config)
        
        num_rows = len(data_matrix)
        num_cols = max(len(row) for row in data_matrix) if data_matrix else 0
        
        # Validate pruning parameters
        if pruned_start < 0:
            pruned_start = 0
        if pruned_final > num_cols:
            pruned_final = num_cols
        if pruned_start >= pruned_final:
            self.log("Warning: Invalid pruning range, showing full matrix", 1)
            return self.create_full_matrix(data_matrix, config, output_file)
        
        # Calculate display dimensions
        gap_cells = 3  # Width of discontinuity indicator
        prefix_length = pruned_start
        suffix_length = num_cols - pruned_final
        total_display_cols = prefix_length + gap_cells + suffix_length
        
        # Setup figure
        cell_size = 0.3
        fig_width = min(20, max(8, total_display_cols * cell_size + 3))
        fig_height = num_rows * cell_size + 2
        
        fig, ax = plt.subplots(figsize=(fig_width, fig_height))
        
        # Draw background grid
        self._draw_grid(ax, total_display_cols, num_rows)
        
        # Fill gap area with light gray
        empty_color = '#f3f4f6'
        for row_idx in range(num_rows):
            for i in range(gap_cells):
                rect = mpatches.Rectangle((prefix_length + i, row_idx), 1, 1,
                                         facecolor=empty_color, edgecolor='white', linewidth=0.5)
                ax.add_patch(rect)
        
        # Plot each row
        for row_idx, row in enumerate(data_matrix):
            display_col = 0
            
            # Draw prefix
            for col_idx in range(min(pruned_start, len(row))):
                value = row[col_idx]
                color = self.get_color_for_value(value, value_mapping)
                rect = mpatches.Rectangle((display_col, row_idx), 1, 1,
                                         facecolor=color, edgecolor='white', linewidth=0.5)
                ax.add_patch(rect)
                display_col += 1
            
            # Draw discontinuity indicator
            gap_start = display_col
            for i in range(3):
                circle = plt.Circle((gap_start + 0.5 + i*0.8, row_idx + 0.5), 0.08,
                                  color='#666666', alpha=0.8)
                ax.add_patch(circle)
            
            display_col = gap_start + gap_cells
            
            # Draw suffix
            for col_idx in range(pruned_final, len(row)):
                value = row[col_idx]
                color = self.get_color_for_value(value, value_mapping)
                rect = mpatches.Rectangle((display_col, row_idx), 1, 1,
                                         facecolor=color, edgecolor='white', linewidth=0.5)
                ax.add_patch(rect)
                display_col += 1
        
        # Configure axes for pruned view
        self._configure_axes_pruned_new(ax, num_rows, total_display_cols, config,
                                       pruned_start, pruned_final, gap_cells)
        
        # Add color bar
        self._add_colorbar(fig, ax, value_mapping, config)
        
        # Adjust layout and save
        plt.tight_layout()
        plt.savefig(output_file, dpi=300, bbox_inches='tight', facecolor='white')
        plt.close()
        
        self.log(f"  Saved pruned matrix to: {output_file}", 1)
    
    def _draw_grid(self, ax, cols: int, rows: int):
        """Draw a white grid aligned with the pattern matrix"""
        grid_color = 'white'
        
        # Draw vertical grid lines
        for col in range(cols + 1):
            ax.axvline(x=col, color=grid_color, linewidth=1, zorder=0)
        
        # Draw horizontal grid lines
        for row in range(rows + 1):
            ax.axhline(y=row, color=grid_color, linewidth=1, zorder=0)
    
    def _configure_axes_new(self, ax, num_rows: int, num_cols: int, config: Dict):
        """Configure axes for full matrix view with new label system"""
        ax.set_xlim(0, num_cols)
        ax.set_ylim(0, num_rows)
        ax.set_aspect('equal')
        
        # Set Y labels
        if config['y_labels']:
            y_ticks = []
            y_labels = []
            # Reverse the Y labels mapping: first label goes to last row
            for i, label in enumerate(config['y_labels']):
                if i < num_rows:
                    # Map first label to last row (num_rows - 1 - i)
                    row_idx = num_rows - 1 - i
                    y_ticks.append(row_idx + 0.5)
                    y_labels.append(label)
            
            # Sort by tick position to maintain proper order
            sorted_pairs = sorted(zip(y_ticks, y_labels))
            y_ticks, y_labels = zip(*sorted_pairs) if sorted_pairs else ([], [])
            
            ax.set_yticks(y_ticks)
            ax.set_yticklabels(y_labels, fontsize=10)
        else:
            # Use incremental counter with reverse order
            ax.set_yticks([i + 0.5 for i in range(num_rows)])
            # First row (index 0) gets highest number, last row gets 1
            ax.set_yticklabels([str(num_rows - i) for i in range(num_rows)], fontsize=10)
        
        # Set X labels
        if config['x_labels']:
            x_ticks = []
            x_labels = []
            for i, label in enumerate(config['x_labels']):
                if i < num_cols:
                    x_ticks.append(i + 0.5)
                    x_labels.append(label)
            ax.set_xticks(x_ticks)
            ax.set_xticklabels(x_labels, fontsize=10, rotation=45, ha='right')
        else:
            # Use incremental counter
            if num_cols <= 20:
                step = 1
            elif num_cols <= 50:
                step = 5
            else:
                step = 10
            
            x_ticks = [i + 0.5 for i in range(0, num_cols, step)]
            ax.set_xticks(x_ticks)
            ax.set_xticklabels([str(i+1) for i in range(0, num_cols, step)], fontsize=10)
        
        # Position x-axis at bottom
        ax.xaxis.tick_bottom()
        
        # Remove all spines
        for spine in ax.spines.values():
            spine.set_visible(False)
        
        # Remove tick marks but keep labels
        ax.tick_params(left=False, bottom=False, top=False, right=False)
        
        # Invert y-axis
        ax.invert_yaxis()
    
    def _configure_axes_pruned_new(self, ax, num_rows: int, total_cols: int, config: Dict,
                                  pruned_start: int, pruned_final: int, gap_cells: int):
        """Configure axes for pruned matrix view with new label system"""
        ax.set_xlim(0, total_cols)
        ax.set_ylim(0, num_rows)
        ax.set_aspect('equal')
        
        # Set Y labels
        if config['y_labels']:
            y_ticks = []
            y_labels = []
            # Reverse the Y labels mapping: first label goes to last row
            for i, label in enumerate(config['y_labels']):
                if i < num_rows:
                    # Map first label to last row (num_rows - 1 - i)
                    row_idx = num_rows - 1 - i
                    y_ticks.append(row_idx + 0.5)
                    y_labels.append(label)
            
            # Sort by tick position to maintain proper order
            sorted_pairs = sorted(zip(y_ticks, y_labels))
            y_ticks, y_labels = zip(*sorted_pairs) if sorted_pairs else ([], [])
            
            ax.set_yticks(y_ticks)
            ax.set_yticklabels(y_labels, fontsize=10)
        else:
            # Use incremental counter with reverse order
            ax.set_yticks([i + 0.5 for i in range(num_rows)])
            # First row (index 0) gets highest number, last row gets 1
            ax.set_yticklabels([str(num_rows - i) for i in range(num_rows)], fontsize=10)
        
        # Set X labels for pruned view
        if config['x_labels']:
            x_ticks = []
            x_labels = []
            display_col = 0
            
            # Add labels for prefix
            for i in range(pruned_start):
                if i < len(config['x_labels']):
                    x_ticks.append(display_col + 0.5)
                    x_labels.append(config['x_labels'][i])
                display_col += 1
            
            # Skip gap cells
            display_col += gap_cells
            
            # Add labels for suffix
            for i in range(pruned_final, len(config['x_labels'])):
                x_ticks.append(display_col + 0.5)
                x_labels.append(config['x_labels'][i])
                display_col += 1
            
            ax.set_xticks(x_ticks)
            ax.set_xticklabels(x_labels, fontsize=10, rotation=45, ha='right')
        else:
            # Use incremental counter for pruned view
            x_labels = []
            x_positions = []
            
            # Add labels for prefix
            if pruned_start > 0:
                step = max(1, pruned_start // 5)
                for i in range(0, pruned_start, step):
                    x_labels.append(str(i+1))
                    x_positions.append(i + 0.5)
            
            # Add labels for suffix
            suffix_start_display = pruned_start + gap_cells
            suffix_length = total_cols - suffix_start_display
            
            if suffix_length > 0:
                step = max(1, suffix_length // 5)
                for i in range(pruned_final, pruned_final + suffix_length, step):
                    display_pos = suffix_start_display + (i - pruned_final)
                    if display_pos < total_cols:
                        x_labels.append(str(i+1))
                        x_positions.append(display_pos + 0.5)
            
            if x_positions:
                ax.set_xticks(x_positions)
                ax.set_xticklabels(x_labels, fontsize=10)
        
        ax.set_xlabel(f'Columns {pruned_start+1}-{pruned_final} hidden', fontsize=10)
        
        # Remove all spines
        for spine in ax.spines.values():
            spine.set_visible(False)
        
        ax.tick_params(left=False, bottom=False, top=False, right=False)
        ax.invert_yaxis()
    
    def _add_colorbar(self, fig, ax, value_mapping: Dict, config: Dict):
        """Add a color bar to show the value-color mapping"""
        from matplotlib.colors import LinearSegmentedColormap
        import matplotlib.colorbar as colorbar
        
        # Create colormap
        gradient = value_mapping['gradient']
        n_colors = len(gradient)
        colors_rgb = [self.hex_to_rgb(c) for c in gradient]
        
        cmap = LinearSegmentedColormap.from_list('custom', colors_rgb, N=n_colors)
        
        # Create a ScalarMappable for the colorbar
        norm = plt.Normalize(vmin=value_mapping['min'], vmax=value_mapping['max'])
        sm = plt.cm.ScalarMappable(cmap=cmap, norm=norm)
        sm.set_array([])
        
        # Add colorbar
        cbar = fig.colorbar(sm, ax=ax, orientation='vertical', pad=0.02, fraction=0.05)
        
        # Set label based on color type
        if config['color_type'] == 'probability':
            cbar.set_label('Probability', rotation=270, labelpad=15)
        else:
            cbar.set_label('Value', rotation=270, labelpad=15)
    
    def run(self, pattern_file: Path, pruned: bool = False,
            pruned_start: Optional[int] = None, pruned_final: Optional[int] = None):
        """Run the visualization generation"""
        # Create output directory if it doesn't exist
        self.output_dir.mkdir(parents=True, exist_ok=True)
        
        self.log(f"Processing pattern file: {pattern_file}", 1)
        
        # Parse patterns
        data_matrix, config = self.parse_pattern_file(pattern_file)
        
        if not data_matrix:
            self.log("Error: No valid data found", 0)
            return
        
        self.log(f"Loaded {len(data_matrix)} rows of data", 1)
        
        # Generate visualization
        base_name = pattern_file.stem
        
        if pruned:
            if pruned_start is None:
                pruned_start = 0
            if pruned_final is None:
                pruned_final = max(len(row) for row in data_matrix)
            
            output_file = self.output_dir / f"{base_name}_matrix_pruned.png"
            self.create_pruned_matrix(data_matrix, config, output_file, pruned_start, pruned_final)
        else:
            output_file = self.output_dir / f"{base_name}_matrix.png"
            self.create_full_matrix(data_matrix, config, output_file)
        
        # Report summary
        num_rows = len(data_matrix)
        num_cols = max(len(row) for row in data_matrix) if data_matrix else 0
        
        print(f"\nPattern Analysis:")
        print(f"  Total rows: {num_rows}")
        print(f"  Total columns: {num_cols}")
        print(f"  Color type: {config['color_type']}")
        
        if pruned:
            hidden_cols = pruned_final - pruned_start
            print(f"  Hidden columns: {hidden_cols} (from {pruned_start} to {pruned_final-1})")
            print(f"  Shown columns: {num_cols - hidden_cols}")
        
        self.log("\nVisualization generation complete!", 1)


def main():
    parser = argparse.ArgumentParser(
        description='Generate pattern matrix visualizations with custom color gradients',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  # Full matrix visualization
  %(prog)s -p patterns.txt
  
  # Pruned matrix hiding columns 10-50
  %(prog)s -p patterns.txt --pruned --pruned-start 10 --pruned-final 50
  
  # Short form
  %(prog)s -p patterns.txt -pr -prs 15 -prf 45
  
  # With custom directories and verbose output
  %(prog)s -p patterns.txt -pr -prs 10 -prf 40 -i ./data -o ./output -v 1

Pattern file format:
  Color_type: Integer  # or Probability
  Min_color: '#431407'  # or auto
  Max_color: '#fed7aa'  # or auto
  Min_number_of_colors: 10
  Gradient: orange  # or custom gradient list
  Y: 1 2 3 4 5 6 7 8
  X: MOV LD1 LD2 ST1 ST2 P1
  7 1 1 1 1 1 1  
  6 1 1 1 1 1 2  
  5 1 1 1 1 1 3  
  
Configuration options:
  - Color_type: 'Integer' or 'Probability'
    * Integer: Values range from min to max in data
    * Probability: Values always range from 0 to 1
  
  - Gradient: Three ways to specify colors:
    1. Predefined gradient name: 'blue', 'blue_dark', 'green', 'red', 'orange', 'purple', 'gray'
       Example: Gradient: orange
    2. Custom gradient list: List of hex colors
       Example: Gradient: ['#ffffff', '#fed7aa', '#fb923c', '#ea580c', '#431407']
    3. Min/Max colors: Define endpoints for auto-generated gradient
       Min_color: '#431407' or 'auto'
       Max_color: '#fed7aa' or 'auto'
    
    Note: Gradient setting overrides Min_color/Max_color if provided
  
  - Min_number_of_colors: Minimum steps in gradient
    * For Integer mode: Uses max(unique_values, min_number)
    * For Probability mode: Always uses this value
    * If custom gradient has fewer colors, it will be interpolated
  
  - X/Y labels: Custom labels for axes
    * If not provided, uses incremental counters

Predefined gradients:
  - blue: Light to dark blue gradient
  - blue_dark: White to very dark blue
  - green: Light to dark green gradient
  - red: Light to dark red gradient
  - orange: Light to dark orange gradient (default)
  - purple: Light to dark purple gradient
  - gray: Light to dark gray gradient
  - thermal: Viridis colormap (black to purple to yellow)
  - inferno: Inferno colormap (dark to bright yellow)
  - magma: Magma colormap (dark to bright yellow/white)
  - plasma: Plasma colormap (dark purple to bright yellow)
  - rocket: Rocket colormap (dark blue to bright orange)

Data format:
  - Each line is a row of numeric values
  - Optional color name prefix (ignored if present)
  - Comments start with # and are ignored

Output files:
  <pattern_name>_matrix.png        - Full matrix visualization
  <pattern_name>_matrix_pruned.png - Pruned matrix visualization
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
                       help='Start index of section to hide')
    parser.add_argument('-prf', '--pruned-final', type=int,
                       help='Final index of section to hide')
    
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
            print(f"  Pruning mode: Manual (hiding columns {args.pruned_start}-{args.pruned_final-1})")
        else:
            print(f"  Pruning mode: Auto")
    else:
        print(f"  Visualization mode: Full matrix")
    print("\nDone!")


if __name__ == '__main__':
    main()