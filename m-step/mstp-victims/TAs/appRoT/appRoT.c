/*
 * Copyright (c) 2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdint.h>
#include "psa/service.h"
#include "psa_manifest/appRoT.h"
#include "tfm_sp_log.h"
#include <assert.h>
#include "cmsis.h"
#include "victims.h"

int var = 0xff;

static void mstp_eval_normal_inst(void)
{
    psa_status_t status;
    psa_msg_t msg;
    uint8_t x; //needs to be a buffer (or tf-m will fail)
    int i = 0; // Initialize i
    /* Retrieve the message corresponding to the example service signal */
    status = psa_get(MSTP_EVAL_NORMAL_INST_SIGNAL, &msg);
    if (status != PSA_SUCCESS) {
        return;
    }

    /* Decode the message */
    switch (msg.type) {
    case PSA_IPC_CALL:
        if (msg.in_size[i] != 0) {
                psa_read(msg.handle, 0, &x, 1);
            } 
        // LOG_INFFMT("mstp_eval_normal_inst\n\r");
        victim_normal_inst();
        status = PSA_SUCCESS;
        break;
    default:
        /* Invalid message type */
        status = PSA_ERROR_PROGRAMMER_ERROR;
        break;
    }
    /* Reply with the message result status to unblock the client */
    psa_reply(msg.handle, status);
}

static void mstp_eval_ici_inst(void)
{
    psa_status_t status;
    psa_msg_t msg;
    uint8_t x; //needs to be a buffer (or tf-m will fail)
    int i = 0; // Initialize i
    /* Retrieve the message corresponding to the example service signal */
    status = psa_get(MSTP_EVAL_ICI_INST_SIGNAL, &msg);
    if (status != PSA_SUCCESS) {
        return;
    }

    /* Decode the message */
    switch (msg.type) {
    case PSA_IPC_CALL:
        if (msg.in_size[i] != 0) {
                psa_read(msg.handle, 0, &x, 1);
            } 
        // LOG_INFFMT("mstp_eval_ici_inst\n\r");
        victim_ICI_inst();
        status = PSA_SUCCESS;
        break;
    default:
        /* Invalid message type */
        status = PSA_ERROR_PROGRAMMER_ERROR;
        break;
    }
    /* Reply with the message result status to unblock the client */
    psa_reply(msg.handle, status);
}

static void mstp_eval_iri_inst(void)
{
    psa_status_t status;
    psa_msg_t msg;
    uint8_t x; //needs to be a buffer (or tf-m will fail)
    int i = 0; // Initialize i
    /* Retrieve the message corresponding to the example service signal */
    status = psa_get(MSTP_EVAL_IRI_INST_SIGNAL, &msg);
    if (status != PSA_SUCCESS) {
        return;
    }

    /* Decode the message */
    switch (msg.type) {
    case PSA_IPC_CALL:
        if (msg.in_size[i] != 0) {
                psa_read(msg.handle, 0, &x, 1);
            } 
        // LOG_INFFMT("mstp_eval_iri_inst\n\r");
        victim_IRI_inst();
        status = PSA_SUCCESS;
        break;
    default:
        /* Invalid message type */
        status = PSA_ERROR_PROGRAMMER_ERROR;
        break;
    }
    /* Reply with the message result status to unblock the client */
    psa_reply(msg.handle, status);
}

static void mstp_eval_iri_atomic_inst(void)
{
    psa_status_t status;
    psa_msg_t msg;
    uint8_t x; //needs to be a buffer (or tf-m will fail)
    int i = 0; // Initialize i
    /* Retrieve the message corresponding to the example service signal */
    status = psa_get(MSTP_EVAL_IRI_ATOMIC_INST_SIGNAL, &msg);
    if (status != PSA_SUCCESS) {
        return;
    }

    /* Decode the message */
    switch (msg.type) {
    case PSA_IPC_CALL:
        if (msg.in_size[i] != 0) {
                psa_read(msg.handle, 0, &x, 1);
            } 
        victim_IRI_ATOMIC_inst();
        status = PSA_SUCCESS; // Add missing status assignment
        break;
    default:
        /* Invalid message type */
        status = PSA_ERROR_PROGRAMMER_ERROR;
        break;
    }
    /* Reply with the message result status to unblock the client */
    psa_reply(msg.handle, status);
}

