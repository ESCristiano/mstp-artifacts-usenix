#!/usr/bin/env bash
ROOT="$(realpath .)"

COPILOT="${ROOT}/../copilot/copilot.sh"
COPILOT_DIR="$(dirname "${COPILOT}")"

# Static Configuration Values
TARGET="STM32L5"
CONFIG=true
BUILD_TYPE="ns_costum"
PROFILE="mstp"
CLEAN=false

#-------------------------------------------------------------------------------
# Parse arguments
#-------------------------------------------------------------------------------
set -e

while [[ $# -gt 0 ]]; do
    case "$1" in
        -c|--clean)
            CLEAN=true
            shift 
            ;;
        -h|--help)
            echo "Usage: $0 [-c|--clean]"
            exit 0
            ;;
        *)
            echo "Unknown argument: $1"
            echo "Usage: $0 [-c|--clean]"
            exit 1
            ;;
    esac
done

#-------------------------------------------------------------------------------
# Decode target 
#-------------------------------------------------------------------------------
if [[ "${TARGET}" == *"L5"* ]]; then
        PLATFORM="nucleo_l552ze_q"
elif [[ "${TARGET}" == *"U5"* ]]; then
        PLATFORM="b_u585i_iot02a"
else
        echo "Unknown target: ${TARGET}"
        exit 1
fi

#-------------------------------------------------------------------------------
# Clean Build Directories
#-------------------------------------------------------------------------------
if [[ "$CLEAN" == "true" ]]; then
    echo "Cleaning NS and TF-M building directories."
    
    sudo rm -rf ${ROOT}/build  \
                ${ROOT}/src/NonSecure/build/ \
                ${ROOT}/src/Secure/build/ \
                ${COPILOT_DIR}/build/${PLATFORM}_${PROFILE}_s

else
#-------------------------------------------------------------------------------
# Config and build S and NS
#-------------------------------------------------------------------------------
    ORIGINAL_DIR="$(pwd)"
    cd "$(dirname "${COPILOT}")"
    #S Config and Build
    ${COPILOT} -c s -b s -t ${TARGET} -p ${PROFILE}
    cd "${ORIGINAL_DIR}"

    # NS
    cmake -G "Eclipse CDT4 - Unix Makefiles" ./src -B ./build/

    # NS Build
    ORIGINAL_DIR="$(pwd)"
    cd "$(dirname "${COPILOT}")"
    ${COPILOT} -b ${BUILD_TYPE} -t ${TARGET} -p ${PROFILE}
    cd "${ORIGINAL_DIR}"
fi