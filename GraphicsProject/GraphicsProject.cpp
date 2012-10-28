// GraphicsProject.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "utils.h"
#include "shaderutils.h"
#include <lib3ds.h>
using namespace std;
#include "Car.h"

//Global Variables
Car* car;



//Use and Define as required. 
void displayFunc();
void reshapeFunc(int,int);
void keyboardFunc(unsigned char, int, int);
void mouseFunc(int,int,int,int);
void motionFunc(int,int);

void initObjects()
{
	car=new Car();
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
	glutMainLoop();
	return 0;
}

void reshapeFunc(int w, int h)
{
	if (h == 0) // Prevent divide by zero
		h = 1;

	float ratio = 1.0 * w / h;
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);


	glViewport(0, 0, w, h);

	gluPerspective(45, ratio, 1, 1000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0, 0.0, 3.0,
		0.0, 0.0, -1.0,
		0.0, 1.0, 0.0);
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
  glTranslatef(0.0, -50.0, -300.0);
  glRotatef(-90, 0.0, 1.0, 0.0);
	car->display();
  glPopMatrix();
	glutSwapBuffers();
}
