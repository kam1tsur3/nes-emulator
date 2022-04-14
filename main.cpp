#include "common.h"

// functions 
extern void load_rom(struct rom*);

extern void init_cpu(struct cpu*);
extern void init_regs(struct cpu*);
extern void reset(struct cpu*);
extern void debug(struct cpu*);
extern void dump(struct cpu*, uint16_t, int32_t);
extern void run(struct cpu*);

void for_debug(struct cpu* emu_cpu)
{
	uint8_t* ptr = &(emu_cpu->memory[0]);
	emu_cpu->reg_X = 0x02;
	emu_cpu->reg_Y = 0x03;
	emu_cpu->reg_A = 0x00;
	*(uint32_t*)ptr = 0x00800201;
	*(uint32_t*)&ptr[0x40] = 0x0fc0ffc0;
	*(uint32_t*)&ptr[0x80] = 0xfbfaf9f8;
	*(uint32_t*)&ptr[0x1080] = 0xfffefdfc;

	*(uint8_t*)&ptr[0x8000] = '\x61';
	*(uint8_t*)&ptr[0x8001] = '\x00';
	*(uint8_t*)&ptr[0x8002] = '\x71';
	*(uint8_t*)&ptr[0x8003] = '\x02';
	
	//*(uint8_t*)&ptr[0x8003] = '\xEA';
}

int main()
{
	struct rom emu_rom;
	struct cpu emu_cpu;

	puts("hello world");

	load_rom(&emu_rom);
	init_cpu(&emu_cpu);	
	reset(&emu_cpu);
	for_debug(&emu_cpu);
	run(&emu_cpu);
	debug(&emu_cpu);
	//dump(&emu_cpu, 0x00c0, 0x8);
	return 0;
}
