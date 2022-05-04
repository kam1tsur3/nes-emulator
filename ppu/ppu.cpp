#include "../common.h"
#include <GLUT/glut.h>

#define PPU_CTRL1     0
#define PPU_CTRL2     1
#define PPU_STATUS    2
#define SPRT_ADDR     3
#define SPRT_DATA     4
#define PPU_SCROLL    5
#define VRAM_ADDR     6
#define VRAM_DATA     7

// Mask for ctrl1
#define MASK_NMI            0x80
#define MASK_MASTER_SLAVE   0x40
#define MASK_SPRITE_SIZE    0x20
#define MASK_BG_CHR         0x10
#define MASK_SPRITE_CHR     0x08
#define MASK_INC_VRAM       0x04
#define MASK_SCREEN_SELECT  0x03

#define WIDTH_DISPLAY   256
#define HEIGHT_DISPLAY  240

const uint8_t base_color[64][3] = {
  {0x80, 0x80, 0x80}, {0x00, 0x3D, 0xA6}, {0x00, 0x12, 0xB0}, {0x44, 0x00, 0x96},
  {0xA1, 0x00, 0x5E}, {0xC7, 0x00, 0x28}, {0xBA, 0x06, 0x00}, {0x8C, 0x17, 0x00},
  {0x5C, 0x2F, 0x00}, {0x10, 0x45, 0x00}, {0x05, 0x4A, 0x00}, {0x00, 0x47, 0x2E},
  {0x00, 0x41, 0x66}, {0x00, 0x00, 0x00}, {0x05, 0x05, 0x05}, {0x05, 0x05, 0x05},
  {0xC7, 0xC7, 0xC7}, {0x00, 0x77, 0xFF}, {0x21, 0x55, 0xFF}, {0x82, 0x37, 0xFA},
  {0xEB, 0x2F, 0xB5}, {0xFF, 0x29, 0x50}, {0xFF, 0x22, 0x00}, {0xD6, 0x32, 0x00},
  {0xC4, 0x62, 0x00}, {0x35, 0x80, 0x00}, {0x05, 0x8F, 0x00}, {0x00, 0x8A, 0x55},
  {0x00, 0x99, 0xCC}, {0x21, 0x21, 0x21}, {0x09, 0x09, 0x09}, {0x09, 0x09, 0x09},
  {0xFF, 0xFF, 0xFF}, {0x0F, 0xD7, 0xFF}, {0x69, 0xA2, 0xFF}, {0xD4, 0x80, 0xFF},
  {0xFF, 0x45, 0xF3}, {0xFF, 0x61, 0x8B}, {0xFF, 0x88, 0x33}, {0xFF, 0x9C, 0x12},
  {0xFA, 0xBC, 0x20}, {0x9F, 0xE3, 0x0E}, {0x2B, 0xF0, 0x35}, {0x0C, 0xF0, 0xA4},
  {0x05, 0xFB, 0xFF}, {0x5E, 0x5E, 0x5E}, {0x0D, 0x0D, 0x0D}, {0x0D, 0x0D, 0x0D},
  {0xFF, 0xFF, 0xFF}, {0xA6, 0xFC, 0xFF}, {0xB3, 0xEC, 0xFF}, {0xDA, 0xAB, 0xEB},
  {0xFF, 0xA8, 0xF9}, {0xFF, 0xAB, 0xB3}, {0xFF, 0xD2, 0xB0}, {0xFF, 0xEF, 0xA6},
  {0xFF, 0xF7, 0x9C}, {0xD7, 0xE8, 0x95}, {0xA6, 0xED, 0xAF}, {0xA2, 0xF2, 0xDA},
  {0x99, 0xFF, 0xFC}, {0xDD, 0xDD, 0xDD}, {0x11, 0x11, 0x11}, {0x11, 0x11, 0x11}
};

extern uint8_t ppu_bus_read(uint16_t);
extern void ppu_bus_write(uint16_t, uint8_t);

struct ppu emu_ppu;

uint8_t bitmap[256*240*4] = {256};

extern bool terminate_flag;

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
      if(val == 0x1e) {
      //  int tmp;
      //  for(tmp=0; tmp<16; tmp++){
      //    printf("0x%02x ", emu_ppu.bg_palette[tmp]);
      //  }
      //  ERROR("access ctrl2");
        terminate_flag = true;
      }
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

