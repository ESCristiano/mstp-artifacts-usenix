/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    Secure/Src/secure_nsc.c
  * @author  MCD Application Team
  * @brief   This file contains the non-secure callable APIs (secure world)
  ******************************************************************************
    * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* USER CODE BEGIN Non_Secure_CallLib */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "secure_nsc.h"
/** @addtogroup STM32L5xx_HAL_Examples
  * @{
  */

/** @addtogroup Templates
  * @{
  */

/* Global variables ----------------------------------------------------------*/
void *pSecureFaultCallback = NULL;   /* Pointer to secure fault callback in Non-secure */
void *pSecureErrorCallback = NULL;   /* Pointer to secure error callback in Non-secure */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Secure registration of non-secure callback.
  * @param  CallbackId  callback identifier
  * @param  func        pointer to non-secure function
  * @retval None
  */
CMSE_NS_ENTRY void SECURE_RegisterCallback(SECURE_CallbackIDTypeDef CallbackId, void *func)
{
  if(func != NULL)
  {
    switch(CallbackId)
    {
      case SECURE_FAULT_CB_ID:           /* SecureFault Interrupt occurred */
        pSecureFaultCallback = func;
        break;
      case GTZC_ERROR_CB_ID:             /* GTZC Interrupt occurred */
        pSecureErrorCallback = func;
        break;
      default:
        /* unknown */
        break;
    }
  }
}

__attribute__((naked)) void dummy_function(){
  __asm("mov r0, r0");
  __asm("mov r0, r0");
  __asm("mov r0, r0");
  __asm("mov r0, r0");
  __asm("mov r0, r0");
  __asm("mov r0, r0");
  __asm("bx lr");
}

unsigned int src = 0;

void victim_ici_resume(){
  __asm("ldr r0, =src");
  __asm("ldr r1, [r0]");
  __asm("mov r0, r0");
  __asm("ldr r1, [r0]");
  __asm("mov r0, r0");
  // __asm("ldr r1, [r0]");
  // __asm("mov r0, r0");
  // __asm("ldr r1, [r0]");
  // __asm("mov r0, r0");
  // __asm("ldr r1, [r0]");
  // __asm("ldr r1, [r0]");
  // __asm("mov r0, r0");
  // __asm("mov r0, r0");
  // dummy_function();
  // __asm("LDMIA R0!, {R1-R4}");
  // __asm("push {r0,r1,r2,r3,r4,r5}"); 
  // __asm("pop {r0,r1,r2,r3,r4,r5} ");
}

CMSE_NS_ENTRY void victim_S()
{
  victim_ici_resume();
}

__attribute((cmse_nonsecure_entry))
void victim_normal_inst_s(){
  victim_normal_inst();
}

__attribute((cmse_nonsecure_entry))
void victim_ICI_inst_s(){
  victim_ICI_inst();
}

__attribute((cmse_nonsecure_entry))
void victim_IRI_inst_s(){
  victim_IRI_inst();
}

// C 
__attribute((cmse_nonsecure_entry))
void victim_printf_inst_s(){
  victim_printf_inst();
}

__attribute((cmse_nonsecure_entry))
void victim_strlen_inst_s(){
  victim_strlen_inst();
}

__attribute((cmse_nonsecure_entry))
uint32_t get_sp_s(){
  uint32_t sp_s = 0;
  uint32_t control;
  __asm("MRS %0, CONTROL" : "=r" (control));
  // Return PSP if we are in thread mode, else return MSP
  if (control & 0x2) {
    __asm("MRS %0, PSP" : "=r" (sp_s));
  } else {
    __asm("MRS %0, MSP" : "=r" (sp_s));
  }
  return sp_s;
}

__attribute((cmse_nonsecure_entry))
uint32_t read_mem_s(uint32_t * addr){
  return *addr;
}

/**
  * @}
  */

/**
  * @}
  */
/* USER CODE END Non_Secure_CallLib */

