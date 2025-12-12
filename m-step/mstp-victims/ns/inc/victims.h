#ifndef INC_VICTIMS_H_
#define INC_VICTIMS_H_

#include <stdio.h>

#define N_VICTIMS 5 
#define N_VICTIMS_S_TFM 10

//------------------------------------------------------------------------------
// TF-M: Victims running on a TA on TF-M
//------------------------------------------------------------------------------
void victim_baseline_s_tfm();
void victim_normal_inst_s_tfm();
void victim_ICI_inst_s_tfm();
void victim_IRI_inst_s_tfm();
void victim_IRI_ATOMIC_inst_s_tfm();
void victim_printf_inst_s_tfm();
void victim_eval_div_inst_s_tfm();
void victim_eval_inst_diff_s_tfm();
void victim_eval_cache_s_tfm();
void victim_eval_cont_s_tfm();

extern void (*victim_s_tfm[N_VICTIMS_S_TFM])();
extern char *victim_names_s_tfm[N_VICTIMS_S_TFM] ;


//----------------------------------------------------------------------------
// Secure veneers 
// - From src/Secure/Core/Src/secure_nsc.c
//----------------------------------------------------------------------------
extern victim_normal_inst_s();
extern victim_ICI_inst_s();
extern victim_IRI_inst_s();
// C 
extern void victim_printf_inst_s();
extern void victim_strlen_inst_s();

extern void (*victim_s[N_VICTIMS])();
extern char *victim_names_s[N_VICTIMS] ;
//----------------------------------------------------------------------------
       
// Asm 
void victim_normal_inst();
void victim_ICI_inst();
void victim_IRI_inst();
// C 
void victim_printf_inst();
void victim_strlen_inst();

extern void (*victim[N_VICTIMS])();
extern char *victim_names[N_VICTIMS] ;

#endif 
