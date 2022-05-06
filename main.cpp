#include "common.h"
#include <GLUT/glut.h>

// functions 
extern void load_rom(char*);

extern void init_regs();
extern void reset();
extern void debug();
extern void dump(uint16_t, int32_t);
extern void run();

extern void display_all();
extern void interval_display(int);
extern void init_ppu();

extern void push_button(uint8_t, int, int);
extern void unpush_button(uint8_t, int, int);

int main(int argc, char *argv[])
{
	if(argc < 2){
		puts("Usage: ./emulator <nesfile>");
		exit(1);
	}

	load_rom(argv[1]);
	init_ppu();
	init_regs();
	//dump(0x8000, 0x100);
	//reset();

	std::thread cpu_thread(run);

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);

  glutInitWindowSize(256, 240);
  glutCreateWindow("NES Emulator");
  glutDisplayFunc(display_all);
  glutTimerFunc(16,interval_display,0);
	glutKeyboardFunc(push_button);
	glutKeyboardUpFunc(unpush_button);
  glutMainLoop();

	ERROR("Program is ended");
	return 0;
}
