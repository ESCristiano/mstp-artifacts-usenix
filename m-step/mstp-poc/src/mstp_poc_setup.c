
#include "main.h"
#include "mstp.h"
#include "mstp_poc_setup.h"

extern void MX_TIM3_Init(void);
extern void MX_TIM2_Init(void);
extern void MX_ICACHE_Init(void);
extern void MX_ICACHE_DeInit(void);

void setup_default(){
    MX_TIM3_Init();
    MX_TIM2_Init();
    MX_ICACHE_Init();
}

void setup(setup_conf_t config, char world) {
    switch (config)
    {
    case SETUP_DEFAULT:
        setup_ns_mpu(); 
        setup_default();
        break; 

    default:
        break;
    }
}

extern mstp_conf_t mstp_conf;

void poc_config(poc_conf_t poc_config, char world) {
    switch (poc_config)
    {
    case POC_SETUP_DEFAULT:
        mstp_conf.streak_threshold = STREAK_THRESHOLD;      
        break;
    default:
        break;
    }
}