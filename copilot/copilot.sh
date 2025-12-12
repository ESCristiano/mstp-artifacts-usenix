#!/usr/bin/env bash
ROOT="$(realpath .)"
TFM="${ROOT}/../s-world/trusted-firmware-m"
NSPE_RUNTIME=${ROOT}/../s-world/tf-m-tests
# NSPE_APP=${ROOT}/tf-m-tests/tests_reg
NSPE_APP=${ROOT}/../ns-world-rtos
NSPE_MSTP_APP=${ROOT}/../ns-world-bare
MSTP=${ROOT}/../m-step

# Default values
TARGET="STM32L5"
PROFILE="bare"

#-------------------------------------------------------------------------------
# Parse arguments
#-------------------------------------------------------------------------------
set -e

DEPLOY=false  # default

while [[ $# -gt 0 ]]; do
    case "$1" in
        -b|--build)
            BUILD_TYPE="$2"
            if [[   "$BUILD_TYPE" != "s"        && 
                    "$BUILD_TYPE" != "ns"       && 
                    "$BUILD_TYPE" != "ns_costum" ]]; then
                echo "Error: Invalid build type '$BUILD_TYPE'. Supported values are 's', 'ns', or 'ns_costum'."
                exit 1
            fi
            shift 2
            ;;
        -c|--config)
            CONFIG_TYPE="$2"
            if [[ "$CONFIG_TYPE" != "s" && "$CONFIG_TYPE" != "ns" ]]; then
                echo "Error: Invalid config type '$CONFIG_TYPE'. Supported values are 's' or 'ns'."
                exit 1
            fi
            shift 2
            ;;
        -p|--profile)
            PROFILE="$2"
            if [[   "$PROFILE" != "bare"    && 
                    "$PROFILE" != "crypto"  &&
                    "$PROFILE" != "mstp" 
                ]]; then
                echo "Error: Invalid profile '$PROFILE'. Supported values are 'bare', 'crypto' or mstp."
                exit 1
            fi
            shift 2
            ;;
        -t|--target)
            TARGET="$2"
            shift 2
            ;;
        -d|--deploy)
            DEPLOY=true
            shift
            ;;
        *)
            echo "Unknown argument: $1"
            echo "Usage: $0 [-b|--build <s|ns|ns_costum>] [-c|--config <s|ns>] [-t|--target <BoardName>] [-d|--deploy] [-p|--profile <bare|crypto|mstp>]"
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

CONFIG="build/${PLATFORM}_${PROFILE}_${CONFIG_TYPE}"

#-------------------------------------------------------------------------------
# Configure SPE build system 
#-------------------------------------------------------------------------------
if [[ "${CONFIG_TYPE}" == "s" ]]; then

        # Out of tree Custom Secure Services Build
        APPROT_TA=${MSTP}/mstp-victims/TAs/appRoT
        PROT_CRYPTO_ATTACK_TA=${MSTP}/mstp-victims/TAs/PRoT_crypto_attack

        PARTITION_PATHS="${APPROT_TA};${PROT_CRYPTO_ATTACK_TA}"
        MANIFEST_LIST_FILE="${APPROT_TA}/appRoT_manifest_list.yaml;${PROT_CRYPTO_ATTACK_TA}/PRoT_crypto_attack_manifest_list.yaml"

        cmake   -S ${TFM}                                               \
                -B ${CONFIG}                                            \
                -DTFM_EXTRA_PARTITION_PATHS=${PARTITION_PATHS}          \
                -DTFM_EXTRA_MANIFEST_LIST_FILES=${MANIFEST_LIST_FILE}   \
                -C tfm-configs/${PLATFORM}-${PROFILE}-config.cmake
fi

#-------------------------------------------------------------------------------
# Configure NSPE build system
#-------------------------------------------------------------------------------
if [[ "${CONFIG_TYPE}" == "ns" ]]; then

        CONFIG_S="build/${PLATFORM}_${PROFILE}_s"
        TFM_SPE="$(realpath ${CONFIG_S})"

        if [[ ! -d ${TFM_SPE}/api_ns/ ]]; then
                echo "Error: The $TFM_SPE/api_ns/ does not exist. Please build the secure image first."
                exit 1
        fi

        cmake   -S ${TFM} ${NSPE_APP}                                          \
        -B ${CONFIG}                                                           \
        -DCMAKE_BUILD_TYPE=Debug                                               \
        -DNSPE_RUNTIME_PATH=${NSPE_RUNTIME}                                    \
        -DCONFIG_SPE_PATH=${TFM_SPE}/api_ns/                                   \
        -DTFM_TOOLCHAIN_FILE=${TFM_SPE}/api_ns/cmake/toolchain_ns_GNUARM.cmake
fi

BUILD="build/${PLATFORM}_${PROFILE}_${BUILD_TYPE}"

#-------------------------------------------------------------------------------
# Build SPE build system 
#-------------------------------------------------------------------------------
if [[ "${BUILD_TYPE}" == "s" ]]; then
        cmake --build ${BUILD} -- install
fi

#-------------------------------------------------------------------------------
# Build NSPE build system
#-------------------------------------------------------------------------------
if [[ "${BUILD_TYPE}" == "ns" ]]; then
        BUILD_S="build/${PLATFORM}_${PROFILE}_s"
            
        cmake --build ${BUILD}
        cp ${BUILD}/bin/tfm_ns_signed.bin ${BUILD_S}/api_ns/image_signing/scripts
fi

#-------------------------------------------------------------------------------
# Build Costum NS
#-------------------------------------------------------------------------------
if [[ "${BUILD_TYPE}" == "ns_costum" ]]; then
        BUILD_S="build/${PLATFORM}_${PROFILE}_s"
            
        cmake --build ${NSPE_MSTP_APP}/build

        arm-none-eabi-objcopy -O binary \
        ${NSPE_MSTP_APP}/src/NonSecure/build/tfm_ns.elf \
        ${NSPE_MSTP_APP}/src/NonSecure/build/tfm_ns.bin

        imgtool sign \
          -k ${BUILD_S}/api_ns/image_signing/keys/image_ns_signing_private_key.pem \
          --public-key-format full \
          --align 1 \
          -v 0.0.0 \
          -s 1 \
          -H 1024 \
          --pad-header \
          -S 0x10000 \
          --pad \
          --boot-record boot \
          ${NSPE_MSTP_APP}/src/NonSecure/build/tfm_ns.bin \
          ${BUILD_S}/api_ns/image_signing/scripts/tfm_ns_signed.bin
fi

#-------------------------------------------------------------------------------
# Flash TFM
#-------------------------------------------------------------------------------
if $DEPLOY; then        
        BUILD_S="build/${PLATFORM}_${PROFILE}_s"
        TFM_SPE="$(realpath ${BUILD_S})"

        echo "Deploying for target ${TARGET} with profile ${TFM_SPE}"
        
        ${TFM_SPE}/api_ns/postbuild.sh
        # Just need once to setup option bytes. If it's first time, please 
        # uncomment the line below and run it once.
        # ${TFM_SPE}/api_ns/regression.sh
        ${TFM_SPE}/api_ns/TFM_UPDATE.sh      
        # ${TFM_SPE}/api_ns/TFM_UPDATE_BL2_ONLY.sh
fi
