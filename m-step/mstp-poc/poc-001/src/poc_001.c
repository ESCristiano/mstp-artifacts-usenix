#include "poc_001.h"

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "psa/client.h"
#include "psa_manifest/sid.h"
#include "psa/crypto.h"

// Victim function pointers
void (*poc[N_POC_001])() = {
  poc_001_rsa_gen,
  poc_001_rsa_encryption,
  poc_001_rsa_decryption
};

char *poc_names[N_POC_001] = {
  "poc_001_rsa_gen",
  "poc_001_rsa_encryption",
  "poc_001_rsa_decryption"
};

const char *test_message = "Hello, PSA Crypto RSA!";

uint8_t ciphertext[256];
size_t ciphertext_length;

psa_status_t generate_rsa_key_pair(psa_key_id_t *key_id, size_t key_bits) {
  psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
  
  psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT | PSA_KEY_USAGE_EXPORT);
  psa_set_key_algorithm(&attributes, PSA_ALG_RSA_PKCS1V15_CRYPT);
  psa_set_key_type(&attributes, PSA_KEY_TYPE_RSA_KEY_PAIR);
  psa_set_key_bits(&attributes, key_bits);
  
  psa_status_t status = psa_generate_key(&attributes, key_id);
  psa_reset_key_attributes(&attributes);
  
  return status;
}

// void generate_rsa_key_pair()
// {
//     mbedtls_svc_key_id_t key = MBEDTLS_SVC_KEY_ID_INIT;
//     psa_status_t status;
//     psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;

//     printf("RSA Start\r\n");

//     psa_set_key_type(&attributes, PSA_KEY_TYPE_RSA_KEY_PAIR); 
//     psa_set_key_bits(&attributes, 1024);
//     psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_SIGN_HASH);
//     psa_set_key_algorithm(&attributes, PSA_ALG_RSA_PKCS1V15_SIGN_RAW);
//     psa_set_key_enrollment_algorithm(&attributes, PSA_ALG_NONE);

//     status = psa_generate_key(&attributes, &key);
    
//     if (status != PSA_SUCCESS) {
//         printf("Failed to generate RSA key pair: %d\r\n", status);
//         return;
//     }

//     printf("RSA key pair generated successfully.\r\n");
// }


psa_status_t rsa_decrypt(psa_key_id_t key_id, const uint8_t *ciphertext, 
                        size_t ciphertext_length, uint8_t *plaintext, 
                        size_t plaintext_size, size_t *plaintext_length) {
    return psa_asymmetric_decrypt(key_id, 
                                  PSA_ALG_RSA_PKCS1V15_CRYPT,
                                  ciphertext,   
                                  ciphertext_length, 
                                  NULL, 0,
                                  plaintext, 
                                  plaintext_size, 
                                  plaintext_length);
}

psa_status_t import_rsa_key(psa_key_id_t *key_id, const uint8_t *key_data, size_t key_length) {
  psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
  
  psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_ENCRYPT );
  psa_set_key_algorithm(&attributes, PSA_ALG_RSA_PKCS1V15_CRYPT);
  psa_set_key_type(&attributes, PSA_KEY_TYPE_RSA_KEY_PAIR);
  
  psa_status_t status = psa_import_key(&attributes, key_data, key_length, key_id);
  psa_reset_key_attributes(&attributes);
  
  return status;
}

//------------------------------------------------------------------------------
// Request RSA key generaton
// TODO: Make this a service of the PRoT_crypto_attack TA
//------------------------------------------------------------------------------
void poc_001_rsa_gen() {
  psa_status_t status;
  psa_key_id_t key_id = 0;

  printf("RSA generate.\n");
  
  status = psa_crypto_init();
  if (status != PSA_SUCCESS) {
    printf("Failed to init: %d\r\n", status);
    return;
  }
  
  status = generate_rsa_key_pair(&key_id, 1024);
  
  if (status != PSA_SUCCESS) {
    printf("Failed to generate RSA key pair: %d\r\n", status);
    return;
  }
  
  printf("RSA key pair generated successfully.\n");
}


