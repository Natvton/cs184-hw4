#include <iostream>
#include "imageloader.h" 
#include <GL/glut.h>
#include <GL/glfw.h>
#include <math.h>
 
using namespace std;
 
GLint windowWidth  = 1000;
GLint windowHeight = 1000;

GLfloat camXPos = 0.0f;
GLfloat camYPos = 0.0f;
GLfloat camZPos = 0.0f;

GLfloat fieldOfView = 45.0f;
GLfloat near = 1.0f;
GLfloat far = 1500.0f;

GLfloat camXRot = 0.0f;
GLfloat camYRot = 0.0f;
GLfloat camZRot = 0.0f; 

GLfloat camXSpeed = 0.0f;
GLfloat camYSpeed = 0.0f;
GLfloat camZSpeed = 0.0f;
 
GLint frameCount = 0;
 
bool holdingForward     = false;
bool holdingBackward    = false;
bool holdingLeftStrafe  = false;
bool holdingRightStrafe = false;
GLUquadricObj *sphere = NULL;

GLuint earth_textureID;
GLuint mars_textureID;
GLfloat earthRot = 0.0f;
GLfloat marsRot = 0.0f;

void initGL()
{
    glViewport(0, 0, (GLsizei)windowWidth, (GLsizei)windowHeight);
    glEnable(GL_TEXTURE_2D);    // Required for textures
    glEnable(GL_DEPTH_TEST);    // Enable depth testing (So you can't see inside the sphere..makes textures weird otherwise)
    glClearDepth(1.0f);
    glDepthFunc(GL_LEQUAL);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
 
    GLfloat aspectRatio = (windowWidth > windowHeight)? float(windowWidth)/float(windowHeight) : float(windowHeight)/float(windowWidth);
    gluPerspective(fieldOfView/2.0f, aspectRatio, near, far) ;
 
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
 
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    //Loading textures, probably want to put in a loop/separate method
    Image* img = loadBMP("earth.bmp");
    glGenTextures(1, &earth_textureID);
    glBindTexture(GL_TEXTURE_2D, earth_textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img->width, img->height, 0, GL_RGB, GL_UNSIGNED_BYTE, img->pixels);

    img = loadBMP("mars.bmp");
    glGenTextures(1, &mars_textureID);
    glBindTexture(GL_TEXTURE_2D, mars_textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img->width, img->height, 0, GL_RGB, GL_UNSIGNED_BYTE, img->pixels);

    delete img;

    //Creating the quadric for use with gluSphere
    sphere = gluNewQuadric();
    gluQuadricDrawStyle(sphere, GLU_FILL);
    gluQuadricTexture(sphere, GL_TRUE);
    gluQuadricNormals(sphere, GLU_SMOOTH);

 }
 
void moveCamera()
{
  if (holdingForward)
    camZPos -= 3;
  if (holdingBackward)
    camZPos += 3;
  if (holdingLeftStrafe)
    camXPos -= 3;
  if (holdingRightStrafe)
    camXPos += 3;
}
 
void handleKeypress(int theKey, int theAction)
{
    if (theAction == GLFW_PRESS)
    {
        switch(theKey)
        {
        case 'W':
            holdingForward = true;
            break;
 
        case 'S':
            holdingBackward = true;
            break;
 
        case 'A':
            holdingLeftStrafe = true;
            break;
 
        case 'D':
            holdingRightStrafe = true;
            break;
 
        default:
            break;
        }
    }
    else // If a key is released, toggle the relevant key-release flag
    {
        switch(theKey)
        {
        case 'W':
            holdingForward = false;
            break;
 
        case 'S':
            holdingBackward = false;
            break;
 
        case 'A':
            holdingLeftStrafe = false;
            break;
 
        case 'D':
            holdingRightStrafe = false;
            break;
 
        default:
            break;
        }
    }
}

void drawScene()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
 
    // Move the camera to our location in space
    glRotatef(camXRot, 1.0f, 0.0f, 0.0f);        // Rotate our camera on the x-axis (looking up and down)
    glRotatef(camYRot, 0.0f, 1.0f, 0.0f);        // Rotate our camera on the  y-axis (looking left and right)
    glTranslatef(-camXPos,-camYPos,-camZPos);    // Translate the modelviewm matrix to the position of our camera
 
    // Move everything "into" the screen (i.e. move 300 units along the Z-axis into the screen) so that all positions are now relative to the location of the sun
    glTranslatef(0.0f, 0.0f, -200.0f);
 
    // Draw the earth
    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, earth_textureID); 
    glRotatef(90.0, 0.0, 1.0, 0.0); //orient earth
    glRotatef(-90.0, 1.0, 0.0, 0.0);

    glRotatef(earthRot++,0.0,0.0,1.0); //spin earth 1 degree every frame
    if (earthRot > 360.0)
      earthRot = 0.0;

    gluSphere(sphere, 50.0, 50, 50);
    glPopMatrix();

    // Draw the mars
    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, mars_textureID); 
    glRotatef(90.0, 0.0, 1.0, 0.0); //orient mars
    glRotatef(-90.0, 1.0, 0.0, 0.0);
    glTranslatef(100.0,0.0,0.0); //move it back a bit

    marsRot -= 2.0;
    glRotatef(marsRot,0.0,0.0,1.0); //spin mars -2 degrees every frame
    if (marsRot > 360.0)
      marsRot = 0.0;

    gluSphere(sphere, 100.0, 50, 50);
    glPopMatrix();

    glfwSwapBuffers(); // Swap the buffers to display the scene (so we don't have to watch it being drawn!)
}
 
int main(int argc, char **argv)
{
    bool running = true;
 
    glutInit(&argc, argv);
    glfwInit();
 
    // Create a window
    if(!glfwOpenWindow(windowWidth, windowHeight, 8, 8, 8, 8, 24, 0, GLFW_WINDOW))
    {
        cout << "Failed to open window!" << endl;
        glfwTerminate();
        return 0;
    }
 
    initGL();
 
    glfwSetKeyCallback(handleKeypress);
 
    while (running == true)
    {
        drawScene();
        moveCamera();
        frameCount++;

        // exit if ESC was pressed or window was closed
        running = !glfwGetKey(GLFW_KEY_ESC) && glfwGetWindowParam(GLFW_OPENED);
    }
 
    glfwTerminate();
    return 0;
}
