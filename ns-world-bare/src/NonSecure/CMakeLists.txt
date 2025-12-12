cmake_minimum_required(VERSION 3.22)

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Compiler options
# set(STM32_MCU_FLAGS  "-mcpu=cortex-m33" )
set(STM32_MCU_FLAGS  "-mcpu=cortex-m33 -mfpu=fpv4-sp-d16 -mfloat-abi=hard " )

#Linker options
set(STM32_LINKER_SCRIPT STM32L552xE_FLASH_ns.ld)
set(STM32_LINKER_OPTION  )

# Include toolchain file
include("../gcc-arm-none-eabi.cmake")

# Set the project name
set(CMAKE_PROJECT_NAME tfm_ns)
project(${CMAKE_PROJECT_NAME})

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Include mx-generated 
include("mx-generated.cmake")


# In order to use CMake for cross-compiling
set(CMAKE_CXX_COMPILER_FORCED "true") 

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE 
    # Add user defined symbols
)

# Enable CMake support for ASM and C languages
enable_language(C ASM)

####################### Bring SPE Configs to the NS ###########################
set(CONFIG_SPE_PATH "${CMAKE_SOURCE_DIR}/../../../copilot/build/nucleo_l552ze_q_mstp_s/api_ns")

list(APPEND CMAKE_MODULE_PATH ${CONFIG_SPE_PATH}/cmake)

# A platform specific MCPU and architecture flags for NS side
include(${CONFIG_SPE_PATH}/platform/cpuarch.cmake)
# Include common configs exported from TF-M
include(${CONFIG_SPE_PATH}/cmake/spe_config.cmake)

# Interface files exported from TF-M secure build
set(SPE_INSTALL_INTERFACE_SRC    ${CONFIG_SPE_PATH}/interface/src)
set(SPE_INSTALL_INTERFACE_INC    ${CONFIG_SPE_PATH}/interface/include)

include(spe_config)
include(spe_export)

set_target_properties(tfm_config psa_interface psa_crypto_config PROPERTIES IMPORTED_GLOBAL True)
target_link_libraries(tfm_config INTERFACE psa_interface)

##################### NS API To Interact with the SPE ##########################
# This was imported/based on the following files from TF-M:
# - build_s/api_ns/platform/CMakeLists.txt
# - build_s/api_ns/CMakeLists.txt

add_library(tfm_api_ns STATIC)

target_sources(tfm_api_ns
    PUBLIC
        $<$<BOOL:${TFM_PARTITION_PLATFORM}>:${SPE_INSTALL_INTERFACE_SRC}/tfm_platform_api.c>
        $<$<BOOL:${TFM_PARTITION_PROTECTED_STORAGE}>:${SPE_INSTALL_INTERFACE_SRC}/tfm_ps_api.c>
        $<$<BOOL:${TFM_PARTITION_INTERNAL_TRUSTED_STORAGE}>:${SPE_INSTALL_INTERFACE_SRC}/tfm_its_api.c>
        $<$<BOOL:${TFM_PARTITION_CRYPTO}>:${SPE_INSTALL_INTERFACE_SRC}/tfm_crypto_api.c>
        $<$<BOOL:${TFM_PARTITION_INITIAL_ATTESTATION}>:${SPE_INSTALL_INTERFACE_SRC}/tfm_attest_api.c>
        $<$<BOOL:${TFM_PARTITION_FIRMWARE_UPDATE}>:${SPE_INSTALL_INTERFACE_SRC}/tfm_fwu_api.c>
)

# Include interface headers exported by TF-M
target_include_directories(tfm_api_ns
    PUBLIC
        ${SPE_INSTALL_INTERFACE_INC}
        ${SPE_INSTALL_INTERFACE_INC}/crypto_keys
)

if (CONFIG_TFM_USE_TRUSTZONE)
    add_library(tfm_api_ns_tz INTERFACE)

    target_sources(tfm_api_ns_tz
        INTERFACE
            ${SPE_INSTALL_INTERFACE_SRC}/tfm_tz_psa_ns_api.c
    )

    target_link_libraries(tfm_api_ns_tz
        INTERFACE
            ${CONFIG_SPE_PATH}/interface/lib/s_veneers.o
    )
endif()

if (TFM_PARTITION_CRYPTO)
    target_link_libraries(tfm_api_ns
        PUBLIC
            psa_crypto_config
    )
endif()

add_library(platform_region_defs INTERFACE)

