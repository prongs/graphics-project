// GraphicsProject.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "utils.h"
#include "Model.h"
#include <lib3ds.h>
#include "shaderutils.h"
#include <omp.h>
using namespace std;

#ifdef _WIN32
#include "windows.h"
#endif

#include <stdio.h>


// Expressed as float so gluPerspective division returns a float and not 0 (640/480 != 640.0/480.0).

//Light mouvement circle radius
float light_mvnt = 30.0f;

double draw_time;
Model *car, *track;
void loadModels()
{
	//car=new Model("models/audi/audir8.3ds");
	//track=new Model("models/track/track_simple.3ds");
}


// This update only change the position of the light.
//int elapsedTimeCounter = 0;
bool whether_update=true;
void update(void)
{

	p_light[0] = light_mvnt * cos(glutGet(GLUT_ELAPSED_TIME)/1000.0);
	p_light[2] = light_mvnt * sin(glutGet(GLUT_ELAPSED_TIME)/1000.0);

	glLightfv(GL_LIGHT0, GL_POSITION, p_light);

	//p_light[0] = light_mvnt * cos(3652/1000.0);
	//p_light[2] = light_mvnt * sin(3652/1000.0);
}

void drawObjects(void)
{

	glColor4f(0.9f,0.9f,0.9f,1);

	startTranslate(0,4,-16);
	glutSolidCube(4);
	endTransformation();
	(modelMatrix, viewMatrix, projectionMatrix);
	float a[]={0.2, 0.2, 0.2, 1.0};
	float d[]={0.8, 0.1, 0.8, 1.0};
	float s[]={0.5, 0.5, 0.5, 1.0};
	glMaterialfv(GL_FRONT, GL_AMBIENT, a);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, d);
	glMaterialfv(GL_FRONT, GL_SPECULAR, s);
	glMaterialf(GL_FRONT, GL_SHININESS, 4.0);
	startTranslate(10,4,-5);
  glutSolidSphere(2, 128, 128);
	endTransformation();

	glColor4f(0.3f,0.3f,0.3f,1);
	glBegin(GL_QUADS);
	glVertex3f(-35,2,-35);
	glVertex3f(-35,2, 15);
	glVertex3f( 15,2, 15);
	glVertex3f( 15,2,-35);
	glEnd();

}

void drawCar(void)
{
	startTranslate(-330,-100,-100); 
	car->display();
	endTransformation();

	//startTranslate(-360,-96,-100); 
	//glutSolidCube(8);
	//endTransformation();
	//glColor4f(0.3f,0.3f,0.8f,1);
	glBegin(GL_QUADS);
	glVertex3f(-375,-100,-265);
	glVertex3f(-375,-100, 15);
	glVertex3f(-225,-100, 15);
	glVertex3f(-225,-100,-265);
	glEnd();
}

void renderScene(void) 
{

	if(whether_update) update();

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,fboId);
	glUseProgramObjectARB(0);

	glViewport(0,0,RENDER_WIDTH * SHADOW_MAP_RATIO,RENDER_HEIGHT* SHADOW_MAP_RATIO);

	glClear( GL_DEPTH_BUFFER_BIT);
	glClearColor(1.0, 1.0, 1.0, 1.0);

	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE); 

	setupMatrices(p_light[0],p_light[1],p_light[2],l_light[0],l_light[1],l_light[2], true);

	glCullFace(GL_FRONT);
	drawObjects();
	//drawCar();

	setTextureMatrix();

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,0);

	glViewport(0,0,RENDER_WIDTH,RENDER_HEIGHT);

	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE); 

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgramObjectARB(shadowShaderId);

	previousProjectionMatrix = glm::mat4(projectionMatrix);
	previousViewMatrix = glm::mat4(viewMatrix);


	glUniform1iARB(shadowMapUniform,7);
  glUniform1fARB(shadowMapStepXUniform,1.0/ (RENDER_WIDTH * SHADOW_MAP_RATIO));
	glUniform1fARB(shadowMapStepYUniform,1.0/ (RENDER_HEIGHT * SHADOW_MAP_RATIO));

	glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]); // Send our projection matrix to the shader
	glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]); // Send our view matrix to the shader
	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]); // Send our model matrix to the shader
	
	glUniformMatrix4fv(previousProjectionMatrixLocation, 1, GL_FALSE, &previousProjectionMatrix[0][0]); // Send our projection matrix to the shader
	glUniformMatrix4fv(previousViewMatrixLocation, 1, GL_FALSE, &previousViewMatrix[0][0]); // Send our view matrix to the shader

	glActiveTextureARB(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D,depthTextureId);

	setupMatrices(p_camera[0],p_camera[1],p_camera[2],l_camera[0],l_camera[1],l_camera[2], false);

	glCullFace(GL_BACK);
	drawObjects();
	//drawCar();

	// DEBUG only. this piece of code draw the depth buffer onscreen

	//glUseProgramObjectARB(0);
	//glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();
	//glOrtho(-RENDER_WIDTH/2,RENDER_WIDTH/2,-RENDER_HEIGHT/2,RENDER_HEIGHT/2,1,20);
	//glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();
	//glColor4f(1,1,1,1);
	//glActiveTextureARB(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D,depthTextureId);
	//glEnable(GL_TEXTURE_2D);
	//glTranslated(0,0,-1);
	//glBegin(GL_QUADS);
	//glTexCoord2d(0,0);glVertex3f(0,0,0);
	//glTexCoord2d(1,0);glVertex3f(RENDER_WIDTH/2,0,0);
	//glTexCoord2d(1,1);glVertex3f(RENDER_WIDTH/2,RENDER_HEIGHT/2,0);
	//glTexCoord2d(0,1);glVertex3f(0,RENDER_HEIGHT/2,0);
	//
	//
	//glEnd();
	//glDisable(GL_TEXTURE_2D);
	//

	glutSwapBuffers();
  double draw_time2 = omp_get_wtime();
  double diff = draw_time2-draw_time;
  double fps = 1.0/diff;
  draw_time = draw_time2;
  cout<<fps<<endl;
}

void processNormalKeys(unsigned char key, int x, int y) {

	switch(key)
	{
	case 27:
		exit(0);
	case '\r':
	case '\n':
		if(whether_update)
			debug(p_light[0],p_light[1],p_light[2]);
		whether_update=!whether_update;
		break;
	}
}


int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(RENDER_WIDTH,RENDER_HEIGHT);
	glutCreateWindow("GLSL Shadow mapping");
	glewInit();
	generateShadowFBO();
	loadShadowShader();

	glEnable(GL_DEPTH_TEST);
	glClearColor(0,0,0,1.0f);

	glEnable(GL_CULL_FACE);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	float a[]={0.4, 0.4, 0.4, 1.0};
	float d[]={0.8, 0.8, 0.8, 1.0};
	float s[]={0.5, 0.5, 0.5, 1.0};
	glLightfv(GL_LIGHT0, GL_AMBIENT, a);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, d);
	glLightfv(GL_LIGHT0, GL_SPECULAR, s);
	glLightfv(GL_LIGHT0, GL_POSITION, p_light);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);

	glutDisplayFunc(renderScene);
	glutIdleFunc(renderScene);

	glutKeyboardFunc(processNormalKeys);
	loadModels();
  draw_time = omp_get_wtime();
	glutMainLoop();
}