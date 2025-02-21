/******************************************
*
* Official Name:  Renan Cruz
*
* Nickname:  Pablo
*
* E-mail:  rrodr100@syr.edu
*
* Assignment:  Homework 4
*
* Environment/Compiler:  Visual Studio Pro 2013
*
* Date:  November 5, 2014
*
*******************************************/

#include <cstdlib>
#include <cmath>
#include <iostream>
#include <time.h>
#include "utils.h"

#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

// constants
#define PI 3.14159265358979324
#define LEFT 0
#define RIGHT 1

using namespace std;

// Globals
static float scrW;
static float scrH;
static float camX = 150, camY = 30, camZ = 330, camR = 100;
static float camX0 = camX, camY0 = camY, camZ0 = camZ;
static double phi = 0;
static double theta = 0;
static float light0_x = 180, light0_y = 120, light0_z = 20;
	// object measures	
static int switchz0 = 165;
static int switchz1 = 170;
static int switchy0 = 25;
static int switchy1 = 30;
	// Light
static bool light0On = false; // White light on?
static float t = 0.0005; // attenuation factor
static float ambLight = 0.1;
static float cutoffAng = 10;
	// Material property vectors.
static float matAmbAndDif[] = { 0.0, 0.0, 1.0, 1.0 };
static float matSpec[] = { 1.0, 1.0, 1, 0, 1.0 };
static float matShine[] = { 0.0 };
	// picking and selecting
static bool isSelecting = false; // In selection mode?
static int hits; // Number of entries in hit buffer.
static unsigned int buffer[1024]; // Hit buffer.
static unsigned int closestName = 0; // Name of closest hit.
	// animations
static int frameRate = 100; //control speed of animations
static bool gotKey = false;
static bool keyAnimationEnded = false;
static double keyX = 100, keyY = 2.5, keyZ = 120;
static double keyX0 = 100, keyY0 = 2.5, keyZ0 = 120;
static bool movFrontDoor = false;
static int frontDoorAngle = 0;
static int backDoorAngle = 0;
static bool gotFlashlight = false;
static bool chooseSideToTurnKey = false;
static bool gotAnswer = false;
static bool clickedOnPumpkin = false;
static int correctSideToTurnKey;
static int finalCountdown = 0;
	//writing
static long font = (long)GLUT_BITMAP_8_BY_13; // Font selection.
static double doorknobPos[] = {135, 35, 0};
	//special
static bool developerMode = false;

void drawWalls(){
	if (isSelecting) glLoadName(1);
	// south wall
		// draw wall
	glNormal3f(0, 0, -1);
	matAmbAndDif[0] = 1.0; matAmbAndDif[1] = 1.0; matAmbAndDif[2] = 0.0; matAmbAndDif[3] = 1.0; //yellow
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, matAmbAndDif);
	glBegin(GL_QUADS);
	for (int i = 0; i < 200; i++){
		for (int j = 0; j < 200; j++){
			if (i < 130 || i >= 160 || j>80){
				glVertex3f(i, j, 200);
				glVertex3f(i + 1, j, 200);
				glVertex3f(i + 1, j + 1, 200);
				glVertex3f(i, j + 1, 200);
			}
		}
	}
	glEnd();
	// north wall
	glNormal3f(0, 0, 1);
	matAmbAndDif[0] = 0.0; matAmbAndDif[1] = 0.0; matAmbAndDif[2] = 1.0; matAmbAndDif[3] = 1.0; //blue
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, matAmbAndDif);
	//matShine[0] = 20.0;
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 10);
	glBegin(GL_QUADS);
	for (int i = 0; i < 200; i++){
		for (int j = 0; j < 200; j++){
			if (i < 130 || i >= 160 || j>80){
				glVertex3f(i, j, 0);
				glVertex3f(i + 1, j, 0);
				glVertex3f(i + 1, j + 1, 0);
				glVertex3f(i, j + 1, 0);
			}
		}
	}
	glEnd();
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 128);
	//matShine[0] = 0.0;
	//glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, matShine);
	// east wall
	glBegin(GL_QUADS);
	glNormal3f(-1, 0, 0);
	matAmbAndDif[0] = 0.0; matAmbAndDif[1] = 1.0; matAmbAndDif[2] = 0.0; matAmbAndDif[3] = 1.0; //green
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, matAmbAndDif);
	for (int i = 0; i < 200; i++){
		for (int j = 0; j < 200; j++){
			glVertex3f(200, j, i);
			glVertex3f(200, j, i+1);
			glVertex3f(200, j+1, i+1);
			glVertex3f(200, j+1, i);
		}
	}
	glEnd();
	// west wall
	glBegin(GL_QUADS);
	glNormal3f(1, 0, 0);
	matAmbAndDif[0] = 0.0; matAmbAndDif[1] = 1.0; matAmbAndDif[2] = 0.0; matAmbAndDif[3] = 1.0; //green
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, matAmbAndDif);
	for (int i = 0; i < 200; i++){
		for (int j = 0; j < 200; j++){
			glVertex3f(0, j, i);
			glVertex3f(0, j, i + 1);
			glVertex3f(0, j + 1, i + 1);
			glVertex3f(0, j + 1, i);
		}
	}
	glEnd();
}