target_compile_definitions(platform_region_defs
    INTERFACE
        $<$<BOOL:${BL1}>:BL1>
        $<$<BOOL:${BL2}>:BL2>
        BL2_HEADER_SIZE=${BL2_HEADER_SIZE}
        BL2_TRAILER_SIZE=${BL2_TRAILER_SIZE}
        BL1_HEADER_SIZE=${BL1_HEADER_SIZE}
        BL1_TRAILER_SIZE=${BL1_TRAILER_SIZE}
        $<$<BOOL:${MCUBOOT_IMAGE_NUMBER}>:MCUBOOT_IMAGE_NUMBER=${MCUBOOT_IMAGE_NUMBER}>
        $<$<BOOL:${TEST_PSA_API}>:PSA_API_TEST_${TEST_PSA_API}>
        $<$<OR:$<CONFIG:Debug>,$<CONFIG:relwithdebinfo>>:ENABLE_HEAP>
)

target_link_libraries(platform_region_defs
    INTERFACE
        tfm_config
)

add_library(platform_ns)

target_sources(platform_ns
    PRIVATE
        ${CONFIG_SPE_PATH}/platform/hal/Src/stm32l5xx_hal.c
        ${CONFIG_SPE_PATH}/platform/Device/Source/system_stm32l5xx.c
        ${CONFIG_SPE_PATH}/platform/CMSIS_Driver/low_level_com.c
        ${CONFIG_SPE_PATH}/platform/hal/Src/stm32l5xx_hal_dma.c
        ${CONFIG_SPE_PATH}/platform/hal/Src/stm32l5xx_hal_pwr.c
        ${CONFIG_SPE_PATH}/platform/hal/Src/stm32l5xx_hal_pwr_ex.c
        ${CONFIG_SPE_PATH}/platform/hal/Src/stm32l5xx_hal_rcc.c
        ${CONFIG_SPE_PATH}/platform/hal/Src/stm32l5xx_hal_gpio.c
        ${CONFIG_SPE_PATH}/platform/hal/Src/stm32l5xx_hal_uart.c
        ${CONFIG_SPE_PATH}/platform/hal/Src/stm32l5xx_hal_uart_ex.c
        ${CONFIG_SPE_PATH}/platform/hal/Src/stm32l5xx_hal_cortex.c
)

target_include_directories(platform_ns
    PUBLIC
        ${CONFIG_SPE_PATH}/platform/include
        ${CONFIG_SPE_PATH}/platform/ext/cmsis/Include
        ${CONFIG_SPE_PATH}/platform/ext/cmsis/Include/m-profile
        ${CONFIG_SPE_PATH}/platform/ext/common
        ${CONFIG_SPE_PATH}/platform/Device/Include
        ${CONFIG_SPE_PATH}/platform/hal/Inc
)

# Include region_defs.h and flash_layout.h
target_include_directories(platform_region_defs
    INTERFACE
        partition
)

target_sources(platform_ns
    PRIVATE
        $<$<BOOL:${PLATFORM_DEFAULT_UART_STDOUT}>:${CONFIG_SPE_PATH}/platform/ext/common/uart_stdout.c>
)

target_compile_definitions(platform_ns
    PUBLIC
        DOMAIN_NS=1
        $<$<BOOL:${PLATFORM_DEFAULT_CRYPTO_KEYS}>:PLATFORM_DEFAULT_CRYPTO_KEYS>
        $<$<STREQUAL:${CONFIG_TFM_FLOAT_ABI},hard>:CONFIG_TFM_FLOAT_ABI=2>
        $<$<STREQUAL:${CONFIG_TFM_FLOAT_ABI},soft>:CONFIG_TFM_FLOAT_ABI=0>
        $<$<BOOL:${CONFIG_TFM_ENABLE_CP10CP11}>:CONFIG_TFM_ENABLE_CP10CP11>
)

target_link_libraries(tfm_api_ns
    PUBLIC
        platform_region_defs
        platform_ns
)

##################### NS Application ###########################################
#-------------------------------------------------------------------------------
# M-Step library
# Include m-step from relative path
# Set a unique build directory for m-step 
set(M_STEP_DIR  "${CMAKE_SOURCE_DIR}/../../../m-step")
add_subdirectory(${M_STEP_DIR} ${CMAKE_BINARY_DIR}/m-step_build)
#-------------------------------------------------------------------------------

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} 
    PRIVATE
    # Add user defined include paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} 
    PRIVATE
    ${SPE_INSTALL_INTERFACE_SRC}/os_wrapper/tfm_ns_interface_bare_metal.c

)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} 
    PRIVATE
    # Add user defined library search paths
)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME} 
    # Veneers
    tfm_api_ns_tz
    tfm_api_ns
    # M-Step library
    mstp
    mstp_eval
    mstp_poc
)

# Validate that STM32CubeMX code is compatible with C standard
if((CMAKE_C_STANDARD EQUAL 90) OR (CMAKE_C_STANDARD EQUAL 99))
    message(ERROR "Generated code requires C11 or higher")
endif()









