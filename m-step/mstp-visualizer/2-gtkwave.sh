#!/usr/bin/env bash
ROOT="$(realpath .)"

OUTPUTS=${ROOT}/outputs
GTKW_CONFS=${ROOT}/gtkw_confs

#-------------------------------------------------------------------------------
# Parse arguments
#-------------------------------------------------------------------------------
set -e

while [[ $# -gt 0 ]]; do
    case "$1" in
        -h|--help)
            echo "Usage: $0 "
            exit 0
            ;;
        *)
            echo "Unknown argument: $1"
            echo "Usage: $0 "
            exit 1
            ;;
    esac
done

gtkwave ${OUTPUTS}/trace.vcd \
        ${GTKW_CONFS}/trace_visualization.gtkw \
        --cpu 16