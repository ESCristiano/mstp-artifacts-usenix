# Set default build type
set(CMAKE_BUILD_TYPE Debug CACHE STRING "")

# Set target platform
set(TFM_PLATFORM stm/nucleo_l552ze_q CACHE STRING "")

# Base path: directory of this config file
set(CONFIG_BASE_PATH "${CMAKE_CURRENT_LIST_DIR}/../../s-world" CACHE INTERNAL "Base path for relative resolution")

set(TFM_TOOLCHAIN_FILE "${CONFIG_BASE_PATH}/trusted-firmware-m/toolchain_GNUARM.cmake" CACHE FILEPATH "")
set(CONFIG_TFM_SOURCE_PATH "${CONFIG_BASE_PATH}/trusted-firmware-m" CACHE PATH "")
set(MBEDCRYPTO_PATH "${CONFIG_BASE_PATH}/mbedtls" CACHE PATH "")
set(MCUBOOT_PATH "${CONFIG_BASE_PATH}/mcuboot" CACHE PATH "")