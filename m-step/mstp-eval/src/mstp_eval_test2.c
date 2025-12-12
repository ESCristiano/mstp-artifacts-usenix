#include "main.h"
#include "mstp.h"
#include "victims.h"
#include "mstp_eval.h"
#include "mstp_eval_setup.h"
#include "mstp_eval_tests.h"

extern uint8_t code_under_test; 

//------------------------------------------------------------------------------
// Test#2: Data-dependent Instructions
//------------------------------------------------------------------------------
void test2_div_instructions_eval(uint16_t n_runs, uint8_t print, uint8_t world) {   
    // the n_runs has no meaning here, the victim has N instructions
    run_specific_setup_victim(SETUP_DEFAULT, TEST_DIV, print, world);
}

//------------------------------------------------------------------------------
// Test#3: Instructions Differentiation
//------------------------------------------------------------------------------
void test3_instructions_diff(uint16_t n_runs, uint8_t print, uint8_t world) {   
    // the n_runs has no meaning here, the victim has N instructions
    run_specific_setup_victim(SETUP_DEFAULT, TEST_INST_DIFF, print, world);
}

#define SYSTICK_AVAILABLE
#include "uArchChannel_lib.h"

extern void ICache_invalidation();

int covert_channel_test() 
{
    char lines;
    int probe_time;

    ICache_invalidation();

    SysTick->CTRL = 0x0;
    SysTick->CTRL = 0x0;
    SysTick->LOAD  = (uint32_t)(1000000 - 1);/* set reload register */
    SysTick->VAL = 0;

    for (int input_symbol = 0; input_symbol < I_N_CACHE_SETS/16; input_symbol++)
    {
        for(int sample = 0; sample < 10; sample++)
        {
            lines = input_symbol;
            //prime all I-cache 
            prime();
            // Call the NS world trojan, to measure the channel without ns interference
            touch_ns(input_symbol);
            // measure the time to access the N_CAHCE_SETS I-cache line buffer
            probe_time = probe_all();
            printf("%d %d\n", input_symbol, probe_time);
        }
    }
    return 1;
}


//------------------------------------------------------------------------------
// Test#4: Cache Misses
//------------------------------------------------------------------------------
void test4_cache(uint16_t n_runs, uint8_t print, uint8_t world) {   
    // the n_runs has no meaning here, the victim has N instructions
    // run_specific_setup_victim(SETUP_DEFAULT, TEST_CACHE, print, world);
    covert_channel_test();
}

//------------------------------------------------------------------------------
// Test#5: Memory Contention
//------------------------------------------------------------------------------
void test5_memory_contention(uint16_t n_runs, uint8_t print, uint8_t world) {  
    // the n_runs has no meaning here, the victim has N instructions
    run_specific_setup_victim(SETUP_DEFAULT, TEST_CONT, print, world);
}
