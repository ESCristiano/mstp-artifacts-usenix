//******************************************************************************
//
// Lib Decode Opcodes & Dump Instructions Being Executed 
//
// This code is part of the MSTP project. It is a library to help with debug, 
// and it is only intended to be used in debug mode to help with the development
// evaluation and testing of the MSTP project. 
// 
// This lib follows armv8-m instruction set defined in Arm®  v8-M Architecture
// Reference Manual, and decodes the instruction being executed, based on the the
// opcode of the instruction. You can provide directly the opcode, or use the 
// the lib facilities to dump the instruction being executed using interrupts.
// Be aware that for this to work correctly, the system has to have all memory
// protections disabled, because we need to directly read the target app memory
// space. This is just for debug purposes, and it is not intended to be used in 
// production code.
//******************************************************************************

#include "armv8_decop_lib.h"

#define RED_COLOR() printf("\033[1;31m")
#define RESET_COLOR() printf("\033[0m")

// Clean left junk bits
#define AND_X_BITS(X) ((1<<(X))-1)
// Bits -> The value to compare
// N_X -> number don't care bits (right position), e.g., in 0b110xx N_X is 2
// N_BITS -> Total bits to compare in OP0
#define DECODE_BITS_EQ(OP0,BITS,N_X,N_BITS)(((OP0 >> N_X)&AND_X_BITS(N_BITS-N_X))==BITS)
// Bits -> The value to compare
// N_X -> number don't care bits (right position), e.g., in 0b110xx N_X is 2
// N_BITS -> Total bits to compare in OP0
#define DECODE_BITS_NEQ(OP0,BITS,N_X,N_BITS)(((OP0 >> N_X)&AND_X_BITS(N_BITS-N_X))!=BITS)

// op0 Subgroup ----------------------------------------------------------------
// 1010xx Add PC/SP (immediate)
#define OP_ADD_PC    0b1010
#define OP_ADD_PC_x  2
int opcode_16_1010xx(uint16_t opcode){
    char flags = (opcode >> 11) & AND_X_BITS(1);
  if( DECODE_BITS_EQ(flags, 0b1, 0, 1))
    return ADD;
  else
    return ADR;
}

// 010000 Data-processing (two low registers)
#define OP_DP    0b010000
#define OP_DP_x  0
#define DP_pos 6
int opcode_16_010000(uint16_t opcode){
  char flags = (opcode >> DP_pos) & AND_X_BITS(4);
  switch (flags){
      case 0b0110:
        return SBC;
      case 0b1011:
        return CMN;
      case 0b1000:
        return TST;
      case 0b1100:
        return ORR;
      case 0b1001:
        return RSB;
      case 0b0101:
        return ADC;
      case 0b0000:
        return AND;
      case 0b0001:
        return EOR;
      case 0b0010:
        return MOVS;
      case 0b0011:
        return MOVS;
      case 0b0100:
        return MOVS;
      case 0b0111:
        return MOVS;
      case 0b1101:
        return MUL;
      case 0b1010:
        return CMP;
      case 0b1111:
        return MVN;
      default:
        return I16;
    }
}

// 0101xx Load/store (register offset)
#define OP_LD    0b0101
#define OP_LD_x  2
#define LD_pos 9
  #define LD_STRH 0b010
  #define LD_STRB 0b100
  #define LD_STR  0b000
int opcode_16_0101xx(uint16_t opcode){
  char flags = (opcode >> LD_pos) & AND_X_BITS(3);
  switch (flags){
      case 0b000:
        return STR;
      case 0b100:
        return STR;
      case 0b010:
        return STR;
      default:
        return LDR;
    }
}

// 1100xx Load/store multiple
#define OP_LD_M     0b1100
#define OP_LD_M_x   2
#define LD_M_pos 11
  #define LD_M      0b1
int opcode_16_1100xx(uint16_t opcode){
  char flags = (opcode >> LD_M_pos) & AND_X_BITS(1);
  if( DECODE_BITS_EQ(flags, LD_M, 0, 1))
    return LDM;
  else
    return STM;
}

// 1101xx Conditional branch, and Supervisor Call
#define OP_CB   0b1101
#define OP_CB_x 2
#define CB_pos  8
  #define CB_EX 0b1110
