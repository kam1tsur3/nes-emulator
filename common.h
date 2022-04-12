#include <stdio.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <string.h>
#include <cstdint>

#define MEMORY_SIZE				0x10000

struct rom {
	void* prg_rom;
	void* chr_rom;
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
  uint8_t memory[MEMORY_SIZE];
};