void drawFrontDoor(){
	if (isSelecting) glLoadName(2);
	if (closestName == 2) movFrontDoor = true;
	glPushMatrix();
	if (movFrontDoor){ glTranslatef(160, 0, 200); glRotatef(-frontDoorAngle, 0, 1, 0); glTranslatef(-160, 0, -200); }
	matAmbAndDif[0] = 1.0; matAmbAndDif[1] = 1.0; matAmbAndDif[2] = 1.0; matAmbAndDif[3] = 1.0; //yellow
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, matAmbAndDif);
	glBegin(GL_QUADS);
	for (int i = 0; i < 200; i++){
		for (int j = 0; j < 200; j++){
			if (i >= 130 && i < 160 && j <= 80){
				glVertex3f(i, j, 200);
				glVertex3f(i + 1, j, 200);
				glVertex3f(i + 1, j + 1, 200);
				glVertex3f(i, j + 1, 200);
			}
		}
	}
	glEnd();
	glPopMatrix();
}

void drawBackDoor(){
	if (isSelecting) glLoadName(7);
	glPushMatrix();
	if (gotAnswer){ glTranslatef(160, 0, 0); glRotatef(backDoorAngle, 0, 1, 0); glTranslatef(-160, 0, -0); }
	matAmbAndDif[0] = 1.0; matAmbAndDif[1] = 1.0; matAmbAndDif[2] = 1.0; matAmbAndDif[3] = 1.0; //white
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, matAmbAndDif);
	glBegin(GL_QUADS); //door
	for (int i = 0; i < 200; i++){
		for (int j = 0; j < 200; j++){
			if (i >= 130 && i < 160 && j <= 80){
				glVertex3f(i, j, 0);
				glVertex3f(i + 1, j, 0);
				glVertex3f(i + 1, j + 1, 0);
				glVertex3f(i, j + 1, 0);
			}
		}
	}
	glEnd();
	// doorknob
	matAmbAndDif[0] = 0.0; matAmbAndDif[1] = 0.0; matAmbAndDif[2] = 0.0; matAmbAndDif[3] = 1.0; //white
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, matAmbAndDif);
	glTranslatef(doorknobPos[0], doorknobPos[1], doorknobPos[2]);
	glutSolidSphere(2, 8, 8); 
	glPopMatrix();
}

