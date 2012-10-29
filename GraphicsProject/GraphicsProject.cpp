// GraphicsProject.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "utils.h"
#include "shaderutils.h"
#include <lib3ds.h>
using namespace std;
#include "Model.h"

//Global Variables
Model* car;
float angle = 0.0;


//Use and Define as required. 
void displayFunc();
void reshapeFunc(int,int);
void keyboardFunc(unsigned char, int, int);
void mouseFunc(int,int,int,int);
void motionFunc(int,int);

void initObjects()
{
	car=new Model("models/audi/AudiR8.3DS");
}

void display1(int x) {
  glutPostRedisplay();
  glutTimerFunc(10, display1, 0);
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(1280, 720);
	glutCreateWindow(GAME_NAME);
	//glutFullScreen();
	glutDisplayFunc(displayFunc);
	glutIdleFunc(displayFunc);
	glutReshapeFunc(reshapeFunc);
	glutKeyboardFunc(keyboardFunc);
	glutMouseFunc(mouseFunc);
	glutMotionFunc(motionFunc);
	glEnable(GL_DEPTH_TEST);
	glewInit();
	glMatrixMode(GL_PROJECTION);
	gluLookAt(0.0, 0.0, 99.0, 0.0, 0.0, -1.0, 0.0, 1.0, 0.0);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHT2);
	initObjects();
  glutTimerFunc(100, display1, 0);
	glutMainLoop();
	return 0;
}

void reshapeFunc(int w, int h)
{
	if (h == 0) // Prevent divide by zero
		h = 1;

	float ratio = 1.0 * w / h;


	glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
	gluPerspective(45, ratio, 1.0f, 1000.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
  glutPostRedisplay();
}
void keyboardFunc(unsigned char key, int x, int y)
{
	switch(key)
	{
	case 27: //escape

		exit(0);
	}
}

void mouseFunc(int button, int state, int x, int y)
{
}
void motionFunc(int x, int y)
{
}
void displayFunc()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glutSolidSphere(5,16,16);
  glPushMatrix();
  glTranslatef(0.0, -60.0, -300.0);
  glRotatef(angle, 0.0, 1.0, 0.0);
  glRotatef(-10, 0.0, 0.0, 1.0);
	car->display();
  glPopMatrix();
	glutSwapBuffers();
  angle += 0.4;
  if (angle >= 360.0) angle = 0.0;
}
