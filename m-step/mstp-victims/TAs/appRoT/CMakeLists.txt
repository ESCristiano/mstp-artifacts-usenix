#-------------------------------------------------------------------------------
# Copyright (c) 2021, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------
if (NOT APPROT)
    return()
endif()

cmake_minimum_required(VERSION 3.21)

#-------------------------------------------------------------------------------
# M-Step Eval library
set(M_STEP_VICTIMS_DIR  "../../s")
add_subdirectory(${M_STEP_VICTIMS_DIR} ${CMAKE_BINARY_DIR}/m-step-victim_build)
#-------------------------------------------------------------------------------

add_library(tfm_app_rot_partition_appRoT STATIC)

target_sources(tfm_app_rot_partition_appRoT
    PRIVATE
        appRoT.c
)

add_dependencies(tfm_app_rot_partition_appRoT manifest_tool)

# The generated sources
target_sources(tfm_app_rot_partition_appRoT
    PRIVATE
        ${CMAKE_BINARY_DIR}/generated/appRoT/auto_generated/intermedia_appRoT.c
)
target_sources(tfm_partitions
    INTERFACE
        ${CMAKE_BINARY_DIR}/generated/appRoT/auto_generated/load_info_appRoT.c
)

# Set include directory
target_include_directories(tfm_app_rot_partition_appRoT
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        ${CMAKE_BINARY_DIR}/generated/appRoT
)
target_include_directories(tfm_partitions
    INTERFACE
        ${CMAKE_BINARY_DIR}/generated/appRoT
)

target_link_libraries(tfm_app_rot_partition_appRoT
    PRIVATE
        # psa_interface
        platform_s
        tfm_config
        tfm_sprt
        mstp_victims_s
)

############################ Partition Defs ####################################
target_link_libraries(tfm_partitions
    INTERFACE
        tfm_app_rot_partition_appRoT
)
target_compile_definitions(tfm_config
    INTERFACE
        APPROT
)
