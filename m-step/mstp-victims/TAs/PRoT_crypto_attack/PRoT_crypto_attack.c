/*
* Copyright (c) 2021, Arm Limited. All rights reserved.
*
* SPDX-License-Identifier: BSD-3-Clause
*
*/

#include <stdint.h>
#include <string.h>
#include "psa/service.h"
#include "psa_manifest/PRoT_crypto_attack.h"
#include "tfm_sp_log.h"
#include <assert.h>
#include "cmsis.h"
// #include "psa/crypto.h"

// // Maximum size for RSA-1024 ciphertext
// #define MAX_CIPHERTEXT_SIZE 256

// // mbedtls_rsa_gen_key: RSA key size 1024
// // n = 0xa35faa91956ce746a67fd26d3da1c8aea8febe735da0f58c4af70385a22065d8924e6f52663e4acaa6d175cc372689ae5ad855f7fd0e4d8979a739ef86e425a4f05fc9e460217509de35e42a3745ff01099d414b168f6443a879a6fca23d232e22d99325343578d53556c458ebe42415a14644fc01dee266f0485ed6255e247b
// // e = 0x010001
// // d = 0x0833e4f2035120f994a18554b3cbfc31fccd318dc44d3116f6aa99542bd43998da9bb28e2e11019b542ddf1ebe4422948cfe20aedf5749005689d56857dafba2cc3d093fae300c8be4009fafcf2f06595022b32d2d8e158707dca43a869802abc93137c60f215393b3c9e7474451ee5fdcd0af75ba1a10dd047e7b0ef90e5a0d
// // p = 0xe552fbd31a4ce2b3512a99bbed0cee3bbe73029ded12b9eb0507170cc09693e232a869ae28f2123f74048d6c0f8e0f2c039dec6d436020ed4b65e8bbaa018965
// // q = 0xb660c0797d8e56d37461667b9a9064a6c228a02b41e54cb491449358e2d09703c03076e4a1e4780c7ced4d97320e5fc59e649990eaef55ad5a643a2990ed085f
// // dP = 0x0b5f4c21b1e23233d648988b1b3c52ec1f8bb08b88602f5c1a49b9b8e62c11755c31c1dbe327ee03bb8b26403f9d39df62e687fcfb245f3da5498e7b0ff93789
// // dQ = 0x6afb9e6ca60655507d389591743db78d4adfe1f4fe16b0444be2da3c8df0a66580dbed2279053896bac1c97e23d2e2c35e02cd42814e1c0830ba1294477b8429
// // qInv = 0x0e8605cd5c8e21a0e7a09228794475cb6dacc0af13eee9ca4a19b5cc642265425572f715b69e3c7004faebb00b9959bdf39e7dc6b49fe1d0f6c1e5acb66972f8