int opcode_16_1101xx(uint16_t opcode){
  char flags = (opcode >> CB_pos) & AND_X_BITS(4);
  if( DECODE_BITS_EQ(flags, CB_EX, 0, 4))
    return UDF;
  else{
    char flags = (opcode >> 8) & AND_X_BITS(4);
    switch (flags){
      case 0b0000:
        return BEQ;
      case 0b0001:
        return BNE;
      case 0b0010:
        return BCS;
      case 0b0011:
        return BCC;
      case 0b0100:
        return BMI;
      case 0b0101:
        return BPL;
      case 0b0110:
        return BVS;
      case 0b0111:
        return BVC;
      case 0b1000:
        return BHI;
      case 0b1001:
        return BLS;
      case 0b1010:
        return BGE;
      case 0b1011:
        return BLT;
      case 0b1100:
        return BGT;
      case 0b1101:
        return BLE;
      case 0b1110:
        return B;
      case 0b1111:
        return SVC;
    }
  }
}

// 1001xx Load/store (SP-relative)
#define OP_LD_SP   0b1001
#define OP_LD_SP_x 2
#define LD_SP_pos 11
  #define LD_SP   0b1
int opcode_16_1001xx(uint16_t opcode){
  char flags = (opcode >> LD_SP_pos) & AND_X_BITS(1);
  if( DECODE_BITS_EQ(flags, LD_SP, 0, 1))
    return LDR;
  else
    return STR;
}

// 011xxx Load/store word/byte (immediate offset)
#define OP_LD_WB      0b011
#define OP_LD_WB_x    3
#define LD_WB_pos     11
  #define LD_WB_STR   0b00
  #define LD_WB_STRB  0b10
int opcode_16_011xxx(uint16_t opcode){
  char flags = (opcode >> LD_WB_pos) & AND_X_BITS(2);
  switch (flags){
      case 0b00:
        return STR;
      case 0b10:
        return STR;
      default:
        return LDR;
    }
}

// 010001 Special data instructions and branch and exchange
#define OP_BX    0b010001
#define OP_BX_x  0
#define BX_pos 8
  #define BX_OP 0b11
int opcode_16_010001(uint16_t opcode){
  char flags = (opcode >> BX_pos) & AND_X_BITS(2);
  if(DECODE_BITS_EQ(flags, BX_OP, 0, 2)){
    char flags_1 = (opcode >> 7) & AND_X_BITS(1);
    switch (flags_1){
      case 0b0:
        return BX;
      case 0b01:
        return BLX;
      default:
        return I16;
    }
  }
  else{
    char flags_1 = (opcode >> BX_pos) & AND_X_BITS(2);
    switch (flags_1){
      case 0b00:
        return ADD;
      case 0b01:
        return CMP;
      case 0b10:
        return MOV;
      default:
        return I16;
    }
  }
}

// 1000xx Load/store halfword (immediate offset)
#define OP_LD_H    0b1000
#define OP_LD_H_x  2
#define LD_H_pos 11
  #define LD_H   0b1
int opcode_16_1000xx(uint16_t opcode){
  char flags = (opcode >> LD_H_pos) & AND_X_BITS(1);
  if( DECODE_BITS_EQ(flags, LD_H, 0, 1))
    return LDR;
  else
    return STR;
}

// 00xxxx Shift (immediate), add, subtract, move, and compare
#define OP_SH    0b00
#define OP_SH_x  4
#define SH_pos 13
int opcode_16_00xxxx(uint16_t opcode){
    char flags = (opcode >> SH_pos) & AND_X_BITS(1);
  if(DECODE_BITS_EQ(flags, 0b1, 0, 1)){
    char flags_1 = (opcode >> 11) & AND_X_BITS(2);
    switch (flags_1){
      case 0b10:
        return ADD;
      case 0b11:
        return SUB;
      case 0b01:
        return CMP;
      case 0b00:
        return MOV;
      default:
        return I16;
    }
  }
  else{
    char flags_1 = (opcode >> 9) & AND_X_BITS(5);
    switch (flags_1){
      case 0b01100:
        return ADD;
      case 0b01101:
        return SUB;
      case 0b01110:
        return ADD;
      case 0b01111:
        return SUB;
      default:
        return MOV;
    }
  }
}

// 1011xx Miscellaneous 16-bit instructions
#define OP_MISC     0b1011
#define OP_MISC_x   2  
#define OP_MISC_pos 8  
  // Push and Pop 0bx10x 
  #define MISC_PUSH_POP 0b10
  #define MISC_PUSH_POP_x 1
