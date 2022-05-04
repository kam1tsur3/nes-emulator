#include <GL/glew.h> // include GLEW and new version of GL on Windows
#include <GLFW/glfw3.h> // GLFW helper library for window management
#include <iostream> //for cout
#include <cstdlib>

int main (int argc, char** argv) 
{
  // start GL context and O/S window using the GLFW helper library
  if (!glfwInit ()) 
    {
      std::cerr<<"ERROR: could not start GLFW3"<<std::endl;
      return 1;
    } 
	
  atexit(glfwTerminate);

  //Setting window properties
  glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint (GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  
  //Actually create the window
  GLFWwindow* window = glfwCreateWindow (640, 480, "OpenGL Initialization Example", NULL, NULL);
  //GLFWwindow* window = glfwCreateWindow (256, 240, "NES Emulator", NULL, NULL);
  if (!window) 
    {
      std::cerr<<"ERROR: could not open window with GLFW3"<<std::endl;
      //glfwTerminate();
      return 1;
    }
  glfwMakeContextCurrent (window);
                                  
  // start GLEW extension handler
  glewExperimental = GL_TRUE;
  glewInit ();

  // get version info
  const GLubyte* renderer = glGetString (GL_RENDERER); // get renderer string
  const GLubyte* version = glGetString (GL_VERSION); // version as a string
  std::cout<<"Renderer: "<<renderer<<std::endl;
  std::cout<<"OpenGL version supported "<<version<<std::endl;

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.0f, 640, 0.0f, 480, -1.0f, 1.0f);
  //glOrtho(0.0f, 256, 0.0f, 240, -1.0f, 1.0f);

  while(glfwWindowShouldClose(window) == GL_FALSE)
  {
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

  glColor3f(0.0,1.0,0.0); 
  glPointSize(10);
  glBegin(GL_POINTS);
  glVertex2i(0,0);
  glEnd();
  glFlush();

  //glMatrixMode(GL_MODELVIEW);
  //glEnableClientState(GL_VERTEX_ARRAY);
  //glDrawArrays(GL_POINTS, 0, 1);
  //glDisableClientState(GL_VERTEX_ARRAY);

	glfwSwapBuffers(window);
	glfwWaitEvents();
  }
  // close GL context and any other GLFW resources
  //glfwTerminate();
  return 0;
}