// static const uint8_t precomputed_rsa_key[] = {
//     0x30,0x82,0x02,0x5b,0x02,0x01,0x00,0x02,0x81,0x81,0x00,0xa3,0x5f,0xaa,0x91,0x95,
//     0x6c,0xe7,0x46,0xa6,0x7f,0xd2,0x6d,0x3d,0xa1,0xc8,0xae,0xa8,0xfe,0xbe,0x73,0x5d,
//     0xa0,0xf5,0x8c,0x4a,0xf7,0x03,0x85,0xa2,0x20,0x65,0xd8,0x92,0x4e,0x6f,0x52,0x66,
//     0x3e,0x4a,0xca,0xa6,0xd1,0x75,0xcc,0x37,0x26,0x89,0xae,0x5a,0xd8,0x55,0xf7,0xfd,
//     0x0e,0x4d,0x89,0x79,0xa7,0x39,0xef,0x86,0xe4,0x25,0xa4,0xf0,0x5f,0xc9,0xe4,0x60,
//     0x21,0x75,0x09,0xde,0x35,0xe4,0x2a,0x37,0x45,0xff,0x01,0x09,0x9d,0x41,0x4b,0x16,
//     0x8f,0x64,0x43,0xa8,0x79,0xa6,0xfc,0xa2,0x3d,0x23,0x2e,0x22,0xd9,0x93,0x25,0x34,
//     0x35,0x78,0xd5,0x35,0x56,0xc4,0x58,0xeb,0xe4,0x24,0x15,0xa1,0x46,0x44,0xfc,0x01,
//     0xde,0xe2,0x66,0xf0,0x48,0x5e,0xd6,0x25,0x5e,0x24,0x7b,0x02,0x03,0x01,0x00,0x01,
//     0x02,0x81,0x80,0x08,0x33,0xe4,0xf2,0x03,0x51,0x20,0xf9,0x94,0xa1,0x85,0x54,0xb3,
//     0xcb,0xfc,0x31,0xfc,0xcd,0x31,0x8d,0xc4,0x4d,0x31,0x16,0xf6,0xaa,0x99,0x54,0x2b,
//     0xd4,0x39,0x98,0xda,0x9b,0xb2,0x8e,0x2e,0x11,0x01,0x9b,0x54,0x2d,0xdf,0x1e,0xbe,
//     0x44,0x22,0x94,0x8c,0xfe,0x20,0xae,0xdf,0x57,0x49,0x00,0x56,0x89,0xd5,0x68,0x57,
//     0xda,0xfb,0xa2,0xcc,0x3d,0x09,0x3f,0xae,0x30,0x0c,0x8b,0xe4,0x00,0x9f,0xaf,0xcf,
//     0x2f,0x06,0x59,0x50,0x22,0xb3,0x2d,0x2d,0x8e,0x15,0x87,0x07,0xdc,0xa4,0x3a,0x86,
//     0x98,0x02,0xab,0xc9,0x31,0x37,0xc6,0x0f,0x21,0x53,0x93,0xb3,0xc9,0xe7,0x47,0x44,
//     0x51,0xee,0x5f,0xdc,0xd0,0xaf,0x75,0xba,0x1a,0x10,0xdd,0x04,0x7e,0x7b,0x0e,0xf9,
//     0x0e,0x5a,0x0d,0x02,0x41,0x00,0xe5,0x52,0xfb,0xd3,0x1a,0x4c,0xe2,0xb3,0x51,0x2a,
//     0x99,0xbb,0xed,0x0c,0xee,0x3b,0xbe,0x73,0x02,0x9d,0xed,0x12,0xb9,0xeb,0x05,0x07,
//     0x17,0x0c,0xc0,0x96,0x93,0xe2,0x32,0xa8,0x69,0xae,0x28,0xf2,0x12,0x3f,0x74,0x04,
//     0x8d,0x6c,0x0f,0x8e,0x0f,0x2c,0x03,0x9d,0xec,0x6d,0x43,0x60,0x20,0xed,0x4b,0x65,
//     0xe8,0xbb,0xaa,0x01,0x89,0x65,0x02,0x41,0x00,0xb6,0x60,0xc0,0x79,0x7d,0x8e,0x56,
//     0xd3,0x74,0x61,0x66,0x7b,0x9a,0x90,0x64,0xa6,0xc2,0x28,0xa0,0x2b,0x41,0xe5,0x4c,
//     0xb4,0x91,0x44,0x93,0x58,0xe2,0xd0,0x97,0x03,0xc0,0x30,0x76,0xe4,0xa1,0xe4,0x78,
//     0x0c,0x7c,0xed,0x4d,0x97,0x32,0x0e,0x5f,0xc5,0x9e,0x64,0x99,0x90,0xea,0xef,0x55,
//     0xad,0x5a,0x64,0x3a,0x29,0x90,0xed,0x08,0x5f,0x02,0x40,0x0b,0x5f,0x4c,0x21,0xb1,
//     0xe2,0x32,0x33,0xd6,0x48,0x98,0x8b,0x1b,0x3c,0x52,0xec,0x1f,0x8b,0xb0,0x8b,0x88,
//     0x60,0x2f,0x5c,0x1a,0x49,0xb9,0xb8,0xe6,0x2c,0x11,0x75,0x5c,0x31,0xc1,0xdb,0xe3,
//     0x27,0xee,0x03,0xbb,0x8b,0x26,0x40,0x3f,0x9d,0x39,0xdf,0x62,0xe6,0x87,0xfc,0xfb,
//     0x24,0x5f,0x3d,0xa5,0x49,0x8e,0x7b,0x0f,0xf9,0x37,0x89,0x02,0x40,0x6a,0xfb,0x9e,
//     0x6c,0xa6,0x06,0x55,0x50,0x7d,0x38,0x95,0x91,0x74,0x3d,0xb7,0x8d,0x4a,0xdf,0xe1,
//     0xf4,0xfe,0x16,0xb0,0x44,0x4b,0xe2,0xda,0x3c,0x8d,0xf0,0xa6,0x65,0x80,0xdb,0xed,
//     0x22,0x79,0x05,0x38,0x96,0xba,0xc1,0xc9,0x7e,0x23,0xd2,0xe2,0xc3,0x5e,0x02,0xcd,
//     0x42,0x81,0x4e,0x1c,0x08,0x30,0xba,0x12,0x94,0x47,0x7b,0x84,0x29,0x02,0x40,0x0e,
//     0x86,0x05,0xcd,0x5c,0x8e,0x21,0xa0,0xe7,0xa0,0x92,0x28,0x79,0x44,0x75,0xcb,0x6d,
//     0xac,0xc0,0xaf,0x13,0xee,0xe9,0xca,0x4a,0x19,0xb5,0xcc,0x64,0x22,0x65,0x42,0x55,
//     0x72,0xf7,0x15,0xb6,0x9e,0x3c,0x70,0x04,0xfa,0xeb,0xb0,0x0b,0x99,0x59,0xbd,0xf3,
//     0x9e,0x7d,0xc6,0xb4,0x9f,0xe1,0xd0,0xf6,0xc1,0xe5,0xac,0xb6,0x69,0x72,0xf8
//   };
  
