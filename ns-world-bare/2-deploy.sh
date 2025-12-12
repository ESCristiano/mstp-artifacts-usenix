#!/usr/bin/env bash
ROOT="$(realpath .)"

COPILOT="${ROOT}/../copilot/copilot.sh"

# Static Configuration Values
TARGET="STM32L5"
PROFILE="mstp"

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

#-------------------------------------------------------------------------------
# Deploy
#-------------------------------------------------------------------------------
echo "Deploying for target $TARGET with profile $PROFILE"

ORIGINAL_DIR="$(pwd)"
cd "$(dirname "${COPILOT}")"
# Path passed explicitly for nix+Ubuntu compatibility (https://unix.stackexchange.com/a/83194)
sudo env "PATH=$PATH" ${COPILOT} -d -t ${TARGET} -p ${PROFILE}
cd "${ORIGINAL_DIR}"