#define OP_PUSH_POP_pos 11  
int opcode_16_1011xx(uint16_t opcode){
  char flags = (opcode >> OP_MISC_pos) & AND_X_BITS(4);
  // 3 bits, because the MSB is x
  if(DECODE_BITS_EQ(flags, MISC_PUSH_POP, MISC_PUSH_POP_x, 3)){
    if(DECODE_BITS_EQ((opcode>>11)&AND_X_BITS(1), 0b1, 0, 1))
      if(DECODE_BITS_EQ((opcode>>8)&AND_X_BITS(1), 0b1, 0, 1))
        return POP_PC;
      else 
        return POP;
    else
      return PUSH;
  }
  else{
    char flags_1 = (opcode >> OP_MISC_pos) & AND_X_BITS(4);
    switch (flags_1){
      case 0b0000:
        if(DECODE_BITS_EQ((opcode>>7)&AND_X_BITS(1), 0b1, 0, 1))
          return SUB;
        else
          return ADD;
      case 0b0010:{
        char flags_2 = (opcode >> 6) & AND_X_BITS(2);
        switch (flags_2){
          case 0b11:
            return UXTB;
          case 0b01:
            return UXTH;
          case 0b00:
            return SXTH;
          case 0b10:
            return SXTB;
        }
      }
      case 0b1010:{
          if(DECODE_BITS_NEQ((opcode>>6)&AND_X_BITS(2), 0b10, 0, 2)){
          char flags_2 = (opcode >> 6) & AND_X_BITS(2);
          switch (flags_2){
            case 0b01:
              return REV16;
            case 0b11:
              return REVSH;
            case 0b00:
              return REV;
          }
        }
      }
      case 0b0001:
        return CBZ;
      case 0b0011:
        return CBZ;
      case 0b1001:
        return CBNZ;
      case 0b1011:
        return CBNZ;
      case 0b1111:{
          if(DECODE_BITS_EQ((opcode)&AND_X_BITS(4), 0b0000, 0, 4)){
          char flags_2 = (opcode >> 4) & AND_X_BITS(4);
          switch (flags_2){
            case 0b0000:
              return NOP;
            case 0b0001:
              return YIELD;
            case 0b0010:
              return WFE;
            case 0b0011:
              return WFI;
            case 0b0100:
              return SEV;
            case 0b0101:
            case 0b0110: 
            case 0b0111:
            case 0b1000: 
            case 0b1001: 
            case 0b1010:
            case 0b1011:
            case 0b1100:
            case 0b1101:
            case 0b1110:
            case 0b1111:
              return NOP;
          }
        } else 
          return IT;
      }
      case 0b0110:
        if(DECODE_BITS_EQ((opcode>>6)&AND_X_BITS(2), 0b01, 0, 2)){
          if(DECODE_BITS_EQ((opcode>>5)&AND_X_BITS(1), 0b1, 0, 1)){
            if(DECODE_BITS_EQ((opcode>>4)&AND_X_BITS(1), 0b1, 0, 1))
              return CPSID;
            else
              return CPSIE;
          } else 
            return I16;
        } else 
          return I16;
      default:
        return I16;
    }
  }
}

// 01001x LDR (literal) T1
#define OP_LDR    0b01001
#define OP_LDR_x  1  
int opcode_16_01001x(uint16_t opcode){
  return LDR;
}

//------------------------------------------------------------------------------

int decode_opcode_16(uint16_t opcode){
  char op0 = (opcode >> 10) & AND_X_BITS(6);
  int inst = I16; 
  // 1010xx Add PC/SP (immediate)
  if(DECODE_BITS_EQ(op0,OP_ADD_PC, OP_ADD_PC_x,6))
    inst = opcode_16_1010xx(opcode);

  // 010000 Data-processing (two low registers)
  if(DECODE_BITS_EQ(op0,OP_DP, OP_DP_x,6))
    inst = opcode_16_010000(opcode);

  // 0101xx Load/store (register offset)
  if(DECODE_BITS_EQ(op0,OP_LD, OP_LD_x,6))
    inst = opcode_16_0101xx(opcode);

  // 1100xx Load/store multiple
  if(DECODE_BITS_EQ(op0,OP_LD_M, OP_LD_M_x,6))
    inst = opcode_16_1100xx(opcode);

  // 1101xx Conditional branch, and Supervisor Call
  if(DECODE_BITS_EQ(op0,OP_CB, OP_CB_x,6))
    inst = opcode_16_1101xx(opcode);

  // 1001xx Load/store (SP-relative)
  if(DECODE_BITS_EQ(op0,OP_LD_SP, OP_LD_SP_x,6))
    inst = opcode_16_1001xx(opcode);

  // 011xxx Load/store word/byte (immediate offset)
  if(DECODE_BITS_EQ(op0,OP_LD_WB, OP_LD_WB_x,6))
    inst = opcode_16_011xxx(opcode);

  // 010001 Special data instructions and branch and exchange
  if(DECODE_BITS_EQ(op0,OP_BX, OP_BX_x,6))
    inst = opcode_16_010001(opcode);

  // 1000xx Load/store halfword (immediate offset)
  if(DECODE_BITS_EQ(op0,OP_LD_H, OP_LD_H_x,6))
    inst = opcode_16_1000xx(opcode);

  // 00xxxx Shift (immediate), add, subtract, move, and compare
  if(DECODE_BITS_EQ(op0,OP_SH, OP_SH_x,6))
    inst = opcode_16_00xxxx(opcode);

  // 1011xx Miscellaneous 16-bit instructions
  if(DECODE_BITS_EQ(op0,OP_MISC, OP_MISC_x,6))
    inst = opcode_16_1011xx(opcode);

  // 01001x LDR (literal) T1
  if(DECODE_BITS_EQ(op0,OP_LDR, OP_LDR_x,6))
    inst = opcode_16_01001x(opcode);

  return inst;
}

