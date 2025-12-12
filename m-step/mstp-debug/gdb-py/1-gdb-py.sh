#!/usr/bin/env bash
ROOT="$(realpath .)"

SETUP=""
SETUP_DIR=${ROOT}/setups

#-------------------------------------------------------------------------------
# Parse arguments
#-------------------------------------------------------------------------------
set -e

while [[ $# -gt 0 ]]; do
    case "$1" in
        -s|--setup)
            SETUP="$2"
            shift 2
            ;;
        -h|--help)
            echo "Usage: $0 [-s|--setup <setup_name>]"
            exit 0
            ;;
        *)
            echo "Unknown argument: $1"
            echo "Usage: $0 [-s|--setup <setup_name>]"
            exit 1
            ;;
    esac
done

# Check if setup parameter is provided
if [ -z "$SETUP" ]; then
    echo "Usage: $0 [-s|--setup <setup_name>]"
    exit 1
fi

# Check if the gdbinit file exists
if [ -f "${SETUP_DIR}/${SETUP}.gdbinit" ]; then
    arm-none-eabi-gdb-py -x ${SETUP_DIR}/${SETUP}.gdbinit
else
    echo "Error: ${SETUP} does not exist"
    echo ""
    echo "Available setups:"
    for file in "${SETUP_DIR}"/*.gdbinit; do
        if [ -f "$file" ]; then
            basename=$(basename "$file" .gdbinit)
            echo "  - $basename"
        fi
    done
    exit 1
fi
