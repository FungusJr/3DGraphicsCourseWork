
#include <math.h>						
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <iostream>
#include <gl/freeglut.h>
#include <iostream>

#ifdef WIN32
#include "gltools.h"
#include <windows.h>		
#include <gl\gl.h>			
#include <gl\glu.h>			
#include "glm.h"

#endif
using namespace std;

//global declarations
//textures
#define TEXTURE_STAR 0
#define TEXTURE_SPACE 1
#define TEXTURE_JUPITER 2
#define TEXTURE_HUD 3
#define TEXTURE_COUNT 4
//floats
float angle_z = 0.0f;
float move_x = 0.0f, move_y = 0.0f, move_z = -300.0f;
double spaceRotate = 0.0;
bool hudActive = false;
GLfloat score = 0;
GLfloat x_coord = 0.0;
GLfloat y_coord = 0.0;
GLfloat z_coord = 0.0;
//chars
char shipmotion_x[20];
char shipmotion_y[20];
char shipmotion_z[20];
char ScoreTotal[12];
//bools
bool cryone = false, crytwo = false, crythree = false, cryfour = false, cryfive = false, crysix = false;
//texture array
GLuint  textures[TEXTURE_COUNT];

const char *textureFiles[TEXTURE_COUNT] = {"starField.tga","tycho_cyl_glow.tga","jupiterC.tga", "hud.tga"};


//for lighting if you want to experiment with these

GLfloat mKa[4] = {0.11f,0.06f,0.11f,1.0f}; //ambient
GLfloat mKd[4] = {0.43f,0.47f,0.54f,1.0f}; //diffuse
GLfloat mKs[4] = {1.0f,1.0f,1.0f,1.0f}; //specular
GLfloat mKe[4] = {0.5f,0.5f,0.0f,1.0f}; //emission

//key handling - only one shown simplly as an example
bool upKeyPressed = false;