//------------------------------------------------------------------------------
// op0    op1   op2
// 0100    -     - Load/store (multiple, dual, exclusive, acquire-release)
int opcode_32_0100_x_x(uint32_t opcode){
  // - 0x Load/store multiple
  if(DECODE_BITS_EQ(opcode>>21, 0b0, 1, 2)){
    if( DECODE_BITS_EQ(opcode>>20, 0b1, 0, 1) &&
        DECODE_BITS_EQ(opcode>>16, 0b1111, 0, 4) &&
        DECODE_BITS_EQ(opcode>>23, 0b01, 0, 2))
      return CLRM;
    if( DECODE_BITS_EQ(opcode>>20, 0b0, 0, 1) &&
        DECODE_BITS_EQ(opcode>>23, 0b10, 0, 2))
      return STMDB;
    if( DECODE_BITS_EQ(opcode>>20,  0b1, 0, 1) &&
        DECODE_BITS_NEQ(opcode>>16, 0b1111, 0, 4) &&
        DECODE_BITS_EQ(opcode>>23,  0b01, 0, 2))
        if (DECODE_BITS_EQ(opcode>>15, 0b1, 0, 1))
          return LDM_PC;
        else      
          return LDM;
    if( DECODE_BITS_EQ(opcode>>20, 0b1, 0, 1) &&
        DECODE_BITS_EQ(opcode>>23, 0b10, 0, 2))
      return LDMDB;
    if( DECODE_BITS_EQ(opcode>>20, 0b0, 0, 1) &&
        DECODE_BITS_EQ(opcode>>23, 0b01, 0, 2))
      return STM;
    if( DECODE_BITS_EQ(opcode>>20,  0b1, 0, 1) &&
        DECODE_BITS_NEQ(opcode>>16, 0b1101, 0, 4) &&
        DECODE_BITS_EQ(opcode>>23,  0b01, 0, 2))
        if (DECODE_BITS_EQ(opcode>>15, 0b1, 0, 1))
          return POP_PC;
        else      
          return POP;
    if( DECODE_BITS_EQ(opcode>>20, 0b0, 0, 1) &&
        DECODE_BITS_EQ(opcode>>16, 0b1101, 0, 4) &&
        DECODE_BITS_EQ(opcode>>23, 0b10, 0, 2))
      return STMDB;
  }
  // 0 11 Load/store dual (post-indexed)
  if( DECODE_BITS_EQ(opcode>>24, 0b0, 0, 1) &&
      DECODE_BITS_EQ(opcode>>21, 0b11, 0, 2)){
        if( DECODE_BITS_EQ(opcode>>20, 0b1, 0, 1))
          return LDRB;
        else
          return STRD;
    }
  // 1 10 Load/store dual (literal and immediate)
  if( DECODE_BITS_EQ(opcode>>24, 0b1, 0, 1) &&
      DECODE_BITS_EQ(opcode>>21, 0b10, 0, 2)){
        if(DECODE_BITS_EQ(opcode>>16, 0b1111, 0, 4))
          return LDRB;
        else{
          if( DECODE_BITS_EQ(opcode>>20, 0b1, 0, 1))
            return LDRB;
          else
            return STRD;
        }
    }
  //0 10 Load/store exclusive, load-acquire/store-release
  if( DECODE_BITS_EQ(opcode>>24, 0b0, 0, 1) &&
      DECODE_BITS_EQ(opcode>>21, 0b10, 0, 2)){
        if( DECODE_BITS_EQ(opcode>>23, 0b1, 0, 1) &&
            DECODE_BITS_EQ(opcode>>12, 0b1, 8, 9) &&
            DECODE_BITS_EQ(opcode>>5, 0b000, 0, 3))
          return TBB;
        else
          return I32;
    }
  //1 11 Load/store dual (pre-indexed), secure gateway
  if( DECODE_BITS_EQ(opcode>>24, 0b1, 0, 1) &&
      DECODE_BITS_EQ(opcode>>21, 0b11, 0, 2)){
        if(DECODE_BITS_EQ(opcode>>23, 0b0, 0, 1))
          if(DECODE_BITS_EQ(opcode>>0, 0b111111110100101111111, 0, 20))
            return SG;
        if(DECODE_BITS_EQ(opcode>>20, 0b1, 0, 1))
          if(DECODE_BITS_NEQ(opcode>>16, 0b1111, 0, 4))
            return LDRD;
    }
  return I32;
}
// op0    op1   op2
// 10xx   -      1 Branches and miscellaneous control
int opcode_32_10xx_x_1(uint32_t opcode){
  char flags_1 = (opcode >> 22) & AND_X_BITS(4);

  if( DECODE_BITS_EQ(opcode>>14, 0b0, 0, 1) &&
      DECODE_BITS_EQ(opcode>>12, 0b1, 0, 1))
      return B;
  else
  if( DECODE_BITS_EQ(opcode>>14, 0b1, 0, 1) &&
      DECODE_BITS_EQ(opcode>>12, 0b1, 0, 1))
      return BL;
  else
  if(DECODE_BITS_NEQ(flags_1, 0b111, 1, 4)){
    if( DECODE_BITS_EQ(opcode>>14, 0b0, 0, 1) &&
        DECODE_BITS_EQ(opcode>>12, 0b0, 0, 1))
      return B;
  }
  return I32;
}
// op0    op1   op2
// 10x0   -      0 Data-processing (modiﬁed immediate)
int opcode_32_10x0_x_0(uint32_t opcode){
  char flags = (opcode >> 20) & AND_X_BITS(5);
  switch (flags){
    case 0b00001:
      if(DECODE_BITS_NEQ(opcode>>8, 0b1111, 0, 4))
        return ANDS;
      if(DECODE_BITS_EQ(opcode>>8, 0b1111, 0, 4))
        return TST;
    case 0b01001:
      if(DECODE_BITS_EQ(opcode>>8, 0b1111, 0, 4))
        return TEQ;
      if(DECODE_BITS_NEQ(opcode>>8, 0b1111, 0, 4))
        return EORS;
    case 0b11011:
      if(DECODE_BITS_NEQ(opcode>>8, 0b1111, 0, 4)) 
        return SUBS;
      if(DECODE_BITS_EQ(opcode>>8, 0b1111, 0, 4))
        return CMP;
    case 0b10000:
      return ADD;
    case 0b11010:
      return SUB;
    case 0b10001:
      if(DECODE_BITS_EQ(opcode>>8, 0b1111, 0, 4))
        return CMN;
      else
        return ADDS;
    case 0b00000:
      return AND;
    case 0b01000:
      return EOR;
  }
  flags = (opcode >> 21) & AND_X_BITS(4);
  switch (flags){
    case 0b0010:
      if(DECODE_BITS_NEQ(opcode>>16, 0b1111, 0, 4))
        return ORR;
      else
        return MOV;
    case 0b0001:
        return BIC;
    case 0b0011:
      if(DECODE_BITS_NEQ(opcode>>16, 0b1111, 0, 4))
        return ORN;
      else
        return MOVN;
    case 0b1011:
      return SBC;
    case 0b1110:
      return SUB;
    case 0b1010:
      return ADC;
  }
  return I32;
}

