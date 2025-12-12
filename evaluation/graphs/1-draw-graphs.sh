#!/usr/bin/env bash
ROOT="$(realpath .)"

PY_SCRIPTS=${ROOT}/scripts
OUTPUTS=${ROOT}

#-------------------------------------------------------------------------------
# Parse arguments
#-------------------------------------------------------------------------------
set -e

while [[ $# -gt 0 ]]; do
    case "$1" in
        -o|--output)
            if [[ -z "$2" ]]; then
                echo "Error: --output requires a dir path"
                exit 1
            fi
            OUTPUTS="$2"
            shift 2
            ;;
        -h|--help)
            echo "Usage: $0 [-o|--output OUTPUTS_DIR]"
            echo ""
            echo "Options:"
            echo "  -o, --output OUTPUTS_DIR Specify output dir (default: ${ROOT})"
            echo "  -h, --help               Show this help message"
            echo ""
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

echo "Output directory: $OUTPUTS"
echo ""


PATTERN=${ROOT}/patterns/06-div.txt

python3 ${PY_SCRIPTS}/gen_matrix.py  \
        -p "${PATTERN}"        \
        -o "${OUTPUTS}"    

PATTERN=${ROOT}/patterns/06-inst_diff.txt

python3 ${PY_SCRIPTS}/gen_matrix.py  \
        -p "${PATTERN}"        \
        -o "${OUTPUTS}"    

PATTERN=${ROOT}/patterns/06-cache.txt

python3 ${PY_SCRIPTS}/gen_matrix.py  \
        -p "${PATTERN}"        \
        -o "${OUTPUTS}"    

PATTERN=${ROOT}/patterns/06-contention.txt

python3 ${PY_SCRIPTS}/gen_matrix.py  \
        -p "${PATTERN}"        \
        -o "${OUTPUTS}"    

PATTERN=${ROOT}/patterns/06-sub_template.txt

python3 ${PY_SCRIPTS}/gen_template_matrix.py  \
        -p "${PATTERN}"        \
        -o "${OUTPUTS}"    

PATTERN=${ROOT}/patterns/06-shift_template.txt

python3 ${PY_SCRIPTS}/gen_template_matrix.py    \
        -p "${PATTERN}"                         \
        -o "${OUTPUTS}/"

python3 ${PY_SCRIPTS}/gen_template_matrix.py    \
        -p "${PATTERN}"                         \
        -o "${OUTPUTS}"                         \
        --pruned                                \
        --pruned-start 10                       \
        --pruned-final 138                 
