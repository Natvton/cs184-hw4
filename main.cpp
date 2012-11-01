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
#include "shaders.h"

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
GLuint eye_textureID;
GLuint sun_textureID;

float earthRev = 0.0f;
float earthRot = 0.0f;
float sunRot = 0.0f;
float moonRev = 0.0f;
float triforceRev = 200.0f;
float wheatleyRev = 0.0f;

GLMmodel* wheatley = glmReadOBJ("wheatley.obj");

// Shaders
GLuint vertexshader, fragmentshader, shaderprogram;

// Lighting parameter array
const int numLights = 10;

// Materials
GLfloat ambient[4];
GLfloat diffuse[4];
GLfloat specular[4];
GLfloat emission[4];
GLfloat shininess;

// Variables to set uniform params for lighting fragment shader
GLuint lightcol ; 
GLuint lightpos ; 
GLuint numusedcol ; 
GLuint enablelighting ; 
GLuint ambientcol ; 
GLuint diffusecol ; 
GLuint specularcol ; 
GLuint emissioncol ; 
GLuint shininesscol ; 

GLuint enableTextures;
GLuint t0;

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
    string textures[] = { "earth.png", "mars.png", "smiley.png", "stars2.png", "moon.png", "eye.png", "sun.png" };
    GLuint * textureID[] = { &earth_textureID, &mars_textureID, &smiley_textureID, &stars_textureID, &moon_textureID, &eye_textureID, &sun_textureID };
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
    
    // Initialize shaders

    vertexshader = initshaders(GL_VERTEX_SHADER, "shaders/light.vert.glsl") ;
    fragmentshader = initshaders(GL_FRAGMENT_SHADER, "shaders/light.frag.glsl") ;
    shaderprogram = initprogram(vertexshader, fragmentshader) ; 

    // These variables are defined in the shader
    enablelighting = glGetUniformLocation(shaderprogram,"enablelighting") ;
    lightpos = glGetUniformLocation(shaderprogram,"lightposn") ;       
    lightcol = glGetUniformLocation(shaderprogram,"lightcolor") ;       
    numusedcol = glGetUniformLocation(shaderprogram,"numused") ;       
    ambientcol = glGetUniformLocation(shaderprogram,"ambient") ;       
    diffusecol = glGetUniformLocation(shaderprogram,"diffuse") ;       
    specularcol = glGetUniformLocation(shaderprogram,"specular") ;       
    emissioncol = glGetUniformLocation(shaderprogram,"emission") ;       
    shininesscol = glGetUniformLocation(shaderprogram,"shininess") ;           
    t0 = glGetUniformLocation(shaderprogram, "tex0");
    enableTextures = glGetUniformLocation(shaderprogram, "enableTex");

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

    glfwSetKeyCallback(handleKeyboard);
    glfwSetMousePosCallback(handleMouse);

    glmFacetNormals(wheatley);
    glmVertexNormals(wheatley, 90.0);

    glEnable(GL_LIGHTING);
    glUniform1i(enablelighting, true);
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

void drawTetrahedron()
{
    glBegin(GL_TRIANGLES);
        glNormal3f(0, 1, 0);
        glVertex3f(0, 0, 0);
        glVertex3f(5, 0, 0);
        glVertex3f(2.5, 0, 4.33);

        glNormal3f(-0.816317, 0.333901, 0.471315);
        glVertex3f(0, 0, 0);
        glVertex3f(2.5, 0, 4.33);
        glVertex3f(2.5, 4.085, 1.436);

        glNormal3f(0, 0.331636, -0.943407);
        glVertex3f(0, 0, 0);
        glVertex3f(5, 0, 0);
        glVertex3f(2.5, 4.085, 1.436);

        glNormal3f(0.816317, 0.333901, 0.471315);
        glVertex3f(5, 0, 0);
        glVertex3f(2.5, 0, 4.33);
        glVertex3f(2.5, 4.085, 1.436);
    glEnd();
}