// op0    op1   op2
// 1101   0xxxx  - Data-processing (register)
int opcode_32_1101_0xxxx_x(uint32_t opcode){
  if( DECODE_BITS_EQ(opcode>>14, 0b0, 0, 1) &&
      DECODE_BITS_EQ(opcode>>23, 0b0000, 0, 4))
    return MOV;
  else
    return I32;
}
// op0    op1   op2
// 1100 != 1xxx0 - Load/store single
int opcode_32_1100_1xxx0_x(uint32_t opcode){
  // if Rt = R15 && L = 1, then it is a PC-relative load
  if( DECODE_BITS_EQ(opcode>>12, 0b1111, 0, 4) &&
      DECODE_BITS_EQ(opcode>>20, 0b1, 0, 1))
    return LD_PC;
  else
    return I32;
}
// op0    op1   op2
// x11x   -      - Coprocessor, ﬂoating-point, and vector instructions
int opcode_32_x11x_x_x(uint32_t opcode){
  return I32;
}
// op0    op1   op2
// 1101   11xxx  - Long multiply and divide
int opcode_32_1101_11xxx_x(uint32_t opcode){
  return I32;
}
// op0    op1   op2
// 0101   -      - Data-processing (shifted register)
int opcode_32_0101_x_x(uint32_t opcode){
  return I32;
}
// op0    op1   op2
// 1100   1xxx0  - UNALLOCATED

