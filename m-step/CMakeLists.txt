cmake_minimum_required(VERSION 3.22)

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

enable_language(C ASM)

set (MSTP_DRIVERS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Drivers)

add_subdirectory(mstp-victims/ns)

#-------------------------------------------------------------------------------
# M-Step Platform library
#-------------------------------------------------------------------------------
add_library(mstp_platform 
    STATIC
        ${MSTP_DRIVERS_DIR}/stm32l5xx-hal-driver/Src/stm32l5xx_hal.c
        ${MSTP_DRIVERS_DIR}/stm32l5xx-hal-driver/Src/stm32l5xx_hal_cortex.c
        ${MSTP_DRIVERS_DIR}/stm32l5xx-hal-driver/Src/stm32l5xx_hal_dma.c
        ${MSTP_DRIVERS_DIR}/stm32l5xx-hal-driver/Src/stm32l5xx_hal_dma_ex.c
        ${MSTP_DRIVERS_DIR}/stm32l5xx-hal-driver/Src/stm32l5xx_hal_exti.c
        ${MSTP_DRIVERS_DIR}/stm32l5xx-hal-driver/Src/stm32l5xx_hal_flash.c
        ${MSTP_DRIVERS_DIR}/stm32l5xx-hal-driver/Src/stm32l5xx_hal_flash_ex.c
        ${MSTP_DRIVERS_DIR}/stm32l5xx-hal-driver/Src/stm32l5xx_hal_flash_ramfunc.c
        ${MSTP_DRIVERS_DIR}/stm32l5xx-hal-driver/Src/stm32l5xx_hal_gpio.c
        ${MSTP_DRIVERS_DIR}/stm32l5xx-hal-driver/Src/stm32l5xx_hal_gtzc.c
        ${MSTP_DRIVERS_DIR}/stm32l5xx-hal-driver/Src/stm32l5xx_hal_i2c.c
        ${MSTP_DRIVERS_DIR}/stm32l5xx-hal-driver/Src/stm32l5xx_hal_i2c_ex.c
        ${MSTP_DRIVERS_DIR}/stm32l5xx-hal-driver/Src/stm32l5xx_hal_icache.c
        ${MSTP_DRIVERS_DIR}/stm32l5xx-hal-driver/Src/stm32l5xx_hal_pwr.c
        ${MSTP_DRIVERS_DIR}/stm32l5xx-hal-driver/Src/stm32l5xx_hal_pwr_ex.c
        ${MSTP_DRIVERS_DIR}/stm32l5xx-hal-driver/Src/stm32l5xx_hal_rcc.c
        ${MSTP_DRIVERS_DIR}/stm32l5xx-hal-driver/Src/stm32l5xx_hal_rcc_ex.c
        ${MSTP_DRIVERS_DIR}/stm32l5xx-hal-driver/Src/stm32l5xx_hal_tim.c
        ${MSTP_DRIVERS_DIR}/stm32l5xx-hal-driver/Src/stm32l5xx_hal_tim_ex.c
        ${MSTP_DRIVERS_DIR}/stm32l5xx-hal-driver/Src/stm32l5xx_hal_uart.c
        ${MSTP_DRIVERS_DIR}/stm32l5xx-hal-driver/Src/stm32l5xx_hal_uart_ex.c 
) 

target_include_directories(mstp_platform 
    PUBLIC
        ${MSTP_DRIVERS_DIR}/CMSIS/Device/Include
        ${MSTP_DRIVERS_DIR}/CMSIS/Include
        ${MSTP_DRIVERS_DIR}/stm32l5xx-hal-driver/Inc
    PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/mstp/inc
)

#-------------------------------------------------------------------------------
# M-Step library
#-------------------------------------------------------------------------------
add_library(mstp 
    STATIC
        ${CMAKE_CURRENT_SOURCE_DIR}/mstp/src/mstp.c
        ${CMAKE_CURRENT_SOURCE_DIR}/mstp/src/mstp_metrics.c
        ${CMAKE_CURRENT_SOURCE_DIR}/mstp/src/mstp.s
)

target_include_directories(mstp 
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/mstp/inc
        ${CMAKE_CURRENT_SOURCE_DIR}/mstp-eval/inc
)

target_link_libraries(mstp 
    PRIVATE
        mstp_platform
)

#-------------------------------------------------------------------------------
# M-Step Evaluation library
#-------------------------------------------------------------------------------
add_library(mstp_eval 
    STATIC
        ${CMAKE_CURRENT_SOURCE_DIR}/mstp-eval/src/mstp_eval.c
        ${CMAKE_CURRENT_SOURCE_DIR}/mstp-eval/src/mstp_eval_setup.c
        ${CMAKE_CURRENT_SOURCE_DIR}/mstp-eval/src/mstp_eval_test1.c
        ${CMAKE_CURRENT_SOURCE_DIR}/mstp-eval/src/mstp_eval_test2.c
        ${CMAKE_CURRENT_SOURCE_DIR}/mstp-eval/src/armv8_decop_lib.c
        ${CMAKE_CURRENT_SOURCE_DIR}/mstp-eval/src/i_cache_buffer_ns.s
)

target_include_directories(mstp_eval 
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/mstp-eval/inc
)

target_link_libraries(mstp_eval 
    PRIVATE
        mstp
        mstp_platform
        mstp_victims_ns
)

#-------------------------------------------------------------------------------
# M-Step Evaluation library
#-------------------------------------------------------------------------------
add_library(mstp_poc 
    STATIC
        ${CMAKE_CURRENT_SOURCE_DIR}/mstp-poc/src/mstp_poc.c
        ${CMAKE_CURRENT_SOURCE_DIR}/mstp-poc/src/mstp_poc_setup.c
        ${CMAKE_CURRENT_SOURCE_DIR}/mstp-poc/poc-001/src/poc_001.c
)
        
target_include_directories(mstp_poc 
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/mstp-poc/inc
        ${CMAKE_CURRENT_SOURCE_DIR}/mstp-poc/poc-001/inc/
)

target_link_libraries(mstp_poc 
    PRIVATE
        mstp
        mstp_platform
        mstp_victims_ns
)
