#include "../common.h"
#include "cpu.h"

void init_cpu(struct cpu* emu_cpu)
{
  init_regs(emu_cpu);
  // for test
  emu_cpu->memory[0xFFFC] = 0x00;
  emu_cpu->memory[0xFFFD] = 0x80;
}

void init_regs(struct cpu* emu_cpu)
{
  emu_cpu->reg_A = 0;
  emu_cpu->reg_X = 0;
  emu_cpu->reg_Y = 0;
  emu_cpu->reg_P = 0x34;
  emu_cpu->reg_SP = 0x1fd;
  emu_cpu->reg_PC = 0;
}

void reset(struct cpu* emu_cpu)
{
  init_regs(emu_cpu);
  emu_cpu->reg_PC = *(uint16_t*)&(emu_cpu->memory[VEC_RESET]);
}

uint8_t fetch(struct cpu* emu_cpu)
{
  return emu_cpu->memory[emu_cpu->reg_PC++];
}

uint8_t get_status_at(struct cpu* emu_cpu, uint8_t at)
{
  if((emu_cpu->reg_P & (1 << at)) != 0) 
    return 1;
  else
    return 0;
}

// TODO none sense
void set_reg_P(struct cpu* emu_cpu, uint8_t at, uint8_t val)
{
  uint8_t set, unset;
  bool flag;
  set = (1<<at);
  unset = (0xff ^ set);
  flag = false;
  switch(at) {
    case STATUS_N:
      if(val & 0x80)
        flag = true;
      break;
    case STATUS_Z:
      if(val == 0)
        flag = true;
      break;
    case STATUS_B:
    case STATUS_D:
    case STATUS_I:
    case STATUS_V:
    case STATUS_C:
      if(val)
        flag = true;
      break;
  }
  if(flag)
    emu_cpu->reg_P |= set;
  else
    emu_cpu->reg_P &= unset;
}
void set_carry(struct cpu* emu_cpu, uint16_t val)
{
  if(val > 0xff)
    set_reg_P(emu_cpu, STATUS_C, 1);
  else
    set_reg_P(emu_cpu, STATUS_C, 0);
}

void set_overflow(struct cpu* emu_cpu, uint8_t x, uint8_t y, uint8_t op)
{
  // TODO debug
  uint8_t tmp;
  uint8_t c;
  c = get_status_at(emu_cpu, STATUS_C);
  if(op == OP_ADC){
    tmp = x + y + c;
    if(((x ^ y) & 0x80) == 0 && ((tmp ^ x) & 0x80) != 0)
      set_reg_P(emu_cpu, STATUS_V, 1);
    else
      set_reg_P(emu_cpu, STATUS_V, 0);
  } 
  else if(op == OP_SBC){
    tmp = x - y - ((c&1) ^ 1);
    if(((x ^ y) & 0x80) != 0 && ((tmp ^ x) & 0x80) != 0)
      set_reg_P(emu_cpu, STATUS_V, 1);
    else 
      set_reg_P(emu_cpu, STATUS_V, 0);
  }
  else {
    puts("[E] Error in set_overflow");
    exit(1);
  }
  return;
}

