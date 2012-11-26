// GraphicsProject.cpp : Defines the entry point for the console application.
//

#include "BitmapTexture.h"
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

GLuint skybox[6];
Image* skybox_images[6];

double draw_time;
Model *car, *track;
void loadModels()
{
	//car=new Model("models/audi/audir8.3ds");
	//track=new Model("models/track/track_simple.3ds");
}

void loadSkybox()
{
  skybox_images[0] = (Image*)malloc(sizeof(Image));
  skybox_images[1] = (Image*)malloc(sizeof(Image));
  skybox_images[2] = (Image*)malloc(sizeof(Image));
  skybox_images[3] = (Image*)malloc(sizeof(Image));
  skybox_images[4] = (Image*)malloc(sizeof(Image));
  skybox_images[5] = (Image*)malloc(sizeof(Image));

  if (ImageLoad("checkered_back.bmp", skybox_images[0]) != 1)
    exit(0);
  if (ImageLoad("checkered_back.bmp", skybox_images[1]) != 1)
    exit(0);
  if (ImageLoad("checkered_back.bmp", skybox_images[2]) != 1)
    exit(0);
  if (ImageLoad("checkered_back.bmp", skybox_images[3]) != 1)
    exit(0);
  if (ImageLoad("checkered_back.bmp", skybox_images[4]) != 1)
    exit(0);
  if (ImageLoad("checkered_back.bmp", skybox_images[5]) != 1)
    exit(0);

  glGenTextures(6, &skybox[0]);
  glBindTexture(GL_TEXTURE_2D, skybox[0]);

  //glBindTexture(GL_TEXTURE_CUBE_MAP_POSITIVE_X, skybox[0]);
  //glBindTexture(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, skybox[1]);
  //glBindTexture(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, skybox[2]);
  //glBindTexture(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, skybox[3]);
  //glBindTexture(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, skybox[4]);
  //glBindTexture(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, skybox[5]);

  //glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, 3, skybox_images[0]->sizeX, skybox_images[0]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, skybox_images[0]->data);
  //glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, 3, skybox_images[1]->sizeX, skybox_images[1]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, skybox_images[1]->data);
  //glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, 3, skybox_images[2]->sizeX, skybox_images[2]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, skybox_images[2]->data);
  //glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, 3, skybox_images[3]->sizeX, skybox_images[3]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, skybox_images[3]->data);
  //glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, 3, skybox_images[4]->sizeX, skybox_images[4]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, skybox_images[4]->data);
  //glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, 3, skybox_images[5]->sizeX, skybox_images[5]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, skybox_images[5]->data);

  glTexImage2D(GL_TEXTURE_2D, 0, 3, skybox_images[5]->sizeX, skybox_images[5]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, skybox_images[5]->data);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  //glEnable(GL_TEXTURE_CUBE_MAP);
}

void bindCubeTextures()
{
  glBindTexture(GL_TEXTURE_CUBE_MAP_POSITIVE_X, skybox[0]);
  glBindTexture(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, skybox[1]);
  glBindTexture(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, skybox[2]);
  glBindTexture(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, skybox[3]);
  glBindTexture(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, skybox[4]);
  glBindTexture(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, skybox[5]);
}

void EnableReflectionMap()
{
  glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
  glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
  glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);

  glEnable(GL_TEXTURE_GEN_S);
  glEnable(GL_TEXTURE_GEN_T);
  glEnable(GL_TEXTURE_GEN_R);
}

void DisableReflectionMap()
{
  glDisable(GL_TEXTURE_GEN_S);
  glDisable(GL_TEXTURE_GEN_T);
  glDisable(GL_TEXTURE_GEN_R);
}

// This update only change the position of the light.
//int elapsedTimeCounter = 0;
bool whether_update=true;
int first_frame = 02;
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

	startTranslate(0,6,-16);
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
	//setupMatrices(p_camera[0],p_camera[1],p_camera[2],l_camera[0],l_camera[1],l_camera[2], false);

  //bindCubeTextures();
  //glEnable(GL_TEXTURE_CUBE_MAP);
  //startTranslate(0, 6,-16);
  //EnableReflectionMap();
  //glutSolidCube(4);
  //endTransformation();
  //DisableReflectionMap();
  //glDisable(GL_TEXTURE_CUBE_MAP);

  //glMatrixMode(GL_PROJECTION);
  //glLoadIdentity();
  //gluPerspective(45.0, RENDER_WIDTH/RENDER_HEIGHT, 1.0, 1000.0);
  //glMatrixMode(GL_MODELVIEW);
  //glLoadIdentity();
  //gluLookAt(0.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 1.0, 0.0);
  //glEnable(GL_TEXTURE_2D);
  //glBindTexture(GL_TEXTURE_2D, skybox[0]);
  //glBegin(GL_QUADS);
  //glTexCoord2f(0.0, 0.0);
  //glVertex3f(10.0, 0.0, -56.0);
  //glTexCoord2f(1.0, 0.0);
  //glVertex3f(20.0, 0.0, -56.0);
  //glTexCoord2f(1.0, 1.0);
  //glVertex3f(20.0, 5.0, -56.0);
  //glTexCoord2f(0.0, 1.0);
  //glVertex3f(10.0, 5.0, -56.0);
  //glEnd();


	glUseProgramObjectARB(shadowShaderId);

	previousProjectionMatrix = glm::mat4(projectionMatrix);
	previousViewMatrix = glm::mat4(viewMatrix);


	glBindTexture(GL_TEXTURE_2D,textureId);
	glUniform1i(firstFrameBoolLoc, first_frame);
	glUniform1i(textureId, 0);
	glUniform1i(renderWidthLocation, RENDER_WIDTH);
	glUniform1i(renderHeightLocation, RENDER_HEIGHT);


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
	copyFrameBufferToTexture();
	glutSwapBuffers();
  double draw_time2 = omp_get_wtime();
  double diff = draw_time2-draw_time;
  double fps = 1.0/diff;
  draw_time = draw_time2;
  //cout<<fps<<endl;
  first_frame = first_frame?first_frame-1:first_frame;
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
	case 'w':
		p_camera[0]-=1.0;
		break;
	case 's':
		p_camera[0]+=1.0;
		break;
	case 'a':
		p_camera[2]-=1.0;
		break;
	case 'd':
		p_camera[2]+=1.0;
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
  glEnable(GL_TEXTURE_2D);

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
	
	motionBlurInit();

	glutDisplayFunc(renderScene);
	glutIdleFunc(renderScene);

	glutKeyboardFunc(processNormalKeys);
	loadModels();
  loadSkybox();
  draw_time = omp_get_wtime();
	glutMainLoop();
}