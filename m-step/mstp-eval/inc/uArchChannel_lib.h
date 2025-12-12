#ifndef COVERT_H
#define COVERT_H
  
// #include "board.h" 
// #include "config_tests.h"

#ifndef BOARD_H
#define BOARD_H

#define STM32L552

// I_CACHE 
#define I_CACHE_SIZE 8192
#define I_CACHE_LINE_SIZE 16
#define I_N_WAYS 2
#define I_N_CACHE_SETS 256
#define I_REPLACEMENT_POLICY PLRU

// D_CACHE 
#define NO_D_CACHE 

#endif

  // #ifndef REVERSE_ENG_CACHE
  //   #define I_N_CACHE_SETS    I_CACHE_SIZE/(I_CACHE_LINE_SIZE*I_N_WAYS)
  // #else
  //   #define I_N_CACHE_SETS    I_CACHE_SIZE/(I_CACHE_LINE_SIZE*(I_N_WAYS/I_N_WAYS)) // assume that the cahce is direct-mapped for test porpuses 
  // #endif

  //extern void block_jumps_0_0();
  extern void prime_set_0_way_0();
  extern void probe_set_0_0();
  
  extern void second_prime_set_0_way_0();

  //extern void block_bx_lr_way_0();
  //extern void block_bx_lr_last_way();
  extern void probe_set_0_way_0();
  extern void probe_set_0_way_1();
  extern void probe_set_0_way_2();
  extern void probe_set_0_way_3();
  
  // touch_s uses the same block of jumps of the prime 1 
  __attribute__ ((noinline)) void touch_s(int secret){
    void (*func_ptr)(void)=(&prime_set_0_way_0+1+((I_N_CACHE_SETS-secret-1)*I_CACHE_LINE_SIZE));
    func_ptr();
  }

  // touch_ns uses the same block of jumps of the prime 2
  __attribute__ ((noinline)) void touch_ns(int secret){
    void (*func_ptr)(void)=(&second_prime_set_0_way_0+1+((I_N_CACHE_SETS-secret-1)*I_CACHE_LINE_SIZE));
    func_ptr();
  }

  void prime(){
  prime_set_0_way_0();
  }

  void prime_2(){
    second_prime_set_0_way_0();
  }

//----------------------------------------------------------------------------------
// Wall-clocks definitions (Board Peripheral Timers)
//----------------------------------------------------------------------------------
#ifdef EKRA6M3
#define UP_COUNTER
  int volatile *ptr_ctr = 0x4007802C + 0x800,
        *ptr_cnt = 0x40078048 + 0x800, 
        *timer_GTSSR = 0x40078010 + 0x800, 
        *timer_GTPSR = 0x40078014 + 0x800, 
        *timer_GTCSR = 0x40078018 + 0x800,
        *timer_GTUDDTYC = 0x40078030 + 0x800, 
        *MSTPCRD = 0x40047000+0x8;
  
  // use these "functions" as a MACRO to reduce cache polution. These functions will be called
  // in a __attribute__((optimize(0))) function, so inline functions are not allowed
  #define config_timer() do{ \
    *MSTPCRD = (*MSTPCRD)&~(1<<6); \ 
    *timer_GTUDDTYC = 3; \ 
    *timer_GTSSR = (*timer_GTSSR) | (1<<31); \ 
    *timer_GTPSR = (*timer_GTPSR) | (1<<31); \
    *timer_GTCSR = (*timer_GTCSR) | (1<<31); \ 
    *timer_GTUDDTYC = 1; \ 
  }while(0)
  #define enable_timer()(*ptr_ctr = (*ptr_ctr)|1<<0)
  #define disable_timer()(*ptr_ctr = (*ptr_ctr)&~(1<<0))
#endif

#ifdef STM32F767
   #define UP_COUNTER
  int *ptr_ctr = 0x40001000, *ptr_cnt = 0x40001000+0x24, *rcc_ctrl = 0x40023800+0x040;
  
  // use these "functions" as a MACRO to reduce cache polution. These functions will be called
  // in a __attribute__((optimize(0))) function, so inline functions are not allowed
  #define config_timer()(*rcc_ctrl = *rcc_ctrl | (1<<4))
  #define enable_timer()(*ptr_ctr = (*ptr_ctr)|1<<0)
  #define disable_timer()(*ptr_ctr = (*ptr_ctr)&~(1<<0))
#endif 