void drawKey(){
	if (isSelecting) glLoadName(4);
	if (!gotFlashlight) glColorMask(false, false, false, false);
	else { matAmbAndDif[0] = 1.0; matAmbAndDif[1] = 1.0; matAmbAndDif[2] = 0.0; matAmbAndDif[3] = 1.0;} //yellow
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, matAmbAndDif);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 10);
	glPushMatrix();
	if (gotAnswer){ glTranslatef(160, 0, 0); glRotatef(backDoorAngle, 0, 1, 0); glTranslatef(-160, 0, -0); }
	glTranslatef(keyX, keyY, keyZ);
	glRotatef(180, 0, 1, 0);
	glutSolidSphere(1, 8, 8);
	glutSolidCone(1, 3, 8, 8);
	if (!gotFlashlight) glColorMask(true, true, true, true);
	glPopMatrix();
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 128);
}

void drawTable(){
	if (isSelecting) glLoadName(8);
	matAmbAndDif[0] = 0.4; matAmbAndDif[1] = 0.4; matAmbAndDif[2] = 0.5; matAmbAndDif[3] = 1.0; // grey
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, matAmbAndDif);
	for (int i = 70; i < 120; i++){
		for (int j = 90; j < 110; j++){
			glVertex3f(i, 10, j);
			glVertex3f(i+1, 10, j);
			glVertex3f(i+1, 10, j + 1);
			glVertex3f(i, 10, j+1);
		}
	}
	glEnd();
}

void drawFlashlight(){
	if (isSelecting) glLoadName(6);
	if (closestName == 6) gotFlashlight = true;
	if (!gotFlashlight){
		matAmbAndDif[0] = 1.0; matAmbAndDif[1] = 0.0; matAmbAndDif[2] = 0.0; matAmbAndDif[3] = 1.0; // red
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, matAmbAndDif);
		glColor3f(1.0, .0, .0);
		glPushMatrix();//tube
		GLUquadric* qobj = gluNewQuadric();
		gluQuadricNormals(qobj, GLU_SMOOTH);
		glTranslatef(110,10+1, 100);
		gluCylinder(qobj, 1.0, 1.0, 6, 8, 8);
		gluDeleteQuadric(qobj);
		glPopMatrix();
		glPushMatrix();//"head"
		glTranslatef(110, 10 + 1, 100-2);
		glutSolidCone(2, 4, 8, 8);
		glPopMatrix();
	}
}

void drawPumpkin(){
	if (isSelecting) glLoadName(3);
	if (closestName == 3) clickedOnPumpkin = true;
	if (clickedOnPumpkin){
		matAmbAndDif[0] = 1.0; matAmbAndDif[1] = 0.4; matAmbAndDif[2] = 0.0; matAmbAndDif[3] = 1.0; // orange
		float matEmission[] = {1.0, 0.4, 0.0, 1.0 };
		glMaterialfv(GL_FRONT, GL_EMISSION, matEmission);
	}
	else{
		matAmbAndDif[0] = 0.6; matAmbAndDif[1] = 0.2; matAmbAndDif[2] = 0.0; matAmbAndDif[3] = 1.0; // brown
	}
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, matAmbAndDif);
	glPushMatrix();
	glTranslatef(80, 14, 100);
	glutSolidSphere(4, 8, 8);
	glPopMatrix();
	float matEmission[] = { 0.0, 0.0, 0.0, 1.0 };
	glMaterialfv(GL_FRONT, GL_EMISSION, matEmission);
}

void drawBulb(){
	if (isSelecting) glLoadName(9);
	glColor3f(1.0, 1.0, 1.0);
	glPushMatrix();
	glTranslatef(light0_x, light0_y, light0_z);
	glutWireSphere(4, 8, 8);
	glPopMatrix();
}