//------------------------------------------------------------------------------
// Encryption with RSA public key
//------------------------------------------------------------------------------
void poc_001_rsa_encryption() {
  psa_status_t status;
  psa_key_id_t public_key_id = 0;  
  uint8_t public_key_buffer[PSA_EXPORT_PUBLIC_KEY_MAX_SIZE];
  size_t public_key_length = 0;  
  psa_key_attributes_t public_key_attributes = PSA_KEY_ATTRIBUTES_INIT;

  status = psa_crypto_init();
  if (status != PSA_SUCCESS) {
      printf("Failed to init: %d\r\n", status);
      return;
  }

  //----------------------------------------------------------------------------
  // Step 1: Call secure service to export RSA public key
  //----------------------------------------------------------------------------
  struct psa_outvec outvec[2] = {
      {public_key_buffer, sizeof(public_key_buffer)}, // Buffer for public key data
      {&public_key_length, sizeof(size_t)}            // Length of the public key
  };

  // Request the secure service public key 
  status = psa_call(RSA_EXPORT_PUB_KEY_HANDLE, PSA_IPC_CALL, NULL, 0, outvec, 2);
  if (status != PSA_SUCCESS) {
      printf("[NS] Failed to call secure service: %d\r\n", status);
      goto cleanup;
  } 

  // Step 2: Import the public key for encryption operations
  psa_set_key_usage_flags(&public_key_attributes, PSA_KEY_USAGE_ENCRYPT);
  psa_set_key_algorithm(&public_key_attributes, PSA_ALG_RSA_PKCS1V15_CRYPT);
  psa_set_key_type(&public_key_attributes, PSA_KEY_TYPE_RSA_PUBLIC_KEY);
  psa_set_key_lifetime(&public_key_attributes, PSA_KEY_LIFETIME_VOLATILE);

  //----------------------------------------------------------------------------
  // Step 2: Import exported RSA public key
  //----------------------------------------------------------------------------
  status = psa_import_key(&public_key_attributes,
                         public_key_buffer,
                         public_key_length,
                         &public_key_id);
  if (status != PSA_SUCCESS) {
      printf("Failed to import public key: %d\n", status);
      goto cleanup;
  }

  //----------------------------------------------------------------------------
  // Step 3: Encrypt the message using the public key
  //----------------------------------------------------------------------------
  status = psa_asymmetric_encrypt(public_key_id,  
                                 PSA_ALG_RSA_PKCS1V15_CRYPT,
                                 (const uint8_t*)test_message,
                                 strlen(test_message),
                                 NULL, 0,
                                 ciphertext,
                                 sizeof(ciphertext),
                                 &ciphertext_length);
  if (status != PSA_SUCCESS) {
      printf("Failed to encrypt message: %d\n", status);
  } 
  //else {
      // printf("Message encrypted successfully, encrypted length: %d bytes\n", ciphertext_length);
      
      // // Print encrypted data in hex format
      // printf("Encrypted data: ");
      // for (size_t i = 0; i < ciphertext_length; i++) {
      //     printf("%02x", ciphertext[i]);
      // }
      // printf("\n");
 // }

cleanup:
  if (public_key_id != 0) {
      psa_destroy_key(public_key_id);
  }
  psa_reset_key_attributes(&public_key_attributes);
}


//------------------------------------------------------------------------------
// Decryption with RSA priv key (Secure world)
//------------------------------------------------------------------------------
void poc_001_rsa_decryption() {
  psa_status_t status;
  // Prepare the invec for the secure service call
  // We send both the ciphertext and its length
  struct psa_invec invec[2] = {
      {ciphertext, ciphertext_length},           // The ciphertext data
      {&ciphertext_length, sizeof(size_t)}       // The length of the ciphertext
  };

  //----------------------------------------------------------------------------
  // Step 1: Request decryption, we won't receive the plaintext back
  //----------------------------------------------------------------------------
  status = psa_call(RSA_DECRYPT_REQUEST_HANDLE, PSA_IPC_CALL, invec, 2, NULL, 0);
  if (status != PSA_SUCCESS) {
      printf("[NS] Failed to call secure service: %d\r\n", status);
  }

  return;
}