// op0    op1   op2
// 10x1   -      0 Data-processing (plain binary immediate)
int __attribute__((optimize(0)))  opcode_32_10x1_x_0(uint32_t opcode){
  if( DECODE_BITS_EQ(opcode>>24, 0b0, 0, 1) &&
      DECODE_BITS_EQ(opcode>>21, 0b10, 0, 2))
  {
    if(DECODE_BITS_EQ(opcode>>23, 0b1, 0, 1))
      return MOVT;
    else
      return MOVW;
  }
  else
    return I32;
}
// op0    op1   op2
// 1101   10xxx  - Multiply, multiply accumulate, and absolute difference
int opcode_32_1101_10xxx_x(uint32_t opcode){
  return I32;
}

int __attribute__((optimize(0))) decode_opcode_32(const uint32_t opcode){
  char op0 = opcode >> 25;
  int inst = I32; 
  // op0    op1   op2
  // 0100    -     - Load/store (multiple, dual, exclusive, acquire-release)
  if(DECODE_BITS_EQ(op0, 0b0100, 0, 4))
    inst = opcode_32_0100_x_x(opcode);

  // op0    op1   op2
  // 10xx   -      1 Branches and miscellaneous control
  if(DECODE_BITS_EQ(opcode>>27, 0b10, 0,2) && 
      DECODE_BITS_EQ(opcode>>15, 0b1, 0, 1))
    inst = opcode_32_10xx_x_1(opcode);
  
  // op0    op1   op2
  // 10x0   -      0 Data-processing (modiﬁed immediate)
  if(DECODE_BITS_EQ(opcode>>27, 0b10, 0,2) &&
      DECODE_BITS_EQ(opcode>>25, 0b0, 0, 1) &&
      DECODE_BITS_EQ(opcode>>15, 0b0, 0, 1))
    inst = opcode_32_10x0_x_0(opcode);
  
  // op0    op1   op2
  // 1101   0xxxx  - Data-processing (register)
  if(DECODE_BITS_EQ(op0, 0b1101, 0,4) &&
      DECODE_BITS_EQ(opcode>>20, 0b0, 4, 5))
    inst = opcode_32_1101_0xxxx_x(opcode);

  // op0    op1   op2
  // 1100 != 1xxx0 - Load/store single
  if(DECODE_BITS_EQ(op0,0b1100, 0,4) &&
      DECODE_BITS_NEQ(opcode>>24, 0b1, 0, 1) &&
      DECODE_BITS_NEQ(opcode>>20, 0b0, 0, 1))
    inst = opcode_32_1100_1xxx0_x(opcode);

  // op0    op1   op2
  // x11x   -      - Coprocessor, ﬂoating-point, and vector instructions
  if(DECODE_BITS_EQ(opcode>>25,0b11, 1,3))
    inst = opcode_32_x11x_x_x(opcode);

  // op0    op1   op2
  // 1101   11xxx  - Long multiply and divide
  if(DECODE_BITS_EQ(op0, 0b1101, 0,4)&&
      DECODE_BITS_EQ(opcode>>20, 0b11, 3, 5))
    inst = opcode_32_1101_11xxx_x(opcode);

  // op0    op1   op2
  // 0101   -      - Data-processing (shifted register)
  if(DECODE_BITS_EQ(op0,0b0101, 0,4))
    inst = opcode_32_0101_x_x(opcode);

  // op0    op1   op2
  // 1100   1xxx0  - UNALLOCATED

  // op0    op1   op2
  // 10x1   -      0 Data-processing (plain binary immediate)
  if(DECODE_BITS_EQ(opcode>>27, 0b10, 0,2) &&
      DECODE_BITS_EQ(opcode>>25, 0b1, 0, 1) &&
      DECODE_BITS_EQ(opcode>>15, 0b0, 0, 1))
    inst = opcode_32_10x1_x_0(opcode);

  // op0    op1   op2
  // 1101   10xxx  - Multiply, multiply accumulate, and absolute difference
  if(DECODE_BITS_EQ(op0, 0b1101, 0, 4)&&
      DECODE_BITS_EQ(opcode>>20, 0b10, 3, 5))
    inst = opcode_32_1101_10xxx_x(opcode);
  return inst;
}

//------------------------------------------------------------------------------
#define ENCONDING_32BITS 0b11100

union op
{
  uint32_t word[2];
  uint16_t nibble[4];
};

