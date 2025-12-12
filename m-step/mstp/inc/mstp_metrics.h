#ifndef INC_M_STEP_METRICS_H_
#define INC_M_STEP_METRICS_H_

#include <stdint.h>
#include "mstp.h"
#include "armv8_decop_lib.h"

void process_metrics_debug(uint32_t, mstp_ctx_t *, mstp_conf_t , lr_t *);
// void process_metrics_production(uint32_t, mstp_ctx_t *, lr_t *);
void print_metrics();
void clear_metrics();

typedef struct {
    uint32_t zero_steps;
    uint32_t partial_steps;
    uint32_t single_steps;
    uint32_t instruction_folding;
    uint32_t multi_steps;
    uint32_t interrupts;
} mstp_metrics_t;

#endif /* INC_M_STEP_H_ */
