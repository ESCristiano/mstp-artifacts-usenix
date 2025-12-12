#include "main.h"
#include "mstp.h"
#include "poc_001.h"
#include "mstp_poc_setup.h"

#define N_VICTIMS N_POC_001
#define MSTP_DISABLE    0
#define MSTP_ENABLE     1

void print_poc_names(uint8_t index, uint8_t word){
    printf("poc %d: %s\r\n", index, poc_names[index]);
}

void trace_poc(uint8_t index, uint8_t word){
    trace(poc[index]);
}

void run_poc(uint8_t index, uint8_t word){
    poc[index]();
}

void run_poc_trace(setup_conf_t setup_conf, uint8_t poc_id, uint8_t world){ 
    setup(setup_conf, world);
    printf("---------------------------------------\r\n");
    printf("Setup %d\r\n", setup_conf);
    print_poc_names(poc_id, world);
    printf("---------------------------------------\r\n");
    poc_config(POC_SETUP_DEFAULT, world);
    trace_poc(poc_id, world); 
}

void run_poc_no_trace(setup_conf_t setup_conf, uint8_t poc_id, uint8_t world){ 
    setup(setup_conf, world);
    printf("---------------------------------------\r\n");
    printf("Setup %d\r\n", setup_conf);
    print_poc_names(poc_id, world);
    printf("---------------------------------------\r\n");
    poc_config(POC_SETUP_DEFAULT, world);
    run_poc(poc_id , world); 
}

uint32_t volatile   *tim5_CR1   = (uint32_t *) 0x40000C00,
                    *tim5_SR    = (uint32_t *) 0x40000C10,
                    *tim5_DIER  = (uint32_t *) 0x40000C0C,
                    *tim5_EGR   = (uint32_t *) 0x40000C14,
                    *tim5_CNT   = (uint32_t *) 0x40000C24,
                    *tim5_ARR   = (uint32_t *) 0x40000C2C,
                    *tim5_CCR1  = (uint32_t *) 0x40000C34;
#include <inttypes.h>

#define UINT32_MAX 0xFFFFFFFF
uint64_t elapsed_time = 0; // Initialize to a large value
uint32_t count = 0;

void TIM5_IRQHandler(void)
{
    if (count++ != 0)
        printf("count: %u\r\n", count-1);

    *tim5_SR  = 0;
}

void print_uint64(uint64_t value) {
    if (value == 0) {
        printf("0");
        return;
    }
    
    uint32_t billions = (uint32_t)(value / 1000000000ULL);
    uint32_t remainder = (uint32_t)(value % 1000000000ULL);
    
    if (billions > 0) {
        printf("%u", billions);
        printf("%09u", remainder);  // Pad with zeros
    } else {
        printf("%u", remainder);
    }
}

extern void MX_TIM5_Init(void);

void config_measurement_timer(void) {
    static first_time = 0;
    if(!first_time++)
        MX_TIM5_Init();
    *tim5_CR1 &= ~(1<<0);
    *tim5_CNT = 0;
    *tim5_SR = 0;
    *tim5_DIER = 1<<0;
    *tim5_ARR = -1;
}

void start_timer(void) {
    config_measurement_timer();
    *tim5_CR1 |= (1<<0);
}

// Fix 2: Calculate elapsed time correctly
void stop_timer(uint8_t mstp) {
    *tim5_CR1 &= ~(1<<0);
    
    // Calculate total elapsed time
    // Ignore first interrupt it was due to initialization
    uint64_t total_time = ((uint64_t)(count-1) * UINT32_MAX) + *tim5_CNT;
    elapsed_time = total_time;
    
    print_time_metrics(mstp);
    elapsed_time = 0;
    count = 1; // Reset is 1 to ignore first interrupt due to initialization
}

// void print_time_metrics(uint8_t mstp) {
//     uint64_t time_ms = elapsed_time / 110000ULL;
//     uint64_t time_seconds = elapsed_time / 110000000ULL;
//     uint64_t time_min = elapsed_time / 6600000000ULL;
    
//     printf("Time: %llu clks\r\n", elapsed_time);
//     printf("Time: %llu ms\r\n", time_ms);
//     printf("Time: %llu seconds\r\n", time_seconds);
//     printf("Time: %llu minutes\r\n", time_min);
// }

