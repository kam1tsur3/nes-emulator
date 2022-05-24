#include "../common.h"
#include "header/cpu.h"

// extern functions
extern uint8_t cpu_bus_read(uint16_t);
extern void cpu_bus_write(uint16_t, uint8_t);
extern uint16_t cpu_bus_read_word(uint16_t);

// global variable
struct cpu emu_cpu;
bool flag_nmi = 0;

void init_regs()
{
  emu_cpu.reg_A = 0;
  emu_cpu.reg_X = 0;
  emu_cpu.reg_Y = 0;
  emu_cpu.reg_P = 0x34;
  emu_cpu.reg_SP = 0x1fd;
  emu_cpu.reg_PC = 0x8000;
}

void reset()
{
  init_regs();
  emu_cpu.reg_PC = cpu_bus_read_word(VEC_RESET);
}

uint8_t fetch()
{
  return cpu_bus_read(emu_cpu.reg_PC++);
}

uint8_t get_status_at(uint8_t at)
{
  if((emu_cpu.reg_P & (1 << at)) != 0) 
    return 1;
  else
    return 0;
}

void set_reg_P(uint8_t at, bool tf)
{
  uint8_t set, unset;
  set = (1<<at);
  unset = (0xff ^ set);
  if(tf)
    emu_cpu.reg_P |= set;
  else
    emu_cpu.reg_P &= unset;
}

bool check_carry(uint8_t x, uint8_t y, uint8_t op)
{
  uint16_t tmp;
  uint8_t c;
  c = get_status_at(STATUS_C);
  if(op == OP_ADC){
    tmp = x + y + c;
    if(tmp > 0xff)
      return true;
    else
      return false;  
  }
  else if(op == OP_SBC){
    tmp = x + ((y^0xff) +1) - ((c&1)^1);
    if(tmp > 0xff)
      return true;
    else 
      return false;
  } else if(op == OP_CMP || op == OP_CPX || op == OP_CPY) {
    tmp = x + ((y^0xff) +1);
    if(tmp > 0xff)
      return true;
    else 
      return false;
  } else {
    ERROR("Error in check_carry");
  }
}

bool check_overflow(uint8_t x, uint8_t y, uint8_t op)
{
  // TODO debug
  uint8_t tmp;
  uint8_t c;
  c = get_status_at(STATUS_C);
  if(op == OP_ADC){
    tmp = x + y + c;
    if(((x ^ y) & 0x80) == 0 && ((tmp ^ x) & 0x80) != 0)
      return true;
    else
      return false;
  } 
  else if(op == OP_SBC){
    tmp = x - y - ((c&1) ^ 1);
    if(((x ^ y) & 0x80) != 0 && ((tmp ^ x) & 0x80) != 0)
      return true;
    else 
      return false;
  }
  else {
    ERROR("Error in check_overflow");
  }
  return false;
}

uint8_t pop_SP()
{
  emu_cpu.reg_SP = ((emu_cpu.reg_SP + 1) & 0xff) | 0x100;
  return cpu_bus_read(emu_cpu.reg_SP);
}

void push_SP(uint8_t val)
{
  cpu_bus_write(emu_cpu.reg_SP, val);
  emu_cpu.reg_SP = ((emu_cpu.reg_SP - 1) & 0xff) | 0x100;
}

void nmi()
{
  set_reg_P(STATUS_B,false);
  push_SP(emu_cpu.reg_PC >> 8);
  push_SP(emu_cpu.reg_PC&0xff);
  push_SP(emu_cpu.reg_P);
  set_reg_P(STATUS_I,true);
  emu_cpu.reg_PC = cpu_bus_read_word(VEC_NMI);
  flag_nmi = false;
}

