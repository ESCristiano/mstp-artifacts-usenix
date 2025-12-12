# Pattern Matrix Visualization Generator

Quick reference for generating color-coded matrix visualizations from data patterns.

## Usage

```bash
python gen_matrix.py -p <pattern_file> [options]
```

### Command Line Options

| Option | Description | Default |
|--------|-------------|---------|
| `-p, --patterns` | Pattern file path (required) | - |
| `-i, --input-dir` | Input directory | `.` |
| `-o, --output-dir` | Output directory | `.` |
| `-v, --verbosity` | Verbosity (0=quiet, 1=verbose) | `0` |
| `-pr, --pruned` | Create pruned visualization | `False` |
| `-prs, --pruned-start` | Start column to hide | - |
| `-prf, --pruned-final` | End column to hide | - |

## Pattern File Format

### Configuration Headers

```
Color_type: Integer          # Integer or Probability
Gradient: orange             # Predefined or custom (overrides Min/Max)
Min_color: #431407          # Hex or 'auto' (ignored if Gradient set)
Max_color: #fed7aa          # Hex or 'auto' (ignored if Gradient set)
Min_number_of_colors: 10    # Minimum gradient steps
Y: Row1 Row2 Row3           # Row labels (optional)
X: Col1 Col2 Col3           # Column labels (optional)
```

### Data Section
```
7 1 1 1 1 1    # Numeric values per row
6 1 1 1 1 2    # Optional color prefix (ignored)
5 1 1 1 1 3    # Comments with # are ignored
```

## Color Configuration

### Method 1: Predefined Gradients
```
Gradient: blue    # Options: blue, blue_dark, green, red, orange, purple, gray
```

### Method 2: Custom Gradient List
```
Gradient: ['#ffffff', '#fed7aa', '#fb923c', '#ea580c', '#431407']
```

### Method 3: Min/Max Colors
```
Min_color: #ffffff
Max_color: #431407
```

## Color Types

### Integer Mode
- Maps actual data range (min to max in data)
- Colors = max(unique_values, Min_number_of_colors)

### Probability Mode
- Always maps 0.0 to 1.0 range
- Always uses Min_number_of_colors

## Examples

### Basic Integer Matrix
```
Color_type: Integer
Gradient: blue
Min_number_of_colors: 10
Y: A B C D
X: T1 T2 T3 T4
1 2 3 4
2 3 4 5
3 4 5 6
4 5 6 7
```

### Probability Matrix with Custom Gradient
```
Color_type: Probability
Gradient: ['#ffffff', '#ffcccc', '#ff9999', '#ff6666', '#ff0000']
Min_number_of_colors: 20
Y: P1 P2 P3
X: Time1 Time2 Time3 Time4
0.1 0.3 0.5 0.7
0.2 0.4 0.6 0.8
0.15 0.35 0.55 0.75
```

### Auto Colors with Labels
```
Color_type: Integer
Min_color: auto
Max_color: auto
Min_number_of_colors: 15
Y: 1 2 3 4 5
X: MOV LD1 LD2 ST1 ST2
7 1 1 1 1
6 1 1 1 2
5 1 1 3 3
4 1 2 4 1
3 2 3 1 2
```

### Pruned Visualization
```bash
# Hide columns 10-50
python gen_matrix.py -p data.txt --pruned --pruned-start 10 --pruned-final 50

# Short form
python gen_matrix.py -p data.txt -pr -prs 10 -prf 50
```

## Output

- `<filename>_matrix.png` - Full matrix visualization
- `<filename>_matrix_pruned.png` - Pruned matrix (if --pruned used)

## Tips

- Use **Integer** mode for discrete values
- Use **Probability** mode for normalized [0,1] data
- Gradient interpolation auto-adjusts to Min_number_of_colors
- Pruning shows ellipsis (...) for hidden columns
- Comments (#) anywhere in pattern file are ignored