static void mstp_eval_empty(void)
{
    psa_status_t status;
    psa_msg_t msg;
    uint8_t x; //needs to be a buffer (or tf-m will fail)
    int i = 0; // Initialize i
    /* Retrieve the message corresponding to the example service signal */
    status = psa_get(MSTP_EVAL_EMPTY_SIGNAL, &msg);
    if (status != PSA_SUCCESS) {
        return;
    }

    /* Decode the message */
    switch (msg.type) {
    case PSA_IPC_CALL:
        if (msg.in_size[i] != 0) {
                psa_read(msg.handle, 0, &x, 1);
            } 
        victim_empty();
        status = PSA_SUCCESS;
        break;
    default:
        /* Invalid message type */
        status = PSA_ERROR_PROGRAMMER_ERROR;
        break;
    }
    /* Reply with the message result status to unblock the client */
    psa_reply(msg.handle, status);
}

static void mstp_eval_printf(void)
{
    psa_status_t status;
    psa_msg_t msg;
    uint8_t x; //needs to be a buffer (or tf-m will fail)
    int i = 0; // Initialize i
    /* Retrieve the message corresponding to the example service signal */
    status = psa_get(MSTP_EVAL_PRINTF_SIGNAL, &msg);
    if (status != PSA_SUCCESS) {
        return;
    }

    /* Decode the message */
    switch (msg.type) {
    case PSA_IPC_CALL:
        if (msg.in_size[i] != 0) {
                psa_read(msg.handle, 0, &x, 1);
            } 
        victim_printf();
        status = PSA_SUCCESS;
        break;
    default:
        /* Invalid message type */
        status = PSA_ERROR_PROGRAMMER_ERROR;
        break;
    }
    /* Reply with the message result status to unblock the client */
    psa_reply(msg.handle, status);
}

static void mstp_eval_div_inst(void)
{
    psa_status_t status;
    psa_msg_t msg;
    uint8_t x; //needs to be a buffer (or tf-m will fail)
    int i = 0; // Initialize i
    /* Retrieve the message corresponding to the example service signal */
    status = psa_get(MSTP_EVAL_DIV_INS_SIGNAL, &msg);
    if (status != PSA_SUCCESS) {
        return;
    }

    /* Decode the message */
    switch (msg.type) {
    case PSA_IPC_CALL:
        if (msg.in_size[i] != 0) {
                psa_read(msg.handle, 0, &x, 1);
            } 
        victim_eval_div_inst();
        status = PSA_SUCCESS; // Add missing status assignment
        break;
    default:
        /* Invalid message type */
        status = PSA_ERROR_PROGRAMMER_ERROR;
        break;
    }
    /* Reply with the message result status to unblock the client */
    psa_reply(msg.handle, status);
}

static void mstp_eval_inst_diff(void)
{
    psa_status_t status;
    psa_msg_t msg;
    uint8_t victim = 0; 
    int i = 0; // Initialize i
    /* Retrieve the message corresponding to the example service signal */
    status = psa_get(MSTP_EVAL_INST_DIFF_SIGNAL, &msg);
    if (status != PSA_SUCCESS) {
        return;
    }

    /* Decode the message */
    switch (msg.type) {
    case PSA_IPC_CALL:
        if (msg.in_size[i] != 0) {
            psa_read(msg.handle, 0, &victim, 1);
        } 
        victim_eval_inst_diff_mov();
        victim_eval_inst_diff_ldr_sram1();
        victim_eval_inst_diff_ldr_periph();
        victim_eval_inst_diff_ldr_bl();
        status = PSA_SUCCESS;
        break;
    default:
        /* Invalid message type */
        status = PSA_ERROR_PROGRAMMER_ERROR;
        break;
    }
    /* Reply with the message result status to unblock the client */
    psa_reply(msg.handle, status);
}

static void mstp_eval_cache(void)
{
    psa_status_t status;
    psa_msg_t msg;
    uint8_t x; //needs to be a buffer (or tf-m will fail)
    int i = 0; // Initialize i
    /* Retrieve the message corresponding to the example service signal */
    status = psa_get(MSTP_EVAL_CACHE_SIGNAL, &msg);
    if (status != PSA_SUCCESS) {
        return;
    }

    /* Decode the message */
    switch (msg.type) {
    case PSA_IPC_CALL:
        if (msg.in_size[i] != 0) {
                psa_read(msg.handle, 0, &x, 1);
            } 
        victim_eval_cache_mov_hit();
        victim_eval_cache_mov_miss();
        victim_eval_cache_ldr_flash_hit();
        victim_eval_cache_ldr_flash_miss();
        victim_eval_cache_ldr_sram1_hit();
        victim_eval_cache_ldr_sram1_miss();
        status = PSA_SUCCESS; // Add missing status assignment
        break;
    default:
        /* Invalid message type */
        status = PSA_ERROR_PROGRAMMER_ERROR;
        break;
    }
    /* Reply with the message result status to unblock the client */
    psa_reply(msg.handle, status);
}

