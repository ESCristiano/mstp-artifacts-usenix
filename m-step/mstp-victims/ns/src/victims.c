#include "victims.h"

#include "psa/client.h"
#include "psa_manifest/sid.h"
#include "psa/crypto.h" 

#define STRING "M-Step\r\n"

// Victim function pointers
void (*victim[N_VICTIMS])() = {
  victim_normal_inst,
  victim_ICI_inst,
  victim_IRI_inst,
  victim_printf_inst,
  victim_strlen_inst
};

char *victim_names[N_VICTIMS] = {
  "victim_normal_inst",
  "victim_ICI_inst",
  "victim_IRI_inst",
  "victim_printf_inst",
  "victim_strlen_inst"
};

// Victim function pointers
void (*victim_s[N_VICTIMS])() = {
  victim_normal_inst_s,
  victim_ICI_inst_s,
  victim_IRI_inst_s,
  victim_printf_inst_s,
  victim_strlen_inst_s
};

char *victim_names_s[N_VICTIMS] = {
  "victim_normal_inst_s",
  "victim_ICI_inst_s",
  "victim_IRI_inst_s",
  "victim_printf_inst_s",
  "victim_strlen_inst_s",
};

// Victim function pointers
void (*victim_s_tfm[N_VICTIMS_S_TFM])() = {
  victim_baseline_s_tfm,
  victim_normal_inst_s_tfm,
  victim_ICI_inst_s_tfm,
  victim_IRI_inst_s_tfm,
  victim_IRI_ATOMIC_inst_s_tfm,
  victim_printf_inst_s_tfm,
  victim_eval_div_inst_s_tfm,
  victim_eval_inst_diff_s_tfm,
  victim_eval_cache_s_tfm,
  victim_eval_cont_s_tfm

};

char *victim_names_s_tfm[N_VICTIMS_S_TFM] = {
  "victim_baseline_s_tfm",
  "victim_normal_inst_s_tfm",
  "victim_ICI_inst_s_tfm",
  "victim_IRI_inst_s_tfm",
  "victim_IRI_ATOMIC_inst_s_tfm",
  "victim_printf_inst_s_tfm",
  "victim_eval_div_inst_s_tfm",
  "victim_eval_inst_diff_s_tfm",
  "victim_eval_cache_s_tfm",
  "victim_eval_cont_s_tfm"
};


void victim_printf_inst(){
  printf(STRING);
}

void victim_strlen_inst(){
  strlen(STRING);
}


void victim_baseline_s_tfm() {
  psa_status_t status;
  status = psa_call(MSTP_EVAL_EMPTY_HANDLE, PSA_IPC_CALL, NULL, 0, NULL, 0);
  if (status != PSA_SUCCESS){
      printf("psa_call has failed!\r\n");
      return;
  }
}

void victim_normal_inst_s_tfm() {
  psa_status_t status;
  status = psa_call(MSTP_EVAL_NORMAL_INST_HANDLE, PSA_IPC_CALL, NULL, 0, NULL, 0);
  if (status != PSA_SUCCESS){
      printf("psa_call has failed!\r\n");
      return;
  }
}

void victim_ICI_inst_s_tfm() {
  psa_status_t status;
  status = psa_call(MSTP_EVAL_ICI_INST_HANDLE, PSA_IPC_CALL, NULL, 0, NULL, 0);
  if (status != PSA_SUCCESS){
      printf("psa_call has failed!\r\n");
      return;
  }
}

void victim_IRI_inst_s_tfm() {
  psa_status_t status;
  status = psa_call(MSTP_EVAL_IRI_INST_HANDLE, PSA_IPC_CALL, NULL, 0, NULL, 0);
  if (status != PSA_SUCCESS){
      printf("psa_call has failed!\r\n");
      return;
  }
}

void victim_IRI_ATOMIC_inst_s_tfm() {
  psa_status_t status;
  status = psa_call(MSTP_EVAL_IRI_ATOMIC_INST_HANDLE, PSA_IPC_CALL, NULL, 0, NULL, 0);
  if (status != PSA_SUCCESS){
      printf("psa_call has failed!\r\n");
      return;
  }
}

void victim_printf_inst_s_tfm() {
  psa_status_t status;
  status = psa_call(MSTP_EVAL_PRINTF_HANDLE, PSA_IPC_CALL, NULL, 0, NULL, 0);
  if (status != PSA_SUCCESS){
      printf("psa_call has failed!\r\n");
      return;
  }
}

void victim_eval_div_inst_s_tfm() {
  psa_status_t status;
  status = psa_call(MSTP_EVAL_DIV_INS_HANDLE, PSA_IPC_CALL, NULL, 0, NULL, 0);
  if (status != PSA_SUCCESS){
      printf("psa_call has failed!\r\n");
      return;
  }
}

void victim_eval_inst_diff_s_tfm() {
  psa_status_t status;

  // struct psa_invec invec[1] = {
  //   {&code_under_test, sizeof(uint8_t)}  
  // };

  status = psa_call(MSTP_EVAL_INST_DIFF_HANDLE, PSA_IPC_CALL, NULL, 0, NULL, 0);
  if (status != PSA_SUCCESS){
      printf("psa_call has failed!\r\n");
      return;
  }
}

void victim_eval_cache_s_tfm() {
  psa_status_t status;
  status = psa_call(MSTP_EVAL_CACHE_HANDLE, PSA_IPC_CALL, NULL, 0, NULL, 0);
  if (status != PSA_SUCCESS){
      printf("psa_call has failed!\r\n");
      return;
  }
}

void victim_eval_cont_s_tfm() {
  psa_status_t status;
  status = psa_call(MSTP_EVAL_CONTENTION_HANDLE, PSA_IPC_CALL, NULL, 0, NULL, 0);
  if (status != PSA_SUCCESS){
      printf("psa_call has failed!\r\n");
      return;
  }
}

void generate_rsa_key_pair()
{
    mbedtls_svc_key_id_t key = MBEDTLS_SVC_KEY_ID_INIT;
    psa_status_t status;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;

    printf("RSA Start\r\n");

    psa_set_key_type(&attributes, PSA_KEY_TYPE_RSA_KEY_PAIR); 
    psa_set_key_bits(&attributes, 1024);
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_SIGN_HASH);
    psa_set_key_algorithm(&attributes, PSA_ALG_RSA_PKCS1V15_SIGN_RAW);
    psa_set_key_enrollment_algorithm(&attributes, PSA_ALG_NONE);

    status = psa_generate_key(&attributes, &key);
    
    if (status != PSA_SUCCESS) {
        printf("Failed to generate RSA key pair: %d\r\n", status);
        return;
    }

    printf("RSA key pair generated successfully.\r\n");
}
