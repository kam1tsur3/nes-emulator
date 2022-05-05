#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <string.h>
#include <cstdint>
#include <pthread.h>
#include <thread>

#define MEMORY_SIZE				0x10000

#define ERROR(msg) \
while(1){ \
	printf("[E] " msg "\n"); \
	exit(1); \
}

struct rom {
	uint8_t* prg_rom;
	uint8_t* chr_rom;
	int size_prg_rom;
	int size_chr_rom;
};

struct cpu {
	uint8_t reg_A;
	uint8_t reg_X;
	uint8_t reg_Y;
	uint16_t reg_SP;
	uint8_t reg_P;
	uint16_t reg_PC;
};

struct ppu {
  uint8_t reg_ctrl1;
  uint8_t reg_ctrl2;
  uint8_t sprite_addr;
  uint16_t vram_addr;
  uint8_t vram_switch;
  uint8_t bg_palette[0x10];
  uint8_t sp_palette[0x10];
};
