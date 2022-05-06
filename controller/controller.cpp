#include "../common.h"

uint8_t buffer[8] = {0};

void push_button(uint8_t key, int x, int y)
{
	switch(key){
		case 'j':
			// A
      buffer[0] = 1;
			break;
		case 'k':
			// B
      buffer[1] = 1;
			break;
		case 't':
			// select
      buffer[2] = 1;
			break;
		case 'y':
			// start
      buffer[3] = 1;
			break;
		case 'w':
			// up
      buffer[4] = 1;
			break;
		case 's':
			// down
      buffer[5] = 1;
			break;
		case 'a':
			// left
      buffer[6] = 1;
			break;
		case 'd':
			// right
      buffer[7] = 1;
			break;
		default:
			break;
	}
}

void unpush_button(uint8_t key, int x, int y)
{
  switch(key){
		case 'j':
			// A
      buffer[0] = 0;
			break;
		case 'k':
			// B
      buffer[1] = 0;
			break;
		case 't':
			// select
      buffer[2] = 0;
			break;
		case 'y':
			// start
      buffer[3] = 0;
			break;
		case 'w':
			// up
      buffer[4] = 0;
			break;
		case 's':
			// down
      buffer[5] = 0;
			break;
		case 'a':
			// left
      buffer[6] = 0;
			break;
		case 'd':
			// right
      buffer[7] = 0;
			break;
		default:
			break;
	}
}

