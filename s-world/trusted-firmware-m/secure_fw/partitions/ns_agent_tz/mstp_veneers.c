#include <stdint.h>
#include "security_defs.h"

__tz_c_veneer
uint32_t tfm_get_sp_s(){
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

__tz_c_veneer
uint32_t tfm_read_mem_s(uint32_t * addr){
  return *addr;
}