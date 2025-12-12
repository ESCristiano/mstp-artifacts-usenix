/*
 * victims.c
 *
 *  Created on: 25/02/2025
 *      Author: cris9
 */

#include "single-step.h"
#include "victims.h"

int vector[100] = {0};

__attribute__((naked)) void victim(){
  // __asm volatile("str r0, [r0]");  // 0x2001808c
  // __asm volatile("str r1, [r0]");  // 0
  // __asm volatile("str r2, [r0]");  // 7
  // __asm volatile("str r0, [r0]");  // 0x2001808c
  // __asm volatile("str r1, [r0]");  // 0
  // __asm volatile("str r2, [r0]");  // 7
  // __asm volatile("bx lr");
  __asm volatile("LABEL_:");
  __asm volatile("mov r0, r0");
  __asm volatile("mov r0, r0");
  __asm volatile("mov r0, r0");
  __asm volatile("mov r0, r0");
  __asm volatile("mov r0, r0");
  __asm volatile("mov r0, r0");
  __asm volatile("mov r0, r0");
  __asm volatile("mov r0, r0");
  __asm volatile("mov r0, r0");
  __asm volatile("mov r0, r0");

  __asm volatile("mov r0, r0");
  __asm volatile("mov r0, r0");
  __asm volatile("mov r0, r0");
  __asm volatile("mov r0, r0");
  __asm volatile("mov r0, r0");
  __asm volatile("mov r0, r0");
  __asm volatile("mov r0, r0");
  __asm volatile("mov r0, r0");
  __asm volatile("mov r0, r0");
  __asm volatile("mov r0, r0");

  __asm volatile("mov r0, r0");
  __asm volatile("mov r0, r0");
  __asm volatile("mov r0, r0");
  __asm volatile("mov r0, r0");
  __asm volatile("mov r0, r0");
  __asm volatile("mov r0, r0");
  __asm volatile("mov r0, r0");
  __asm volatile("mov r0, r0");
  __asm volatile("mov r0, r0");
  __asm volatile("mov r0, r0");

  __asm volatile("b LABEL_");
}

__attribute__((optimize(0))) 
__attribute__((used)) 
__attribute__((naked)) 
void dummy_function(){
  __asm volatile("ldr r2, [r0]");
  __asm volatile("bx lr");
}

#define GET_TIMESTAMP_1() do { \
	SysTick->CTRL = 0x0;\
	SysTick->LOAD  = (uint32_t)(0xFFFFFF - 1); \
	SysTick->VAL = 0; \
	SysTick->CTRL = 0x05; \
	__asm volatile("mov r0, #0xE018");\
	__asm volatile("movt r0, #0xE000");\
	__asm volatile("ldr r1, [r0]");\
} while(0)

#define GET_TIMESTAMP_2() do { \
  __asm volatile("ldr r2, [r0]");\
} while(0)

#define PRINT_LATENCY(msg) do { \
  /* - 1 to compensate for the overhead of reading the timer (1 clk) */ \
  inst_latency = (time_stamp1 - time_stamp2) - 1; \
  printf(" %d | %s\r\n", inst_latency, msg); \
} while(0)

int inst_latency = 0;

__attribute__((optimize(0))) void measure_latency(){
  register int time_stamp1 __asm("r1");
  register int time_stamp2 __asm("r2");
  
  printf("\r\n");
  printf("CPU Frequency: %d MHz\r\n", (SystemCoreClock/1000000));
  printf("\r\n");

  printf("Lat| Inst\r\n");

  __asm volatile("mov r4, #0x3100");
	__asm volatile("movt r4, #0x3000");
  GET_TIMESTAMP_1();
	__asm volatile("ldr r3, [r4]");
  GET_TIMESTAMP_2();
  PRINT_LATENCY("ldr r3, [r4], r4 -> SRAM 1");

  __asm volatile("mov r4, #0x3100");
	__asm volatile("movt r4, #0x3000");
  GET_TIMESTAMP_1();
	__asm volatile("str r3, [r4]");
  GET_TIMESTAMP_2();
  PRINT_LATENCY("str r3, [r4], r4 -> SRAM 1");

  __asm volatile("mov r4, #0x2100");
	__asm volatile("movt r4, #0x3000");
  GET_TIMESTAMP_1();
	__asm volatile("ldr r3, [r4]");
  GET_TIMESTAMP_2();
  PRINT_LATENCY("ldr r3, [r4], r4 -> SRAM 2");

  __asm volatile("mov r4, #0x2100");
	__asm volatile("movt r4, #0x3000");
  GET_TIMESTAMP_1();
	__asm volatile("str r3, [r4]");
  GET_TIMESTAMP_2();
  PRINT_LATENCY("str r3, [r4], r4 -> SRAM 2");

  __asm volatile("mov r4, #0x0000");
	__asm volatile("movt r4, #0xc000");
  GET_TIMESTAMP_1();
	__asm volatile("ldr r3, [r4]");
  GET_TIMESTAMP_2();
  PRINT_LATENCY("ldr r3, [r4], r4 -> FLASH");

  GET_TIMESTAMP_1();
  __asm volatile("mov r0, r0");
  GET_TIMESTAMP_2();
  PRINT_LATENCY("mov r0, r0");

  GET_TIMESTAMP_1();
  dummy_function();
  // GET_TIMESTAMP_2();
  PRINT_LATENCY("bl dummy_function");


}
__attribute__((optimize(0))) __attribute__((naked)) void victim_mov_bl(){
  __asm volatile("LABEL_VICTIM_MOV_BL:");
  __asm volatile("mov r0, r0");
  dummy_function();
  __asm volatile("mov r0, r0");
  __asm volatile("ldr r1, [r0]");
  __asm volatile("cmp r0, r5");
  __asm volatile("bne LABEL_VICTIM_MOV_BL_BRANCH");
  __asm volatile("mov r0, r0");
  __asm volatile("mov r0, r0");
  __asm volatile("LABEL_VICTIM_MOV_BL_BRANCH:");
  __asm volatile("mov r0, r0");
  __asm volatile("mov r0, r0");
  __asm volatile("push {r0,r1,r2,r3,r4,r5}");
  __asm volatile("mov r0, r0");
  __asm volatile("mov r0, r0");
  __asm volatile("mov r0, r0");
  __asm volatile("pop {r0,r1,r2,r3,r4,r5} ");
  __asm volatile("b LABEL_VICTIM_MOV_BL");
}

