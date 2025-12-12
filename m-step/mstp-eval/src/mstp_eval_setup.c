
#include "main.h"
#include "mstp.h"
#include "mstp_eval_setup.h"

extern void MX_TIM3_Init(void);
extern void MX_TIM2_Init(void);
extern void MX_TIM5_Init(void);
extern void MX_TIM7_Init(void);
extern void MX_ICACHE_Init(void);
extern void MX_ICACHE_DeInit(void);

void setup_default(){
    MX_TIM3_Init();
    MX_TIM2_Init();
    MX_TIM5_Init();
    MX_TIM7_Init();
    MX_ICACHE_Init();
    // disable fpu -> Clear ASPEN bit to disable FPU
    FPU->FPCCR &= ~(0x1 << 31);     

    SET_BIT(ICACHE->CR, ICACHE_CR_HITMEN);
    SET_BIT(ICACHE->CR, ICACHE_CR_MISSMEN);
}

void setup_no_int_multi_cycle(){
    MX_TIM3_Init();
    MX_TIM2_Init();
    MX_ICACHE_Init();
    // disable fpu -> Clear ASPEN bit to disable FPU
    FPU->FPCCR &= ~(0x1 << 31); 

    uint32_t *ptr = 0xE000E008;
    // Disable interrups in multi cycle instructions. All instructions are atomic
    // now.
    *ptr = 0x1; 
}

void setup_default_no_cache(){
    MX_TIM3_Init();
    MX_TIM2_Init();
    MX_ICACHE_DeInit();
    // disable fpu -> Clear ASPEN bit to disable FPU
    FPU->FPCCR &= ~(0x1 << 31); 

}

void setup_fpu(){
    MX_TIM3_Init();
    MX_TIM2_Init();
    MX_ICACHE_Init();
    // enable fpu -> Set ASPEN bit to disable FPU
    FPU->FPCCR |= (0x1 << 31); 
    // disable lazy stacking -> Clear the LSPEN bit to disable lazy stacking
    FPU->FPCCR &= ~(1 << 30);

}

void setup_fpu_lazy(){
    MX_TIM3_Init();
    MX_TIM2_Init();
    MX_ICACHE_Init();
    // enable fpu -> Set ASPEN bit to disable FPU
    FPU->FPCCR |= (0x1 << 31); 
    // disable lazy stacking -> Set the LSPEN bit to enable lazy stacking
    FPU->FPCCR |= (1 << 30); 
}

void setup(setup_conf_t config, char world) {
    switch (config)
    {
    case SETUP_DEFAULT:

        // To make sure NS is not intreferring with the S world code, put all
        // NS code non-cacheable. 
        // For the NS test, we are not doing that, because if we do, we can't
        // evalute the printf function, because it will be also non-cacheable.
        // if (world == S_BARE || world == S_TFM) 
            setup_ns_mpu(); 

        setup_default();
        break; 

    case SETUP_DEFAULT_NO_CACHE:
        // Test Cache disabled -> Not working bacause the M-Step is not configured
        // fine-tunned for this setup.
        setup_default_no_cache();
        break;

    case SETUP_NO_INT_MULTI_CYCLE:
        setup_no_int_multi_cycle();
        break;
    
    case SETUP_FPU:
        setup_fpu();
        break;

    case SETUP_FPU_LAZY:
        setup_fpu_lazy();
        break;

    default:
        break;
    }
}

extern mstp_conf_t mstp_conf[2];
static uint8_t conf_index = 0;
extern uint8_t code_under_test;