void drawSwitch(){
	if (isSelecting) glLoadName(5);
	if (closestName == 5) { light0On = !light0On; closestName = 0; }
	glBegin(GL_QUADS);
	glNormal3f(1, 0, 0);
	matAmbAndDif[0] = 0.0; matAmbAndDif[1] = 0.0; matAmbAndDif[2] = 1.0; matAmbAndDif[3] = 1.0; //cyan
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, matAmbAndDif);
	for (int i = switchz0; i < switchz1; i++){
		for (int j = switchy0; j < switchy1; j++){
			glVertex3f(1, j, i);
			glVertex3f(1, j, i + 1);
			glVertex3f(1, j + 1, i + 1);
			glVertex3f(1, j + 1, i);
		}
	}
	glEnd();
}

void drawWindow(){
	if (isSelecting) glLoadName(10);
	glNormal3f(1, 0, 0);
	glPushMatrix();
	glTranslatef(1, 0, 0);
	glRotatef(-90, 0, 1, 0);
	glColor3f(0, 0, 0); //backgroung
	glRectf(120, 12, 80, 42);
	glColor3f(1.0, 1.0, 1.0); //bright side of moon
	glBegin(GL_POLYGON);
	for (int i = 0, R= 5 ; i < 20; i++){
		glVertex3f(110 + R*cos(2 * PI*i / 20), 30 + R*sin(2 * PI*i / 20), -1);
	}
	glEnd();
	glColor3f(0, 0, 0); //dark side of moon
	glBegin(GL_POLYGON);
	for (int i = 0, R = 5; i < 20; i++){
		glVertex3f(109 + R*cos(2 * PI*i / 20), 29 + R*sin(2 * PI*i / 20), -2);
	}
	glEnd();
	glColor3f(1.0, 1.0, 1.0); // stars
	glTranslatef(0, 0, -3);
	glRectf(90, 30, 89, 31);
	glRectf(92, 24, 91, 25);
	glRectf(95, 27, 94, 28);
	glRectf(100, 29, 99, 30);
	glRectf(105, 28, 104, 29);
	glPopMatrix();
	glColor3f(1.0, 0.0, 1.0);
}