// Useful lighting colour values
GLfloat  whiteLightBright[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat  redLight[] = { 1.0f, 0.0f, 0.0f, 1.0f };
GLfloat  whiteLightLessBright[] = { 0.8f, 0.8f, 0.8f, 1.0f };
GLfloat	 lightPos[] = { 100.0f, 100.0f, 50.0f, 1.0f };


//we need these for the texture loader
//they are to do with the image format and size
GLint iWidth, iHeight, iComponents;
GLenum eFormat;
// this is a pointer to memory where the image bytes will be held 
GLbyte *pBytes0;

//end of intialisation 

//methods
// draws the torus requires dimenisions and texture number
void drawTorus(double inner, double outer, int sides, int rings, int texture)
{ // included features to add and bind the texture to the torus
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP); //generates texture coords
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE,GL_SPHERE_MAP);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textures[texture]); 
	// value to make the torus continuously rotate
	glRotatef(angle_z, 0, 0, 1);
	//drawing the torus
	glutSolidTorus(inner,outer,sides,rings);
	//disable texture feature 
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glDisable(GL_TEXTURE_2D);

}
// draw for the crystals, requries a bool for colour display
void drawCrystal(bool passed)
{	// scalled to visible size
	glScaled(30, 30, 0);
	// colour dependant on a bool which tells weither a collision has occured
	glColor3f(1.0, 0.0, 0.0);
	if (passed == true) { glColor3f(0.0, 1.0, 0.0); }
	//drawing the crystal
	glutSolidIcosahedron();	
//reseting the colour of the project
	glColor3f(1.0,1.0,1.0);
	
}
// drawing the outer sphere 
void drawSpace()
{//enable texture requirements
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textures[TEXTURE_SPACE]);
	GLUquadricObj *quadricSpace;
	quadricSpace = gluNewQuadric();
	gluQuadricDrawStyle(quadricSpace, GLU_FILL);
	gluQuadricNormals(quadricSpace,GLU_SMOOTH);
	//sets the texutre to appear on the inside of the sphere
	gluQuadricOrientation(quadricSpace,GLU_INSIDE);
	gluQuadricTexture(quadricSpace, GL_TRUE);
	//scaling the sphere
	glScaled(30,30,30);
	// drawing the sphere
	gluSphere(quadricSpace,50.0f,50,35);
	// disableing texture drawing
	glDisable(GL_TEXTURE_2D);

}
// code to draw background planet
void drawJupiter()
{// enable texture drawing and bind texture to a quadric
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textures[TEXTURE_JUPITER]);
	GLUquadricObj *quadricJupiter;
	quadricJupiter = gluNewQuadric();
	gluQuadricDrawStyle(quadricJupiter, GLU_FILL);
	gluQuadricNormals(quadricJupiter, GLU_SMOOTH);
	// texutre appears on the outside of the sphere
	gluQuadricOrientation(quadricJupiter, GLU_OUTSIDE);
	gluQuadricTexture(quadricJupiter, GL_TRUE);
	// rotate the texture to look normal and then add constant rotation in the y-axis
	glPushMatrix();
	glRotatef(angle_z, 0, 1, 0);
	glRotatef(270.0, 1, 0, 0);
	// creating the sphere
	gluSphere(quadricJupiter, 50.0f, 50, 35);
	glPopMatrix();
	// disabling texture drawing
	glDisable(GL_TEXTURE_2D);
}
// changing the modelview to project in 2D drawing perspective
void setOrthographicProjection()
{
	glMatrixMode(GL_PROJECTION);

	glPushMatrix();
    glLoadIdentity();
	gluOrtho2D(0, 800, 0, 600);
	glScalef(1, -1, 1);
	glTranslatef(0, -800, 0);
	glMatrixMode(GL_MODELVIEW);
}
// changing back to a modelview for 3D perspective
void resetPerspectiveProjection()
{
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}
// code for drawing each individual char out onto the screen at the given position
void drawCoords(float x, float y, char *string, bool stroke)
{//changing colour to green
	glColor3f(0.0f,1.0f,0.0f);
	// selecting position to start printing at
	glRasterPos2f(x, y);
	// loop to print each char in the string
	int len = strlen(string);
	for (int i = 0; i < len; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, string[i]);
	}
	// reseting colour to white;
	glColor3f(1.0f, 1.0f, 1.0f);
}
// setting the coordinates and printing for the ship coordinates displayed in the HUD
void setCoordinates()
{// add the cuurrent coordinates of the ship to the given string
	sprintf_s(shipmotion_x, "x:position:%2.0f", x_coord);
	sprintf_s(shipmotion_y, "y:position:%2.0f", y_coord);
	sprintf_s(shipmotion_z, "z:position:%2.0f",z_coord);
	sprintf_s(ScoreTotal,"Score%2.0f",score);
	glLoadIdentity();
	// print the string in the selected positions
	drawCoords(800/2,600/2,shipmotion_x,false);
	drawCoords(800/2, (600/2) + 10, shipmotion_y, false);
	drawCoords(800/2, (600 / 2) + 20, shipmotion_z, false);
	drawCoords(800 / 2, (600 / 2) - 50, ScoreTotal, false);

	glutPostRedisplay();
}
// checks weither the ship coordinates match the given coorinates of a crystal and return a true false value
bool checkcollision(float x, double y, double z, bool collision)
{// compares coordinates and makes sure value can only be changes once 
	if( x == x_coord && y == y_coord && z == z_coord && collision == false )
	{
		score++;
		return true;
	}
	// makes sure in loop value stays true if already true
	else if (collision == true) return true;
	else return false;

}

void drawHUD()
{// sets the perspective to 2D draws the texture and then goes back to 3D drawing
	glLoadIdentity();
	
	setOrthographicProjection();
/*	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(1.0, 1.0, 1.0, 0.50);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textures[TEXTURE_HUD]);
	//draw quad
	glBegin(GL_QUADS);
	glVertex3f(0.0f, 0.0f,1.0f);              
	glVertex3f(0.0f, 1000.0f,1.0f);              
	glVertex3f(800.0f, 1000.0f,1.0f);              
	glVertex3f(800.0f, 0.0f,1.0f);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	*/
	setCoordinates();
	
	resetPerspectiveProjection();
}