void print_time_metrics(uint8_t mstp) {
    // Convert to double for calculations
    double elapsed_time_d = (double)elapsed_time;
    
    // Convert to time units (clock is 110MHz
    double time_ms = elapsed_time_d / 110000.0;
    double time_seconds = elapsed_time_d / 110000000.0;
    double time_min = elapsed_time_d / 6600000000.0;
    
    printf("---------------------------------------\r\n");
    printf("Time: ");
    print_uint64(elapsed_time);
    printf(" clks\r\n");

    // Print as integers by casting
    // I'm multuplying by 5 because I measure the actuall real time, and I took
    // at lest 5x more, and I'm not getting why this conversions are not working
    // are the freq really 110MHz?
    printf("Time: %u ms\r\n", (uint32_t)time_ms*5);
    printf("Time: %u seconds\r\n", (uint32_t)time_seconds*5);
    printf("Time: %u minutes\r\n", (uint32_t)time_min*5);
    printf("count: %u \r\n", count-1);

    //     if(mstp != MSTP_DISABLE){
//         // Give an estimation of the time with M-Step if we just measured the 
//         // execution time without it. Around 10000x worse than base line
//         time_ms = elapsed_time / 11;
//         time_seconds = elapsed_time / 11000;
//         time_min = elapsed_time / 660000ULL;
//         printf("Estimation with Mstp\r\n");
//         printf("Time: "); print_uint64(elapsed_time); printf(" clks\r\n");
//         printf("Time: %u ms\r\n", time_ms);
//         printf("Time: %u seconds\r\n", time_seconds);
//         printf("Time: %u minutes\r\n", time_min);
//         printf("count: %u \r\n", count);
//         printf("---------------------------------------\r\n");
//     }
    
}

// void run_poc_get_timing(setup_conf_t setup_conf, uint8_t poc_id, uint8_t world){ 
//     setup(setup_conf, world);
//     printf("---------------------------------------\r\n");
//     printf("Setup %d\r\n", setup_conf);
//     print_poc_names(poc_id, world);
//     printf("---------------------------------------\r\n");
    
//     // Measure the poc execution time
//     start_timer();
//     run_poc(poc_id, world);     
//     print_time_metrics();    
// }

extern mstp_conf_t mstp_conf;

void mstp_poc(void) {    
    printf("####################################################################\r\n");
    
    // run_poc_trace(SETUP_DEFAULT, POC_SETUP_DEFAULT, S_TFM);
    // run_poc_no_trace(SETUP_DEFAULT, POC_001_RSA_GEN, S_TFM);
    // // 209777209 -> 1s
    
    // start_timer();
    // run_poc_no_trace(SETUP_DEFAULT, POC_001_RSA_ENC_DEC, S_TFM);
    // stop_timer(MSTP_DISABLE);
    
    // // Takes around 20 min I think in this approach, and I'm even printing 
    // start_timer();
    // run_poc_trace(SETUP_DEFAULT, POC_001_RSA_ENC_DEC, S_TFM);
    // stop_timer(MSTP_ENABLE);
    
    // start_timer();
    // run_poc_no_trace(SETUP_DEFAULT, POC_001_RSA_GEN, S_TFM);
    // stop_timer(MSTP_DISABLE);

    // start_timer();
    // run_poc_trace(SETUP_DEFAULT, POC_001_RSA_GEN, S_TFM);
    // stop_timer(MSTP_ENABLE);
    
    // start_timer();
    // run_poc_trace(SETUP_DEFAULT, POC_001_RSA_ENC_DEC, S_TFM);
    // stop_timer(MSTP_ENABLE);

    // start_timer();
    // run_poc_no_trace(SETUP_DEFAULT, POC_001_RSA_ENCRYPTION, S_TFM);
    // stop_timer(MSTP_DISABLE);

    // start_timer();
    // run_poc_no_trace(SETUP_DEFAULT, POC_001_RSA_DECRYPTION, S_TFM);
    // stop_timer(MSTP_DISABLE);

    // start_timer();
    // run_poc_no_trace(SETUP_DEFAULT, POC_001_RSA_GEN, S_TFM);
    // stop_timer(MSTP_DISABLE);

    start_timer();
    run_poc_no_trace(SETUP_DEFAULT, POC_001_RSA_GEN, S_TFM);
    stop_timer(MSTP_DISABLE);

    start_timer();
    run_poc_trace(SETUP_DEFAULT, POC_001_RSA_GEN, S_TFM);
    stop_timer(MSTP_DISABLE);

    // run_poc_no_trace(SETUP_DEFAULT, POC_001_RSA_DECRYPTION, S_TFM);


    // config_measurement_timer();
    // run_poc_no_trace(SETUP_DEFAULT, POC_001_RSA_GEN, S_TFM);
    // run_poc_no_trace(SETUP_DEFAULT, POC_001_RSA_GEN, S_TFM);
    // run_poc_no_trace(SETUP_DEFAULT, POC_001_RSA_GEN, S_TFM);
    // run_poc_no_trace(SETUP_DEFAULT, POC_001_RSA_GEN, S_TFM);
    // run_poc_no_trace(SETUP_DEFAULT, POC_001_RSA_GEN, S_TFM);
    // run_poc_no_trace(SETUP_DEFAULT, POC_001_RSA_GEN, S_TFM);
    // run_poc_no_trace(SETUP_DEFAULT, POC_001_RSA_GEN, S_TFM);

    // start_timer();
    // run_poc_trace(SETUP_DEFAULT, POC_001_RSA_GEN, S_TFM);
    // stop_timer();
    // print_time_metrics();
    //run_poc_get_timing(SETUP_DEFAULT, POC_001_RSA_ENC_DEC, S_TFM);
}