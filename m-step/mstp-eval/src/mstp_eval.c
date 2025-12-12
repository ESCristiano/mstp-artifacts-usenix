#include "main.h"
#include "mstp.h"
#include "victims.h"
#include "mstp_eval.h"
#include "mstp_eval_setup.h"
#include "mstp_metrics.h"
#include "mstp_eval_tests.h"

extern uint32_t tfm_psa_framework_version_veneer(void);

void print_victim_names(uint8_t index, uint8_t word){
    switch (word){
        case NS:
            printf("Victim %d: %s\r\n", index, victim_names[index]);
            break;
        case S_BARE:
            printf("Victim %d: %s\r\n", index, victim_names_s[index]);
            break;
        case S_TFM:
            printf("Victim %d: %s\r\n", index, victim_names_s_tfm[index]);
            break;
        default:
            break;
    }
}

void trace_victim(uint8_t index, uint8_t word){
    switch (word){
        case NS:
            trace(victim[index]);
            break;
        case S_BARE:
            trace(victim_s[index]);
            break;
        case S_TFM:
            trace(victim_s_tfm[index]);
            break;
        default:
            break;
    }
}

void run_victim(uint8_t index, uint8_t word){
    switch (word){
        case NS:
            victim[index]();
            break;
        case S_BARE:
            victim_s[index]();
            break;
        case S_TFM:
            victim_s_tfm[index]();
            break;
        default:
            break;
    }
}

void run_all_victims(setup_conf_t setup_conf, uint8_t world){
    test_conf_t test_conf = NORMAL_INST; 

    setup(setup_conf, world);
    
    for (int i = 0; i < N_VICTIMS; i++){
        printf("\r\n");
        printf("- - - - - - - - - - - - - - - - - - - -\r\n");
        print_victim_names((uint8_t)i, world);
        printf("- - - - - - - - - - - - - - - - - - - -\r\n");
        test_config(test_conf++, world);
        trace_victim((uint8_t)i, world); 
    }

}

void run_all_setups_victims() {
    setup_conf_t setup_conf = SETUP_DEFAULT;

    for(uint8_t world = NS; world <= S_TFM; world++){
        for (; setup_conf <= 0; setup_conf++){
            setup(setup_conf, world);
            printf("---------------------------------------\r\n");
            printf("Setup %d\r\n", setup_conf);
            printf("---------------------------------------\r\n");
            run_all_victims(setup_conf, world);
        }
        setup_conf = SETUP_DEFAULT;
    }
}

void run_specific_setup_victim(setup_conf_t setup_conf, test_conf_t test_conf, uint8_t print, uint8_t world){ 
    setup(setup_conf, world);
    if(print){
        printf("---------------------------------------\r\n");
        printf("Setup %d\r\n", setup_conf);
        print_victim_names((uint8_t)test_conf, world);
        printf("---------------------------------------\r\n");
    }
    test_config(test_conf, world);
    trace_victim((uint8_t)test_conf, world); 
}

void run_victim_no_trace(setup_conf_t setup_conf, test_conf_t test_conf, uint8_t world){ 
    setup(setup_conf, world);
    printf("---------------------------------------\r\n");
    printf("Setup %d\r\n", setup_conf);
    print_victim_names((uint8_t)test_conf, world);
    printf("---------------------------------------\r\n");
    test_config(test_conf, world);
    run_victim((uint8_t)test_conf, world); 
}

// void ICache_invalidation(){
//     if (READ_BIT(ICACHE->SR, ICACHE_SR_BUSYF) != 0U) {
//         return -1;
//     }
//     else{
//         /* Make sure BSYENDF is reset before to start cache invalidation */
//         CLEAR_BIT(ICACHE->FCR, ICACHE_FCR_CBSYENDF);

//         /* Launch cache invalidation */
//         SET_BIT(ICACHE->CR, ICACHE_CR_CACHEINV);

//         while (READ_BIT(ICACHE->SR, ICACHE_SR_BSYENDF) == 0U);
//     }
//     /* Clear BSYENDF */
//     WRITE_REG(ICACHE->FCR, ICACHE_FCR_CBSYENDF);
// }
extern mstp_conf_t mstp_conf[2];

void mstp_eval(void) {    
    printf("####################################################################\r\n");
    printf("####################################################################\r\n");
    //--------------------------------------------------------------------------
    // Evaluation for USENIX SECURITY'26 Paper
    //--------------------------------------------------------------------------
    // test1_mstp_metrics_eval(10, NO_PRINT, S_TFM);
    // test2_div_instructions_eval(1, NO_PRINT, S_TFM);
    test3_instructions_diff(1, NO_PRINT, S_TFM);
    // test4_cache(1, NO_PRINT, S_TFM);
    // test5_memory_contention(1, NO_PRINT, NS);
    // test5_memory_contention(1, NO_PRINT, S_TFM);

    //--------------------------------------------------------------------------
    // run_specific_setup_victim(SETUP_DEFAULT, ICI_INST, PRINT, S_TFM);
}