#ifndef INC_ARMV8_DECOP_LIB_H_
#define INC_ARMV8_DECOP_LIB_H_

    #include <stdint.h>
    #include <stdio.h>

    // Instructions Codes
    #define NOP     0
    #define MOV     1
    #define LDR     2
    #define END     4
    #define PUSH    5      
    #define SUB     6       
    #define ADD     7   
    #define MOVS    8     
    #define CMP     9   
    #define BLE     10   
    #define B       11 
    #define ADDS    12    
    #define LDRW    13     
    #define BX      14   
    #define BLX     15   
    #define MOVW    16
    #define STR     17
    #define LSL     18
    #define ANDS    19
    #define BNE     20
    #define UXTB    21
    #define STRB    22
    #define BEQ     23 
    #define LD_PC   24 
    #define BL      25 
    #define RSB     26 
    #define I16     27 // Not defined 16-bits
    #define I32     28 // Not defined 32-bits
    #define LDM     29 
    #define STM     30 
    #define POP     31
    #define SBC     32
    #define CMN     33
    #define TST     34
    #define ORR     35
    #define ADC     36
    #define AND     37
    #define EOR     38
    #define MUL     39
    #define MVN     40
    #define CBZ     41
    #define CBNZ    42
    #define ADR     43
    #define CLRM    44
    #define STMDB   45
    #define LDMDB   46
    #define UXTH    47
    #define SXTH    48
    #define SXTB    49
    #define REV16   50
    #define REVSH   51
    #define REV     52
    #define LDRB    53
    #define STRD    54
    #define SG      55
    #define LDRD    56
    #define TEQ     57
    #define EORS    58
    #define SUBS    59
    #define BIC     60
    #define ORN     61
    #define MOVN    62
    #define MOVT    63
    #define BCS     64
    #define BCC     65
    #define BMI     66
    #define BPL     67
    #define BVS     68
    #define BVC     69
    #define BHI     70
    #define BLS     71
    #define BGE     72
    #define BLT     73
    #define BGT     74
    #define YIELD   75
    #define WFE     76
    #define WFI     77
    #define SEV     78
    #define IT      79
    #define POP_PC  80
    #define SVC     81
    #define LDM_PC  82
    #define UDF     83
    #define CPSID   84
    #define CPSIE   85
    #define TBB     86
    

    #define TFM

    #ifdef TFM
        #define GET_SP_S(aligned_pc) tfm_get_sp_s(aligned_pc)
        #define READ_MEM_S(aligned_pc) tfm_read_mem_s(aligned_pc)
    #else
        #define GET_SP_S(aligned_pc) get_sp_s(aligned_pc)
        #define READ_MEM_S(aligned_pc) read_mem_s(aligned_pc)
    #endif


    // Input: The pc address of the instruction to decode
    // Output: The instruction code from the list above
    uint32_t decode_opcode(uint32_t, char secure);

    // Input: The instruction code from the list above
    void print_instruction(uint32_t);

    uint32_t is_opcode_32(uint32_t opcode, char secure);

#endif