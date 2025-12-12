#ifndef INC_MSTP_EVAL_SETUP_H_
#define INC_MSTP_EVAL_SETUP_H_

// General porpose configuration regarding the Hardware setup or firmware setup
typedef enum {
    SETUP_DEFAULT, // cache is enabled, FPU is disabled
    SETUP_DEFAULT_NO_CACHE, // cache is disabled, FPU is disabled
    SETUP_NO_INT_MULTI_CYCLE, // No ICI/IRI instructions, all 3 types are atomic
    SETUP_FPU,     // cache is enabled, FPU is enabled, Lazy stacking disabled
    SETUP_FPU_LAZY // cache is enabled, FPU is enabled, Lazy stacking enabled
} setup_conf_t;

// Specific configuration regarding the test setup (e.g., specific configuration of
// M-Step)
typedef enum {
    BASELINE,
    NORMAL_INST, 
    ICI_INST,     
    IRI_INST,
    IRI_ATOMIC_INST,
    GENERIC_FUNCTION_PRINTF,
    TEST_DIV,
    TEST_INST_DIFF,
    TEST_CACHE,
    TEST_CONT,
    END_MARKER
} test_conf_t;

#define N_SETUPS (SETUP_FPU_LAZY + 1)
#define N_TEST_CONFIG (END_MARKER)

#define NS      0
#define S_TFM   1
#define S_BARE  2

void setup(setup_conf_t, char);
void test_config(test_conf_t, char);

#endif 
