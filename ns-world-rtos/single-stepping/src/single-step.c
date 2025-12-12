/*
 * single-step.c
 *
 *  Created on: 25/02/2025
 *      Author: cris9
 */

#include "single-step.h"
#include "stdio.h"

uint32_t counter = 0;
uint32_t start_timestamp = 0;
uint32_t end_timestamp = 0;
uint32_t result = 0;
uint32_t inst_time = 0;
uint32_t partial_step = 0;
uint32_t streak = 0;
uint32_t clk_it = 30;

extern int volatile *tim7_CR1,
			        *tim7_CR2,
                    *tim7_SR,
                    *tim7_DIER,
                    *tim7_EGR,
                    *tim7_CNT,
                    *tim7_ARR;

extern int volatile *tim3_CR1,
                    *tim3_SR,
                    *tim3_DIER,
                    *tim3_EGR,
                    *tim3_CNT,
                    *tim3_ARR,
                    *tim3_CCR1;

extern int volatile *tim2_CR1,
                    *tim2_SR,
                    *tim2_DIER,
                    *tim2_EGR,
                    *tim2_CNT,
                    *tim2_ARR;

//------------------------------------------------------------------------------
// Single-Step FSM Definition
//------------------------------------------------------------------------------
typedef enum {
    ATOMIC_INST,
    ICI_RESUME,
    ICI_RESUME_PC,
    ICI_RESTART,
    ICI_RESTART_STEP
} state_t;

void atomic_inst();
void ici_resume();
void ici_resume_pc();
void ici_restart();

typedef void (*state_func)();

state_func state_functions[4] = {   atomic_inst, 
                                    ici_resume, 
                                    ici_resume_pc, 
                                    ici_restart};

state_t state_n = ATOMIC_INST, state_p = ATOMIC_INST;

//------------------------------------------------------------------------------

int base_line = BASE_LINE;
int base_clk = BASE_CLK;
register int sp __asm("sp");

#define UP_COUNTER
int *ptr_ctr = 0x50001000, *ptr_cnt = 0x50001000+0x24, *rcc_ctrl = 0x50021000+0x058;

// use these "functions" as a MACRO to reduce cache polution. These functions will be called
// in a __attribute__((optimize(0))) function, so inline functions are not allowed
#define config_timer()(*rcc_ctrl = *rcc_ctrl | (1<<4))
#define enable_timer()(*ptr_ctr = (*ptr_ctr)|1<<0)
#define disable_timer()(*ptr_ctr = (*ptr_ctr)&~(1<<0))

void __attribute__((optimize(0))) TIM7_IRQHandler(){
    //--------------------------------------------------------------------------
    // Get end timestamp
    //--------------------------------------------------------------------------
//	config_timer();
//	disable_timer();
    SysTick->CTRL = 0x0; // stops the systick
    end_timestamp = SysTick->VAL;
//    end_timestamp = *ptr_cnt;
    result = (start_timestamp - end_timestamp); // inverted subtraction because it is a down counter
//    result = (end_timestamp-start_timestamp); // inverted subtraction because it is a down counter
    SysTick->VAL = 0; // reset the val  counter
//    *ptr_cnt = 0;
    inst_time = result - base_line;

    //--------------------------------------------------------------------------
    // Analyze the LR EXC_RETURN Value
    //--------------------------------------------------------------------------
    register int lr __asm("lr");
    int lr_saved = lr;
    counter++;
    // See EXC_RETURN values -> Definitive Guide to Arm Cortex-M23 and Cortex-M33 Processors, pag.354
    if( (lr_saved&0xf) == 0) // Detect Preemption
        while(1); // Stop the program

    //--------------------------------------------------------------------------
    // Single Step Logic (Algortihm to adjust next IT)
    //--------------------------------------------------------------------------
    // state_functions[state_n]();

    //--------------------------------------------------------------------------
    // Config Next It
    //--------------------------------------------------------------------------
    *tim7_SR  = 0; // Clean interrupt
    *tim7_CNT = 0;
    *tim7_CR1 |= (1<<3); // One pulse mode
    *tim7_ARR = (uint32_t)((clk_it) - 1);

    //--------------------------------------------------------------------------
    // Get start timestamp
    //--------------------------------------------------------------------------
    SysTick->CTRL = 0x0;
    SysTick->LOAD  = (uint32_t)(0xFFFFFF - 1); //mac counter value
    SysTick->VAL = 0; // reset the val  counter
    SysTick->CTRL = 0x05; // starts the systick
    start_timestamp = SysTick->VAL;
//    enable_timer();
//    start_timestamp = *ptr_cnt;
    //--------------------------------------------------------------------------
    // Fire Next It
    //--------------------------------------------------------------------------
    *tim7_CR1 |= (1<<0); // EN
}

