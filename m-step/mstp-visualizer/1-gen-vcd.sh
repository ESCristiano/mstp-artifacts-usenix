#!/usr/bin/env bash
ROOT="$(realpath .)"

PY_SCRIPTS=${ROOT}/scripts
INPUTS=${ROOT}/../traces/tfm-inv-mod-001
OUTPUTS=${ROOT}/outputs
TRACE=${INPUTS}/trace.txt  # Default trace file
BIN=${INPUTS}/tfm_s.elf

#-------------------------------------------------------------------------------
# Parse arguments
#-------------------------------------------------------------------------------
set -e

while [[ $# -gt 0 ]]; do
    case "$1" in
        -t|--trace)
            if [[ -z "$2" ]]; then
                echo "Error: --trace requires a file path"
                exit 1
            fi
            TRACE="$2"
            shift 2
            ;;
        -h|--help)
            echo "Usage: $0 [-t|--trace TRACE_FILE]"
            echo ""
            echo "Options:"
            echo "  -t, --trace TRACE_FILE    Specify trace file (default: ${INPUTS}/trace.txt)"
            echo "  -h, --help               Show this help message"
            echo ""
            echo "Examples:"
            echo "  $0                                    # Use default trace file"
            echo "  $0 -t /path/to/my_trace.txt         # Use custom trace file"
            echo "  $0 --trace ./custom_trace.txt       # Use custom trace file"
            exit 0
            ;;
        *)
            echo "Unknown argument: $1"
            echo "Usage: $0 [-t|--trace TRACE_FILE]"
            echo "Use -h or --help for more information"
            exit 1
            ;;
    esac
done

# Validate trace file exists
if [[ ! -f "$TRACE" ]]; then
    echo "Error: Trace file '$TRACE' not found"
    exit 1
fi

echo "Using trace file: $TRACE"
echo "Using binary file: $BIN"
echo "Output directory: $OUTPUTS"
echo ""

python3 ${PY_SCRIPTS}/gen-vcd.py    \
        "${TRACE}"                   \
        "${BIN}"                     \
        --output-dir "${OUTPUTS}"    \
        --vcd-name trace.vcd