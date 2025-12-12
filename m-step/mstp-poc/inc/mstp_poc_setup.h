#ifndef INC_MSTP_EVAL_H_
#define INC_MSTP_EVAL_H_

// General porpose configuration regarding the Hardware setup or firmware setup
typedef enum {
    SETUP_DEFAULT = 0,
} setup_conf_t;

// Specific configuration regarding the poc setup (e.g., specific configuration of
// M-Step)
typedef enum {
    POC_SETUP_DEFAULT = 0,
} poc_conf_t;

#define N_SETUPS (SETUP_DEFAULT + 1)

#define NS      0
#define S_BARE  1
#define S_TFM   2

void setup(setup_conf_t, char);
void poc_config(poc_conf_t, char);

#endif 
