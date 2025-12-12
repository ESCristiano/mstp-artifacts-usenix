cmake_minimum_required(VERSION 3.22)

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

enable_language(C ASM)

#-------------------------------------------------------------------------------
# M-Step library S Victims
#-------------------------------------------------------------------------------
add_library(mstp_victims_s 
    STATIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/victims.c
        # ${CMAKE_CURRENT_SOURCE_DIR}/src/victims.s
)

target_include_directories(mstp_victims_s 
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

target_link_libraries(mstp_victims_s 
    PRIVATE

)
#-------------------------------------------------------------------------------