int array[256] = {0};
unsigned int index = 0;

int clock_to_collide = 18;

// ------------------------- MACROS FOR SHIFTS AND DELAYS ----------------------
// "nop" instruction cannot used, because it is not reliable! Sometimes it
// last 1 clock other times 0 clocks. Even with -o0 and the actual nop being
// present in the asm code. I think the processor sometimes does not execute the
// nop instructions
#define MY_NOP()  __asm volatile("mov r8,r8")

#define DELAY_8_CLOCKS() do { \
		MY_NOP(); \
		MY_NOP(); \
		MY_NOP(); \
		MY_NOP(); \
		MY_NOP(); \
		MY_NOP(); \
		MY_NOP(); \
		MY_NOP(); \
	} while(0)

__attribute__((aligned(64)))
__attribute__((optimize(0)))
__attribute__((naked))
void TIM3_IRQHandler(){

    //--------------------------------------------------------------------------
    // ATTENTION: This function is naked, I'm not saving the registers. So,
    // if a call to other functions are made, we have to save the registers
    // before the call and restore them after the call. Otherwise, we will  
    // lose the context of the registers.
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    // Get end timestamp
    //--------------------------------------------------------------------------
    // *tim3_CR1 &= ~(1<<0); // EN
    __asm volatile(
        "ldr    r3, =tim3_CR1  \n"
        "ldr    r3, [r3, #0]   \n"
        "mov    r2, #0x80      \n" // ARPE 
        "str    r2, [r3, #0]   \n"
    );

//    array[counter++] = *tim3_CNT;

    //--------------------------------------------------------------------------
    // Config Next It
    //--------------------------------------------------------------------------
    // *tim3_SR  = 0; // Clean interrupt
    __asm volatile(
        "ldr    r3, =tim3_SR    \n"
        "ldr    r3, [r3, #0]    \n"
        "mov    r2, #0          \n" 
        "str    r2, [r3, #0]    \n"
    );

    // *tim3_CNT = 0;
    __asm volatile(
        "ldr    r3, =tim3_CNT   \n"
        "ldr    r3, [r3, #0]    \n"
        "mov    r2, #0          \n" 
        "str    r2, [r3, #0]    \n"
    );

    // *tim3_CCR1 = (uint32_t)((clk_it) - 1); // Compare value
    __asm volatile(
        "ldr    r3, =tim3_CCR1   \n"
        "ldr    r3, [r3, #0]    \n"
        "ldr    r2, =clk_it     \n"
        "ldr    r2, [r2, #0]    \n"
        "subs   r2, r2, #1      \n"
        "str    r2, [r3, #0]    \n"
    );

    *tim7_CNT = 0;
    *tim7_CR1 |= (1<<0); // EN
    *tim2_CR1 &= ~(1<<0);
    *tim2_CNT = 0 - clock_to_collide;
    *tim2_CR1 |= (1<<0);
    DELAY_8_CLOCKS(); /* delay to ensure the 1st collision happens in the victim 1st clock*/

    //--------------------------------------------------------------------------
    // Get start timestamp & Fire Next It
    //--------------------------------------------------------------------------
    // *tim3_CR1 |= (1<<0); // EN
    __asm volatile(
        "ldr    r3, =tim3_CR1  \n"
        "ldr    r3, [r3, #0]   \n"
        "mov    r2, #0x81      \n" // ARPE + CEN
        "str    r2, [r3, #0]   \n"
    );


    // Return to the victim
    __asm("bx lr"); 
}


int ici_restart_clk_it = 0;
int ici_resume_pc_clk_it = 0;

