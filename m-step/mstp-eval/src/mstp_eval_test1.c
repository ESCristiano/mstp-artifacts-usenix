#include "main.h"
#include "mstp.h"
#include "victims.h"
#include "mstp_eval.h"
#include "mstp_eval_setup.h"
#include "mstp_metrics.h"
#include "mstp_eval_tests.h"

#define N_EXPERIMENTS 6

extern mstp_metrics_t mstp_metrics;
mstp_metrics_t metrics_tmp = {0};
mstp_metrics_t metrics_eval[N_EXPERIMENTS] = {0};

//------------------------------------------------------------------------------
// Test#1: Mstp Metrics Evaluation
//------------------------------------------------------------------------------
void run_test1_n_times(setup_conf_t setup_conf, test_conf_t test_conf, uint16_t runs, uint8_t world){ 
    
    metrics_tmp.zero_steps          = 0;
    metrics_tmp.partial_steps       = 0;
    metrics_tmp.single_steps        = 0;
    metrics_tmp.instruction_folding = 0;
    metrics_tmp.multi_steps         = 0;
    metrics_tmp.interrupts          = 0;

    for(uint16_t i = 0; i < runs; i++){
        if (!i){
            run_specific_setup_victim(setup_conf, test_conf, PRINT, world);
            printf("R#%d\r\n", i);
        }
        else{
            printf("R#%d\r\n", i);
            run_specific_setup_victim(setup_conf, test_conf, NO_PRINT, world);
        }   
        // Remove the zero-steps that were partial steps
        metrics_tmp.zero_steps            += (mstp_metrics.zero_steps-mstp_metrics.partial_steps) < 0 ? 0 : (mstp_metrics.zero_steps-mstp_metrics.partial_steps);
        metrics_tmp.partial_steps         += mstp_metrics.partial_steps;
        metrics_tmp.single_steps          += mstp_metrics.single_steps;
        metrics_tmp.instruction_folding   += mstp_metrics.instruction_folding;
        metrics_tmp.multi_steps           += mstp_metrics.multi_steps;
        metrics_tmp.interrupts            += mstp_metrics.interrupts;
        clear_metrics();
    }
    metrics_tmp.zero_steps          /= runs;
    metrics_tmp.partial_steps       /= runs;
    metrics_tmp.single_steps        /= runs;
    metrics_tmp.instruction_folding /= runs;
    metrics_tmp.multi_steps         /= runs;
    metrics_tmp.interrupts          /= runs;
}

void print_test1_results( test_conf_t test, uint16_t runs) {
    printf("\r\n");
    int32_t zero_diff = metrics_eval[test].zero_steps - metrics_eval[BASELINE].zero_steps;
    int32_t partial_diff = metrics_eval[test].partial_steps - metrics_eval[BASELINE].partial_steps;
    int32_t single_diff = metrics_eval[test].single_steps - metrics_eval[BASELINE].single_steps;
    int32_t folding_diff = metrics_eval[test].instruction_folding - metrics_eval[BASELINE].instruction_folding;
    int32_t multi_diff = metrics_eval[test].multi_steps - metrics_eval[BASELINE].multi_steps;
    int32_t interrupts_diff = metrics_eval[test].interrupts - metrics_eval[BASELINE].interrupts;
    
    printf("Zero Steps    (Avg %d runs): %8d\t%8d\r\n", 
        runs, 
        metrics_eval[BASELINE].zero_steps,
        zero_diff < 0 ? 0 : zero_diff
    );
    printf("Partial Steps (Avg %d runs): %8d\t%8d\r\n", 
        runs, 
        metrics_eval[BASELINE].partial_steps,
        partial_diff < 0 ? 0 : partial_diff
    );
    printf("Single Steps  (Avg %d runs): %8d\t%8d\r\n", 
        runs, 
        metrics_eval[BASELINE].single_steps,
        single_diff < 0 ? 0 : single_diff
    );
    printf("Inst Folding  (Avg %d runs): %8d\t%8d\r\n", 
        runs, 
        metrics_eval[BASELINE].instruction_folding,
        folding_diff < 0 ? 0 : folding_diff
    );
    printf("Multi Steps   (Avg %d runs): %8d\t%8d\r\n", 
        runs, 
        metrics_eval[BASELINE].multi_steps,
        multi_diff < 0 ? 0 : multi_diff
    );
    printf("Interrupts    (Avg %d runs): %8d\t%8d\r\n", 
        runs, 
        metrics_eval[BASELINE].interrupts,
        interrupts_diff < 0 ? 0 : interrupts_diff
    );
}

