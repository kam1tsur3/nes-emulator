#include "../common.h"
#include "cassette.h"

void load_rom(struct rom* emu_rom)
{
  int nes;
  int prg_size, chr_size;
  struct header buf_header;

  nes = open(NES_FILE_NAME, O_RDONLY);
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
  
  emu_rom->prg_rom = malloc(prg_size);
  emu_rom->chr_rom = malloc(chr_size);
  emu_rom->size_prg_rom = prg_size;
  emu_rom->size_chr_rom = chr_size;

  read(nes, emu_rom->prg_rom, prg_size);
  read(nes, emu_rom->chr_rom, chr_size);
  return;
}