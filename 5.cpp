#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <glut.h>

static int day = 200; // day的变化：从0到359
void myDisplay(void) {
  glDepthFunc(GL_LEQUAL); // 设置深度<=通过,与物体之间的前后有关
  // 原《OpenGL入门教程》遗落了glDepthFunc(GL_LEQUAL)这个函数出现了问题，需要加上
  glEnable(GL_DEPTH_TEST);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(75, 1, 1, 400000000);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(0, -200000000, 200000000, 0, 0, 0, 0, 0, 1);

  // 绘制红色的“太阳”
  glColor3f(1.0f, 0.0f, 0.0f);
  glutSolidSphere(69600000, 20, 20);
  // 绘制蓝色的“地球”
  glColor3f(0.0f, 0.0f, 1.0f);
  glRotatef(day / 360.0 * 360.0, 0.0f, 0.0f, -1.0f);
  glTranslatef(150000000, 0.0f, 0.0f);
  glutSolidSphere(15945000, 20, 20);
  // 绘制黄色的“月亮”
  glColor3f(1.0f, 1.0f, 0.0f);
  glRotatef(day / 30.0 * 360.0 - day / 360.0 * 360.0, 0.0f, 0.0f, -1.0f);
  glTranslatef(38000000, 0.0f, 0.0f);
  glutSolidSphere(4345000, 20, 20);

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