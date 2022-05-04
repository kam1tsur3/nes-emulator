#include "../common.h"
#include "ppu.h"
#include <GLUT/glut.h>


extern uint8_t ppu_bus_read(uint16_t);
extern void ppu_bus_write(uint16_t, uint8_t);

struct ppu emu_ppu;
uint8_t bitmap[256*240*4] = {256};

void init_ppu()
{
  emu_ppu.vram_switch = 1;
}

void inc_vram_addr()
{
  if(emu_ppu.reg_ctrl1 & MASK_INC_VRAM)
    emu_ppu.vram_addr += 0x20;
  else
    emu_ppu.vram_addr += 0x01;
}

void ppu_reg_write(uint16_t addr, uint8_t val)
{
  switch(addr) {
    case PPU_CTRL1:
      emu_ppu.reg_ctrl1 = val; 
      break;
    case PPU_CTRL2:
      emu_ppu.reg_ctrl2 = val; 
      break;
    case SPRT_ADDR:
    case SPRT_DATA:
    case PPU_SCROLL:
      // not implemented
      break;
    case VRAM_ADDR:
      emu_ppu.vram_addr = (emu_ppu.vram_addr & 0xff<<(8*(emu_ppu.vram_switch^1))) | (val << (8*emu_ppu.vram_switch));
      emu_ppu.vram_switch ^= 1;
      break;
    case VRAM_DATA:
      ppu_bus_write(emu_ppu.vram_addr, val);
      inc_vram_addr();
      break;
    default:
      printf("val: 0x%02x, addr: 0x%04x\n", val, addr);
      ERROR("can't write PPU register");
  }
}

uint8_t ppu_reg_read(uint16_t addr)
{
  uint8_t tmp;
  switch(addr) {
    case PPU_STATUS:
    case SPRT_DATA:
    case VRAM_DATA:
      tmp = ppu_bus_read(emu_ppu.vram_addr);
      inc_vram_addr();
      return tmp;
    default:
      ERROR("can't read PPU register");
  }
}

void sprite_to_tile(uint8_t* addr)
{
  int i, j;
  uint8_t p;
  for(i=0;i<8;i++){
    for(j=0;j<8;j++){
      p = 0;
      if(addr[i] & (1<<(7-j)))
        p |= 1;
      if(addr[i+8] & (1<<(7-j)))
        p |= 2;
      printf("%d",p);
    }
    printf("\n");
  }
}

uint8_t sprite_to_bit(int idx, int x, int y)
{
  uint8_t p = 0;
  uint8_t p1 = ppu_bus_read(idx+y);
  uint8_t p2 = ppu_bus_read(idx+8+y);
  if(p1 & (1<<(7-x)))
    p |= 1;
  if(p2 & (1<<(7-x)))
    p |= 2; 
  return p;
}

uint32_t xy_to_tile(int x, int y)
{
  return (x/8)+(y/8)*(WIDTH_DISPLAY/8);
}

uint8_t xy_to_palette(int x, int y)
{
  uint8_t tb, idx;
  tb = ppu_bus_read(0x23c0+x/32+(y/32)*16);
  idx = ((x/16) & 1) + 2*((y/16) & 1);
  return (tb >> (2*idx)) & 0x3;
}

void display_point(uint8_t color_idx, int x, int y)
{
  bitmap[((HEIGHT_DISPLAY-y-1)*WIDTH_DISPLAY+x)*4+0] = base_color[color_idx][0];
  bitmap[((HEIGHT_DISPLAY-y-1)*WIDTH_DISPLAY+x)*4+1] = base_color[color_idx][1];
  bitmap[((HEIGHT_DISPLAY-y-1)*WIDTH_DISPLAY+x)*4+2] = base_color[color_idx][2];
}


void display_line(int y)
{
  int x;
  int p_idx;
  uint8_t p_num;
  float *f_ptr;
  for(x=0; x<WIDTH_DISPLAY; x++){
    p_num = sprite_to_bit(ppu_bus_read(0x2000+xy_to_tile(x,y))*0x10, x%8, y%8);
    p_idx = xy_to_palette(x,y);
    display_point(emu_ppu.bg_palette[p_idx*4+p_num], x, y);
  }
}

void display_all()
{
  glClearColor(0,0,0,1);
  glClear(GL_COLOR_BUFFER_BIT);
  glDrawPixels(256, 240, GL_RGBA, GL_UNSIGNED_BYTE, bitmap);
  glFlush();
}

void interval_display(int val)
{
  int y;
  for(y=0; y<HEIGHT_DISPLAY; y++){
    display_line(y);
  }
  glutPostRedisplay();
  glutTimerFunc(16,interval_display,0);
}

