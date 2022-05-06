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

extern uint8_t wram[MEMORY_SIZE];
extern struct rom emu_rom;
extern uint8_t buffer[8];
uint8_t ctrl_pointer = 0;

extern uint8_t ppu_reg_read(uint16_t);
extern void ppu_reg_write(uint16_t, uint8_t);
extern void debug();

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
    if(addr == 0x4016){
      if(ctrl_pointer > 7)
        ERROR("Invalid ctrl_pointer");
      return buffer[ctrl_pointer++];
    }
    ERROR("Not mapped APU_IO_PAD");
  }
  else if (addr <= EXT_ROM_END){
    ERROR("Not mapped EXT_ROM");
  }
  else if (addr <= EXT_RAM_END){
    ERROR("Not mapped EXT_RAM");
  }
  else if (addr <= PRG_ROM2_END){
    if(addr - PRG_ROM1_START > emu_rom.size_prg_rom){
      debug();
      ERROR("Invalid Access To PRG_ROM");
    }
    return ((uint8_t*)emu_rom.prg_rom)[addr - PRG_ROM1_START];
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
    if(addr == 0x4016){
      if(val&1)
        ctrl_pointer = 0xff;
      else {
        if(ctrl_pointer != 0xff)
          ERROR("Invalid controller usage");
        ctrl_pointer = 0;
      }
    } 
    else
      ERROR("Not mapped APU_IO_PAD");
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