// static const size_t precomputed_rsa_key_len = sizeof(precomputed_rsa_key);
  
// // Simple DER parser for RSA private key
// typedef struct {
//     uint8_t *data;
//     size_t length;
// } der_integer_t;

// static int parse_der_integer(uint8_t **ptr, uint8_t *end, der_integer_t *integer) {
//     if (*ptr + 2 > end || **ptr != 0x02) return -1;
//     (*ptr)++;
    
//     size_t len = **ptr;
//     (*ptr)++;
    
//     if (len & 0x80) {
//         size_t len_bytes = len & 0x7f;
//         if (*ptr + len_bytes > end) return -1;
//         len = 0;
//         for (size_t i = 0; i < len_bytes; i++) {
//             len = (len << 8) | **ptr;
//             (*ptr)++;
//         }
//     }
    
//     if (*ptr + len > end) return -1;
    
//     // Skip leading zero byte if present
//     if (len > 0 && **ptr == 0x00) {
//         (*ptr)++;
//         len--;
//     }
    
//     integer->data = *ptr;
//     integer->length = len;
//     *ptr += len;
    
//     return 0;
// }

// static void print_hex_integer(const char *name, der_integer_t *integer) {
//     printf("%s = 0x", name);
//     for (size_t i = 0; i < integer->length; i++) {
//         printf("%02x", integer->data[i]);
//     }
//     printf("\n");
// }

// void print_rsa_parameters(psa_key_id_t key_id) {
//     uint8_t private_key_data[1024];
//     size_t private_key_length;
    
//     if (psa_export_key(key_id, private_key_data, sizeof(private_key_data), &private_key_length) != PSA_SUCCESS) {
//         return;
//     }
    
//     // Parse DER structure: SEQUENCE { version, n, e, d, p, q, dP, dQ, qInv }
//     uint8_t *ptr = private_key_data;
//     uint8_t *end = private_key_data + private_key_length;
    
//     if (ptr + 4 > end || *ptr != 0x30) return;
//     ptr++;
    
//     // Skip sequence length
//     size_t seq_len = *ptr++;
//     if (seq_len & 0x80) {
//         size_t len_bytes = seq_len & 0x7f;
//         ptr += len_bytes;
//     }
    
//     der_integer_t integers[9];
//     for (int i = 0; i < 9; i++) {
//         if (parse_der_integer(&ptr, end, &integers[i]) != 0) return;
//     }
    
//     print_hex_integer("n", &integers[1]);   // modulus
//     print_hex_integer("e", &integers[2]);   // public exponent
//     print_hex_integer("d", &integers[3]);   // private exponent
//     print_hex_integer("p", &integers[4]);   // prime1
//     print_hex_integer("q", &integers[5]);   // prime2
//     print_hex_integer("dP", &integers[6]);  // exponent1
//     print_hex_integer("dQ", &integers[7]);  // exponent2
//     print_hex_integer("qInv", &integers[8]); // coefficient
    
//     printf("DER key (%d bytes): ", private_key_length);
//     for (size_t i = 0; i < private_key_length; i++) {
//         printf("0x%02x,", private_key_data[i]);
//         if ((i + 1) % 16 == 0) printf("\n");
//     }
//     if (private_key_length % 16 != 0) printf("\n");
// }


// psa_status_t generate_rsa_key_pair(psa_key_id_t *key_id, size_t key_bits) {
//     psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    
//     psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT | PSA_KEY_USAGE_EXPORT);
//     psa_set_key_algorithm(&attributes, PSA_ALG_RSA_PKCS1V15_CRYPT);
//     psa_set_key_type(&attributes, PSA_KEY_TYPE_RSA_KEY_PAIR);
//     psa_set_key_bits(&attributes, key_bits);
    