void drawScenario(){
	if (gotKey && !chooseSideToTurnKey){
		if (!keyAnimationEnded) gluLookAt(camX, camY, camZ, keyX, keyY, keyZ, 0, 1, 0);
		else gluLookAt(keyX, keyY, camR - keyZ, keyX, keyY, keyZ, 0, 1, 0);
	}		
	else
		gluLookAt(camX, camY, camZ, camX + camR*cos(degToRad(phi))*sin(degToRad(theta)), camY + camR*sin(degToRad(phi)), camZ - camR*cos(degToRad(phi))*cos(degToRad(theta)), 0, 1, 0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Turn lights on again to draw the other objects
	glEnable(GL_LIGHTING);
	drawWalls();
	drawFrontDoor();
	drawBackDoor();
	drawTable();
	drawFlashlight();
	drawPumpkin();
	drawKey();
	drawSwitch();
	drawFlashlight();

	// Turn lights off to draw lamp
	glDisable(GL_LIGHTING);
	drawBulb();
	drawWindow();

	glEnable(GL_LIGHTING);

	if (isSelecting) glPopName(); // Clear name stack.
}

// Routine to draw a bitmap character string.
void writeBitmapString(void *font, char *string)
{
	char *c;

	for (c = string; *c != '\0'; c++) glutBitmapCharacter(font, *c);
}

// Routine to convert floating point to char string.
void floatToString(char * destStr, int precision, float val)
{
	sprintf(destStr, "%f", val);
	destStr[precision] = '\0';
}

void writeText(){
	glDisable(GL_LIGHTING); // Disable lighting.
	glColor3f(1.0, 1.0, 1.0);

	
	if (!chooseSideToTurnKey){
		glRasterPos3f(keyX, keyY - 20, keyZ);
		writeBitmapString((void*)font, "Press c to turn right or C to left");
	}
	else if (!gotAnswer){
		glRasterPos3f(camX + camR/2*cos(degToRad(phi))*sin(degToRad(theta)), camY + camR/2*sin(degToRad(phi)), camZ - camR/2*cos(degToRad(phi))*cos(degToRad(theta)));
		writeBitmapString((void*)font, "Oh! Wait!!!");
	}
}

void drawScene()
{
	//Lighting

		// Light0 is the light bulb
	float lightAmb0[] = { 0.0, 0.0, 0.0, 1.0 };
	float lightDifAndSpec0[] = { 1.0, 1.0, 1.0, 1.0 };
	float lightPos0[] = { light0_x, light0_y, light0_z, 1.0 };
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmb0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDifAndSpec0);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightDifAndSpec0);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, t);

	// light1 is the flashlight
	float lightAmb1[] = { 1.0, 1.0, 1.0, 1.0 };
	float lightDifAndSpec1[] = { 1.0, 1.0, 1.0, 1.0 };
	float lightPos1[] = { camX, camY, camZ, 1.0 };
	float lightDir1[3];
	if (gotKey){
		lightDir1[0] = keyX - camX;
		lightDir1[1] = keyY - camY;
		lightDir1[2] = keyZ - camZ;
	}
	else{
		lightDir1[0] = cos(degToRad(phi))*sin(degToRad(theta)); 
		lightDir1[1] = sin(degToRad(phi));
		lightDir1[2] = -cos(degToRad(phi))*cos(degToRad(theta));
	}
	glLightfv(GL_LIGHT1, GL_AMBIENT, lightAmb1);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, lightDifAndSpec1);
	glLightfv(GL_LIGHT1, GL_SPECULAR, lightDifAndSpec1);
	glLightfv(GL_LIGHT1, GL_POSITION, lightPos1);
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, lightDir1);
	glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, cutoffAng);
	glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, t);

	// global ambient light
	float globAmb[] = { ambLight, ambLight, ambLight, 1.0 };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globAmb); // Global ambient light.

		// Turn lights off/on.
	if (light0On) glEnable(GL_LIGHT0); else glDisable(GL_LIGHT0);
	if (gotFlashlight) glEnable(GL_LIGHT1); else glDisable(GL_LIGHT1);


	// Draws
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor3f(0.0, 0.0, 0.0);
	glLoadIdentity();

	// ey scenario in rendering mode.
	isSelecting = false;
	drawScenario();
	if (keyAnimationEnded) writeText();

	//if (keyAnimationEnded){
	//	if (!chooseSideToTurnKey) writeText();
	//	else if (!gotAnswer); //spin around
	//	else; // open door and finish it
	//}

	glutSwapBuffers();
}

// Process hit buffer to find record with smallest min-z value.
void findClosestHit(int hits, unsigned int buffer[])
{
	unsigned int *ptr, minZ;

	minZ = 0xffffffff; // 2^32 - 1
	ptr = buffer;
	closestName = 0;
	for (int i = 0; i < hits; i++)
	{
		ptr++;
		if (*ptr < minZ)
		{
			minZ = *ptr;
			ptr += 2;
			closestName = *ptr;
			ptr++;
		}
		else ptr += 3;
	}
}

// The mouse callback routine.
void pickFunction(int button, int state, int x, int y)
{
	int viewport[4]; // Viewport data.

	if (button != GLUT_LEFT_BUTTON || state != GLUT_DOWN) return; // Don't react unless left button is pressed.

	glGetIntegerv(GL_VIEWPORT, viewport); // Get viewport data.

	glSelectBuffer(1024, buffer); // Specify buffer to write hit records in selection mode
	(void)glRenderMode(GL_SELECT); // Enter selection mode.

	// Save the viewing volume defined in the resize routine.
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();

	// Define a viewing volume corresponding to selecting in 3 x 3 region around the cursor.
	glLoadIdentity();
	gluPickMatrix((float)x, (float)(viewport[3] - y), 3.0, 3.0, viewport);
	gluPerspective(45.0f, (double)scrW / scrH, 1.0, 1000.0);

	glMatrixMode(GL_MODELVIEW); // Return to modelview mode before drawing.
	glLoadIdentity();

	glInitNames(); // Initializes the name stack to empty.
	glPushName(0); // Puts name 0 on top of stack.

	// Determine hits by calling drawBallAndTorus() so that names are assigned.
	isSelecting = true;
	drawScenario();

	hits = glRenderMode(GL_RENDER); // Return to rendering mode, returning number of hits.

	// Restore viewing volume of the resize routine and return to modelview mode.
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	// Determine closest of the hit objects (if any).
	findClosestHit(hits, buffer);

	glutPostRedisplay();
}