void test_config(test_conf_t test_config, char world) {

    if (world == S_BARE || world == S_TFM) 
        conf_index = 1;
    else
        conf_index = 0;

    switch (test_config)
    {
    case NORMAL_INST:
        // ---------------------------------------------------------------------
        // if (world == NS)
        //     // Due to the way we are doing the tests, with a long stream of movs, that
        //     // will take 1 clk to execute each. We need to disable the streak detection.
                // mstp_conf[conf_index].streak_threshold  = -1; 
        // else 
        //     // We cannot disable the streak for the S world, because we have to
        //     // single step on code that wraps our victim. Put an high enough 
        //     // treshold to allow us to progress. This should be bigger then the
        //     // the number of normal_inst we want to single-step on the test 
        //     mstp_conf[conf_index].streak_threshold = 100;  
        // ---------------------------------------------------------------------
        // ---------------------------------------------------------------------
        // If we run only nops, we have to disable the streak detection, otherwise
        // the test will fail. But that is the expected behavior. That is the 
        // main purpose of the streak threshold.
        // If you wanna run the test only on atomic instructions, see commented 
        // code above
        // ---------------------------------------------------------------------
        mstp_conf[conf_index].streak_threshold      = STREAK_THRESHOLD; 
        mstp_conf[conf_index].print_iri_latency     = DISABLE_ICI_PRINT;    
        mstp_conf[conf_index].start_trace_window    = 0;
        mstp_conf[conf_index].end_trace_window      = (uint64_t) -1; // MAX_UINT64 
        mstp_conf[conf_index].trace_enable          = DISABLE_TRACE;
        mstp_conf[conf_index].mstp_cache_enabled    = DISABLE_MSTP_CACHE;
        break;
    case BASELINE:
    case ICI_INST:
    case IRI_INST:
    case GENERIC_FUNCTION_PRINTF:
        mstp_conf[conf_index].streak_threshold      = STREAK_THRESHOLD; 
        mstp_conf[conf_index].print_iri_latency     = DISABLE_ICI_PRINT;    
        mstp_conf[conf_index].start_trace_window    = 0;
        mstp_conf[conf_index].end_trace_window      = (uint64_t) -1; // MAX_UINT64 
        mstp_conf[conf_index].trace_enable          = DISABLE_TRACE;
        mstp_conf[conf_index].mstp_cache_enabled    = DISABLE_MSTP_CACHE;
        break;

    case TEST_DIV:
        mstp_conf[conf_index].streak_threshold      = STREAK_THRESHOLD; 
        mstp_conf[conf_index].print_iri_latency     = ENABLE_IRI_PRINT;     
        mstp_conf[conf_index].start_trace_window    = 0;
        mstp_conf[conf_index].end_trace_window      = (uint64_t) -1; // MAX_UINT64  
        mstp_conf[conf_index].trace_enable          = DISABLE_TRACE;
        mstp_conf[conf_index].mstp_cache_enabled    = DISABLE_MSTP_CACHE;
        break;

    case TEST_CACHE:
        mstp_conf[conf_index].streak_threshold      = STREAK_THRESHOLD; 
        mstp_conf[conf_index].print_iri_latency     = DISABLE_ICI_PRINT;    
        mstp_conf[conf_index].start_trace_window    = (uint64_t) -1; // Disable Normal trace
        mstp_conf[conf_index].end_trace_window      = (uint64_t) -1; // MAX_UINT64 
        mstp_conf[conf_index].trace_enable          = ENABLE_TRACE;
        mstp_conf[conf_index].mstp_cache_enabled    = ENABLE_MSTP_CACHE;
        break;
    case TEST_INST_DIFF:
    case TEST_CONT:
        mstp_conf[conf_index].streak_threshold      = -1; 
        mstp_conf[conf_index].print_iri_latency     = DISABLE_ICI_PRINT;    
        mstp_conf[conf_index].start_trace_window    = (uint64_t) -1; // Disable Normal trace
        mstp_conf[conf_index].end_trace_window      = (uint64_t) -1; // MAX_UINT64 
        mstp_conf[conf_index].trace_enable          = ENABLE_TRACE;
        mstp_conf[conf_index].mstp_cache_enabled    = DISABLE_MSTP_CACHE;
        break;
    
    default:
        break;
    }
}