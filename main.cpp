#include "common.h"


// functions 
extern void load_rom(struct rom*);

extern void init_cpu();
extern void init_regs();
extern void reset();
extern void debug();
extern void dump(uint16_t, int32_t);
extern void run();

struct rom emu_rom;
struct cpu emu_cpu;

void for_debug()
{
	uint8_t* ptr = &(emu_cpu.memory[0]);
	emu_cpu.reg_X = 0x1;
	emu_cpu.reg_Y = 0x3;
	emu_cpu.reg_A = 0x1;
	*(uint32_t*)ptr = 0x10808040;
	*(uint32_t*)&ptr[0x40] = 0x0ff0ffc0;
	*(uint32_t*)&ptr[0x80] = 0xfbfa0100;
	*(uint32_t*)&ptr[0x1080] = 0x0ef00ff0;

	*(uint8_t*)&ptr[0x8000] = '\x38';
	*(uint8_t*)&ptr[0x8001] = '\xb0';
	*(uint8_t*)&ptr[0x8002] = '\x3d';
	*(uint8_t*)&ptr[0x8003] = '\x02';
	
	*(uint8_t*)&ptr[0x8040] = '\x90';
	*(uint8_t*)&ptr[0x8041] = '\xc0';
}

int main()
{
	puts("hello world");

	load_rom(&emu_rom);
	init_cpu();	
	reset();
	for_debug();
	run();
	dump(0x1f8, 0x8);
	ERROR("Program is ended");
	return 0;
}
