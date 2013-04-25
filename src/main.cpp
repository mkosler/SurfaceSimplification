#include <cstdlib>
#include <cstdio>

#include <GL/glfw.h>

#include "Mesh.h"

const unsigned WINDOW_WIDTH  = 640,
               WINDOW_HEIGHT = 480;

static int PREVIOUS_MOUSE_X = 0,
           PREVIOUS_MOUSE_Y = 0;

static float ROTATION_MATRIX[16] = {
  1, 0, 0, 0,
  0, 1, 0, 0,
  0, 0, 1, 0,
  0, 0, 0, 1,
};

static float FOCUS[3] = { 0, 0, -1.5, };

void onExit()
{
  glfwTerminate();
}

void GLFWCALL onMouseClick(int button, int action)
{
  glfwGetMousePos(&PREVIOUS_MOUSE_X, &PREVIOUS_MOUSE_Y);
}

void GLFWCALL onMouseMove(int x, int y)
{

  y = WINDOW_HEIGHT - y;

  float dx = x - PREVIOUS_MOUSE_X,
        dy = y - PREVIOUS_MOUSE_Y;

  if (GLFW_PRESS == glfwGetMouseButton(GLFW_MOUSE_BUTTON_LEFT)) {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glRotatef(dx, 0, 1, 0);
    glRotatef(dy, -1, 0, 0);
    glMultMatrixf(ROTATION_MATRIX);
    glGetFloatv(GL_MODELVIEW_MATRIX, ROTATION_MATRIX);
  } else if (GLFW_PRESS == glfwGetMouseButton(GLFW_MOUSE_BUTTON_MIDDLE)) {
    FOCUS[0] += 0.005f * dx;
    FOCUS[1] += 0.005f * dy;
  } else if (GLFW_PRESS == glfwGetMouseButton(GLFW_MOUSE_BUTTON_RIGHT)) {
    FOCUS[2] += 0.01f * dy;
  }

  PREVIOUS_MOUSE_X = x;
  PREVIOUS_MOUSE_Y = y;
}

void GLFWCALL onKeyClick(int key, int action)
{
  if (GLFW_KEY_ESC == key && GLFW_PRESS == action) {
    glfwCloseWindow();
  }
}

bool initialize()
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

  glClearColor(0, 0, 0, 1);

  glEnable(GL_DEPTH_TEST);

  glShadeModel(GL_SMOOTH);

  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHTING);

  glEnable(GL_CULL_FACE);

  glfwSetKeyCallback(onKeyClick);
  glfwSetMouseButtonCallback(onMouseClick);
  glfwSetMousePosCallback(onMouseMove);

  return true;
}

int main(int argc, char *argv[])
{
  if (argc != 3) {
    fprintf(stderr, "./SurfaceSimplification <path to .obj> <number of vertexes>\n");
    return 4;
  }

  Mesh m = Mesh::load(argv[1]);
  size_t numberOfPolygons = atoi(argv[2]);

  if (!glfwInit()) {
    return 1;
  }

  atexit(onExit);

  glfwOpenWindowHint(GLFW_WINDOW_NO_RESIZE, GL_TRUE);

  if (!glfwOpenWindow(640, 480, 0, 0, 0, 0, 0, 0, GLFW_WINDOW)) {
    return 2;
  }

  glfwSetWindowTitle("CSCE 645 - Surface Simplification - Michael Kosler");

  if (!initialize()) {
    return 3;
  }

  while (glfwGetWindowParam(GLFW_OPENED)) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, float(WINDOW_WIDTH) / float(WINDOW_HEIGHT), 0.0001, 100);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glTranslatef(FOCUS[0], FOCUS[1], FOCUS[2]);
    glMultMatrixf(ROTATION_MATRIX);

    m.draw();

    glfwSwapBuffers();
  }

  return 0;
}