bool findKey(){
	float sightDir[] = { cos(degToRad(phi))*sin(degToRad(theta)), sin(degToRad(phi)), -cos(degToRad(phi))*cos(degToRad(theta))};
	float keyDir[] = { keyX - camX, keyY - camY, keyZ - camZ };
	return cosDiffAng(sightDir, keyDir, 3) > cos(degToRad(cutoffAng));
}

void setSideToTurnKey(){
	srand(time(NULL));
	correctSideToTurnKey = rand() % 2;
}

void resize(int w, int h)
{
	scrW = w;
	scrH = h;
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, (double)w / h, 1.0, 1000.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void setup()
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);

	// Turn on OpenGL lighting.
	glEnable(GL_LIGHTING);

	// Material properties
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, matAmbAndDif);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matSpec);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, matShine);
}

void keyInput(unsigned char key, int scrX, int scrY)
{
	switch (key)
	{
		case 27:
			exit(0);
			break;
		case 'u':
			incAng(phi);
			glutPostRedisplay();
			break;
		case 'v':
			decAng(phi);
			glutPostRedisplay();
			break;
		case 'a':
			if(ambLight > 0) ambLight -= 0.05;
			glutPostRedisplay();
			break;
		case 'A':
			if (ambLight < 1) ambLight += 0.05;
			glutPostRedisplay();
			break;
		case 'c':
			chooseSideToTurnKey = true;
			if (correctSideToTurnKey == 0) gotAnswer = true;
			glutPostRedisplay();
			break;
		case 'C':
			chooseSideToTurnKey = true;
			if (correctSideToTurnKey == 1) gotAnswer = true;
			glutPostRedisplay();
			break;
		case '*':
			developerMode = !developerMode;
			cout << "correctSideToTurnKey: " << correctSideToTurnKey << endl;
			break;
		case 'D':
			movFrontDoor = !movFrontDoor;
			glutPostRedisplay();
			break;
		case 'L':
			light0On = !light0On;
			glutPostRedisplay();
			break;
		case 'P':
			if (developerMode){
				clickedOnPumpkin = !clickedOnPumpkin;
				glutPostRedisplay();
			}
			break;
		case 'F':
			if (developerMode){
				gotFlashlight = !gotFlashlight;
				glutPostRedisplay();
			}
			break;
		case 'K':
			if (developerMode){
				bool oldValue = gotKey;
				gotKey = !gotKey;
				gotFlashlight = gotKey;
				if (oldValue){
					keyX = keyX0;
					keyY = keyY0;
					keyZ = keyZ0;
				}
				glutPostRedisplay();
			}
			break;
		default:
			break;
	}
}

