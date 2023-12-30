#include <cmath>
#include <cstdio>
#include <glut.h>

const int n = 20;
const GLfloat R = 0.5f;
const GLfloat Pi = 3.1415926536f;

void myDisplay(void) {
  int i;
  glClear(GL_COLOR_BUFFER_BIT);
  glBegin(
      GL_POLYGON); // 将GL_POLYGON改为GL_LINE_LOOP、GL_POINTS等其它方式，观察输出的变化情况
  for (i = 0; i < n; ++i)
    glVertex2f(R * cos(2 * Pi / n * i), R * sin(2 * Pi / n * i));
  glEnd();
  glFlush();
}

int main(int argc, char *argv[]) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
  glutInitWindowPosition(100, 100);
  glutInitWindowSize(400, 400);
  glutCreateWindow("第一个OpenGL程序");
  glutDisplayFunc(&myDisplay);
  glutMainLoop();
  return 0;
}