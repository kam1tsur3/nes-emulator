#include "../common.h"
#include "cassette.h"

struct rom emu_rom;

void load_rom(char* file)
{
  int nes;
  int prg_size, chr_size;
  struct header buf_header;

  nes = open(file, O_RDONLY);
  if(nes < 0){
    puts("[E] can't open nes file");
    exit(1);
  }
  read(nes, &buf_header, HEADER_SIZE);

  //header check
  if(memcmp(&(buf_header.MAGIC), HEADER_MAGIC, 4) != 0){
    puts("[E] MAGIC is invalid");
    exit(1);
  }

  // load ROM
  prg_size = buf_header.size_prg_rom * PRG_PAGE_SIZE;
  chr_size = buf_header.size_chr_rom * CHR_PAGE_SIZE;
  
  emu_rom.prg_rom = (uint8_t*)malloc(prg_size);
  emu_rom.chr_rom = (uint8_t*)malloc(chr_size);
  emu_rom.size_prg_rom = prg_size;
  emu_rom.size_chr_rom = chr_size;

  read(nes, emu_rom.prg_rom, prg_size);
  read(nes, emu_rom.chr_rom, chr_size);

  return;
}