__attribute__((naked)) void victim_ici_resume(){
  __asm volatile("LABEL_B1:");
  __asm volatile("ldr r1, [r0]");
  __asm volatile("mov r0, r0");
  __asm volatile("ldr r1, [r0]");
  __asm volatile("mov r0, r0");
  __asm volatile("ldr r1, [r0]");
  // __asm volatile("LDMIA R0!, {R1-R4}");
  // __asm volatile("push {r0,r1,r2,r3,r4,r5}"); 
  // __asm volatile("pop {r0,r1,r2,r3,r4,r5} ");
  __asm volatile("b LABEL_B1");
}

__attribute__((naked)) void victim_ici_resume_pc(){
  __asm volatile("LABEL_B3:");
  __asm volatile("ldr r1, [r0]");
  __asm volatile("mov r0, r0");
  __asm volatile("ldr r1, [r0]");
  __asm volatile("mov r0, r0");
  __asm volatile("ldr r1, [r0]");
  __asm volatile("LDMIA R0!, {R1-R4}");
  __asm volatile("push {r0,r1,r2,r3,r4,r5,lr}"); 
  __asm volatile("mov r0, r0");
  __asm volatile("ldr r1, [r0]");
  __asm volatile("pop {r0,r1,r2,r3,r4,r5,r6} ");
  __asm volatile("b LABEL_B3");
}
__attribute__((naked)) void victim_ici_resume_pc_sfputs_r(){
  __asm volatile("push    {r3, r4, r5, r6, r7, lr}");
  __asm volatile("mov     r6, r0");
  __asm volatile("mov     r7, r1");
  __asm volatile("mov     r4, r2");
  __asm volatile("adds    r5, r2, r3");
  __asm volatile("cmp     r4, r5");
  __asm volatile("movs    r0, #0");
  __asm volatile("mov     r2, r7");
  __asm volatile("mov     r0, r6");
  __asm volatile("adds    r3, r0, #1");
  __asm volatile("pop     {r3, r4, r5, r6, r7, pc}");
  __asm volatile("movs    r0, r0");
}

__attribute__((naked)) void victim_ici_restart(){
  __asm volatile("LABEL_B2:");
  __asm volatile("ldr r1, [sp]");
  __asm volatile("mov r0, #0x1000");
  __asm volatile("mov r1, #0x7");
  // __asm volatile("sdiv r3, r1, r2");
  __asm volatile("udiv r2, r0, r1");
  __asm volatile("udiv r2, r0, r1");
  __asm volatile("mov r3, r3");
  __asm volatile("b LABEL_B2");
}

__attribute__((optimize(0))) 
__attribute__((naked)) 
void measure_unstacking_ldr(){
  __asm volatile("ldr r0, =tim3_CNT");
  __asm volatile("ldr r0, [r0]");
  __asm volatile("LABEL_measure_unstacking_ldr:");
  __asm volatile("ldr r1, [r0]"); // unstacking time without stoping the timer
  __asm volatile("mov r3, r3");
  __asm volatile("mov r3, r3");
  __asm volatile("mov r3, r3");
  // __asm volatile("mov r2, #0x80 "); // unstacking time stoping the timer
  // __asm volatile("ldr r0, =tim3_CR1");
  // __asm volatile("ldr r0, [r0]");
  // __asm volatile("str r2, [r0]"); // stop timer
  __asm volatile("b LABEL_measure_unstacking_ldr");
}

__attribute__((optimize(0))) __attribute__((naked)) void victim_mov_push(){
  __asm volatile("LABEL_victim_mov_push:");
  __asm volatile("mov r0, r0");
  __asm volatile("push {r0, r1}");
  __asm volatile("mov r0, r0");
  __asm volatile("pop {r0, r1}");
  __asm volatile("mov r0, r0");
  __asm volatile("push {r0, r1, r2}");
  __asm volatile("mov r0, r0");
  __asm volatile("pop {r0, r1, r2}");
  __asm volatile("mov r0, r0");
  __asm volatile("push {r0, r1, r2, r3}");
  __asm volatile("mov r0, r0");
  __asm volatile("pop {r0, r1, r2, r3}");
  __asm volatile("mov r0, r0");
  __asm volatile("b LABEL_victim_mov_push");
}

__attribute__((optimize(0))) 
__attribute__((naked)) 
void measure_stacking(){
  __asm volatile("LABEL_measure_stacking:");
  // zero the timer
  __asm volatile(
    "mov    r3, #0xE018     \n"
    "movt   r3, #0xE000     \n"
    "mov    r2, #0          \n" 
    "str    r2, [r3, #0]    \n"
);
// reload
  __asm volatile(
    "mov    r3, #0xE014     \n"
    "movt   r3, #0xE000     \n"
    "mov    r2, #-1         \n" 
    "str    r2, [r3, #0]    \n"
);
// EN
__asm volatile(
  "mov    r3, #0xE010     \n"
  "movt   r3, #0xE000     \n"
  "mov    r2, #0x5        \n" 
  "str    r2, [r3, #0]    \n"
);
__asm volatile("b LABEL_measure_stacking");
}