//display
void RenderScene(void)
{  
	// Clear the window with current clearing colour
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity(); 

// global values that translate everything being drawn in 3D coordinates
#pragma region Global Translates
//global translate goes here
	glTranslatef(move_x,move_y,move_z);
	glRotatef(spaceRotate, 0, 0, 1);
#pragma endregion
	// creating of each of the 6 torus and crystal hoops with a collision check on each to see weither ship is at the same coords
#pragma region CrystalOne

	glPushMatrix();
	glTranslatef(-300.0,-100.0,-500.0);
	drawTorus(10.0,60.0,20,50,TEXTURE_STAR);
	cryone = checkcollision(300,-100,500 + 300,cryone);
	drawCrystal(cryone);
	glPopMatrix();
#pragma endregion	
#pragma region CrystalTwo
//Crystal#2
	glPushMatrix();
	glTranslatef(200.0, -50, -400);
	drawTorus(10.0, 60.0, 20, 50, TEXTURE_STAR);
	crytwo = checkcollision(-200,-50,400 + 300, crytwo);
	drawCrystal(crytwo);
	glPopMatrix();
#pragma endregion
#pragma region CrystalThree
//Crystal#3
	glPushMatrix();
	glTranslatef(-30.0,-300.0,-600.0);
	drawTorus(10.0, 60.0, 20, 50, TEXTURE_STAR);
	crythree = checkcollision(30,-300,600 + 300,crythree);
	drawCrystal(crythree);
	glPopMatrix();
#pragma endregion
#pragma region CrystalFour
// Crystal#4
	glPushMatrix();
	glTranslatef(-30.0, 300.0, -600.0);
	drawTorus(10.0, 60.0, 20, 50, TEXTURE_STAR);
	cryfour = checkcollision(30,300,600 + 300,cryfour);
	drawCrystal(cryfour);
	glPopMatrix();
#pragma endregion
#pragma region CrystalFive
// Crystal#5
	glPushMatrix();
	glTranslatef(-200.0, 250.0, -300.0);
	drawTorus(10.0, 60.0, 20, 50, TEXTURE_STAR);
	cryfive = checkcollision(200,250,300 + 300, cryfive);
	drawCrystal(cryfive);
	glPopMatrix();
#pragma endregion
#pragma region CrystalSix
//Crystal#6
	glPushMatrix();
	glTranslatef(30.0, 0.0, -600.0);
	drawTorus(10.0, 60.0, 20, 50, TEXTURE_STAR);
	crysix = checkcollision(-30,0,600 + 300, crysix);
	drawCrystal(crysix);
	glPopMatrix();

#pragma endregion
	// drawing of the inner textures space sphere and jupiter planet
#pragma region SpaceAndPlanets
//drawing space
	glPushMatrix();
	drawSpace();
	glPopMatrix();

//drawing jupiter planet
	glPushMatrix();
	glTranslatef(300.0,0.0,-1000.0);
	drawJupiter();
	glPopMatrix();

#pragma endregion
	// drawing of HUD based off of bool value being true
#pragma region HUD
if (hudActive == true) { drawHUD();}
	else if (hudActive == false) { glColor4f(1.0, 1.0, 1.0, 1.0); }
#pragma endregion

// end of render
}

