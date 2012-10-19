
#include <iostream>
 
#include <GL/glfw.h>
#include <GL/glut.h>
#include <GL/glu.h>
#include <math.h>   
#include "camera.h"
#include "keyboard.h"

using namespace std;

int windowWidth = 1280;
int windowHeight = 800;

int fov = 60;
int near = 1;
int far = 1500;

float mouseXSensitivity = 500.0;
float mouseYSensitivity = 500.0;

float strafeSpeed = 1.0;
float moveSpeed = 1.0;

const float PI = 3.14159265359;

Camera *camera = new Camera();
Keyboard *keyboard = new Keyboard();

void handleMouse(int x, int y)
{
    int dx = x - (windowWidth/2.0);
    int dy = y - (windowHeight/2.0);

    camera->updateYaw(dx / mouseXSensitivity);
    camera->updatePitch(dy / mouseYSensitivity);

    glfwSetMousePos(windowWidth/2.0, windowHeight/2.0);
}

void handleKeyboard(int key, int action)
{
    switch(action)
    {
        case GLFW_PRESS:
            keyboard->keyPress(key);
            break;
        case GLFW_RELEASE:
            keyboard->keyRelease(key);
            break;
    }
}

bool init()
{
    glfwInit();
 
    if(!glfwOpenWindow(windowWidth, windowHeight, 8, 8, 8, 8, 24, 0, GLFW_WINDOW))
    {
        cout << "Failed to open window!" << endl;
        glfwTerminate();
        return false;
    }
    
    glfwSetMousePos(windowWidth/2, windowHeight/2);

    glfwDisable(GLFW_MOUSE_CURSOR);
    glfwSetWindowTitle("SPAAAAAAACE!!!");
    glViewport(0, 0, (GLsizei)windowWidth, (GLsizei)windowHeight);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fov/2, (float)windowWidth/(float)windowHeight, near, far);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glClearColor(0, 0, 0, 1);
    glfwSwapInterval(1);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glClearDepth(1.0f);
    glDepthFunc(GL_LEQUAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glEnable(GL_CULL_FACE);
    glLineWidth(2.0f);  

    glDisable(GL_LIGHTING);

    glfwSetKeyCallback(handleKeyboard);
    glfwSetMousePosCallback(handleMouse);

    return true;
}

void moveCamera()
{
    if (keyboard->isHeld('W'))
        camera->move(moveSpeed);
    if (keyboard->isHeld('S'))
        camera->move(-moveSpeed);
    if (keyboard->isHeld('A'))
        camera->strafe(-strafeSpeed);
    if (keyboard->isHeld('D'))
        camera->strafe(strafeSpeed);
}

float radToDeg(float radians)
{
    return radians * (180.0/PI);
}

void display()
{ 
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 
    // Reset the matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    moveCamera();

    glRotatef(radToDeg(camera->getPitch()), 1, 0, 0);
    glRotatef(radToDeg(camera->getYaw()), 0, 1, 0);
    glTranslatef(-camera->x, -camera->y, -camera->z);

    glColor3ub(255, 255, 0);
    if (keyboard->isHeld('Z'))
        glColor3ub(0, 255, 0);
    glutSolidTeapot(5);
    
    glfwSwapBuffers();
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    camera->z = 20;

    if (init())
    {
        bool running = true;

        while (running)
        {
            display();

            running = !glfwGetKey(GLFW_KEY_ESC) && glfwGetWindowParam(GLFW_OPENED);
        }
        glfwTerminate();
    }
}
