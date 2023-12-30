#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <glut.h>

#define WindowTitle "OpenGL纹理测试"

static GLint ImageWidth = 404;
static GLint ImageHeight = 453;

#define WindowWidth 400
#define WindowHeight 400
#define BMP_Header_Length 54

/* 函数power_of_two
 * 检查一个整数是否为2的整数次方，如果是，返回1，否则返回0
 * 实际上只要查看其二进制位中有多少个，如果正好有1个，返回1，否则返回0
 * 在“查看其二进制位中有多少个”时使用了一个小技巧
 * 使用n &= (n-1)可以使得n中的减少一个（具体原理大家可以自己思考）
 */
int power_of_two(int n) {
  if (n <= 0)
    return 0;
  return (n & (n - 1)) == 0;
}

/* 函数load_texture
 * 读取一个BMP文件作为纹理
 * 如果失败，返回0，如果成功，返回纹理编号
 */
GLuint load_texture(const char *file_name) {
  GLint width, height, total_bytes, last_texture_ID;
  GLubyte *pixels = 0;
  GLuint texture_ID = 0;

  // 打开文件，如果失败，返回
  FILE *pFile;
  pFile = fopen(file_name, "rb");
  if (pFile == 0) {
    return 0;
  }

  // 读取文件中图象的宽度和高度
  fseek(pFile, 0x0012, SEEK_SET);
  fread(&width, sizeof(ImageWidth), 1, pFile);
  fread(&height, sizeof(ImageHeight), 1, pFile);
  fseek(pFile, BMP_Header_Length, SEEK_SET);

  // 计算每行像素所占字节数，并根据此数据计算总像素字节数
  {
    GLint line_bytes = width * 3;
    while (line_bytes % 4 != 0)
      ++line_bytes;
    total_bytes = line_bytes * height;
  }

  // 根据总像素字节数分配内存
  pixels = (GLubyte *)malloc(total_bytes);

  if (pixels == 0) {
    fclose(pFile);
    return 0;
  }

  // 读取像素数据

  if (fread(pixels, total_bytes, 1, pFile) <= 0) {
    free(pixels);
    fclose(pFile);
    return 0;
  }

  // 在旧版本的OpenGL中
  // 如果图象的宽度和高度不是的整数次方，则需要进行缩放
  // 这里并没有检查OpenGL版本，出于对版本兼容性的考虑，按旧版本处理
  // 另外，无论是旧版本还是新版本，
  // 当图象的宽度和高度超过当前OpenGL实现所支持的最大值时，也要进行缩放
  {
    GLint max;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max);
    if (!power_of_two(width) || !power_of_two(height) || width > max ||
        height > max) {
      const GLint new_width = 256;
      const GLint new_height = 256; // 规定缩放后新的大小为边长的正方形
      GLint new_line_bytes, new_total_bytes;
      GLubyte *new_pixels = 0;

      // 计算每行需要的字节数和总字节数
      new_line_bytes = new_width * 3;
      while (new_line_bytes % 4 != 0)
        ++new_line_bytes;
      new_total_bytes = new_line_bytes * new_height;

      // 分配内存
      new_pixels = (GLubyte *)malloc(new_total_bytes);
      if (new_pixels == 0) {
        free(pixels);
        fclose(pFile);
        return 0;
      }

      // 进行像素缩放
      gluScaleImage(GL_RGB, width, height, GL_UNSIGNED_BYTE, pixels, new_width,
                    new_height, GL_UNSIGNED_BYTE, new_pixels);

      // 释放原来的像素数据，把pixels指向新的像素数据，并重新设置width和height
      free(pixels);
      pixels = new_pixels;
      width = new_width;
      height = new_height;
    }
  }

  // 分配一个新的纹理编号
  glGenTextures(1, &texture_ID);
  if (texture_ID == 0) {
    free(pixels);
    fclose(pFile);
    return 0;
  }

  // 绑定新的纹理，载入纹理并设置纹理参数
  // 在绑定前，先获得原来绑定的纹理编号，以便在最后进行恢复
  glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture_ID);
  glBindTexture(GL_TEXTURE_2D, texture_ID);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR_EXT,
               GL_UNSIGNED_BYTE, pixels);
  glBindTexture(GL_TEXTURE_2D, last_texture_ID);

  // 之前为pixels分配的内存可在使用glTexImage2D以后释放
  // 因为此时像素数据已经被OpenGL另行保存了一份（可能被保存到专门的图形硬件中）
  free(pixels);
  return texture_ID;
}