uint32_t instruction_encoding(uint32_t pc, char secure){
  uint32_t *aligned_pc;
  uint32_t opcode_32;
  uint16_t opcode_16;
  uint8_t align_2 = pc & 0x2 ? 1 : 0;
  union op u_opcode;
  
  // We have to access with 4 bytes alignment 
  //fetch next in case of 1st nibble in pc+2 and 2nd in pc+4
  aligned_pc = (uint32_t)(pc+4)&(~0x3);
  if(secure)
    u_opcode.word[1] = READ_MEM_S(aligned_pc);
  else
    u_opcode.word[1] = *aligned_pc;

  aligned_pc = (uint32_t)(pc)&(~0x3);
  if(secure)
    u_opcode.word[0] = READ_MEM_S(aligned_pc);
  else
    u_opcode.word[0] = *aligned_pc;
  
  // But the flash/code is aligned at 2 bytes, so we need some conversions
  opcode_16 = u_opcode.nibble[align_2];

  if( ((opcode_16 >> 11) & AND_X_BITS(5))> ENCONDING_32BITS )
    return I32;
  else
    return I16;
}

uint32_t is_opcode_32(uint32_t pc, char secure){
  if(instruction_encoding(pc, secure) == I32 )
    return 1;
  else
    return 0;
}

uint32_t decode_opcode(uint32_t pc, char secure){
  uint32_t *aligned_pc;
  uint32_t opcode_32;
  uint16_t opcode_16;
  uint8_t align_2 = pc & 0x2 ? 1 : 0;
  union op u_opcode;
  
  // We have to access with 4 bytes alignment 
  //fetch next in case of 1st nibble in pc+2 and 2nd in pc+4
  aligned_pc = (uint32_t)(pc+4)&(~0x3);
  if(secure)
    u_opcode.word[1] = READ_MEM_S(aligned_pc);
  else
    u_opcode.word[1] = *aligned_pc;

  aligned_pc = (uint32_t)(pc)&(~0x3);
  if(secure)
    u_opcode.word[0] = READ_MEM_S(aligned_pc);
  else
    u_opcode.word[0] = *aligned_pc;
  
  // But the flash/code is aligned at 2 bytes, so we need some conversions
  opcode_16 = u_opcode.nibble[align_2];

  //  > because xx has to be != 0b00 (0b111xx)
  if( ((opcode_16 >> 11) & AND_X_BITS(5))> ENCONDING_32BITS ){
    opcode_32 = (u_opcode.nibble[align_2] << 16) | u_opcode.nibble[align_2+1];
    return decode_opcode_32(opcode_32);
  }
  else if(((opcode_16 >> 11) & AND_X_BITS(5)) == 0b11100)
    return B;
  else
    return decode_opcode_16(opcode_16);
}

uint32_t is_pc_chaning_inst(uint32_t pc, char secure){
  switch (decode_opcode(pc, secure))
  {
    case BLE:   case BCS: case BCC: case BMI: case BPL:
    case BVS:   case BVC: case BHI: case BLS: case BGE:
    case BLT:   case BGT: case BNE: case BEQ: case CBZ:
    case CBNZ:  case B:   case BL:  case BX:  case BLX: 
    case LD_PC: case POP_PC: case SVC: case LDM_PC:
    case TBB:
      return 1;
    default:
      return 0;
  }
}

