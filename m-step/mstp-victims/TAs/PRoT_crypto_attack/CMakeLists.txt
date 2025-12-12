#-------------------------------------------------------------------------------
# Copyright (c) 2021, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------
if (NOT PROT_CRYPTO_ATTACK)
    return()
endif()

cmake_minimum_required(VERSION 3.21)

add_library(tfm_PRoT_crypto_attack_partition STATIC)

target_sources(tfm_PRoT_crypto_attack_partition
    PRIVATE
        PRoT_crypto_attack.c
)

add_dependencies(tfm_PRoT_crypto_attack_partition manifest_tool)

# The generated sources
target_sources(tfm_PRoT_crypto_attack_partition
    PRIVATE
        ${CMAKE_BINARY_DIR}/generated/PRoT_crypto_attack/auto_generated/intermedia_PRoT_crypto_attack.c
)
target_sources(tfm_partitions
    INTERFACE
        ${CMAKE_BINARY_DIR}/generated/PRoT_crypto_attack/auto_generated/load_info_PRoT_crypto_attack.c
)

# Set include directory
target_include_directories(tfm_PRoT_crypto_attack_partition
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        ${CMAKE_BINARY_DIR}/generated/PRoT_crypto_attack
)
target_include_directories(tfm_partitions
    INTERFACE
        ${CMAKE_BINARY_DIR}/generated/PRoT_crypto_attack
)

target_link_libraries(tfm_PRoT_crypto_attack_partition
    PRIVATE
        # psa_interface
        platform_s
        tfm_config
        tfm_sprt
)

############################ Partition Defs ####################################
target_link_libraries(tfm_partitions
    INTERFACE
        tfm_PRoT_crypto_attack_partition
)
target_compile_definitions(tfm_config
    INTERFACE
        PRoT_crypto_attack
)
