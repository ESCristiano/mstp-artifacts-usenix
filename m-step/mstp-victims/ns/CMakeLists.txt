cmake_minimum_required(VERSION 3.22)

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

enable_language(C ASM)

set(TFM_BUILD "${CMAKE_SOURCE_DIR}/../../../copilot/build/nucleo_l552ze_q_mstp_s")
set(CONFIG_SPE_PATH "${TFM_BUILD}/api_ns")

# Interface files exported from TF-M secure build
set(SPE_INSTALL_INTERFACE_INC    ${CONFIG_SPE_PATH}/interface/include)
set(SPE_INSTALL_INTERFACE_SRC    ${CONFIG_SPE_PATH}/interface/src)

# Include SPE configuration
list(APPEND CMAKE_MODULE_PATH ${CONFIG_SPE_PATH}/cmake)
include(${CONFIG_SPE_PATH}/cmake/spe_config.cmake)
include(spe_config)
include(spe_export)

#-------------------------------------------------------------------------------
# TFM NS API Libraries
#-------------------------------------------------------------------------------

# Use existing TFM API libraries if they exist, otherwise create them
if(NOT TARGET tfm_api_ns)
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

    target_include_directories(tfm_api_ns
        PUBLIC
            ${SPE_INSTALL_INTERFACE_INC}
            ${SPE_INSTALL_INTERFACE_INC}/crypto_keys
    )

    if (TFM_PARTITION_CRYPTO)
        target_link_libraries(tfm_api_ns
            PUBLIC
                psa_crypto_config
        )
    endif()
endif()

if(NOT TARGET tfm_api_ns_tz AND CONFIG_TFM_USE_TRUSTZONE)
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

#-------------------------------------------------------------------------------
# M-Step library NS Victims
#-------------------------------------------------------------------------------
add_library(mstp_victims_ns 
    STATIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/victims.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/victims.s
)

target_include_directories(mstp_victims_ns 
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/inc
        ${SPE_INSTALL_INTERFACE_INC}
)

target_link_libraries(mstp_victims_ns 
    PUBLIC
        tfm_api_ns
        $<$<BOOL:${CONFIG_TFM_USE_TRUSTZONE}>:tfm_api_ns_tz>
)

#-------------------------------------------------------------------------------