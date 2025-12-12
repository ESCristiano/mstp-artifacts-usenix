#ifndef INC_M_STEP_DEFS_H_
#define INC_M_STEP_DEFS_H_

// "nop" instruction cannot used, because it is not reliable! Sometimes it
// last 1 clock other times 0 clocks. Even with -o0 and the actual nop being
// present in the asm code. It is optional for the vendor to implement or not
// the nop instruction. So we use a mov r0, r0 instead.
#define MY_NOP()  __asm volatile("mov r0,r0")

#define START_MARKER() do{\
    __asm volatile(".globl START_SINGLE_STEP"); \
    __asm volatile("START_SINGLE_STEP:"); \
    MY_NOP();\
    }while(0)

#define END_MARKER() do{\
    __asm volatile(".globl END_SINGLE_STEP"); \
    __asm volatile("END_SINGLE_STEP:"); \
    MY_NOP();\
}while(0)

extern void START_SINGLE_STEP(void); 
extern void END_SINGLE_STEP(void); 

#define GET_START_MARKER() START_SINGLE_STEP
#define GET_END_MARKER() END_SINGLE_STEP

#endif /* INC_M_STEP_H_ */
