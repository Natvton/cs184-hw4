#include <iostream> 
#include <GL/glfw.h>
#include <GL/glut.h>
#include <GL/glu.h>
#include <math.h>   
#include <FreeImage.h>
#include <string>
#include "camera.h"
#include "keyboard.h"
#include "glm.h"

using namespace std;

int windowWidth = 1280;
int windowHeight = 800;

int fov = 60;
int near = 1;
int far = 15000;

float mouseXSensitivity = 500.0;
float mouseYSensitivity = 500.0;

float strafeSpeed = 5.0;
float moveSpeed = 5.0;
float upSpeed = 5.0;

const float PI = 3.14159265359;

Camera *camera = new Camera();
Keyboard *keyboard = new Keyboard();

GLUquadricObj *sphere = NULL;
GLuint earth_textureID;
GLuint mars_textureID;
GLuint smiley_textureID;
GLuint stars_textureID;
GLuint moon_textureID;
float earthRot = 0.0f;
float moonRev = 0.0f;

GLMmodel* wheatley = glmReadOBJ("wheatley.obj");

// load a bitmap with freeimage
bool loadBitmap(string filename, FIBITMAP* &bitmap) {
  // get the file format
  FREE_IMAGE_FORMAT format = FreeImage_GetFileType(filename.c_str(), 0);
  if (format == FIF_UNKNOWN)
    format = FreeImage_GetFIFFromFilename(filename.c_str());
  if (format == FIF_UNKNOWN)
    return false;

  // load the image
  bitmap = FreeImage_Load(format, filename.c_str());
  if (!bitmap)
    return false;

  return true;
}


// load a texture into opengl with freeimage
bool loadTexture(string filename, GLuint &texture) {
  FIBITMAP *bitmap = NULL;
  if (!loadBitmap(filename, bitmap)) {
    return false;
  }

  // convert to 32 bit bit-depth
  FIBITMAP *bitmap32 = FreeImage_ConvertTo32Bits(bitmap);
  FreeImage_Unload(bitmap);
  if (!bitmap32) {
    return false;
  }
  bitmap = bitmap32;

  // get bits and dimensions
  BYTE *bits = FreeImage_GetBits(bitmap);
  int w = FreeImage_GetWidth(bitmap);
  int h = FreeImage_GetHeight(bitmap);

  // get bit order
  int order = GL_BGRA;

  // upload texture to opengl
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  gluBuild2DMipmaps(GL_TEXTURE_2D, 4, w, h, order, GL_UNSIGNED_BYTE, (GLvoid*)bits);

  // forget our copy of the bitmap now that it's stored the card
  FreeImage_Unload(bitmap);

  return true;
}

void loadTextures()
{
    string texture_location = "textures/";
    string textures[] = { "earth.png","mars.png","smiley.png","stars.png", "moon.png" };
    GLuint * textureID[] = { &earth_textureID, &mars_textureID, &smiley_textureID, &stars_textureID, &moon_textureID };
    for (int i=0; i < sizeof(textures) / sizeof(string); i++) {	
        loadTexture(texture_location + textures[i], *textureID[i]);
    }

}

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

    glClearColor(0, 1, 0, 1);
    glfwSwapInterval(1);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glClearDepth(1.0f);
    glDepthFunc(GL_LEQUAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glLineWidth(2.0f);  

    loadTextures();

    sphere = gluNewQuadric();
    gluQuadricDrawStyle(sphere, GLU_FILL);
    gluQuadricTexture(sphere, GL_TRUE);
    gluQuadricNormals(sphere, GLU_SMOOTH);

    glDisable(GL_LIGHTING);
    glfwSetKeyCallback(handleKeyboard);
    glfwSetMousePosCallback(handleMouse);

    glmFacetNormals(wheatley);
    glmVertexNormals(wheatley, 90.0);

    return true;
}

