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
	gluLookAt(0.0, 0.0, 26.0,
		0.0, 0.0, -1.0,
		0.0, 1.0, 0.0);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glutMainLoop();
	return 0;
}

void reshapeFunc(int w, int h)
{
}
void keyboardFunc(unsigned char key, int x, int y)
{
}

void mouseFunc(int button, int state, int x, int y)
{
}
void motionFunc(int x, int y)
{
}
void displayFunc()
{
}