// Used to transform the lights
void transformvec (const GLfloat input[4], GLfloat output[4]) {
  GLfloat modelview[16] ; // in column major order
  glGetFloatv(GL_MODELVIEW_MATRIX, modelview) ; 
  
  for (int i = 0 ; i < 4 ; i++) {
    output[i] = 0 ; 
    for (int j = 0 ; j < 4 ; j++) 
      output[i] += modelview[4*j+i] * input[j] ; 
  }
}

void set_rgba (GLfloat property[], float r, float g, float b, float a) {
  property[0] = r;
  property[1] = g;
  property[2] = b;
  property[3] = a;
}

void display()
{ 

    if (!keyboard->isHeld('E'))
    {
        earthRot++;
        earthRev    += 0.2;
        moonRev     += 0.5;
        triforceRev -= 0.5;
        wheatleyRev += 2;
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 
    // Reset the matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    moveCamera();

    if (!keyboard->isHeld('T')) {
        glEnable(GL_TEXTURE_2D);
        glColor3f(1, 1, 1); // Set to white when textures are enabled
	glUniform1i(enableTextures, true);
    }
    else {
        glDisable(GL_TEXTURE_2D);
	set_rgba(ambient, 0, 1, 0, 1);
	glUniform1i(enableTextures, false);
    }

    // Instead of using glm::lookAt(eye, center, up) to calculate
    // the modelview matrix and then loading that matrix
    // We manually transform the modelview matrix
    glRotatef(radToDeg(camera->getPitch()), 1, 0, 0);
    glRotatef(radToDeg(camera->getYaw()), 0, 1, 0);
    glTranslatef(-camera->x, -camera->y, -camera->z);


    // Texture 0 send to shader
    glUniform1i(t0, 0);

    int numused = 1;
    glUniform1i(numusedcol, numused);
    GLfloat lightposn[] = {0.0,0.0,0.0,1.0};
    GLfloat lightcolor[] = {1.0,1.0,0.6,1.0};
    GLfloat lightransf[4*numused];
    GLfloat lightin[4];
    GLfloat lightout[4];

    for(int i=0; i < numused; i++) {
      lightin[0] = lightposn[4*i];
      lightin[1] = lightposn[4*i+1];
      lightin[2] = lightposn[4*i+2];
      lightin[3] = lightposn[4*i+3];
      transformvec(lightin, lightout);
      lightransf[4*i] = lightout[0];
      lightransf[4*i+1] = lightout[1];
      lightransf[4*i+2] = lightout[2];
      lightransf[4*i+3] = lightout[3];
    }
    glUniform4fv(lightpos,numused,lightransf);
    glUniform4fv(lightcol,numused,lightcolor);

    // sun
    glPushMatrix();
	glRotatef(sunRot+=0.1,0.0,1.0,0.0);

        glRotatef(90.0, 0.0, 1.0, 0.0);
        glRotatef(-90.0, 1.0, 0.0, 0.0);
	glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, sun_textureID); 

	set_rgba(ambient, 1.0, 0.75, 0.75, 1);
	set_rgba(diffuse, 0.0, 0.0, 0.0, 1);
	set_rgba(specular, 0.0, 0.0, 0.0, 1);
	set_rgba(emission, 0.5, 0.5, 0.5, 1);
	shininess = 0.5;

        if (keyboard->isHeld('T'))
	  set_rgba(ambient, 1, 1, 0, 1);

	glUniform4fv(ambientcol,1, ambient);
       	glUniform4fv(diffusecol,1, diffuse);
	glUniform4fv(specularcol,1, specular);
	glUniform4fv(emissioncol,1, emission);
	glUniform1f(shininesscol, shininess);

        gluSphere(sphere, 100.0, 50, 50);
    glPopMatrix();

    //mars
    glPushMatrix();
        glActiveTexture(GL_TEXTURE0);
	glRotatef(-earthRev+200,0.0,1.0,0.0);
	glTranslatef(800.0,0.0,400.0);
	glBindTexture(GL_TEXTURE_2D, mars_textureID); 
	glRotatef(-earthRot, 0, 1, 0);
	glRotatef(90.0, 0.0, 1.0, 0.0); //orient mars
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	
	set_rgba(ambient, 0.3, 0.3, 0.3, 1);
	set_rgba(diffuse, 0.3, 0.3, 0.3, 1);
	set_rgba(specular, 1.0, 1.0, 1.0, 1);
	set_rgba(emission, 0.0, 0.0, 0.0, 1);
	shininess = 0.3;

	if (keyboard->isHeld('T'))
	  set_rgba(ambient, 0.7, 0.2, 0.2, 1);
	glUniform4fv(ambientcol,1, ambient);
	glUniform4fv(diffusecol,1, diffuse);
	glUniform4fv(specularcol,1, specular);
	glUniform4fv(emissioncol,1, emission);
	glUniform1f(shininesscol, shininess);
	
	gluSphere(sphere, 50.0, 50, 50);
    glPopMatrix();

    glPushMatrix();
        glRotatef(earthRev, 0, 1, 0);
        glTranslatef(500, 0 ,0);

        //earth
        glPushMatrix();
	glActiveTexture(GL_TEXTURE0);
            if (keyboard->isHeld('Z'))
                glBindTexture(GL_TEXTURE_2D, mars_textureID); 
            else if (keyboard->isHeld('X'))
                glBindTexture(GL_TEXTURE_2D, smiley_textureID); 
            else
                glBindTexture(GL_TEXTURE_2D, earth_textureID); 

            glRotatef(earthRot, 0, 1, 0);
            glRotatef(90.0, 0.0, 1.0, 0.0); //orient earth
            glRotatef(-90.0, 1.0, 0.0, 0.0);

	    set_rgba(ambient, 0.3, 0.3, 0.3, 1);
	    set_rgba(diffuse, 0.3, 0.3, 0.3, 1);
	    set_rgba(specular, 1.0, 1.0, 1.0, 1);
	    set_rgba(emission, 0.0, 0.0, 0.0, 1);
	    shininess = 0.3;

            if (keyboard->isHeld('T'))
	      set_rgba(ambient, 0.4, 0.4, 0.7, 1);


	    glUniform4fv(ambientcol,1, ambient);
	    glUniform4fv(diffusecol,1, diffuse);
	    glUniform4fv(specularcol,1, specular);
	    glUniform4fv(emissioncol,1, emission);
	    glUniform1f(shininesscol, shininess);

            gluSphere(sphere, 50.0, 50, 50);
        glPopMatrix();

        //moon
        glPushMatrix();
            glRotatef(15, 1, 0, 0);
            glRotatef(moonRev, 0, 1, 0);
            glTranslatef(100, 0 ,0);
            glRotatef(90.0, 0.0, 1.0, 0.0);
            glRotatef(-90.0, 1.0, 0.0, 0.0);

	    set_rgba(ambient, 0.3, 0.3, 0.3, 1);
	    set_rgba(diffuse, 0.3, 0.3, 0.3, 1);
	    set_rgba(specular, 1.0, 1.0, 1.0, 1);
	    set_rgba(emission, 0.0, 0.0, 0.0, 1);
	    shininess = 0.3;

            if (keyboard->isHeld('T'))
	      set_rgba(ambient, 0.7, 0.7, 0.7, 1);


	    glUniform4fv(ambientcol,1, ambient);
	    glUniform4fv(diffusecol,1, diffuse);
	    glUniform4fv(specularcol,1, specular);
	    glUniform4fv(emissioncol,1, emission);
	    glUniform1f(shininesscol, shininess);

	    glUniform4fv(ambientcol,1, ambient);

	    glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, moon_textureID); 
            gluSphere(sphere, 20.0, 50, 50);
        glPopMatrix();
    glPopMatrix();

    glPushMatrix();
        glRotatef(triforceRev, 0, 1, 0);
        glTranslatef(200, 0 ,0);

	//wheatley
        glPushMatrix();
            glRotatef(wheatleyRev, 0.981, 0.196, 0);
            glTranslatef(0, 60 ,0);
            glRotatef(-wheatleyRev, 0.981, 0.196, 0);
            glScalef(10,10,10);

	    set_rgba(ambient, 0.5, 0.5, 0.5, 1);
	    set_rgba(diffuse, 1.0, 1.0, 1.0, 1);
	    set_rgba(specular, 0.0, 0.0, 0.0, 1);
	    set_rgba(emission, 0.0, 0.0, 0.0, 1);
	    shininess = 1000000;

            if (keyboard->isHeld('T'))
	      set_rgba(ambient, 0.0, 0.0, 1.0, 1.0);

	    glUniform4fv(ambientcol,1, ambient);
	    glUniform4fv(diffusecol,1, diffuse);
	    glUniform4fv(specularcol,1, specular);
	    glUniform4fv(emissioncol,1, emission);
	    glUniform1f(shininesscol, shininess);

	    glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, eye_textureID); 
            glmDraw(wheatley, GLM_SMOOTH | GLM_TEXTURE | GLM_MATERIAL);
        glPopMatrix();

        //triforce
        glPushMatrix();
            glTranslatef(-5, -4.33, -2.894); //center it
            glScalef(5,5,5);

	    set_rgba(ambient, 0.0, 0.0, 0.0, 1);
	    set_rgba(diffuse, 0.0, 0.0, 0.0, 1);
	    set_rgba(specular, 1.0, 0.7, 0.0, 1);
	    set_rgba(emission, 0.0, 0.0, 0.0, 1);
	    shininess = 1.0;
	    glUniform4fv(ambientcol,1, ambient);
	    glUniform4fv(diffusecol,1, diffuse);
	    glUniform4fv(specularcol,1, specular);
	    glUniform4fv(emissioncol,1, emission);
	    glUniform1f(shininesscol, shininess);

            drawTetrahedron();
            glPushMatrix();
                glTranslatef(5,0,0);
                drawTetrahedron();
            glPopMatrix();
            glPushMatrix();
                glTranslatef(2.5,0,4.33);
                drawTetrahedron();
            glPopMatrix();
            glPushMatrix();
                glTranslatef(2.5, 4.085, 1.436);
                drawTetrahedron();
            glPopMatrix();
        glPopMatrix();
    glPopMatrix();

    if (keyboard->isHeld('T'))
        glColor3f(0, 0, 0);

    //stars
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glActiveTexture(GL_TEXTURE0);
    glScalef(5,1,1);
    glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
            glRotatef(90.0, 0.0, 1.0, 0.0);
            glRotatef(-90.0, 1.0, 0.0, 0.0);

	    set_rgba(ambient, 1.0, 1.0, 1.0, 1);
	    set_rgba(diffuse, 1.0, 1.0, 1.0, 1);
	    set_rgba(specular, 0.0, 0.0, 0.0, 1);
	    set_rgba(emission, 0.0, 0.0, 0.0, 1);
	    shininess = 1.0;

            if (keyboard->isHeld('T'))
	      set_rgba(ambient, 0,0,0,1);

	    glUniform4fv(ambientcol,1, ambient);
	    glUniform4fv(diffusecol,1, diffuse);
	    glUniform4fv(specularcol,1, specular);
	    glUniform4fv(emissioncol,1, emission);
	    glUniform1f(shininesscol, shininess);

            glBindTexture(GL_TEXTURE_2D, stars_textureID); 
            gluSphere(sphere, 10000, 50, 50);
        glPopMatrix();
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);

    glColor3f(1, 1, 1);
    
    glfwSwapBuffers();
}

void saveScreenshot(string name)
{
    int pix = windowWidth * windowHeight;
    BYTE pixels[3*pix];	
    glReadBuffer(GL_FRONT);
    glReadPixels(0,0,windowWidth,windowHeight,GL_BGR,GL_UNSIGNED_BYTE, pixels);

    FIBITMAP *img = FreeImage_ConvertFromRawBits(pixels, windowWidth, windowHeight, windowWidth * 3, 24, 0xFF0000, 0x00FF00, 0x0000FF, false);
    
    std::cout << "Saving screenshot: " << name << std::endl;

    FreeImage_Save(FIF_PNG, img, name.c_str(), 0);
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    camera->z = 1000;

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
