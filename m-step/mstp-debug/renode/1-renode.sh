#!/usr/bin/env bash
ROOT="$(realpath .)"

BOARD=l552
SETUP=""
SETUP_DIR=${ROOT}/setups
RENODE_BIN=${ROOT}/src/renode/renode

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
if [ -f "${SETUP_DIR}/${BOARD}_${SETUP}.resc" ]; then
    # Run renode with the specified resource script
    ${RENODE_BIN} ${SETUP_DIR}/${BOARD}_${SETUP}.resc
else
    echo "Error: ${SETUP} does not exist"
    echo ""
    echo "Available setups:"
    for file in "${SETUP_DIR}"/*.resc; do
        if [ -f "$file" ]; then
            basename=$(basename "$file" .resc)
            # Remove ${BOARD}_ prefix from the basename
            basename=${basename#${BOARD}_}
            echo "  - $basename"
        fi
    done
    exit 1
fi