// Callback routine for non-ASCII key entry.
void specialKeyInput(int key, int x, int y)
{
	int modifier = glutGetModifiers();
	switch (key)
	{
		case GLUT_KEY_LEFT:
			if (!developerMode) decAng(theta);
			else if (modifier == GLUT_ACTIVE_ALT){
				chooseSideToTurnKey = true;
				if (correctSideToTurnKey == LEFT) gotAnswer = true;
			}	
			glutPostRedisplay();
			break;
		case GLUT_KEY_RIGHT:
			if (!developerMode) incAng(theta);
			else if (modifier == GLUT_ACTIVE_ALT){
				chooseSideToTurnKey = true;
				if (correctSideToTurnKey == RIGHT) gotAnswer = true;
			}
			glutPostRedisplay();
			break;
		case GLUT_KEY_UP:
			if (modifier == GLUT_ACTIVE_SHIFT) camY += 2;
			else{
				camX += 2 * cos(degToRad(phi)) * sin(degToRad(theta));
				camY += 2 * sin(degToRad(phi));
				camZ += 2 * -cos(degToRad(phi))*cos(degToRad(theta));
			}
			glutPostRedisplay();
			break;
		case GLUT_KEY_DOWN:
			if (modifier == GLUT_ACTIVE_SHIFT) camY -= 2;
			else{
				camX -= 2 * cos(degToRad(phi)) * sin(degToRad(theta));
				camY -= 2 * sin(degToRad(phi));
				camZ -= 2 * -cos(degToRad(phi))*cos(degToRad(theta));
			}
			glutPostRedisplay();
			break;
		default:
			break;
	}
}

void animate(int value)
{
	if (gotKey){
		if (!keyAnimationEnded){ // suppose that the key is on front and below the doorknob; to fix, make the delta global
			if (keyY < doorknobPos[1]){
				keyY += 1;
				camY += 1;
			}
			else if (keyX < doorknobPos[0]){
				keyX += 1;
				camX += 1;
			}
			else if (keyZ > doorknobPos[2]+3){
				keyZ -= 2;
				camZ -= 2;
			}
			else keyAnimationEnded = true;
		}
	}
	else{
		if(gotFlashlight) gotKey = findKey();
	}
	if (movFrontDoor){
		if (frontDoorAngle < 175) frontDoorAngle += 5;
	}
	if (chooseSideToTurnKey){
		if (gotAnswer){
			if (backDoorAngle < 175) backDoorAngle += 5;
			else exit(0);
		}
		else{
			if (finalCountdown < 150){
				int coin = rand() % 2;
				if (coin == 0) theta += rand() % 10;
				if (coin == 1) phi += rand() % 10;
				finalCountdown += 1;
			}
			else exit(0);
		}
	}
	
	glutTimerFunc(frameRate, animate, 1);
	glutPostRedisplay();
}

void printInteraction()
{
	cout << "Interactions:" << endl << endl;
	cout << "Press u to look up" << endl;
	cout << "Press v to look down" << endl;
	cout << "Press a to decrease ambient light" << endl;
	cout << "Press A to increase ambient light" << endl;
	cout << "Press c to turn key left" << endl;
	cout << "Press C to turn key right" << endl;
	cout << "Press * to toggle developer mode" << endl;
	cout << "Press left arrow to turn left" << endl;
	cout << "Press right arrow to turn right" << endl;
	cout << "Press up to move forward" << endl;
	cout << "Press down to move backwards" << endl;
	cout << "Press shift+up to get higher" << endl;
	cout << "Press shift+down to get smaller" << endl << endl;
	cout << "Only in Developer Mode" << endl << endl;
	cout << "Press D to open front door" << endl;
	cout << "Press L to toggle on and off the bare bulb light" << endl;
	cout << "Press P to toggle on and off the pumpkin" << endl;
	cout << "Press F to get and turn on/ tun off and put on table" << endl;
	cout << "Press K to rise the key and see going to the door" << endl;
	cout << "Press alt+left to turn key to the left side" << endl;
	cout << "Press alt+right to turn key to the right side" << endl << endl;


}

// Main routine.
int main(int argc, char **argv)
{
	printInteraction();
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(800, 600);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Halloween House");
	setup();
	glutDisplayFunc(drawScene);
	glutReshapeFunc(resize);
	glutKeyboardFunc(keyInput);
	glutMouseFunc(pickFunction);
	glutSpecialFunc(specialKeyInput);
	setSideToTurnKey();
	glutTimerFunc(5, animate, 1); // wait 5 miliseconds and execute this fuction for the first time
	glutMainLoop();

	return 0;
}
