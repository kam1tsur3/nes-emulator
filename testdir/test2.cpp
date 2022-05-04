#include <GLUT/glut.h>
#include <unistd.h>
#include <stdio.h>
unsigned char bitmap[256*240*4] = {0};
int count = 0;
// ウィンドウへの描画関数
void display(void)
{
 
 // int i;
 // glClear(GL_COLOR_BUFFER_BIT);
 // glBegin(GL_POINTS);
 // glVertex2i(50,40);
 // glEnd();
 // glFlush();

 // glPointSize(1); 
 // glColor3f((float)0x00/0xff,(float)0x3d/0xff,(float)0xa6/0xff);
 // for(i = 10; i < 50; i++){
 //   glBegin(GL_POINTS);
 //   glVertex2i(10+i,240-10);
 //   glEnd();
 // }
 // glFlush();
  glClearColor(0,0,0,1);
  glClear(GL_COLOR_BUFFER_BIT);
  glDrawPixels(256, 240, GL_RGBA, GL_UNSIGNED_BYTE, bitmap);
  glFlush();
}

void timer(int value){
  int y;
  count++;
  for(y=0;y<240*256;y++)
    {
      bitmap[y*4+0] = (count*10)%256;
      bitmap[y*4+1] = (count*10)%256;
      bitmap[y*4+2] = (count*10)%256;
      bitmap[y*4+3] = 255;
    }
  glutPostRedisplay();
  glutTimerFunc(16,timer,0);
}

void init()
{
  glClearColor(1.0,1.0,1.0,0.0);
  glColor3f(0.0,1.0,0.0);
  glPointSize(1);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0,256,0,240);
}

int main(int argc, char *argv[])
{
  int x, y;
  for(y=0;y<240*256;y++)
  {
    bitmap[y*4+0] = (y+count)%256;
    bitmap[y*4+1] = (y+count)%256;
    bitmap[y*4+2] = (y+count)%256;
    bitmap[y*4+3] = 255;
  }
  // OpenGL/GLUT 環境の初期化
  //glutInit(&argc, argv);
  glutInit(&argc, argv);
  // ウィンドウを開く

  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
  
  glutInitWindowSize(256, 240);
  glutCreateWindow("NES Emulator");
  // ウィンドウへの描画関数の登録
  //init();
  // メインループ開始
  glutDisplayFunc(display);
  glutTimerFunc(1000,timer,0);
  glutMainLoop();
  
  return 0;
}