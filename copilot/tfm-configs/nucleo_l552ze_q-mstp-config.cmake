
############################ Base CONFIGURATION ################################
# Base path: directory of this config file
set(CONFIG_BASE_PATH "${CMAKE_CURRENT_LIST_DIR}/../../s-world" CACHE INTERNAL "Base path for relative resolution")

set(CMAKE_BUILD_TYPE Debug CACHE STRING "")
set(TFM_PLATFORM stm/nucleo_l552ze_q CACHE STRING "")

set(TFM_TOOLCHAIN_FILE "${CONFIG_BASE_PATH}/trusted-firmware-m/toolchain_GNUARM.cmake" CACHE FILEPATH "")
set(CONFIG_TFM_SOURCE_PATH "${CONFIG_BASE_PATH}/trusted-firmware-m" CACHE PATH "")
set(MBEDCRYPTO_PATH "${CONFIG_BASE_PATH}/mbedtls" CACHE PATH "")
set(MCUBOOT_PATH "${CONFIG_BASE_PATH}/mcuboot" CACHE PATH "")

############################ Profile CONFIGURATION #############################

set(PROJECT_CONFIG_HEADER_FILE "${CMAKE_CURRENT_LIST_DIR}/profile/tfm/config_profile_large.h" CACHE FILEPATH "User defined header file for TF-M config")

############################ SPM CONFIGURATION  ################################

set(TFM_ISOLATION_LEVEL                    2     CACHE STRING "Isolation level")
set(CONFIG_TFM_SPM_BACKEND                 "IPC" CACHE STRING "The SPM backend [IPC, SFN]")

############################ PARTITION CONFIGURATION  ##########################

set(TFM_PARTITION_CRYPTO                   ON    CACHE BOOL "Enable Crypto partition")
set(TFM_PARTITION_INTERNAL_TRUSTED_STORAGE ON    CACHE BOOL "Enable Internal Trusted Storage partition")
set(TFM_PARTITION_PLATFORM                 ON    CACHE BOOL "Enable the TF-M Platform partition")
set(TFM_PARTITION_PROTECTED_STORAGE        ON    CACHE BOOL "Enable Protected Storage partition")
set(TFM_PARTITION_INITIAL_ATTESTATION      ON    CACHE BOOL "Enable Initial Attestation partition")
set(SYMMETRIC_INITIAL_ATTESTATION          OFF   CACHE BOOL "Use symmetric crypto for inital attestation")
set(TFM_PARTITION_FIRMWARE_UPDATE          OFF   CACHE BOOL "Enable firmware update partition")

################################## Advanced options #############################

set(MCUBOOT_FIH_PROFILE                    MEDIUM      CACHE STRING "Fault injection hardening profile [OFF, LOW, MEDIUM, HIGH]")
set(ITS_BUF_SIZE                           ""          CACHE STRING "Size of the ITS internal data transfer buffer (defaults to ITS_MAX_ASSET_SIZE if not set)")
set(PS_CRYPTO_AEAD_ALG                     PSA_ALG_CCM CACHE STRING "The AEAD algorithm to use for authenticated encryption in protected storage")

################################## Dependencies ################################

set(TFM_MBEDCRYPTO_CONFIG_PATH              "${CMAKE_CURRENT_LIST_DIR}/profile/mbedcrypto/tfm_mbedcrypto_config_profile_large.h" CACHE PATH "Config to use for Mbed Crypto")
set(TFM_MBEDCRYPTO_PSA_CRYPTO_CONFIG_PATH   "${CMAKE_CURRENT_LIST_DIR}/profile/mbedcrypto/crypto_config_profile_large.h" CACHE PATH "Config to use psa crypto setting for Mbed Crypto.")

############################## Enable Costum TAs ###############################

set(APPROT              ON  CACHE BOOL  "Enable the APPROT partition")
set(PROT_CRYPTO_ATTACK  ON  CACHE BOOL  "Enable the TFM PRoT PoC crypto attack partition")