uint16_t get_operand(uint8_t op)
{
  uint16_t upper, lower, tmp_pc;

  switch(AM[op]) {
    case AM_IMPL:
    case AM_ACCM:
      return 0;
    case AM_IMMD:
    case AM_ZPG :
      return (uint16_t)fetch();
    case AM_ZPGX:
      return (uint8_t)(fetch() + emu_cpu.reg_X);
    case AM_ZPGY:
      return (uint8_t)(fetch() + emu_cpu.reg_Y);
    case AM_ABS :
      lower = fetch();
      upper = (uint16_t)fetch() << 8;
      return (lower | upper);
    case AM_ABSX:
      lower = fetch();
      upper = (uint16_t)fetch() << 8;
      return (lower | upper) + (uint16_t)emu_cpu.reg_X;
    case AM_ABSY:
      lower = fetch();
      upper = (uint16_t)fetch() << 8;
      return (lower | upper) + (uint16_t)emu_cpu.reg_Y;
    case AM_REL :
      return (int8_t)fetch() + emu_cpu.reg_PC;
    case AM_XIND:
      tmp_pc = (uint8_t)(fetch() + emu_cpu.reg_X);
      return cpu_bus_read_word(tmp_pc);
      //return *(uint16_t*)&(emu_cpu.memory[tmp_pc]);
    case AM_INDY:
      tmp_pc = (uint16_t)fetch();
      return cpu_bus_read_word(tmp_pc) + (uint16_t)emu_cpu.reg_Y;
      //return *(uint16_t*)&(emu_cpu.memory[tmp_pc]) + (uint16_t)emu_cpu.reg_Y;
    case AM_IND :
      lower = fetch();
      upper = (uint16_t)fetch() << 8;
      return cpu_bus_read_word(lower | upper);
      //return *(uint16_t*)&(emu_cpu.memory[lower | upper]);
    case AM_UNDF:
    default:
      debug();
      printf("%x\n", op);
      dump(emu_cpu.reg_PC-1, 8);
      ERROR("Undefined addressing mode")
      break;
  }
}

