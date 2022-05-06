#include "../common.h"
#include "ppu.h"
#include <GLUT/glut.h>


extern uint8_t ppu_bus_read(uint16_t);
extern void ppu_bus_write(uint16_t, uint8_t);

struct ppu emu_ppu;
uint8_t bitmap[WIDTH_DISPLAY*HEIGHT_DISPLAY*4] = {0};
uint8_t sprite_ram[4*SPRITE_MAX] = {0x20};

void init_ppu()
{
  int i;
  emu_ppu.vram_switch = 1;
  for(i = 0; i < WIDTH_DISPLAY*HEIGHT_DISPLAY; i+=4){
    bitmap[i+3] = 255;
  }
  for(i = 0; i < SPRITE_MAX; i+=4){
    sprite_ram[i+2] = 0x20;
  }
}

void inc_vram_addr()
{
  if(emu_ppu.reg_ctrl1 & MASK_INC_VRAM)
    emu_ppu.vram_addr += 0x20;
  else
    emu_ppu.vram_addr += 0x01;
}

uint8_t get_reg_bit(uint8_t val, uint8_t mask)
{
  if(val & mask)
    return 1;
  else 
    return 0;
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
      emu_ppu.sprite_addr = val;
      break;
    case SPRT_DATA:
      sprite_ram[emu_ppu.sprite_addr] = val;
      emu_ppu.sprite_addr += 1;
      break;
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
       // TODO
       return 0x80;
    case SPRT_DATA:
       // TODO
       return 0;
    case VRAM_DATA:
      tmp = ppu_bus_read(emu_ppu.vram_addr);
      inc_vram_addr();
      return tmp;
    default:
      ERROR("can't read PPU register");
  }
}

void set_sprite(uint8_t* ptr, uint16_t addr)
{
  int i, x, y;
  uint8_t tmp;
  for(i=0; i<2; i++){
    for(y=0; y<8; y++){
      tmp = ppu_bus_read(addr+i*8+y);
      for(x=0; x<8; x++){
        if(tmp&1)
          *(ptr + y*8 + (7-x)) |= (1<<i);
        tmp >>= 1;
      }
    }
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

void map_point(uint8_t color_idx, int x, int y)
{
  bitmap[((HEIGHT_DISPLAY-y-1)*WIDTH_DISPLAY+x)*4+0] = base_color[color_idx][0];
  bitmap[((HEIGHT_DISPLAY-y-1)*WIDTH_DISPLAY+x)*4+1] = base_color[color_idx][1];
  bitmap[((HEIGHT_DISPLAY-y-1)*WIDTH_DISPLAY+x)*4+2] = base_color[color_idx][2];
}

//void display_line(int y)
//{
//  int x;
//  int p_idx;
//  uint8_t p_num;
//  for(x=0; x<WIDTH_DISPLAY; x++){
//    p_num = sprite_to_bit(ppu_bus_read(0x2000+xy_to_tile(x,y))*0x10, x%8, y%8);
//    p_idx = xy_to_palette(x,y);
//    map_point(emu_ppu.bg_palette[p_idx*4+p_num], x, y);
//  }
//}

void display_all()
{
  glClearColor(0,0,0,1);
  glClear(GL_COLOR_BUFFER_BIT);
  glDrawPixels(256, 240, GL_RGBA, GL_UNSIGNED_BYTE, bitmap);
  glFlush();
}

void map_background()
{
  int i, idx_spr, idx_pal, x, y, tx, ty;
  uint8_t tmp_spr[8*8] = {0};
  for(y=0; y<HEIGHT_DISPLAY; y+=8){
    for(x=0; x<WIDTH_DISPLAY; x+=8){
      memset(tmp_spr, 0, 8*8);
      idx_spr = ppu_bus_read(0x2000+0x400*(emu_ppu.reg_ctrl1&0x3)+xy_to_tile(x,y));
      set_sprite(tmp_spr, get_reg_bit(emu_ppu.reg_ctrl1, MASK_BG_CHR)*0x1000+idx_spr*0x10);
      idx_pal = xy_to_palette(x,y);
      for(ty=y; ty<(y+8); ty++){
        for(tx=x; tx<(x+8); tx++){
          //map_point(emu_ppu.bg_palette[idx_pal*4+tmp_spr[(tx-x)+(ty-y)*8]], tx, ty);
          map_point(ppu_bus_read(0x3F00+idx_pal*4+tmp_spr[(tx-x)+(ty-y)*8]), tx, ty);
        }
      }
    }
  }
}

void map_sprite()
{
  int i, idx_spr, idx_pal, x, y, tx, ty;
  uint8_t tmp_spr[8*8] = {0};

  for(i=0; i<SPRITE_MAX; i+=4){
    if(sprite_ram[i+2] & 0x20){
      continue;
    }
    memset(tmp_spr, 0, 8*8);
    y = sprite_ram[i];
    x = sprite_ram[i+3];
    idx_spr = sprite_ram[i+1];
    idx_pal = sprite_ram[i+2]&0x3;
    set_sprite(tmp_spr, get_reg_bit(emu_ppu.reg_ctrl1, MASK_SPRITE_CHR)*0x1000+idx_spr*0x10);
    
    for(ty=y; ty < y+8; ty++){
      for(tx=x; tx < x+8; tx++){
        //map_point(emu_ppu.sp_palette[idx_pal*4+tmp_spr[(tx-x)+(ty-y)*8]], tx, ty);
        map_point(ppu_bus_read(0x3F10+idx_pal*4+tmp_spr[(tx-x)+(ty-y)*8]), tx, ty);
      }
    }
  } 
}

void interval_display(int val)
{
  // bg
  map_background(); 
  // sprite
  map_sprite(); 
  
  glutPostRedisplay();
  glutTimerFunc(16,interval_display,0);
}