void print_instruction(uint32_t instruction){
  switch (instruction) {
    case MOV:    printf(" MOV\t "); break;
    case LDR:    printf(" LDR\t "); break;
    case NOP:    printf(" NOP\t "); break;
    case PUSH:   printf(" PUSH\t "); break;
    case POP:    printf(" POP\t "); break;
    case SUB:    printf(" SUB\t "); break;
    case ADD:    printf(" ADD\t "); break;
    case MOVS:   printf(" MOVS\t "); break;
    case CMP:    printf(" CMP\t "); break;
    case ADDS:   printf(" ADDS\t "); break;
    case LDRW:   printf(" LDRW\t "); break;
    case STR:    printf(" STR\t "); break;
    case LSL:    printf(" LSL\t "); break;
    case ANDS:   printf(" ANDS\t "); break;
    case UXTB:   printf(" UXTB\t "); break;
    case STRB:   printf(" STRB\t "); break;
    case LD_PC:  printf(" LD_PC\t"); break;
    case RSB:    printf(" RSB\t "); break;
    case I16:    printf(" I16\t "); break;
    case I32:    printf(" I32\t "); break;
    case LDM:    printf(" LDM\t "); break;
    case STM:    printf(" STM\t "); break;
    case SBC:    printf(" SBC\t "); break;
    case CMN:    printf(" CMN\t "); break;
    case TST:    printf(" TST\t "); break;
    case ORR:    printf(" ORR\t "); break;
    case ADC:    printf(" ADC\t "); break;
    case AND:    printf(" AND\t "); break;
    case EOR:    printf(" EOR\t "); break;
    case MUL:    printf(" MUL\t "); break;
    case MVN:    printf(" MVN\t "); break;
    case ADR:    printf(" ADR\t "); break;
    case CLRM:   printf(" CLRM\t "); break;
    case STMDB:  printf(" STMDB\t"); break;
    case LDMDB:  printf(" LDMDB\t"); break;
    case UXTH:   printf(" UXTH\t "); break;
    case SXTH:   printf(" SXTH\t "); break;
    case SXTB:   printf(" SXTB\t "); break;
    case REV16:  printf(" REV16\t"); break;
    case REVSH:  printf(" REVSH\t"); break;
    case REV:    printf(" REV\t "); break;
    case LDRB:   printf(" LDRB\t "); break;
    case STRD:   printf(" STRD\t "); break;
    case SG:     printf(" SG\t "); break;
    case LDRD:   printf(" LDRD\t "); break;
    case TEQ:    printf(" TEQ\t "); break;
    case EORS:   printf(" EORS\t "); break;
    case SUBS:   printf(" SUBS\t "); break;
    case BIC:    printf(" BIC\t "); break;
    case ORN:    printf(" ORN\t "); break;
    case MOVN:   printf(" MOVN\t "); break;
    case MOVT:   printf(" MOVT\t "); break;
    case YIELD:  printf(" YIELD\t "); break;
    case WFE:    printf(" WFE\t "); break;
    case WFI:    printf(" WFI\t "); break;
    case SEV:    printf(" SEV\t "); break;
    case IT:     printf(" IT\t "); break;
    case POP_PC: printf(" POP_PC\t "); break;
    case SVC:    printf(" SVC\t "); break;
    case LDM_PC: printf(" LDM_PC\t "); break; 
    case UDF:    printf(" UDF\t "); break; 
    case CPSID:  printf(" CPSID\t "); break; 
    case CPSIE:  printf(" CPSIE\t "); break; 
    
    // Conditional branches with optional color
    case BLE: case BCS: case BCC: case BMI: case BPL:
    case BVS: case BVC: case BHI: case BLS: case BGE:
    case BLT: case BGT: case BNE: case BEQ: case CBZ:
    case CBNZ: case B: case BL: case BX: case BLX:
    case TBB:
      #ifdef COLOR
        RED_COLOR();
        switch (instruction) {
          case BLE:   printf(" BLE\t"); break;
          case BCS:   printf(" BCS\t"); break;
          case BCC:   printf(" BCC\t"); break;
          case BMI:   printf(" BMI\t"); break;
          case BPL:   printf(" BPL\t"); break;
          case BVS:   printf(" BVS\t"); break;
          case BVC:   printf(" BVC\t"); break;
          case BHI:   printf(" BHI\t"); break;
          case BLS:   printf(" BLS\t"); break;
          case BGE:   printf(" BGE\t"); break;
          case BLT:   printf(" BLT\t"); break;
          case BGT:   printf(" BGT\t"); break;
          case BNE:   printf(" BNE\t"); break;
          case BEQ:   printf(" BEQ\t"); break;
          case CBZ:   printf(" CBZ\t"); break;
          case CBNZ:  printf(" CBNZ\t"); break;
          case B:     printf(" B\t"); break;
          case BL:    printf(" BL\t"); break;
          case BX:    printf(" BX\t"); break;
          case BLX:   printf(" BLX\t"); break;
          case TBB:   printf(" TBB\t "); break; 
        }
        RESET_COLOR();
        // printf("|");
      #else
        switch (instruction) {
          case BLE:   printf(" BLE\t"); break;
          case BCS:   printf(" BCS\t"); break;
          case BCC:   printf(" BCC\t"); break;
          case BMI:   printf(" BMI\t"); break;
          case BPL:   printf(" BPL\t"); break;
          case BVS:   printf(" BVS\t"); break;
          case BVC:   printf(" BVC\t"); break;
          case BHI:   printf(" BHI\t"); break;
          case BLS:   printf(" BLS\t"); break;
          case BGE:   printf(" BGE\t"); break;
          case BLT:   printf(" BLT\t"); break;
          case BGT:   printf(" BGT\t"); break;
          case BNE:   printf(" BNE\t"); break;
          case BEQ:   printf(" BEQ\t"); break;
          case CBZ:   printf(" CBZ\t"); break;
          case CBNZ:  printf(" CBNZ\t"); break;
          case B:     printf(" B\t"); break;
          case BL:    printf(" BL\t"); break;
          case BX:    printf(" BX\t"); break;
          case BLX:   printf(" BLX\t"); break;
          case TBB:   printf(" TBB\t "); break; 
        }
      #endif
      break;
    default:
      printf(" UNKNOWN\t");
      break;
  }
}
