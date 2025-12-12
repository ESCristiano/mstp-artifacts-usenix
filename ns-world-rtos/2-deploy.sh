#!/usr/bin/env bash
ROOT="$(realpath .)"

COPILOT="${ROOT}/../copilot/copilot.sh"

# Static Configuration Values
TARGET="STM32L5"
PROFILE="crypto"

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
sudo ${COPILOT} -d -t ${TARGET} -p ${PROFILE}
cd "${ORIGINAL_DIR}"


