/*
 * Copyright (c) 2017-2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "test_app.h"
#include "tfm_log.h"
#include "os_wrapper/thread.h"
#include "psa/crypto.h"


void generate_rsa_key_pair()
{
    mbedtls_svc_key_id_t key = MBEDTLS_SVC_KEY_ID_INIT;
    psa_status_t status;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;

    psa_set_key_type(&attributes, PSA_KEY_TYPE_RSA_KEY_PAIR); 
    psa_set_key_bits(&attributes, 1024);
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_SIGN_HASH);
    psa_set_key_algorithm(&attributes, PSA_ALG_RSA_PKCS1V15_SIGN_RAW);
    psa_set_key_enrollment_algorithm(&attributes, PSA_ALG_NONE);

    status = psa_generate_key(&attributes, &key);
    
    if (status != PSA_SUCCESS) {
        printf("Failed to generate RSA key pair: %d\n\r", status);
        return;
    }

    printf("RSA key pair generated successfully.\n\r");
}

/**
 * \brief Services test thread
 *
 */
void single_step_main()
{
    /* Output EOT char for test environments like FVP. */
    LOG_MSG("Test If I'm Alive \n\r");

    generate_rsa_key_pair();
}