void atomic_inst(){
    clk_it = BASE_CLK;
    // Count only interrupts generated in this state
    if(state_p == ATOMIC_INST){
     // TODO: Put equal to the previous instead of 1. To accommodate for systems/
     // scenarios, where the repeated instruction takes more time then 1 clk.
		if(inst_time == 1)
			streak++;
		else { // Clean once we break the streak
			// Just clean if we are on a middle of multiple partial steps and if
			// we already had a streak count. This is an edge case of the ICI
			// Resume instruction. The first it with BASE_CLK performs a partial
			// step, but the next partial-steps need BASE_CLK+1 to interrupt.
			// I guess this extra clock is needed to do the context restore and
			// resume of the ICI instruction. When we are stucked on partial steps
			// on a ICI resume with PC, this will clean the partial_steps count
			// used to detect if the ICI resume is partial progressing (partial
			// steps) but not performing the single step (i.e., the ICI resume
			// instruction is always being re-executed.)
			if(streak >= 1) {
				//partial_step = 0;
            	ici_resume_pc_clk_it = 0;
			}
			streak = 0;
			ici_restart_clk_it = 0;
		}
    }
    // Detect Zero-Step, long streak of instructions with no variation on
    // the execution time, i.e., stucked on the same instruction/execution stage.
    if(streak >= STREAK_THRESHOLD){
        streak = 0;
        state_n = ICI_RESUME;
    }

    // Store Previous state
    state_p = ATOMIC_INST;
}

void ici_resume(){  
    // Remove clock bias due to the increment of the clk_it. We want to know the 
    // real instruction time. If we don't correct for the change in clk_it, we 
    // will see a fake increase in the instruction time. 
    if(clk_it > BASE_CLK)
        inst_time = inst_time - (clk_it - BASE_CLK);

    // If we tried a partial step and there was no progress, i.e., we are still
    // keeping with the streak, this is a ICI restart instruction
    if(inst_time == 1 && state_p == ICI_RESUME){ // i.e., we tried a partial step and there was no progress
        partial_step = 0;
        state_n = ICI_RESTART;
    } else
    // Check if we are doing partial steps on ICI resume, but we aren't single
    // stepping. This is a edge case of the ICI resume instruction when the PC
    // is on the reglist. e.g., pop {r0, r1, r2, r3, r4, r5, pc}
    if(partial_step >= N_MAX_REGS){
        partial_step = 0;
        state_n = ICI_RESUME_PC;
    } 
    else 
    // We don't know if this is the last partial step, so we have to assume it is
    // and we will check again the maximum streak. This is very time consuming, 
    // but it is the only way to guarantee that we are not stucked. And while
    // this could take a lot of time, ICI instructions are not that common.
    //
    if(inst_time > 1) { // i.e., partial step
        state_n = ATOMIC_INST;
    } else  {
        clk_it++;
        partial_step++;
    }

    // Store Previous state
    state_p = ICI_RESUME;
}

// ICI resume instructions where there is PC on the reglist, take more time to 
// execute. Maybe because on a pop to the PC the CPU will branch to other 
// address. And for some reason, if we don't give enough time, while this is a 
// ICI resume instruction, the instruction will execute again.
void ici_resume_pc(){
    if(!ici_resume_pc_clk_it)
        // Check next clk_it to the one tested on ICI_RESUME
        clk_it++;
    else
        // Restore context 
        clk_it = ici_resume_pc_clk_it + 1;

    // Save ici_restart context
    ici_resume_pc_clk_it = clk_it;

    // Check single step, i.e., try to single-step
    state_n = ATOMIC_INST;

    // Store Previous state
    state_p = ICI_RESUME_PC;
}

void ici_restart(){
    if(!ici_restart_clk_it)
        // Check next clk_it to the one tested on ICI_RESUME
        clk_it++;
    else
        // Restore context 
        clk_it = ici_restart_clk_it + 1;

    // Save ici_restart context
    ici_restart_clk_it = clk_it;

    // Check single step, i.e., try to single-step 
    state_n = ATOMIC_INST;

    // Store Previous state
    state_p = ICI_RESTART;
}