void exec(uint8_t op, uint16_t operand)
{
  uint16_t result, tmp_pc;
  uint8_t tmp_m;
  uint8_t tmp_a;
  switch(OPCODE[op]) {
    case OP_ADC:
      if(AM[op] == AM_IMMD)
        tmp_m = operand;
      else
        tmp_m = cpu_bus_read(operand);
        //tmp_m = emu_cpu.memory[operand];
      result = emu_cpu.reg_A + tmp_m + get_status_at(STATUS_C);
      set_reg_P(STATUS_V, check_overflow(emu_cpu.reg_A, tmp_m, OP_ADC));
      set_reg_P(STATUS_C, check_carry(emu_cpu.reg_A, tmp_m, OP_ADC));
      emu_cpu.reg_A = result;
      set_reg_P(STATUS_N, (emu_cpu.reg_A & 0x80) != 0);
      set_reg_P(STATUS_Z, emu_cpu.reg_A == 0);
      break;
    case OP_SBC:
      if(AM[op] == AM_IMMD)
        tmp_m = operand;
      else
        tmp_m = cpu_bus_read(operand);
        //tmp_m = emu_cpu.memory[operand];
      result = emu_cpu.reg_A - tmp_m - (get_status_at(STATUS_C)^1);
      set_reg_P(STATUS_V, check_overflow(emu_cpu.reg_A, tmp_m, OP_SBC));
      set_reg_P(STATUS_C, check_carry(emu_cpu.reg_A, tmp_m, OP_SBC));
      emu_cpu.reg_A = result;
      set_reg_P(STATUS_N, (emu_cpu.reg_A & 0x80) != 0);
      set_reg_P(STATUS_Z, emu_cpu.reg_A == 0);
      break;
    case OP_AND:
      if(AM[op] == AM_IMMD)
        tmp_m = operand;
      else
        tmp_m = cpu_bus_read(operand);
        //tmp_m = emu_cpu.memory[operand];
      emu_cpu.reg_A &= tmp_m; 
      set_reg_P(STATUS_N, (emu_cpu.reg_A & 0x80) != 0);
      set_reg_P(STATUS_Z, emu_cpu.reg_A == 0);
      break;
    case OP_ORA:
      if(AM[op] == AM_IMMD)
        tmp_m = operand;
      else 
        tmp_m = cpu_bus_read(operand);
        //tmp_m = emu_cpu.memory[operand];
      emu_cpu.reg_A |= tmp_m;
      set_reg_P(STATUS_N, (emu_cpu.reg_A & 0x80) != 0);
      set_reg_P(STATUS_Z, emu_cpu.reg_A == 0);
      break;
    case OP_EOR:
      if(AM[op] == AM_IMMD)
        tmp_m = operand;
      else 
        tmp_m = cpu_bus_read(operand);
        //tmp_m = emu_cpu.memory[operand];
      emu_cpu.reg_A ^= tmp_m;
      set_reg_P(STATUS_N, (emu_cpu.reg_A & 0x80) != 0);
      set_reg_P(STATUS_Z, emu_cpu.reg_A == 0);
      break;
    case OP_ASL:
      if(AM[op] == AM_ACCM){
        set_reg_P(STATUS_C, (emu_cpu.reg_A & 0x80) != 0);
        emu_cpu.reg_A <<= 1;
        tmp_m = emu_cpu.reg_A;
      }
      else {
        tmp_m = cpu_bus_read(operand);
        //tmp_m = emu_cpu.memory[operand];
        set_reg_P(STATUS_C, (tmp_m & 0x80) != 0);
        tmp_m <<= 1; 
        cpu_bus_write(operand, tmp_m);
        //emu_cpu.memory[operand] = tmp_m;
      }
      set_reg_P(STATUS_N, (tmp_m & 0x80) != 0);
      set_reg_P(STATUS_Z, tmp_m == 0);
      break;
    case OP_LSR:
      if(AM[op] == AM_ACCM){
        set_reg_P(STATUS_C, emu_cpu.reg_A & 0x1);
        emu_cpu.reg_A >>= 1;
        tmp_m = emu_cpu.reg_A;
      }
      else {
        tmp_m = cpu_bus_read(operand);
        //tmp_m = emu_cpu.memory[operand];
        set_reg_P(STATUS_C, tmp_m & 0x1);
        tmp_m >>= 1;
        cpu_bus_write(operand, tmp_m);
        //emu_cpu.memory[operand] = tmp_m;
      }
      set_reg_P(STATUS_N, (tmp_m & 0x80) != 0);
      set_reg_P(STATUS_Z, tmp_m == 0);
      break;
    case OP_ROL:
      if(AM[op] == AM_ACCM){
        tmp_m = emu_cpu.reg_A;
        tmp_m <<= 1;
        tmp_m |= get_status_at(STATUS_C);
        set_reg_P(STATUS_C, emu_cpu.reg_A & 0x80);
        emu_cpu.reg_A = tmp_m;
      }
      else {
        tmp_m = cpu_bus_read(operand);
        //tmp_m = emu_cpu.memory[operand];
        tmp_m <<= 1;
        tmp_m |= get_status_at(STATUS_C);
        set_reg_P(STATUS_C, cpu_bus_read(operand) & 0x80);
        cpu_bus_write(operand, tmp_m);
        //emu_cpu.memory[operand] = tmp_m;
      }
      set_reg_P(STATUS_N, (tmp_m & 0x80) != 0);
      set_reg_P(STATUS_Z, tmp_m == 0);
      break;
    case OP_ROR:
      if(AM[op] == AM_ACCM){
        tmp_m = emu_cpu.reg_A;
        tmp_m >>= 1;
        tmp_m |= (get_status_at(STATUS_C)<<7);
        set_reg_P(STATUS_C, emu_cpu.reg_A & 0x1);
        emu_cpu.reg_A = tmp_m;
      }
      else {
        tmp_m = cpu_bus_read(operand);
        //tmp_m = emu_cpu.memory[operand];
        tmp_m >>= 1;
        tmp_m |= (get_status_at(STATUS_C)<<7);
        set_reg_P(STATUS_C, cpu_bus_read(operand) & 0x1);
        cpu_bus_write(operand, tmp_m);
        //emu_cpu.memory[operand] = tmp_m;
      }
      set_reg_P(STATUS_N, (tmp_m & 0x80) != 0);
      set_reg_P(STATUS_Z, tmp_m == 0);
      break;
    case OP_BCC:
      if(get_status_at(STATUS_C) == 0)
        emu_cpu.reg_PC = operand;
      break;
    case OP_BCS:
      if(get_status_at(STATUS_C) == 1)
        emu_cpu.reg_PC = operand;
      break;
    case OP_BEQ:
      if(get_status_at(STATUS_Z) == 1)
        emu_cpu.reg_PC = operand;
      break;
    case OP_BNE:
      if(get_status_at(STATUS_Z) == 0)
        emu_cpu.reg_PC = operand;
      break;
    case OP_BVC:
      if(get_status_at(STATUS_V) == 0)
        emu_cpu.reg_PC = operand;
      break;
    case OP_BVS:
      if(get_status_at(STATUS_V) == 1)
        emu_cpu.reg_PC = operand;
      break;
    case OP_BPL:
      if(get_status_at(STATUS_N) == 0)
        emu_cpu.reg_PC = operand;
      break;
    case OP_BMI:
      if(get_status_at(STATUS_N) == 1)
        emu_cpu.reg_PC = operand;
      break;
    case OP_BIT:
      set_reg_P(STATUS_V, cpu_bus_read(operand) & 0x40);
      set_reg_P(STATUS_N, cpu_bus_read(operand) & 0x80);
      set_reg_P(STATUS_Z, (emu_cpu.reg_A & cpu_bus_read(operand))==0);
      break;
    case OP_JMP:
      emu_cpu.reg_PC = operand;
      break;
    case OP_JSR:
      tmp_pc = emu_cpu.reg_PC - 1;
      push_SP((tmp_pc >> 8) & 0xff);
      push_SP(tmp_pc & 0xff);
      emu_cpu.reg_PC = operand;
      break;
    case OP_RTS:
      tmp_pc = pop_SP() | (pop_SP() << 8);
      tmp_pc++;
      emu_cpu.reg_PC = tmp_pc;
      break;
    case OP_BRK:
      set_reg_P(STATUS_B, true);
      break;
    case OP_RTI:
      emu_cpu.reg_P = pop_SP();
      tmp_pc = pop_SP() | (pop_SP() << 8);
      emu_cpu.reg_PC = tmp_pc;
      break;
    case OP_CMP:
      if(AM[op] == AM_IMMD)
        tmp_m = operand;
      else 
        tmp_m = cpu_bus_read(operand);
        //tmp_m = emu_cpu.memory[operand];
      set_reg_P(STATUS_C, check_carry(emu_cpu.reg_A, tmp_m, OP_CMP));
      tmp_m = emu_cpu.reg_A - tmp_m;
      set_reg_P(STATUS_N, tmp_m & 0x80);
      set_reg_P(STATUS_Z, (tmp_m & 0xff)==0);
      break;
    case OP_CPX:
      if(AM[op] == AM_IMMD)
        tmp_m = operand;
      else 
        tmp_m = cpu_bus_read(operand);
        //tmp_m = emu_cpu.memory[operand];
      set_reg_P(STATUS_C, check_carry(emu_cpu.reg_X, tmp_m, OP_CPX));
      tmp_m = emu_cpu.reg_X - tmp_m;
      set_reg_P(STATUS_N, tmp_m & 0x80);
      set_reg_P(STATUS_Z, (tmp_m & 0xff)==0);
      break;
    case OP_CPY:
      if(AM[op] == AM_IMMD)
        tmp_m = operand;
      else 
        tmp_m = cpu_bus_read(operand);
        //tmp_m = emu_cpu.memory[operand];
      set_reg_P(STATUS_C, check_carry(emu_cpu.reg_Y, tmp_m, OP_CPY));
      tmp_m = emu_cpu.reg_Y - tmp_m;
      set_reg_P(STATUS_N, tmp_m & 0x80);
      set_reg_P(STATUS_Z, (tmp_m & 0xff)==0);
      break;
    case OP_INC:
      //tmp_m = emu_cpu.memory[operand] + 1;
      tmp_m = cpu_bus_read(operand) + 1;
      cpu_bus_write(operand, tmp_m);
      //emu_cpu.memory[operand] = tmp_m;
      set_reg_P(STATUS_N, tmp_m & 0x80);
      set_reg_P(STATUS_Z, (tmp_m & 0xff)==0);
      break;
    case OP_DEC:
      //tmp_m = emu_cpu.memory[operand] - 1;
      tmp_m = cpu_bus_read(operand) - 1;
      //emu_cpu.memory[operand] = tmp_m;
      cpu_bus_write(operand, tmp_m);
      set_reg_P(STATUS_N, tmp_m & 0x80);
      set_reg_P(STATUS_Z, (tmp_m & 0xff)==0);
      break;
    case OP_INX:
      emu_cpu.reg_X++;
      set_reg_P(STATUS_N, emu_cpu.reg_X & 0x80);
      set_reg_P(STATUS_Z, (emu_cpu.reg_X & 0xff)==0);
      break; 
    case OP_DEX:
      emu_cpu.reg_X--;
      set_reg_P(STATUS_N, emu_cpu.reg_X & 0x80);
      set_reg_P(STATUS_Z, (emu_cpu.reg_X & 0xff)==0);
      break; 
    case OP_INY:
      emu_cpu.reg_Y++;
      set_reg_P(STATUS_N, emu_cpu.reg_Y & 0x80);
      set_reg_P(STATUS_Z, (emu_cpu.reg_Y & 0xff)==0);
      break; 
    case OP_DEY:
      emu_cpu.reg_Y--;
      set_reg_P(STATUS_N, emu_cpu.reg_Y & 0x80);
      set_reg_P(STATUS_Z, (emu_cpu.reg_Y & 0xff)==0);
      break; 
    case OP_CLC:
      set_reg_P(STATUS_C, false);
      break;
    case OP_SEC:
      set_reg_P(STATUS_C, true);
      break;
    case OP_CLI:
      set_reg_P(STATUS_I, false);
      break;
    case OP_SEI:
      set_reg_P(STATUS_I, true);
      break;
    case OP_CLD:
      set_reg_P(STATUS_D, false);
      break;
    case OP_SED:
      set_reg_P(STATUS_D, true);
      break;
    case OP_CLV:
      set_reg_P(STATUS_V, false);
      break;
    case OP_LDA:
      if(AM[op] == AM_IMMD)
        emu_cpu.reg_A = operand;
      else
        emu_cpu.reg_A = cpu_bus_read(operand);
        //emu_cpu.reg_A = emu_cpu.memory[operand];
      set_reg_P(STATUS_N, emu_cpu.reg_A & 0x80);
      set_reg_P(STATUS_Z, emu_cpu.reg_A == 0);
      break;
    case OP_LDX:
      if(AM[op] == AM_IMMD)
        emu_cpu.reg_X = operand;
      else
        emu_cpu.reg_X = cpu_bus_read(operand);
        //emu_cpu.reg_X = emu_cpu.memory[operand];
      set_reg_P(STATUS_N, emu_cpu.reg_X & 0x80);
      set_reg_P(STATUS_Z, emu_cpu.reg_X == 0);
      break;
    case OP_LDY:
      if(AM[op] == AM_IMMD)
        emu_cpu.reg_Y = operand;
      else
        //emu_cpu.reg_Y = emu_cpu.memory[operand];
        emu_cpu.reg_Y = cpu_bus_read(operand);
      set_reg_P(STATUS_N, emu_cpu.reg_Y & 0x80);
      set_reg_P(STATUS_Z, emu_cpu.reg_Y == 0);
      break;
    case OP_STA:
      //emu_cpu.memory[operand] = emu_cpu.reg_A;
      cpu_bus_write(operand, emu_cpu.reg_A);
      break;
    case OP_STX:
      //emu_cpu.memory[operand] = emu_cpu.reg_X;
      cpu_bus_write(operand, emu_cpu.reg_X);
      break;
    case OP_STY:
      //emu_cpu.memory[operand] = emu_cpu.reg_Y;
      cpu_bus_write(operand, emu_cpu.reg_Y);
      break;
    case OP_TAX:
      emu_cpu.reg_X = emu_cpu.reg_A;
      set_reg_P(STATUS_N, emu_cpu.reg_X & 0x80);
      set_reg_P(STATUS_Z, emu_cpu.reg_X == 0);
      break;
    case OP_TXA:
      emu_cpu.reg_A = emu_cpu.reg_X;
      set_reg_P(STATUS_N, emu_cpu.reg_A & 0x80);
      set_reg_P(STATUS_Z, emu_cpu.reg_A == 0);
      break;
    case OP_TAY:
      emu_cpu.reg_Y = emu_cpu.reg_A;
      set_reg_P(STATUS_N, emu_cpu.reg_Y & 0x80);
      set_reg_P(STATUS_Z, emu_cpu.reg_Y == 0);
      break;
    case OP_TYA:
      emu_cpu.reg_A = emu_cpu.reg_Y;
      set_reg_P(STATUS_N, emu_cpu.reg_A & 0x80);
      set_reg_P(STATUS_Z, emu_cpu.reg_A == 0);
      break;
    case OP_TSX:
      emu_cpu.reg_X = emu_cpu.reg_SP & 0xff;
      set_reg_P(STATUS_N, emu_cpu.reg_X & 0x80);
      set_reg_P(STATUS_Z, emu_cpu.reg_X == 0);
      break;
    case OP_TXS:
      emu_cpu.reg_SP = emu_cpu.reg_X | 0x100;
      break;
    case OP_PHA:
      push_SP(emu_cpu.reg_A);
      break;
    case OP_PLA:
      emu_cpu.reg_A = pop_SP();
      break;
    case OP_PHP:
      push_SP(emu_cpu.reg_P);
      break;
    case OP_PLP:
      emu_cpu.reg_P = pop_SP();
      break;
    case OP_NOP:
    /* unofficial operation */
    case OP_DOP:
    case OP_TOP:
      break;
    case OP_AAC:
      tmp_m = (uint8_t)operand & emu_cpu.reg_A;
      set_reg_P(STATUS_N, tmp_m & 0x80);
      set_reg_P(STATUS_Z, tmp_m == 0);
      set_reg_P(STATUS_C, tmp_m & 0x80);
      break;
    case OP_AAX:
      tmp_m = emu_cpu.reg_X & emu_cpu.reg_A;
      set_reg_P(STATUS_N, tmp_m & 0x80);
      set_reg_P(STATUS_Z, tmp_m == 0);
      cpu_bus_write(operand, tmp_m);
      break;
    case OP_ARR:
      tmp_m = ((uint8_t)operand & emu_cpu.reg_A);
      tmp_m = ((tmp_m & 0x1)<<7) | tmp_m >> 1;
      emu_cpu.reg_A = tmp_m;
      if((tmp_m & 0x60) == 0x60){
        set_reg_P(STATUS_C, true);
        set_reg_P(STATUS_V, false);
      } else if((tmp_m & 0x60) == 0x0){
        set_reg_P(STATUS_C, false);
        set_reg_P(STATUS_V, false);
      } else if((tmp_m & 0x60) == 0x20){
        set_reg_P(STATUS_C, false);
        set_reg_P(STATUS_V, true);
      } else {
        set_reg_P(STATUS_C, true);
        set_reg_P(STATUS_V, true);
      }
      set_reg_P(STATUS_N, tmp_m & 0x80);
      set_reg_P(STATUS_Z, tmp_m == 0);
      break;
    case OP_ASR:
      tmp_m = ((uint8_t)operand & emu_cpu.reg_A); 
      set_reg_P(STATUS_C, tmp_m & 0x1);
      emu_cpu.reg_A >>= 1;
      set_reg_P(STATUS_N, emu_cpu.reg_A & 0x80);   
      set_reg_P(STATUS_Z, emu_cpu.reg_A == 0);   
      break;
    case OP_ATX:
    case OP_AXA:
    case OP_AXS:
    case OP_DCP:
    case OP_ISC:
    case OP_KIL:
    case OP_LAR:
    case OP_LAX:
    case OP_RLA:
    case OP_RRA:
    case OP_SLO:
    case OP_SRE:
    case OP_SXA:
    case OP_SYA:
    case OP_XAA:
    case OP_XAS:
      break;
    case OP_UND:
      ERROR("Undefined Operation");
      break;
  }
}

