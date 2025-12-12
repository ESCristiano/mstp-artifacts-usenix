#!/usr/bin/env bash
ROOT="$(realpath .)"

COPILOT="${ROOT}/../copilot/copilot.sh"

# Static Configuration Values
TARGET="STM32L5"
BUILD_TYPE="ns"
PROFILE="crypto"

#-------------------------------------------------------------------------------
# Parse arguments
#-------------------------------------------------------------------------------
set -e

BUILD_ALL=true

while [[ $# -gt 0 ]]; do
    case "$1" in
        -b|--build)
            BUILD_ALL=false
            WORLD="$2"
            if [[   "$WORLD" != "s"        && 
                    "$WORLD" != "ns"  ]]; then
                echo "Error: Invalid build type '$WORLD'. Supported values are 's', 'ns'."
                exit 1
            fi
            shift 2
            ;;
        -h|--help)
            echo "Usage: $0 [-b|--build <s|ns>]"
            exit 0
            ;;
        *)
            echo "Unknown argument: $1"
            echo "Usage: $0 [-b|--build <s|ns>]"
            exit 1
            ;;
    esac
done

#-------------------------------------------------------------------------------
# Build S and NS 
#-------------------------------------------------------------------------------
if [[ "$BUILD_ALL" == "true" ]]; then
    echo "Building for target $TARGET with profile $PROFILE"

    ORIGINAL_DIR="$(pwd)"
    cd "$(dirname "${COPILOT}")"
    # Build S
    ${COPILOT} -b s -t ${TARGET} -p ${PROFILE}
    # Build NS
    ${COPILOT} -b ${BUILD_TYPE} -t ${TARGET} -p ${PROFILE}
    cd "${ORIGINAL_DIR}"
fi

#-------------------------------------------------------------------------------
# Build only one image at the time
#-------------------------------------------------------------------------------
if [[ "$BUILD_ALL" == "false" ]]; then
    echo "Building for target $TARGET with profile $PROFILE" for $BUILD_TYPE world
    
    if [[ "$WORLD" == "s" ]]; then
        # Build S
        ORIGINAL_DIR="$(pwd)"
        cd "$(dirname "${COPILOT}")"
        ${COPILOT} -b s -t ${TARGET} -p ${PROFILE}
        cd "${ORIGINAL_DIR}"
    elif [[ "$WORLD" == "ns" ]]; then
        # Build NS
        ORIGINAL_DIR="$(pwd)"
        cd "$(dirname "${COPILOT}")"
        ${COPILOT} -b ${BUILD_TYPE} -t ${TARGET} -p ${PROFILE}
        cd "${ORIGINAL_DIR}"
    fi
fi