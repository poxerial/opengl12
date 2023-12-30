#include <cmath>
#include <cstdio>
#include <glut.h>

const GLfloat factor = 0.1f;
void myDisplay(void) {
  GLfloat x;
  glClear(GL_COLOR_BUFFER_BIT);
  glBegin(GL_LINES);
  glVertex2f(-1.0f, 0.0f);
  glVertex2f(1.0f, 0.0f); // 以上两个点可以画x轴
  glVertex2f(0.0f, -1.0f);
  glVertex2f(0.0f, 1.0f); // 以上两个点可以画y轴
  glEnd();
  glBegin(GL_LINE_STRIP);
  for (x = -1.0f / factor; x < 1.0f / factor; x += 0.01f) {
    glVertex2f(x * factor, sin(x) * factor);
  }
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