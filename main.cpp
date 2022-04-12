#include "common.h"

// functions 
extern void load_rom(struct rom*);

extern void init_cpu(struct cpu*);
extern void init_regs(struct cpu*);
extern void reset(struct cpu*);
extern void debug(struct cpu*);
extern void run(struct cpu*);

int main()
{
	struct rom emu_rom;
	struct cpu emu_cpu;

	puts("hello world");

	load_rom(&emu_rom);
	init_cpu(&emu_cpu);	
	debug(&emu_cpu);
	reset(&emu_cpu);
	debug(&emu_cpu);
	//run(&emu_cpu);
	return 0;
}
