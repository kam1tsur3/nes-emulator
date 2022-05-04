#include "../common.h"

#define PTRN_TBL_0_START    0x0000
#define PTRN_TBL_0_END      0x0FFF
#define PTRN_TBL_1_START    0x1000
#define PTRN_TBL_1_END      0x1FFF
#define NAME_TBL_0_START    0x2000
#define NAME_TBL_0_END      0x23BF
#define ATTR_TBL_0_START    0x23C0
#define ATTR_TBL_0_END      0x23FF
#define NAME_TBL_1_START    0x2400
#define NAME_TBL_1_END      0x27BF
#define ATTR_TBL_1_START    0x27C0
#define ATTR_TBL_1_END      0x27FF
#define NAME_TBL_2_START    0x2800
#define NAME_TBL_2_END      0x2BBF
#define ATTR_TBL_2_START    0x2BC0
#define ATTR_TBL_2_END      0x2BFF
#define NAME_TBL_3_START    0x2c00
#define NAME_TBL_3_END      0x2FBF
#define ATTR_TBL_3_START    0x2FC0
#define ATTR_TBL_3_END      0x2FFF

#define MIRROR_2_START      0x3000
#define MIRROR_2_END        0x3EFF

#define BACK_PLT_START      0x3F00
#define BACK_PLT_END        0x3F0F
#define SPRT_PLT_START      0x3F10
#define SPRT_PLT_END        0x3F1F

extern uint8_t vram[MEMORY_SIZE];
extern struct rom emu_rom;
extern struct ppu emu_ppu;

uint8_t ppu_bus_read(uint16_t addr)
{
  if(addr <= PTRN_TBL_1_END) {
    return emu_rom.chr_rom[addr];
  }
  else if(addr <= ATTR_TBL_3_END) {
    return vram[addr - NAME_TBL_0_START];
  }
  else if (addr <= MIRROR_2_END) {
    printf("addr 0x%04x\n",addr);
    ERROR("error in ppu_bus_read()");
  }
  else if (addr <= BACK_PLT_END) {
    return emu_ppu.bg_palette[addr - BACK_PLT_START];
  }
  else if (addr <= SPRT_PLT_END) {
    return emu_ppu.sp_palette[addr - SPRT_PLT_START];
  }
  else {
    printf("addr 0x%04x\n",addr);
    ERROR("error in ppu_bus_read()");
  }
  return 0;
}

void ppu_bus_write(uint16_t addr, uint8_t val)
{
  if(addr <= PTRN_TBL_1_END) {
    printf("val 0x%02x, addr 0x%04x\n", val, addr);
    ERROR("PPUERROR can't write on ROM");
  }
  else if(addr <= ATTR_TBL_3_END) {
    vram[addr - NAME_TBL_0_START] = val; 
  }
  else if (addr <= MIRROR_2_END) {
    ERROR("error in ppu_bus_write()");
  }
  else if (addr <= BACK_PLT_END) {
    emu_ppu.bg_palette[addr - BACK_PLT_START] = val;
  }
  else if (addr <= SPRT_PLT_END) {
    emu_ppu.sp_palette[addr - SPRT_PLT_START] = val;
  }
  else {
    ERROR("error in ppu_bus_write()");
  }
}