static void mstp_eval_cont(void)
{
    psa_status_t status;
    psa_msg_t msg;
    uint8_t x; //needs to be a buffer (or tf-m will fail)
    int i = 0; // Initialize i
    /* Retrieve the message corresponding to the example service signal */
    status = psa_get(MSTP_EVAL_CONTENTION_SIGNAL, &msg);
    if (status != PSA_SUCCESS) {
        return;
    }

    /* Decode the message */
    switch (msg.type) {
    case PSA_IPC_CALL:
        if (msg.in_size[i] != 0) {
                psa_read(msg.handle, 0, &x, 1);
            } 
        victim_eval_cont();
        status = PSA_SUCCESS; // Add missing status assignment
        break;
    default:
        /* Invalid message type */
        status = PSA_ERROR_PROGRAMMER_ERROR;
        break;
    }
    /* Reply with the message result status to unblock the client */
    psa_reply(msg.handle, status);
}

/**
 * \brief The example FFM-1.1 partition's entry function.
 */
void appRoT_main(void)
{
    psa_signal_t signals;

    LOG_INFFMT("appRoT_main-----------------############\n\r");

    while (1) {
        signals = psa_wait(PSA_WAIT_ANY, PSA_BLOCK);
        if (signals & MSTP_EVAL_NORMAL_INST_SIGNAL) {
            mstp_eval_normal_inst();   
        } else if (signals & MSTP_EVAL_ICI_INST_SIGNAL) {
            mstp_eval_ici_inst();    
        } else if (signals & MSTP_EVAL_IRI_INST_SIGNAL) {
            mstp_eval_iri_inst();   
        } else if (signals & MSTP_EVAL_IRI_ATOMIC_INST_SIGNAL) {
            mstp_eval_iri_atomic_inst(); 
        } else if (signals & MSTP_EVAL_PRINTF_SIGNAL) {
            mstp_eval_printf();    
        } else if (signals & MSTP_EVAL_EMPTY_SIGNAL) {
            mstp_eval_empty(); 
        } else if (signals & MSTP_EVAL_DIV_INS_SIGNAL) {
            mstp_eval_div_inst(); 
        } else if (signals & MSTP_EVAL_INST_DIFF_SIGNAL) {
            mstp_eval_inst_diff(); 
        } else if (signals & MSTP_EVAL_CACHE_SIGNAL) {
            mstp_eval_cache(); 
        } else if (signals & MSTP_EVAL_CONTENTION_SIGNAL) {
            mstp_eval_cont(); 
        } else{
            /* Should not come here */
            psa_panic();
        }
    }
}

// static void mstp_eval_inst_diff(void)
// {
//     psa_status_t status;
//     psa_msg_t msg;
//     uint8_t victim = 0; 
//     int i = 0; // Initialize i
//     /* Retrieve the message corresponding to the example service signal */
//     status = psa_get(MSTP_EVAL_INST_DIFF_SIGNAL, &msg);
//     if (status != PSA_SUCCESS) {
//         return;
//     }

//     /* Decode the message */
//     switch (msg.type) {
//     case PSA_IPC_CALL:
//         if (msg.in_size[i] != 0) {
//             psa_read(msg.handle, 0, &victim, 1);
//         } 
//         printf("# Victim ---------------------------- %d\r\n", victim);
        
//         // Call appropriate victim function based on input parameter
//         switch (victim) {
//             case 0:
//                 victim_eval_inst_diff_mov();
//                 break;
//             case 1:
//                 victim_eval_inst_diff_ldr_sram1();
//                 break;
//             case 2:
//                 victim_eval_inst_diff_ldr_periph();
//                 break;
//             case 3:
//                 victim_eval_inst_diff_ldr_bl();
//                 break;
//             default:
//                 printf("Invalid victim function index: %d\r\n", victim);
//                 break;
//         }
//         status = PSA_SUCCESS;
//         break;
//     default:
//         /* Invalid message type */
//         status = PSA_ERROR_PROGRAMMER_ERROR;
//         break;
//     }
//     /* Reply with the message result status to unblock the client */
//     psa_reply(msg.handle, status);
// }