#ifdef STM32L552
  #define UP_COUNTER
  int *ptr_ctr = 0x50001000, *ptr_cnt = 0x50001000+0x24, *rcc_ctrl = 0x50021000+0x058;
  
  // use these "functions" as a MACRO to reduce cache polution. These functions will be called
  // in a __attribute__((optimize(0))) function, so inline functions are not allowed
  #define config_timer()(*rcc_ctrl = *rcc_ctrl | (1<<4))
  #define enable_timer()(*ptr_ctr = (*ptr_ctr)|1<<0)
  #define disable_timer()(*ptr_ctr = (*ptr_ctr)&~(1<<0))

#endif 

#ifdef PSoC64
  // NOTE: Timer0 of TCPWM0 is only for S world. If it is needed a counter in NS, 
  // timer1 of TCPWM0
  #include "cy_tcpwm_counter.h"
  #define UP_COUNTER
  #define COUNTER_NUMBER 0
  cy_stc_tcpwm_counter_config_t tcpwm_counter_config =
  {
    /* .period            = */ 65000UL, /* Period of 100 (0-99). Terminal count event when rolls over back to 0. */
    /* .clockPrescaler    = */ CY_TCPWM_COUNTER_PRESCALER_DIVBY_1, /* Clk_counter = Clk_input / 4 */
    /* .runMode           = */ CY_TCPWM_COUNTER_CONTINUOUS, /* Wrap around at terminal count. */
    /* .countDirection    = */ CY_TCPWM_COUNTER_COUNT_UP, /* Up counter, counting from 0 to period value. */
    /* .compareOrCapture  = */ CY_TCPWM_COUNTER_MODE_COMPARE, /* Trigger interrupt/event signal when Counter value is equal to Compare0 */
    /* .compare0          = */ 65000UL,
    /* .compare1          = */ 0UL,
    /* .enableCompareSwap = */ false, /* Upon a compare event, swap the Compare0 and Compare1 values. */
    /* .interruptSources  = */ CY_TCPWM_INT_NONE,
    /* .captureInputMode  = */ CY_TCPWM_INPUT_RISINGEDGE, /* This input is NOT used, leave it in default state (CY_TCPWM_INPUT_RISINGEDGE = 0UL) */
    /* .captureInput      = */ CY_TCPWM_INPUT_0,
    /* .reloadInputMode   = */ CY_TCPWM_INPUT_RISINGEDGE, /* This input is NOT used, leave it in default state (CY_TCPWM_INPUT_RISINGEDGE = 0UL) */
    /* .reloadInput       = */ CY_TCPWM_INPUT_0,
    /* .startInputMode    = */ CY_TCPWM_INPUT_RISINGEDGE, /* This input is NOT used, leave it in default state (CY_TCPWM_INPUT_RISINGEDGE = 0UL) */
    /* .startInput        = */ CY_TCPWM_INPUT_0,
    /* .stopInputMode     = */ CY_TCPWM_INPUT_RISINGEDGE, /* This input is NOT used, leave it in default state (CY_TCPWM_INPUT_RISINGEDGE = 0UL) */
    /* .stopInput         = */ CY_TCPWM_INPUT_0,
    /* .countInputMode    = */ CY_TCPWM_INPUT_LEVEL, /* Set this input to LEVEL and 1 (high logic level) */
    /* .countInput        = */ CY_TCPWM_INPUT_1 /* So the counter will count input clock periods (Clk_counter, taking into account the clock prescaler) */
  };

  int *ptr_ctr_set = &TCPWM0->CTRL_SET, *ptr_ctr_clr = &TCPWM0->CTRL_CLR, 
        *ptr_start = &TCPWM0->CMD_START, *ptr_stop = &TCPWM0->CMD_STOP,
        *ptr_cnt = &TCPWM0_CNT0->COUNTER;

  // use these "functions" as a MACRO to reduce cache polution. These functions will be called
  // in a __attribute__((optimize(0))) function, so inline functions are not allowed

  #define config_timer() Cy_TCPWM_Counter_Init(TCPWM0, COUNTER_NUMBER, &tcpwm_counter_config)

  #define enable_timer() do {\
      *ptr_ctr_set = *ptr_ctr_set|1<<COUNTER_NUMBER; \
      *ptr_start = (*ptr_start)|1<<COUNTER_NUMBER;\
      } while(0)

  #define disable_timer() do {\
      *ptr_stop = (*ptr_stop)|1<<COUNTER_NUMBER;\
      *ptr_ctr_clr = *ptr_ctr_clr|1<<COUNTER_NUMBER; \
      } while(0)