//     psa_status_t status = psa_generate_key(&attributes, key_id);
//     psa_reset_key_attributes(&attributes);
    
//     return status;
// }  

// psa_status_t import_rsa_key(psa_key_id_t *key_id, const uint8_t *key_data, size_t key_length) {
//     psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    
//     psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
//     psa_set_key_algorithm(&attributes, PSA_ALG_RSA_PKCS1V15_CRYPT);
//     psa_set_key_type(&attributes, PSA_KEY_TYPE_RSA_KEY_PAIR);
    
//     psa_status_t status = psa_import_key(&attributes, key_data, key_length, key_id);
//     psa_reset_key_attributes(&attributes);
    
//     return status;
// }

// psa_status_t rsa_decrypt(psa_key_id_t key_id, const uint8_t *ciphertext, 
//                         size_t ciphertext_length, uint8_t *plaintext, 
//                         size_t plaintext_size, size_t *plaintext_length) {
//     return psa_asymmetric_decrypt(  key_id, 
//                                     PSA_ALG_RSA_PKCS1V15_CRYPT,
//                                     ciphertext, 
//                                     ciphertext_length, 
//                                     NULL, 0,
//                                     plaintext, 
//                                     plaintext_size, 
//                                     plaintext_length);
// }

// // Global variables for the secure service
// static uint8_t public_key_buffer[PSA_EXPORT_PUBLIC_KEY_MAX_SIZE];
// static size_t public_key_length;

// static psa_status_t rsa_export_pub_key(void) {
//     psa_status_t status;
//     psa_key_id_t key_id = 0;

//     // Initialize PSA Crypto if not already done
//     status = psa_crypto_init();
//     if (status != PSA_SUCCESS) {
//         printf("[S] Failed to init PSA Crypto: %d\r\n", status);
//         return status;
//     }

//     // Import the RSA private key (from which we'll export the public key)
//     status = import_rsa_key(&key_id, precomputed_rsa_key, precomputed_rsa_key_len);
//     if (status != PSA_SUCCESS) {
//         printf("[S] Failed to import RSA key: %d\r\n", status);
//         return status;
//     }

//     // Export the public key
//     status = psa_export_public_key(key_id,
//                                    public_key_buffer,
//                                    sizeof(public_key_buffer),
//                                    &public_key_length);
//     if (status != PSA_SUCCESS) {
//         printf("[S] Failed to export public key: %d\n", status);
//         // Clean up the private key before returning
//         psa_destroy_key(key_id);
//         return status;
//     }

//     printf("[S] Public key exported successfully, length: %d bytes\n", public_key_length);

//     // Clean up the private key (we only needed it to export the public key)
//     psa_destroy_key(key_id);
    
//     return PSA_SUCCESS;
// }

// static void process_rsa_ciphertext(uint8_t *ciphertext, size_t ciphertext_length) {
//     // Example of what decryption would look like:
//     psa_status_t status;
//     psa_key_id_t key_id = 0;
//     uint8_t plaintext[256];
//     size_t plaintext_length;
    
//     // Initialize crypto if not already done
//     status = psa_crypto_init();
//     if (status != PSA_SUCCESS) {
//         LOG_INFFMT("[S] Failed to init crypto: %d\r\n", status);
//         return;
//     }
    
//     // Import the RSA key
//     status = import_rsa_key(&key_id, precomputed_rsa_key, precomputed_rsa_key_len);
//     if (status != PSA_SUCCESS) {
//         LOG_INFFMT("[S] Failed to import key: %d\r\n", status);
//         return;
//     }
    
//     // Decrypt the ciphertext
//     status = rsa_decrypt(key_id, ciphertext, ciphertext_length,
//                         plaintext, sizeof(plaintext), &plaintext_length);
//     if (status != PSA_SUCCESS) {
//         LOG_INFFMT("[S] Failed to decrypt: %d\r\n", status);
//     } else {
//         LOG_INFFMT("[S] Plaintext: ");
//         for (size_t i = 0; i < plaintext_length; i++) {
//             LOG_INFFMT("%c", plaintext[i]);
//         }
//         LOG_INFFMT("\r\n");
//     }
    
//     // Clean up
//     psa_destroy_key(key_id);
// }