uint32_t ratio_steps_interrupts(uint32_t interrupts, uint32_t steps) {
    if (interrupts == 0 || steps <= 0) {
        return 0;
    }
    return (steps * 1000) / interrupts;  // Returns ratio * 1000 for 3 decimal precision
}

void print_final_results(uint16_t runs) {
    printf("\r\n");
    mstp_metrics_t metrics_relative[N_EXPERIMENTS] = {0};
    uint32_t total_steps[N_EXPERIMENTS] = {0};

    for(int i = 0; i < N_EXPERIMENTS; i++) {
        if(i != BASELINE) {
            // Ensure no negative values by using max(0, difference)
            int32_t zero_diff = metrics_eval[i].zero_steps - metrics_eval[BASELINE].zero_steps;
            int32_t partial_diff = metrics_eval[i].partial_steps - metrics_eval[BASELINE].partial_steps;
            int32_t single_diff = metrics_eval[i].single_steps - metrics_eval[BASELINE].single_steps;
            int32_t folding_diff = metrics_eval[i].instruction_folding - metrics_eval[BASELINE].instruction_folding;
            int32_t multi_diff = metrics_eval[i].multi_steps - metrics_eval[BASELINE].multi_steps;
            int32_t interrupts_diff = metrics_eval[i].interrupts - metrics_eval[BASELINE].interrupts;
            
            metrics_relative[i].zero_steps          = zero_diff < 0 ? 0 : zero_diff;
            metrics_relative[i].partial_steps       = partial_diff < 0 ? 0 : partial_diff;
            metrics_relative[i].single_steps        = single_diff < 0 ? 0 : single_diff;
            metrics_relative[i].instruction_folding = folding_diff < 0 ? 0 : folding_diff;
            metrics_relative[i].multi_steps         = multi_diff < 0 ? 0 : multi_diff;
            metrics_relative[i].interrupts          = interrupts_diff < 0 ? 0 : interrupts_diff;
        }
        else
            metrics_relative[i] = metrics_eval[i];
        total_steps[i] = metrics_relative[i].partial_steps + metrics_relative[i].single_steps;
    }

    printf("                             : %8s\t%8s\t%8s\t%8s\t%8s\t%8s\r\n",
           "Base",
           "Atomic", 
           "ICI",
           "IRI",
           "IRI_ATOM",
           "Printf"
    );

    printf("Zero Steps    (Avg %4d runs): %8d\t%8d\t%8d\t%8d\t%8d\t%8d\r\n", 
        runs, 
        metrics_relative[BASELINE].zero_steps,
        metrics_relative[NORMAL_INST].zero_steps,
        metrics_relative[ICI_INST].zero_steps,
        metrics_relative[IRI_INST].zero_steps,
        metrics_relative[IRI_ATOMIC_INST].zero_steps,
        metrics_relative[GENERIC_FUNCTION_PRINTF].zero_steps
    );
    printf("Partial Steps (Avg %4d runs): %8d\t%8d\t%8d\t%8d\t%8d\t%8d\r\n", 
        runs, 
        metrics_relative[BASELINE].partial_steps,
        metrics_relative[NORMAL_INST].partial_steps,
        metrics_relative[ICI_INST].partial_steps,
        metrics_relative[IRI_INST].partial_steps,
        metrics_relative[IRI_ATOMIC_INST].partial_steps,
        metrics_relative[GENERIC_FUNCTION_PRINTF].partial_steps
    );
    printf("Single Steps  (Avg %4d runs): %8d\t%8d\t%8d\t%8d\t%8d\t%8d\r\n", 
        runs, 
        metrics_relative[BASELINE].single_steps,
        metrics_relative[NORMAL_INST].single_steps,
        metrics_relative[ICI_INST].single_steps,
        metrics_relative[IRI_INST].single_steps,
        metrics_relative[IRI_ATOMIC_INST].single_steps,
        metrics_relative[GENERIC_FUNCTION_PRINTF].single_steps
    );
    printf("Inst Folding  (Avg %4d runs): %8d\t%8d\t%8d\t%8d\t%8d\t%8d\r\n", 
        runs, 
        metrics_relative[BASELINE].instruction_folding,
        metrics_relative[NORMAL_INST].instruction_folding,
        metrics_relative[ICI_INST].instruction_folding,
        metrics_relative[IRI_INST].instruction_folding,
        metrics_relative[IRI_ATOMIC_INST].instruction_folding,
        metrics_relative[GENERIC_FUNCTION_PRINTF].instruction_folding
    );
    printf("Multi Steps   (Avg %4d runs): %8d\t%8d\t%8d\t%8d\t%8d\t%8d\r\n", 
        runs, 
        metrics_relative[BASELINE].multi_steps,
        metrics_relative[NORMAL_INST].multi_steps,
        metrics_relative[ICI_INST].multi_steps,
        metrics_relative[IRI_INST].multi_steps,
        metrics_relative[IRI_ATOMIC_INST].multi_steps,
        metrics_relative[GENERIC_FUNCTION_PRINTF].multi_steps
    );
    printf("Interrupts    (Avg %4d runs): %8d\t%8d\t%8d\t%8d\t%8d\t%8d\r\n", 
        runs, 
        metrics_relative[BASELINE].interrupts,
        metrics_relative[NORMAL_INST].interrupts,
        metrics_relative[ICI_INST].interrupts,
        metrics_relative[IRI_INST].interrupts,
        metrics_relative[IRI_ATOMIC_INST].interrupts,
        metrics_relative[GENERIC_FUNCTION_PRINTF].interrupts
    );
    
    printf("Ratio         (Avg %4d runs): %8d\t%8d\t%8d\t%8d\t%8d\t%8d\r\n", 
        runs,
        ratio_steps_interrupts( metrics_relative[BASELINE].interrupts, total_steps[BASELINE]),
        ratio_steps_interrupts(metrics_relative[NORMAL_INST].interrupts, total_steps[NORMAL_INST]),
        ratio_steps_interrupts(metrics_relative[ICI_INST].interrupts, total_steps[ICI_INST]),
        ratio_steps_interrupts(metrics_relative[IRI_INST].interrupts, total_steps[IRI_INST]),
        ratio_steps_interrupts(metrics_relative[IRI_ATOMIC_INST].interrupts, total_steps[IRI_ATOMIC_INST]),
        ratio_steps_interrupts(metrics_relative[GENERIC_FUNCTION_PRINTF].interrupts, total_steps[GENERIC_FUNCTION_PRINTF])
    );
}

