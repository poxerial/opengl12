#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <glut.h>

#define WIDTH 400
#define HEIGHT 400

// 以下是抓取保存为BMP文件的开始代码
#define WindowWidth 400
#define WindowHeight 400

/* 函数grab
 * 抓取窗口中的像素
 * 假设窗口宽度为WindowWidth，高度为WindowHeight
 */
#define BMP_Header_Length 54
void grab(void) {
  glReadBuffer(GL_FRONT);
  FILE *pDummyFile;
  FILE *pWritingFile;
  GLubyte *pPixelData;
  GLubyte BMP_Header[BMP_Header_Length];
  GLint i, j;
  GLint PixelDataLength;

  // 计算像素数据的实际长度
  i = WindowWidth * 3; // 得到每一行的像素数据长度
  while (i % 4 != 0)   // 补充数据，直到i是的倍数
    ++i;               // 本来还有更快的算法，
                       // 但这里仅追求直观，对速度没有太高要求
  PixelDataLength = i * WindowHeight;

  // 分配内存和打开文件
  pPixelData = (GLubyte *)malloc(PixelDataLength);
  if (pPixelData == 0)
    exit(-1);
  pDummyFile = fopen("../du.bmp", "rb");
  if (pDummyFile == 0)
    exit(-1);
  pWritingFile = fopen("../grab.bmp", "wb");
  if (pWritingFile == 0)
    exit(-1);

  // 读取像素
  glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
  glReadPixels(0, 0, WindowWidth, WindowHeight, GL_BGR_EXT, GL_UNSIGNED_BYTE,
               pPixelData);

  // 把dummy.bmp的文件头复制为新文件的文件头
  fread(BMP_Header, sizeof(BMP_Header), 1, pDummyFile);
  fwrite(BMP_Header, sizeof(BMP_Header), 1, pWritingFile);
  fseek(pWritingFile, 0x0012, SEEK_SET);
  i = WindowWidth;
  j = WindowHeight;
  fwrite(&i, sizeof(i), 1, pWritingFile);
  fwrite(&j, sizeof(j), 1, pWritingFile);

  // 写入像素数据
  fseek(pWritingFile, 0, SEEK_END);
  fwrite(pPixelData, PixelDataLength, 1, pWritingFile);

  // 释放内存和关闭文件
  fclose(pDummyFile);
  fclose(pWritingFile);
  free(pPixelData);
}
// 到此结束

void setLight(void) {
  static const GLfloat light_position[] = {1.0f, 1.0f, -1.0f, 1.0f};
  static const GLfloat light_ambient[] = {0.2f, 0.2f, 0.2f, 1.0f};
  static const GLfloat light_diffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
  static const GLfloat light_specular[] = {1.0f, 1.0f, 1.0f, 1.0f};

  glLightfv(GL_LIGHT0, GL_POSITION, light_position);
  glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHTING);
  glEnable(GL_DEPTH_TEST);
}

void setMatirial(const GLfloat mat_diffuse[4], GLfloat mat_shininess) {
  static const GLfloat mat_specular[] = {0.0f, 0.0f, 0.0f, 1.0f};
  static const GLfloat mat_emission[] = {0.0f, 0.0f, 0.0f, 1.0f};

  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_diffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
  glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);
  glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);
}

void myDisplay(void) {
  // 定义一些材质颜色
  const static GLfloat red_color[] = {1.0f, 0.0f, 0.0f, 1.0f};
  const static GLfloat green_color[] = {0.0f, 1.0f, 0.0f, 0.3333f};
  const static GLfloat blue_color[] = {0.0f, 0.0f, 1.0f, 0.5f};

  // 清除屏幕
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // 启动混合并设置混合因子
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // 设置光源
  setLight();

  // 以(0, 0, 0.5)为中心，绘制一个半径为.3的不透明红色球体（离观察者最远）
  setMatirial(red_color, 30.0);
  glPushMatrix();
  glTranslatef(0.0f, 0.0f, 0.5f);
  glutSolidSphere(0.3, 30, 30);
  glPopMatrix();

  // 下面将绘制半透明物体了，因此将深度缓冲设置为只读
  glDepthMask(GL_FALSE);

  // 以(0.2, 0, -0.5)为中心，绘制一个半径为.2的半透明蓝色球体（离观察者最近）
  setMatirial(blue_color, 30.0);
  glPushMatrix();
  glTranslatef(0.2f, 0.0f, -0.5f);
  glutSolidSphere(
      0.2, 30,
      30); // 第一个参数表示球体的半径，后两个参数代表了“面”的数目，简单点说就是球体的精确程度，数值越大越精确，当然代价就是速度越缓慢
  glPopMatrix();

  // 以(0.1, 0, 0)为中心，绘制一个半径为.15的半透明绿色球体（在前两个球体之间）
  setMatirial(green_color, 30.0);
  glPushMatrix();
  glTranslatef(0.1, 0, 0);
  glutSolidSphere(0.15, 30, 30);
  glPopMatrix();

  // 完成半透明物体的绘制，将深度缓冲区恢复为可读可写的形式
  glDepthMask(GL_TRUE);

  glutSwapBuffers();
  grab();
}

int main(int argc, char *argv[]) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
  glutInitWindowPosition(200, 200);
  glutInitWindowSize(WIDTH, HEIGHT);
  glutCreateWindow("OpenGL 窗口");
  glutDisplayFunc(&myDisplay);
  glutMainLoop();
  return 0;
}