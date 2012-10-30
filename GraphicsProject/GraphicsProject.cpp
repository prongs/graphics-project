// GraphicsProject.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "utils.h"
//#include "Model.h"
#include <lib3ds.h>
#include "shaderutils.h"
using namespace std;

#ifdef _WIN32
#include "windows.h"
#endif

#include <stdio.h>


// Expressed as float so gluPerspective division returns a float and not 0 (640/480 != 640.0/480.0).

//Light mouvement circle radius
float light_mvnt = 30.0f;




// This update only change the position of the light.
//int elapsedTimeCounter = 0;
void update(void)
{
	
	p_light[0] = light_mvnt * cos(glutGet(GLUT_ELAPSED_TIME)/1000.0);
	p_light[2] = light_mvnt * sin(glutGet(GLUT_ELAPSED_TIME)/1000.0);
	
	//p_light[0] = light_mvnt * cos(3652/1000.0);
	//p_light[2] = light_mvnt * sin(3652/1000.0);
}


// During translation, we also have to maintain the GL_TEXTURE8, used in the shadow shader
// to determine if a vertex is in the shadow.

void drawObjects(void)
{
	// Ground
	
	glColor4f(0.9f,0.9f,0.9f,1);
	
	// Instead of calling glTranslatef, we need a custom function that also maintain the light matrix
	startTranslate(0,4,-16);
	glutSolidCube(4);
	endTranslate();
	
	startTranslate(0,4,-5);
	glutSolidCube(4);
	endTranslate();
	
	glColor4f(0.3f,0.3f,0.3f,1);
	glBegin(GL_QUADS);
	glVertex3f(-35,2,-35);
	glVertex3f(-35,2, 15);
	glVertex3f( 15,2, 15);
	glVertex3f( 15,2,-35);
	glEnd();
	
}

void renderScene(void) 
{
	//update();
	
	//First step: Render from the light POV to a FBO, story depth values only
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,fboId);	//Rendering offscreen
	//Using the fixed pipeline to render to the depthbuffer
	glUseProgramObjectARB(0);
	
	// In the case we render the shadowmap to a higher resolution, the viewport must be modified accordingly.
	glViewport(0,0,RENDER_WIDTH * SHADOW_MAP_RATIO,RENDER_HEIGHT* SHADOW_MAP_RATIO);
	
	// Clear previous frame values
	glClear( GL_DEPTH_BUFFER_BIT);
	
	//Disable color rendering, we only want to write to the Z-Buffer
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE); 
	
	setupMatrices(p_light[0],p_light[1],p_light[2],l_light[0],l_light[1],l_light[2]);
	
	// Culling switching, rendering only backface, this is done to avoid self-shadowing
	glCullFace(GL_FRONT);
	drawObjects();
	
	//Save modelview/projection matrice into texture7, also add a biais
	setTextureMatrix();
	
	
	// Now rendering from the camera POV, using the FBO to generate shadows
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,0);
	
	glViewport(0,0,RENDER_WIDTH,RENDER_HEIGHT);
	
	//Enabling color write (previously disabled for light POV z-buffer rendering)
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE); 
	
	// Clear previous frame values
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Using the shadow shader
	glUseProgramObjectARB(shadowShaderId);
	glUniform1iARB(shadowMapUniform,7);
	glActiveTextureARB(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D,depthTextureId);
	
	setupMatrices(p_camera[0],p_camera[1],p_camera[2],l_camera[0],l_camera[1],l_camera[2]);
	
	glCullFace(GL_BACK);
	drawObjects();
	
	// DEBUG only. this piece of code draw the depth buffer onscreen
	/*
	 glUseProgramObjectARB(0);
	 glMatrixMode(GL_PROJECTION);
	 glLoadIdentity();
	 glOrtho(-RENDER_WIDTH/2,RENDER_WIDTH/2,-RENDER_HEIGHT/2,RENDER_HEIGHT/2,1,20);
	 glMatrixMode(GL_MODELVIEW);
	 glLoadIdentity();
	 glColor4f(1,1,1,1);
	 glActiveTextureARB(GL_TEXTURE0);
	 glBindTexture(GL_TEXTURE_2D,depthTextureId);
	 glEnable(GL_TEXTURE_2D);
	 glTranslated(0,0,-1);
	 glBegin(GL_QUADS);
	 glTexCoord2d(0,0);glVertex3f(0,0,0);
	 glTexCoord2d(1,0);glVertex3f(RENDER_WIDTH/2,0,0);
	 glTexCoord2d(1,1);glVertex3f(RENDER_WIDTH/2,RENDER_HEIGHT/2,0);
	 glTexCoord2d(0,1);glVertex3f(0,RENDER_HEIGHT/2,0);
	 
	 
	 glEnd();
	 glDisable(GL_TEXTURE_2D);
	 */
	
	glutSwapBuffers();
}

void processNormalKeys(unsigned char key, int x, int y) {
	
	if (key == 27) 
		exit(0);
}


int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(RENDER_WIDTH,RENDER_HEIGHT);
	glutCreateWindow("GLSL Shadow mapping");
	glewInit();
	// This call will grab openGL extension function pointers.
	// This is not necessary for macosx and linux
	generateShadowFBO();
	loadShadowShader();
	
	// This is important, if not here, FBO's depthbuffer won't be populated.
	glEnable(GL_DEPTH_TEST);
	glClearColor(0,0,0,1.0f);
	
	glEnable(GL_CULL_FACE);
	
	glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
	
	
	
	
	glutDisplayFunc(renderScene);
	glutIdleFunc(renderScene);
	
	glutKeyboardFunc(processNormalKeys);
	
	glutMainLoop();
}