#include "../common.h"

#define WRAM_START                 0x0000
#define WRAM_END                   0x07FF
#define WRAM_MIRROR_START          0x0800
#define WRAM_MIRROR_END            0x1FFF
#define PPU_REGS_START             0x2000
#define PPU_REGS_END               0x2007
#define PPU_REGS_MIRROR_START      0x2008
#define PPU_REGS_MIRROR_END        0x3FFF
#define APU_IO_PAD_START           0x4000
#define APU_IO_PAD_END             0x401F
#define EXT_ROM_START              0x4020
#define EXT_ROM_END                0x5FFF
#define EXT_RAM_START              0x6000
#define EXT_RAM_END                0x7FFF
#define PRG_ROM1_START             0x8000
#define PRG_ROM1_END               0xBFFF
#define PRG_ROM2_START             0xC000
#define PRG_ROM2_END               0xFFFF

#define IO_SQ1_CTRL1      0x4000
#define IO_SQ1_CTRL2      0x4001
#define IO_SQ1_FREQ1      0x4002
#define IO_SQ1_FREQ2      0x4003
#define IO_SQ2_CTRL1      0x4004
#define IO_SQ2_CTRL2      0x4005
#define IO_SQ2_FREQ1      0x4006
#define IO_SQ2_FREQ2      0x4007
#define IO_TRI_CTRL       0x4008
#define IO_TRI_FREQ1      0x400A
#define IO_TRI_FREQ2      0x400B
#define IO_NOISE_CTRL     0x400C
#define IO_NOISE_FREQ1    0x400E
#define IO_NOISE_FREQ2    0x400F
#define IO_DMC_CTRL1      0x4010
#define IO_DMC_CTRL2      0x4011
#define IO_DMC_ADDR       0x4012
#define IO_DMC_DATA_LEN   0x4013
#define IO_DMA            0x4014
#define IO_AUDIO_ENABLE   0x4015
#define IO_1P_PAD         0x4016
#define IO_AUDIO_FRAME    0x4017
//#define IO_2P_PAD         0x4017

extern uint8_t wram[MEMORY_SIZE];
extern struct rom emu_rom;
extern struct apu emu_apu;
extern uint8_t buffer[8];
uint8_t ctrl_pointer = 0;

extern uint8_t ppu_reg_read(uint16_t);
extern void ppu_reg_write(uint16_t, uint8_t);
extern void debug();

extern void dma(uint8_t);

extern void audio_switch(uint8_t);
extern void audio_set_ctrl1(uint8_t, uint8_t);
extern void audio_set_ctrl2(uint8_t, uint8_t);
extern void audio_set_freq1(uint8_t, uint8_t);
extern void audio_set_freq2(uint8_t, uint8_t);
extern void audio_set_mode(uint8_t);
extern void audio_set_triangle(uint8_t);

uint8_t cpu_bus_read(uint16_t addr)
{
  if(addr <= WRAM_END){
    return wram[addr];
  } 
  else if(addr <= WRAM_MIRROR_END){
    printf("addr: %p\n", addr);
    ERROR("CPU-BUS: Error Not mapped WRAM_MIRROR(R)");
  }
  else if (addr <= PPU_REGS_MIRROR_END){
    // PPU register
    return ppu_reg_read((addr - PPU_REGS_START)%8);
  }
  else if (addr <= APU_IO_PAD_END){
    switch(addr){
      case IO_AUDIO_ENABLE:
        return emu_apu.reg_sound;
      case IO_1P_PAD:
        if(ctrl_pointer > 7)
          ERROR("Invalid ctrl_pointer");
        return buffer[ctrl_pointer++];
        break;
      default:
        ERROR("Not mapped APU_IO_PAD");
    }
  }
  else if (addr <= EXT_ROM_END){
    ERROR("Not mapped EXT_ROM");
  }
  else if (addr <= EXT_RAM_END){
    ERROR("Not mapped EXT_RAM");
  }
  else if (addr <= PRG_ROM2_END){
    return ((uint8_t*)emu_rom.prg_rom)[addr%0x4000];
  }
  return 0;
}

void cpu_bus_write(uint16_t addr, uint8_t val)
{
  if(addr <= WRAM_END){
    wram[addr] = val;
  } 
  else if(addr <= WRAM_MIRROR_END){
    ERROR("CPU-BUS: Error Not mapped WRAM_MIRROR(W)");
  }
  else if (addr <= PPU_REGS_MIRROR_END){
    // PPU register
    ppu_reg_write((addr - PPU_REGS_START)%8, val);
  }
  else if (addr <= APU_IO_PAD_END){
    switch(addr){
      case IO_SQ1_CTRL1:   
        audio_set_ctrl1(0, val);      
        break;
      case IO_SQ1_CTRL2:   
        audio_set_ctrl2(0, val);      
        break;
      case IO_SQ1_FREQ1:   
        audio_set_freq1(0, val);      
        break;
      case IO_SQ1_FREQ2:   
        audio_set_freq2(0, val);      
        break;
      case IO_SQ2_CTRL1:   
        audio_set_ctrl1(1, val);      
        break;
      case IO_SQ2_CTRL2:   
        audio_set_ctrl2(1, val);      
        break;
      case IO_SQ2_FREQ1:   
        audio_set_freq1(1, val);      
        break;
      case IO_SQ2_FREQ2:   
        audio_set_freq2(1, val);      
        break;
      case IO_TRI_CTRL:
        audio_set_triangle(val);    
      case IO_TRI_FREQ1:   
        audio_set_freq1(2, val);      
        break;
      case IO_TRI_FREQ2:   
        audio_set_freq2(2, val);      
        break;
      case IO_NOISE_CTRL:  
      case IO_NOISE_FREQ1: 
      case IO_NOISE_FREQ2: 
      case IO_DMC_CTRL1:   
      case IO_DMC_CTRL2:   
      case IO_DMC_ADDR:    
      case IO_DMC_DATA_LEN:
        // TODO
        break;
      case IO_DMA:
        // dma
        dma(val);
        break;
      case IO_AUDIO_ENABLE:
        audio_switch(val);
        emu_apu.reg_sound = val;
        break;
      case IO_1P_PAD:
        if(val&1)
          ctrl_pointer = 0xff;
        else {
          if(ctrl_pointer != 0xff)
            ERROR("Invalid controller usage");
          ctrl_pointer = 0;
        }
        break;
      case IO_AUDIO_FRAME:
        audio_set_mode(val);
        break;
      default:
        ERROR("Not mapped APU_IO_PAD");
    }
  }
  else if (addr <= EXT_ROM_END){
    ERROR("Not mapped EXT_ROM");
  }
  else if (addr <= EXT_RAM_END){
    ERROR("Not mapped EXT_RAM");
  }
  else if (addr <= PRG_ROM2_END){
    ERROR("Can't Write on PRG_ROM");
  }
  return;
}

uint16_t cpu_bus_read_word(uint16_t addr)
{
  return cpu_bus_read(addr) | ((uint16_t)cpu_bus_read(addr+1) << 8);
}

