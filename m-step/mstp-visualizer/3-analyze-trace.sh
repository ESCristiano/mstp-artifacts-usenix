#!/usr/bin/env bash
ROOT="$(realpath .)"

PY_SCRIPTS=${ROOT}/scripts
INPUTS=${ROOT}/../traces/tfm-inv-mod-001
OUTPUTS=${ROOT}/outputs
PATTERNS=${ROOT}/patterns
TRACE=${INPUTS}/trace.txt  # Default trace file
TRACE_NUM=${OUTPUTS}/trace_num.txt  
VERBOSITY=0  # Default verbosity level

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
        -v|--verbosity)
            if [[ -z "$2" ]] || ! [[ "$2" =~ ^[01]$ ]]; then
                echo "Error: --verbosity requires a value of 0 or 1"
                exit 1
            fi
            VERBOSITY="$2"
            shift 2
            ;;
        -h|--help)
            echo "Usage: $0 [-t|--trace TRACE_FILE] [-v|--verbosity LEVEL]"
            echo ""
            echo "Options:"
            echo "  -t, --trace TRACE_FILE    Specify trace file (default: ${INPUTS}/trace.txt)"
            echo "  -v, --verbosity LEVEL     Set verbosity level (0 or 1, default: 0)"
            echo "                           0: Show only final output"
            echo "                           1: Show all script outputs"
            echo "  -h, --help               Show this help message"
            echo ""
            echo "Examples:"
            echo "  $0                                    # Use defaults (verbosity 0)"
            echo "  $0 -v 1                              # Show all outputs"
            echo "  $0 -t /path/to/my_trace.txt -v 1     # Custom trace with full output"
            echo "  $0 --trace ./custom_trace.txt --verbosity 0  # Custom trace, quiet mode"
            exit 0
            ;;
        *)
            echo "Unknown argument: $1"
            echo "Usage: $0 [-t|--trace TRACE_FILE] [-v|--verbosity LEVEL]"
            echo "Use -h or --help for more information"
            exit 1
            ;;
    esac
done

# Validate trace file exists
if [[ ! -f "$TRACE" ]]; then
    echo "Error: Trace '$TRACE' not found"
    exit 1
fi

echo "Processing trace file: $TRACE"

#-------------------------------------------------------------------------------
# Add "time" info to the trace to be able to later be able to process the gathered
# patterns in the sequence the underlying operations they represent were executed.
#-------------------------------------------------------------------------------
if [[ $VERBOSITY -eq 1 ]]; then
    echo "Step 1: Adding time info to trace..."
fi

if [[ $VERBOSITY -eq 0 ]]; then
    python3 ${PY_SCRIPTS}/numerate-trace.py \
            --trace ${TRACE}                \
            --output ${TRACE_NUM} >/dev/null 2>&1
else
    python3 ${PY_SCRIPTS}/numerate-trace.py \
            --trace ${TRACE}                \
            --output ${TRACE_NUM}
fi

#-------------------------------------------------------------------------------
# Get the shifts u and shift v patterns
#-------------------------------------------------------------------------------

# Check if patterns-list.txt exists and remove it if it does
if [[ -f "${OUTPUTS}/patterns-list.txt" ]]; then
    rm "${OUTPUTS}/patterns-list.txt"
fi

if [[ $VERBOSITY -eq 1 ]]; then
    echo "Step 2: Getting shifts u and shift v patterns..."
fi

if [[ $VERBOSITY -eq 0 ]]; then
    python3 ${PY_SCRIPTS}/find-patterns.py                      \
            --trace ${TRACE_NUM}                                \
            --pattern ${PATTERNS}/patterns-shifts-inv-mod.txt   \
            --output ${OUTPUTS}                                 \
            --pattern-list                                      \
            --verbosity 0 >/dev/null 2>&1
else
    python3 ${PY_SCRIPTS}/find-patterns.py                      \
            --trace ${TRACE_NUM}                                \
            --pattern ${PATTERNS}/patterns-shifts-inv-mod.txt   \
            --output ${OUTPUTS}                                 \
            --pattern-list                                      \
            --verbosity 0   
fi

#-------------------------------------------------------------------------------
# Get the sub u and sub v patterns
#-------------------------------------------------------------------------------
if [[ $VERBOSITY -eq 1 ]]; then
    echo "Step 3: Searching for mbedtls_mpi_cmp_mpi function..."
fi

## Search for the mbedtls_mpi_cmp_mpi function
if [[ $VERBOSITY -eq 0 ]]; then
    python3 ${PY_SCRIPTS}/find-patterns.py                              \
            --trace ${TRACE_NUM}                                        \
            --pattern-start ${PATTERNS}/pattern-cmp-inv-mod-start.txt   \
            --pattern-end ${PATTERNS}/pattern-cmp-inv-mod-end.txt       \
            --output ${OUTPUTS}                                         \
            --verbosity 0 >/dev/null 2>&1
else
    python3 ${PY_SCRIPTS}/find-patterns.py                              \
            --trace ${TRACE_NUM}                                        \
            --pattern-start ${PATTERNS}/pattern-cmp-inv-mod-start.txt   \
            --pattern-end ${PATTERNS}/pattern-cmp-inv-mod-end.txt       \
            --output ${OUTPUTS}                                         \
            --verbosity 0
fi

if [[ $VERBOSITY -eq 1 ]]; then
    echo "Step 4: Getting sub u and sub v patterns..."
fi

## Get the sub u and sub v patterns 
if [[ $VERBOSITY -eq 0 ]]; then
    python3 ${PY_SCRIPTS}/find-patterns.py                                  \
            --trace ${OUTPUTS}/pattern-match-pattern-cmp-inv-mod-start.txt  \
            --pattern ${PATTERNS}/patterns-cmp-inv-mod.txt                  \
            --output ${OUTPUTS}                                             \
            --pattern-list                                                  \
            --verbosity 0 >/dev/null 2>&1
else
    python3 ${PY_SCRIPTS}/find-patterns.py                                  \
            --trace ${OUTPUTS}/pattern-match-pattern-cmp-inv-mod-start.txt  \
            --pattern ${PATTERNS}/patterns-cmp-inv-mod.txt                  \
            --output ${OUTPUTS}                                             \
            --pattern-list                                                  \
            --verbosity 0
fi

#-------------------------------------------------------------------------------
# Get the shifts u, shift v and subs vectors 
#-------------------------------------------------------------------------------
if [[ $VERBOSITY -eq 1 ]]; then
    echo "Step 5: Getting shifts u, shift v and subs vectors..."
fi

if [[ $VERBOSITY -eq 0 ]]; then
    python3 ${PY_SCRIPTS}/get-shifts-subs.py        \
            --pattern ${OUTPUTS}/patterns-list.txt \
            --input ${OUTPUTS}                      \
            --output ${OUTPUTS} >/dev/null 2>&1
else
    python3 ${PY_SCRIPTS}/get-shifts-subs.py        \
            --pattern ${OUTPUTS}/patterns-list.txt \
            --input ${OUTPUTS}                      \
            --output ${OUTPUTS}
fi

#-------------------------------------------------------------------------------
# Recover the private key
#-------------------------------------------------------------------------------
if [[ $VERBOSITY -eq 1 ]]; then
    echo "Step 6: Recovering private key..."
fi

echo ""
echo "Recovered private key prime numbers:"
echo ""

# The last script always shows output regardless of verbosity level
python3 ${PY_SCRIPTS}/recover-keys-inv-mod.py \
        --patterns ${OUTPUTS}/patterns-list-results.txt  \
        --keys ${PATTERNS}/saved_keys.txt