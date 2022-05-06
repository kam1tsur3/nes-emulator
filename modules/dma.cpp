#include "../common.h"

extern uint8_t wram[MEMORY_SIZE];
extern uint8_t sprite_ram[4*64];

void dma(uint8_t val)
{
  if(val > 7 || val < 0){
    ERROR("DMA error");
  }
  memcpy(sprite_ram, &wram[val*0x100], 0x100);
}