// This function does any needed initialization on the rendering
// context.
void SetupRC()
{
    //textures

    GLuint texture;
    // allocate a texture name
    glGenTextures( 1, &texture );
    glPixelStorei(GL_UNPACK_ALIGNMENT,1);
	// photoshop is a good converter to targa (TGA)
	//the gltLoadTGA method is found in gltools.cpp and is orignally from the OpenGL SuperBible book
	//there are quite a few ways of loading images
    // Load textures in a for loop
    glGenTextures(TEXTURE_COUNT, textures);
    //this puts the texture into OpenGL texture memory
 //   glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE); - not defined so probably need to update GLEW - not needed here so ignore
    for(int iLoop = 0; iLoop < TEXTURE_COUNT; iLoop++)
    {
        // Bind to next texture object
        glBindTexture(GL_TEXTURE_2D, textures[iLoop]);
        
        // Load texture data, set filter and wrap modes
        //note that gltLoadTGA is in the glm.cpp file and not a built-in openGL function
        pBytes0 = gltLoadTGA(textureFiles[iLoop],&iWidth, &iHeight,
                             &iComponents, &eFormat);
        
        glTexImage2D(GL_TEXTURE_2D, 0, iComponents, iWidth, iHeight, 0, eFormat, GL_UNSIGNED_BYTE, pBytes0);
        
            //set up texture parameters
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
         glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        //try these too
       // glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_ADD);
        // glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
        free(pBytes0);
    }
    
	//enable textures
    glEnable(GL_TEXTURE_2D);

    
	glEnable(GL_DEPTH_TEST);	// Hidden surface removal    
    glFrontFace(GL_CCW);// Counter clock-wise polygons face out
	glEnable(GL_CULL_FACE);		// Do not calculate inside

//    glCullFace(GL_FRONT_AND_BACK);
    
	// Enable lighting
//	glEnable(GL_LIGHTING);
	glEnable(GL_POINT_SMOOTH);
	// Setup and enable light 0
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT,whiteLightLessBright);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,redLight);
	glLightfv(GL_LIGHT0,GL_POSITION,lightPos);
	glEnable(GL_LIGHT0);

	// Enable colour tracking
	glEnable(GL_COLOR_MATERIAL);
	
	// Set Material properties to follow glColor values
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    
	// Black blue background
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f );
}
// a continuous loop that updates the angle of rotation and redraws the render scene constantly
void TimerFunc(int value)
{
    glutSwapBuffers();
	angle_z += 0.15f;
    glutPostRedisplay();
    glutTimerFunc(25, TimerFunc, 1);
}

void ChangeSize(int w, int h)
{
    GLfloat fAspect;
    
    // Prevent a divide by zero
    if(h == 0)
        h = 1;
    
    // Set Viewport to window dimensions
    glViewport(0, 0, w, h);
    
    // Calculate aspect ratio of the window
    fAspect = (GLfloat)w/(GLfloat)h;
    
    // Set the perspective coordinate system
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    // field of view of 45 degrees, near and far planes 1.0 and 1000
    //that znear and zfar should typically have a ratio of 1000:1 to make sorting out z depth easier for the GPU
    gluPerspective(65.0f, fAspect, 1.0, 10000.0); //may need to make larger depending on project
    // Modelview matrix reset
    glMatrixMode(GL_MODELVIEW);
    //pull the eye position back to view the scene
    gluLookAt(0.00,0.00,500.0,//eye
              0.00,0.00,0.00,//centre
              0.00,1.00,0.00);//up
}
// keys that adjust the translation and rotation global values
void processsArrowKeys(int key, int x, int y)
{// changes the rotation of all objects about the z-axis
	if (key == GLUT_KEY_LEFT)
	{
		spaceRotate += 1.0;
	}
	glutPostRedisplay();
	if (key == GLUT_KEY_RIGHT)
	{
		spaceRotate -= 1.0;
	}
	glutPostRedisplay();
	// changings translation of global value and position value of the HUD display
	if (key == GLUT_KEY_UP)
	{
		move_y -= 2.0f;
		y_coord += 2.0f;
	}

	if (key == GLUT_KEY_DOWN)
	{
		move_y += 2.0f;
		y_coord -= 2.0f;
	}

}
// keys that are not in GLUT specific
void processKeys(unsigned char key, int x, int y)
{// changings translation of global value and position value of the HUD display
	switch (key) {
	case 'w':
		move_z += 2.0f;
		z_coord += 2.0f;
		break;
	case 's':
		move_z -= 2.0f;
		z_coord -= 2.0f;
		break;
	case 'a':
		move_x += 2.0f;
		x_coord += 2.0f;
		break;
	case 'd':
		move_x -= 2.0f;
		x_coord -= 2.0f;
		break;
		// actives bool to display the HUD
	case 'h':
		if (hudActive == true) { hudActive = false; }
		else hudActive = true;
		break;
	}
}
// main method to run the program
int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
	glutCreateWindow("3D Space Game");
	glutSpecialFunc(processsArrowKeys);
	glutKeyboardFunc(processKeys);
	glutReshapeFunc(ChangeSize);
	glutDisplayFunc(RenderScene);
    glutTimerFunc(25, TimerFunc, 1);
	SetupRC();
	glutMainLoop();
	return 0;
}