uint16_t get_operand(struct cpu* emu_cpu, uint8_t op)
{
  uint16_t upper, lower, tmp_pc;

  switch(AM[op]) {
    case AM_IMPL:
    case AM_ACCM:
      return 0;
    case AM_IMMD:
    case AM_ZPG :
      return (uint16_t)fetch(emu_cpu);
    case AM_ZPGX:
      return (uint8_t)(fetch(emu_cpu) + emu_cpu->reg_X);
    case AM_ZPGY:
      return (uint8_t)(fetch(emu_cpu) + emu_cpu->reg_Y);
    case AM_ABS :
      lower = fetch(emu_cpu);
      upper = (uint16_t)fetch(emu_cpu) << 8;
      return (lower | upper);
    case AM_ABSX:
      lower = fetch(emu_cpu);
      upper = (uint16_t)fetch(emu_cpu) << 8;
      return (lower | upper) + (uint16_t)emu_cpu->reg_X;
      //return (int16_t)(lower | upper) + (int8_t)emu_cpu->reg_X;
    case AM_ABSY:
      lower = fetch(emu_cpu);
      upper = (uint16_t)fetch(emu_cpu) << 8;
      return (lower | upper) + (uint16_t)emu_cpu->reg_Y;
      //return (int16_t)(lower | upper) + (int8_t)emu_cpu->reg_Y;
    case AM_REL :
      // TODO debug
      tmp_pc = emu_cpu->reg_PC;
      return tmp_pc + (int16_t)fetch(emu_cpu);
    case AM_XIND:
      tmp_pc = (uint8_t)(fetch(emu_cpu) + emu_cpu->reg_X);
      return *(uint16_t*)&(emu_cpu->memory[tmp_pc]);
    case AM_INDY:
      tmp_pc = (uint16_t)fetch(emu_cpu);
      return *(uint16_t*)&(emu_cpu->memory[tmp_pc]) + (uint16_t)emu_cpu->reg_Y;
    case AM_IND :
      lower = fetch(emu_cpu);
      upper = (uint16_t)fetch(emu_cpu) << 8;
      return (lower | upper);
    case AM_UNDF:
    default:
      printf("[E] Undefined addressing mode\n");
      exit(1);
      break;
  }
}

void exec(struct cpu* emu_cpu, uint8_t op, uint16_t operand)
{
  uint16_t result;
  uint8_t tmp_m;
  uint8_t tmp_a;
  switch(OPCODE[op]) {
    case OP_ADC:
      if(AM[op] == AM_IMMD)
        tmp_m = operand;
      else
        tmp_m = emu_cpu->memory[operand];
      result = emu_cpu->reg_A + tmp_m + get_status_at(emu_cpu, STATUS_C);
      set_overflow(emu_cpu, emu_cpu->reg_A, tmp_m, OP_ADC); 
      set_carry(emu_cpu, result);
      emu_cpu->reg_A = result;
      set_reg_P(emu_cpu, STATUS_N, emu_cpu->reg_A);
      set_reg_P(emu_cpu, STATUS_Z, emu_cpu->reg_A);
      break;
    case OP_SBC:
    case OP_AND:
    case OP_ORA:
    case OP_EOR:
    case OP_ASL:
    case OP_LSR:
    case OP_ROL:
    case OP_ROR:
    case OP_BCC:
    case OP_BCS:
    case OP_BEQ:
    case OP_BNE:
    case OP_BVC:
    case OP_BVS:
    case OP_BPL:
    case OP_BMI:
    case OP_BIT:
    case OP_JMP:
    case OP_JSR:
    case OP_RTS:
    case OP_BRK:
    case OP_RTI:
    case OP_CMP:
    case OP_CPX:
    case OP_CPY:
    case OP_INC:
    case OP_DEC:
    case OP_INX:
    case OP_DEX:
    case OP_INY:
    case OP_DEY:
    case OP_CLC:
    case OP_SEC:
    case OP_CLI:
    case OP_SEI:
    case OP_CLD:
    case OP_SED:
    case OP_CLV:
    case OP_LDA:
      if(AM[op] == AM_IMMD)
        emu_cpu->reg_A = operand;
      else
        emu_cpu->reg_A = emu_cpu->memory[operand];
      set_reg_P(emu_cpu, STATUS_N, emu_cpu->reg_A);
      set_reg_P(emu_cpu, STATUS_Z, emu_cpu->reg_A);
      break;
    case OP_LDX:
      if(AM[op] == AM_IMMD)
        emu_cpu->reg_X = operand;
      else
        emu_cpu->reg_X = emu_cpu->memory[operand];
      set_reg_P(emu_cpu, STATUS_N, emu_cpu->reg_X);
      set_reg_P(emu_cpu, STATUS_Z, emu_cpu->reg_X);
      break;
    case OP_LDY:
      if(AM[op] == AM_IMMD)
        emu_cpu->reg_Y = operand;
      else
        emu_cpu->reg_Y = emu_cpu->memory[operand];
      set_reg_P(emu_cpu, STATUS_N, emu_cpu->reg_Y);
      set_reg_P(emu_cpu, STATUS_Z, emu_cpu->reg_Y);
      break;
    case OP_STA:
      emu_cpu->memory[operand] = emu_cpu->reg_A;
      break;
    case OP_STX:
      emu_cpu->memory[operand] = emu_cpu->reg_X;
      break;
    case OP_STY:
      emu_cpu->memory[operand] = emu_cpu->reg_Y;
      break;
    case OP_TAX:
      emu_cpu->reg_X = emu_cpu->reg_A;
      set_reg_P(emu_cpu, STATUS_N, emu_cpu->reg_X);
      set_reg_P(emu_cpu, STATUS_Z, emu_cpu->reg_X);
      break;
    case OP_TXA:
      emu_cpu->reg_A = emu_cpu->reg_X;
      set_reg_P(emu_cpu, STATUS_N, emu_cpu->reg_A);
      set_reg_P(emu_cpu, STATUS_Z, emu_cpu->reg_A);
      break;
    case OP_TAY:
      emu_cpu->reg_Y = emu_cpu->reg_A;
      set_reg_P(emu_cpu, STATUS_N, emu_cpu->reg_Y);
      set_reg_P(emu_cpu, STATUS_Z, emu_cpu->reg_Y);
      break;
    case OP_TYA:
      emu_cpu->reg_A = emu_cpu->reg_Y;
      set_reg_P(emu_cpu, STATUS_N, emu_cpu->reg_A);
      set_reg_P(emu_cpu, STATUS_Z, emu_cpu->reg_A);
      break;
    case OP_TSX:
      emu_cpu->reg_X = emu_cpu->reg_SP & 0xff;
      set_reg_P(emu_cpu, STATUS_N, emu_cpu->reg_X);
      set_reg_P(emu_cpu, STATUS_Z, emu_cpu->reg_X);
      break;
    case OP_TXS:
      emu_cpu->reg_SP = emu_cpu->reg_X+0x100;
      break;
    case OP_PHA:
    case OP_PLA:
    case OP_PHP:
    case OP_PLP:
    case OP_NOP:
      break;
    case OP_UND:
      puts("[E] Undefined OPERATION");
      exit(1);
  }
}