void run()
{
  clock_t start, end = 0;
  int tmp;
  uint8_t op;
  uint16_t operand;
  
  puts("execution");

  while(1){
    if(flag_nmi){
      nmi();
    }
    op = fetch();
    operand = get_operand(op);
    exec(op, operand);
  }
}

// FUNCTIONS FOR DEBUG 
void dump(uint16_t addr, int32_t size)
{
  int l,i;
  printf("--- memory dump ---\n");
  for(l = 0; l < size; l+=8){
    printf("[0x%04x]: ", addr+l);
    for(i = 0; i+l < size && i < 8; i++)
      printf("%02x ", cpu_bus_read(addr+i+l));
    puts("");
  }
}

void debug()
{
  int i;
  printf("-------------------\n");
  printf("A :   0x%02x\n", emu_cpu.reg_A);
  printf("X :   0x%02x\n", emu_cpu.reg_X);
  printf("Y :   0x%02x\n", emu_cpu.reg_Y);
  printf("P :   0x%02x\n", emu_cpu.reg_P);
  printf("SP: 0x%04x\n", emu_cpu.reg_SP);
  printf("PC: 0x%04x\n", emu_cpu.reg_PC);
  printf("STATUS: | ");
  for(i = 0; i < 8; i++){
    if(emu_cpu.reg_P & (1<<i)){
      printf("%s | ", status_str[i]);
    }
  }
  //dump(emu_cpu.reg_PC, 8);
  puts("");
}