/* 将当前纹理BGR格式转换为BGRA格式
 * 纹理中像素的RGB值如果与指定rgb相差不超过absolute，则将Alpha设置为0.0，否则设置为1.0
 */
void texture_colorkey(GLubyte r, GLubyte g, GLubyte b, GLubyte absolute) {
  GLint width, height;
  GLubyte *pixels = 0;

  // 获得纹理的大小信息
  glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
  glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);

  // 分配空间并获得纹理像素
  pixels = (GLubyte *)malloc(width * height * 4);
  if (pixels == 0)
    return;
  glGetTexImage(GL_TEXTURE_2D, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, pixels);

  // 修改像素中的Alpha值
  // 其中pixels[i*4], pixels[i*4+1], pixels[i*4+2], pixels[i*4+3]
  //   分别表示第i个像素的蓝、绿、红、Alpha四种分量，0表示最小，255表示最大
  {
    GLint i;
    GLint count = width * height;
    for (i = 0; i < count; ++i) {
      if (abs(pixels[i * 4] - b) <= absolute &&
          abs(pixels[i * 4 + 1] - g) <= absolute &&
          abs(pixels[i * 4 + 2] - r) <= absolute)
        pixels[i * 4 + 3] = 0;
      else
        pixels[i * 4 + 3] = 255;
    }
  }

  // 将修改后的像素重新设置到纹理中，释放内存
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA_EXT,
               GL_UNSIGNED_BYTE, pixels);
  free(pixels);
}

void display(void) {
  static int initialized = 0;
  static GLuint texWindow = 0;
  static GLuint texPicture = 0;

  // 执行初始化操作，包括：读取相片，读取相框，将相框由BGR颜色转换为BGRA，启用二维纹理
  if (!initialized) {
    texPicture = load_texture("../pic.bmp");
    texWindow = load_texture("../window.bmp");
    glBindTexture(GL_TEXTURE_2D, texWindow);
    texture_colorkey(255, 255, 255, 10);

    glEnable(GL_TEXTURE_2D);

    initialized = 1;
  }

  // 清除屏幕
  glClear(GL_COLOR_BUFFER_BIT);

  // 绘制相片，此时不需要进行Alpha测试，所有的像素都进行绘制
  glBindTexture(GL_TEXTURE_2D, texPicture);
  glDisable(GL_ALPHA_TEST);
  glBegin(GL_QUADS);
  glTexCoord2f(0, 0);
  glVertex2f(-1.0f, -1.0f);
  glTexCoord2f(0, 1);
  glVertex2f(-1.0f, 1.0f);
  glTexCoord2f(1, 1);
  glVertex2f(1.0f, 1.0f);
  glTexCoord2f(1, 0);
  glVertex2f(1.0f, -1.0f);
  glEnd();

  // 绘制相框，此时进行Alpha测试，只绘制不透明部分的像素
  glBindTexture(GL_TEXTURE_2D, texWindow);
  glEnable(GL_ALPHA_TEST);
  glAlphaFunc(GL_GREATER, 0.5f);
  glBegin(GL_QUADS);
  glTexCoord2f(0, 0);
  glVertex2f(-1.0f, -1.0f);
  glTexCoord2f(0, 1);
  glVertex2f(-1.0f, 1.0f);
  glTexCoord2f(1, 1);
  glVertex2f(1.0f, 1.0f);
  glTexCoord2f(1, 0);
  glVertex2f(1.0f, -1.0f);
  glEnd();

  // 交换缓冲
  glutSwapBuffers();
}

int main(int argc, char *argv[]) {
  // GLUT初始化
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
  glutInitWindowPosition(100, 100);
  glutInitWindowSize(WindowWidth, WindowHeight);
  glutCreateWindow(WindowTitle);
  glutDisplayFunc(&display);
  // 开始显示
  glutMainLoop();
  return 0;
}