void run(struct cpu* emu_cpu)
{
  int tmp;
  uint8_t op;
  uint16_t operand;
  
  puts("execution");
  
  for(tmp = 0; tmp < 2; tmp++){
    debug(emu_cpu);
    op = fetch(emu_cpu);
    operand = get_operand(emu_cpu, op);
    exec(emu_cpu, op, operand);
  }
}

// FUNCTIONS FOR DEBUG 
void debug(struct cpu* emu_cpu)
{
  int i;
  printf("-------------------\n");
  printf("A : 0x%04x\n", emu_cpu->reg_A);
  printf("X : 0x%04x\n", emu_cpu->reg_X);
  printf("Y : 0x%04x\n", emu_cpu->reg_Y);
  printf("SP: 0x%04x\n", emu_cpu->reg_SP);
  printf("P : 0x%04x\n", emu_cpu->reg_P);
  printf("PC: 0x%04x\n", emu_cpu->reg_PC);
  printf("STATUS: | ");
  for(i = 0; i < 8; i++){
    if(emu_cpu->reg_P & (1<<i)){
      printf("%s | ", status_str[i]);
    }
  }
  puts("");
}

void dump(struct cpu* emu_cpu, uint16_t addr, int32_t size)
{
  int l,i;
  printf("--- memory dump ---\n");
  for(l = 0; l < size; l+=8){
    printf("[0x%04x]: ", addr+l);
    for(i = 0; i+l < size && i < 8; i++)
      printf("%02x ", emu_cpu->memory[addr+i+l]);
    puts("");
  }
}