void moveCamera()
{
    if (keyboard->isHeld('W'))
        camera->forward(moveSpeed);
    if (keyboard->isHeld('S'))
        camera->forward(-moveSpeed);
    if (keyboard->isHeld('A'))
        camera->strafe(-strafeSpeed);
    if (keyboard->isHeld('D'))
        camera->strafe(strafeSpeed);
    if (keyboard->isHeld(' '))
        camera->up(upSpeed);
    if (keyboard->isHeld(GLFW_KEY_LSHIFT))
        camera->up(-upSpeed);
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

    if (!keyboard->isHeld('T'))
    {
        glEnable(GL_TEXTURE_2D);
        if (keyboard->isHeld('Z'))
            glBindTexture(GL_TEXTURE_2D, mars_textureID); 
        else if (keyboard->isHeld('X'))
            glBindTexture(GL_TEXTURE_2D, smiley_textureID); 
        else
            glBindTexture(GL_TEXTURE_2D, earth_textureID); 
        glColor3f(1, 1, 1);
    }
    else
        glDisable(GL_TEXTURE_2D);


    glRotatef(radToDeg(camera->getPitch()), 1, 0, 0);
    glRotatef(radToDeg(camera->getYaw()), 0, 1, 0);
    glTranslatef(-camera->x, -camera->y, -camera->z);

    if (keyboard->isHeld('T'))
        glColor3f(0, 1, 0);
    glPushMatrix();
        glRotatef(earthRot++, 0, 1, 0);
        glRotatef(90.0, 0.0, 1.0, 0.0); //orient earth
        glRotatef(-90.0, 1.0, 0.0, 0.0);
        gluSphere(sphere, 50.0, 50, 50);
    glPopMatrix();
    glPushMatrix();
        glRotatef(15, 1, 0, 0);
        glRotatef(moonRev+=0.5, 0, 1, 0);
        glTranslatef(100, 0 ,0);
        glRotatef(90.0, 0.0, 1.0, 0.0);
        glRotatef(-90.0, 1.0, 0.0, 0.0);
        glBindTexture(GL_TEXTURE_2D, moon_textureID); 
        gluSphere(sphere, 20.0, 50, 50);
    glPopMatrix();

    glPushMatrix();
        glTranslatef(2, 0, 190);
        glScalef(10,10,10);
        glmDraw(wheatley, GLM_SMOOTH|GLM_TEXTURE|GLM_MATERIAL);
    glPopMatrix();

    if (keyboard->isHeld('T'))
        glColor3f(0, 0, 0);
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glScalef(5,1,1);
    glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
            glRotatef(90.0, 0.0, 1.0, 0.0);
            glRotatef(-90.0, 1.0, 0.0, 0.0);
            glBindTexture(GL_TEXTURE_2D, stars_textureID); 
            gluSphere(sphere, 10000, 50, 50);
        glPopMatrix();
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    
    glfwSwapBuffers();
}

void saveScreenshot(string name)
{
    int pix = windowWidth * windowHeight;
    BYTE pixels[3*pix];	
    glReadBuffer(GL_FRONT);
    glReadPixels(0,0,windowWidth,windowHeight,GL_BGR,GL_UNSIGNED_BYTE, pixels);

    FIBITMAP *img = FreeImage_ConvertFromRawBits(pixels, windowWidth, windowHeight, windowWidth * 3, 24, 0xFF0000, 0x00FF00, 0x0000FF, false);
    
    std::cout << "Saving screenshot: " << name << "\n";

    FreeImage_Save(FIF_PNG, img, name.c_str(), 0);
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    camera->z = 300;

    if (init())
    {
        bool running = true;

        while (running)
        {
            display();

            if (keyboard->isHeld('P'))
                saveScreenshot("screenshot.png");

            running = !glfwGetKey(GLFW_KEY_ESC) && glfwGetWindowParam(GLFW_OPENED);
        }
        glfwTerminate();
    }
}