void test1_mstp_metrics_eval(uint16_t n_runs, uint8_t print, uint8_t world) {   
    // Define baseline configuration 
    run_test1_n_times(SETUP_DEFAULT, BASELINE, 1, world);
    metrics_eval[BASELINE] = metrics_tmp;
    if(print)
        print_test1_results(BASELINE, n_runs);
    
    // Run the actual tests
    run_test1_n_times(SETUP_DEFAULT, NORMAL_INST, n_runs, world);
    metrics_eval[NORMAL_INST] = metrics_tmp;
    if(print)
        print_test1_results(NORMAL_INST, n_runs);
    
    run_test1_n_times(SETUP_DEFAULT, ICI_INST, n_runs, world);
    metrics_eval[ICI_INST] = metrics_tmp;
    if(print)
        print_test1_results(ICI_INST, n_runs);
    
    run_test1_n_times(SETUP_DEFAULT, IRI_INST, n_runs, world);
    metrics_eval[IRI_INST] = metrics_tmp;
    if(print)
        print_test1_results(IRI_INST, n_runs);

    run_test1_n_times(SETUP_DEFAULT, IRI_ATOMIC_INST, n_runs, world);
    metrics_eval[IRI_ATOMIC_INST] = metrics_tmp;
    if(print)
        print_test1_results(IRI_ATOMIC_INST, n_runs);
    
    run_test1_n_times(SETUP_DEFAULT, GENERIC_FUNCTION_PRINTF, n_runs, world);
    metrics_eval[GENERIC_FUNCTION_PRINTF] = metrics_tmp;
    if(print)
        print_test1_results(GENERIC_FUNCTION_PRINTF, n_runs);

    print_final_results(n_runs);
}