// //------------------------------------------------------------------------------
// // Export RSA public key service 
// //------------------------------------------------------------------------------
// static void rsa_export_pub_key_request(void)
// {
//     psa_status_t status;
//     psa_msg_t msg;

//     /* Retrieve the message corresponding to the RSA export service signal */
//     status = psa_get(RSA_EXPORT_PUB_KEY_SIGNAL, &msg);  // Fixed: use correct signal
//     if (status != PSA_SUCCESS) {
//         LOG_INFFMT("[S] Failed to get message: %d\r\n", status);
//         return;
//     }

//     /* Decode the message */
//     switch (msg.type) {
//     case PSA_IPC_CALL:
        
//         // Export the public key
//         status = rsa_export_pub_key();
        
//         if (status == PSA_SUCCESS) {
//             // Write the public key data to the first output vector
//             size_t bytes_written = 0;
//             psa_write(msg.handle, 0, public_key_buffer, public_key_length);
            
//             // Write the public key length to the second output vector
//             psa_write(msg.handle, 1, &public_key_length, sizeof(public_key_length));
//         }
        
//         break;
        
//     default:
//         /* Invalid message type */
//         LOG_INFFMT("[S] Invalid message type: %d\r\n", msg.type);
//         status = PSA_ERROR_PROGRAMMER_ERROR;
//         break;
//     }

//     /* Reply with the message result status to unblock the client */
//     psa_reply(msg.handle, status);
// }

// //------------------------------------------------------------------------------
// // RSA Decryption service 
// //------------------------------------------------------------------------------
// static void rsa_decrypt_request(void) 
// {
//     psa_status_t status;
//     psa_msg_t msg;
//     uint8_t ciphertext[MAX_CIPHERTEXT_SIZE];
//     size_t ciphertext_length;
    
//     /* Retrieve the message corresponding to the example service signal */
//     status = psa_get(RSA_DECRYPT_REQUEST_SIGNAL, &msg);
//     if (status != PSA_SUCCESS) {
//         LOG_INFFMT("[S] Failed to get message: %d\r\n", status);
//         return;
//     }

//     /* Decode the message */
//     switch (msg.type) {
//     case PSA_IPC_CALL:        
//         // Check that we have the expected number of input vectors
//         if (msg.in_size[0] == 0 || msg.in_size[1] == 0) {
//             LOG_INFFMT("[S] Invalid input sizes\r\n");
//             status = PSA_ERROR_INVALID_ARGUMENT;
//             break;
//         }
        
//         // First, read the ciphertext length from the second invec
//         if (msg.in_size[1] != sizeof(size_t)) {
//             LOG_INFFMT("[S] Invalid length parameter size\r\n");
//             status = PSA_ERROR_INVALID_ARGUMENT;
//             break;
//         }
        
//         psa_read(msg.handle, 1, &ciphertext_length, sizeof(size_t));
        
//         // Validate the ciphertext length
//         if (ciphertext_length > MAX_CIPHERTEXT_SIZE || ciphertext_length != msg.in_size[0]) {
//             LOG_INFFMT("[S] Invalid ciphertext length: %d (expected %d)\r\n", 
//                     ciphertext_length, msg.in_size[0]);
//             status = PSA_ERROR_INVALID_ARGUMENT;
//             break;
//         }
        
//         // Read the ciphertext from the first invec
//         psa_read(msg.handle, 0, ciphertext, ciphertext_length);
        
//         // Process the ciphertext (print it for now)
//         process_rsa_ciphertext(ciphertext, ciphertext_length);
        
//         status = PSA_SUCCESS;
//         break;
        
//     default:
//         /* Invalid message type */
//         LOG_INFFMT("[S] Invalid message type: %d\r\n", msg.type);
//         status = PSA_ERROR_PROGRAMMER_ERROR;
//         break;
//     }
    
//     /* Reply with the message result status to unblock the client */
//     psa_reply(msg.handle, status);
//  }
 

void PRoT_crypto_attack_main(void)
 {
     psa_signal_t signals;
 
     LOG_INFFMT("[S] PRoT_crypto_attack_main started\r\n");
 
     while (1) {
         signals = psa_wait(PSA_WAIT_ANY, PSA_BLOCK);
        //  if (signals & RSA_DECRYPT_REQUEST_SIGNAL) {
        //     rsa_decrypt_request();   
        //  } else if (signals & RSA_EXPORT_PUB_KEY_SIGNAL) {
        //     rsa_export_pub_key_request();   
        //  } else{
        //      /* Should not come here */
        //      psa_panic();
        //  }
     }
 }