#endif 

#ifdef MUSCA_B1
  #define DOWN_COUNTER

  int *ptr_ctr = 0x50000000, *ptr_cnt = 0x50000000+0x04, *ptr_reload = 0x50000000+0x08;
  
  // use these "functions" as a MACRO, to reduce cache polution and because they will be called
  // in a __attribute__((optimize(0))) function, so inline functions are not allowed
  #define config_timer()(*ptr_reload = 0x0FFFFFFF)
  #define enable_timer()(*ptr_ctr = (*ptr_ctr)|1<<0)
  #define disable_timer()(*ptr_ctr = (*ptr_ctr)&~(1<<0))

#endif 
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------

#if !(defined(nRF9160) || defined(LPC55S6) || defined(M2351))
__attribute__((optimize(0))) __attribute__ ((noinline)) volatile int probe_all_s(){
  volatile int time1, time2;
  config_timer();
  enable_timer();
  time1 = *ptr_cnt;
  #ifndef REVERSE_ENG_CACHE
    prime_set_0_way_0();
  #else
    touch_s(I_N_CACHE_SETS/2);
  #endif
  time2 = *ptr_cnt;
  disable_timer();
   #ifdef UP_COUNTER
    return (time2 - time1);
  #endif
  #ifdef DOWN_COUNTER
    return (time1 - time2);
  #endif
}

// ensure that the code is compiled withou otimizations otherwise we can have false results
__attribute__((optimize(0))) __attribute__ ((noinline)) volatile int probe_line_s(int line, int way){
  volatile int time1, time2;
  void (*func_ptr)(void);
  config_timer();
  #if I_N_WAYS > 1
  if(!way)
  #endif
    func_ptr = (&probe_set_0_way_0+1+((line)*I_CACHE_LINE_SIZE));
  #if I_N_WAYS > 1
  else
    func_ptr = (&probe_set_0_way_1+1+((line)*I_CACHE_LINE_SIZE));
  #endif
  enable_timer();
  time1 = *ptr_cnt;
  func_ptr();
  time2 = *ptr_cnt;
  disable_timer();
  #ifdef UP_COUNTER
    return (time2 - time1);
  #endif
  #ifdef DOWN_COUNTER
    return (time1 - time2);
  #endif
}
#endif

#ifdef SYSTICK_AVAILABLE
// ensure that the code is compiled withou otimizations otherwise we can have false results
// Non Secure (or S tf-m level 1) is doing the prime and probe
__attribute__((optimize(0))) __attribute__ ((noinline)) volatile int probe_all(){
  volatile int time1, time2, result;
  SysTick->CTRL = 0x0;
  SysTick->LOAD  = (uint32_t)(0xFFFFFF - 1); //mac counter value
  SysTick->VAL = 0; // reset the val  counter
  SysTick->CTRL = 0x05; // starts the systick
  time1 = SysTick->VAL;
  #ifndef REVERSE_ENG_CACHE
    prime_set_0_way_0();
  #else
    touch_s(I_N_CACHE_SETS/2);
  #endif
  time2 = SysTick->VAL;
  result = time1 - time2; // inverted subtraction because it is a down counter
  SysTick->CTRL = 0x0; // stops the systick
  SysTick->VAL = 0; // reset the val  counter
  return result;
}


// ensure that the code is compiled without otimizations otherwise we can have false results
  // Non Secure (or S tf-m level 1) is doing the prime and probe
__attribute__((optimize(0))) __attribute__ ((noinline)) volatile int probe_line(int line, int way){
  volatile int time1, time2, result;
  void (*func_ptr)(void);
  SysTick->CTRL = 0x0;
  SysTick->LOAD  = (uint32_t)(0xFFFFFF - 1); //max counter value
  SysTick->VAL = 0; // reset the val  counter
  #if I_N_WAYS > 1
  if(!way)
  #endif
    func_ptr = (&probe_set_0_way_0+1+((line)*I_CACHE_LINE_SIZE));
  #if I_N_WAYS > 1
  else
    func_ptr = (&probe_set_0_way_1+1+((line)*I_CACHE_LINE_SIZE));
  #endif
  SysTick->CTRL = 0x05; // starts the systick
  time1 = SysTick->VAL;
  func_ptr();
  time2 = SysTick->VAL;
  result = time1 - time2; // inverted subtraction because it is a down counter
  SysTick->CTRL = 0x0; // stops the systick
  SysTick->VAL = 0; // reset the val  counter
  return result;
}
#endif

#endif
