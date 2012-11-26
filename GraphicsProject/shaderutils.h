#ifndef SHADERUTILS_H
#define SHADERUTILS_H

#include "utils.h"
#include <stack>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>




#define RENDER_WIDTH 640.0
#define RENDER_HEIGHT 480.0
#define SHADOW_MAP_RATIO 2

using namespace std;

glm::mat4 projectionMatrix; // Store the projection matrix
glm::mat4 viewMatrix; // Store the view matrix
glm::mat4 modelMatrix; // Store the model matrix

glm::mat4 previousProjectionMatrix; // Store the projection matrix
glm::mat4 previousViewMatrix; // Store the view matrix

stack<glm::mat4> modelMatrixStack;



int projectionMatrixLocation;
int viewMatrixLocation;
int modelMatrixLocation;

int previousProjectionMatrixLocation;
int previousViewMatrixLocation;



int firstFrameBoolLoc;
int prevFrameLocation;
int renderWidthLocation;
int renderHeightLocation;

//Camera position
float p_camera[3] = {32,20,0};

//Camera lookAt
float l_camera[3] = {2,0,-10};

//Light position
float p_light[3] = {3,20,0};

//Light lookAt
float l_light[3] = {0,0,-1};

GLuint fboId;
GLuint blurFboId;
GLuint textureId;                   // ID of texture(motion blur)/
// Z values will be rendered to this texture when using fboId framebuffer
GLuint depthTextureId;
GLuint blurTextureId;
GLuint depthrenderbuffer;

// Use to activate/disable shadowShader
GLhandleARB shadowShaderId;
GLuint shadowMapUniform;
GLuint shadowMapStepXUniform;
GLuint shadowMapStepYUniform;


char*
  readShaderSource(const char* shaderFile)
{
  FILE *filePointer;
  char *content = NULL;

  int count=0;

  if (shaderFile != NULL) {
    fopen_s(&filePointer, shaderFile,"rt");

    if (filePointer != NULL) {

      fseek(filePointer, 0, SEEK_END);
      count = ftell(filePointer);
      rewind(filePointer);

      if (count > 0) {
        content = (char *)malloc(sizeof(char) * (count+1));
        count = fread(content,sizeof(char),count,filePointer);
        content[count] = '\0';
      }
      fclose(filePointer);
    }
  }
  return content;

}


// Create a GLSL program object from vertex and fragment shader files
GLuint
  InitShader(const char* vShaderFile, const char* fShaderFile, const char* outputAttributeName) {
    struct Shader {
      const char*  filename;
      GLenum       type;
      GLchar*      source;
    }  shaders[2] = {
      { vShaderFile, GL_VERTEX_SHADER, NULL },
      { fShaderFile, GL_FRAGMENT_SHADER, NULL }
    };

    GLuint program = glCreateProgram();

    for ( int i = 0; i < 2; ++i ) {
      Shader& s = shaders[i];
      s.source = readShaderSource( s.filename );
      if ( shaders[i].source == NULL ) {
        std::cerr << "Failed to read " << s.filename << std::endl;
        exit( EXIT_FAILURE );
      }
      GLuint shader = glCreateShader( s.type );
      glShaderSource( shader, 1, (const GLchar**) &s.source, NULL );
      glCompileShader( shader );

      GLint  compiled;
      glGetShaderiv( shader, GL_COMPILE_STATUS, &compiled );
      if ( !compiled ) {
        std::cerr << s.filename << " failed to compile:" << std::endl;
        GLint  logSize;
        glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &logSize );
        char* logMsg = new char[logSize];
        glGetShaderInfoLog( shader, logSize, NULL, logMsg );
        std::cerr << logMsg << std::endl;
        delete [] logMsg;

        exit( EXIT_FAILURE );
      }

      delete [] s.source;

      glAttachShader( program, shader );
    }

    /* Link output */
    glBindFragDataLocation(program, 0, outputAttributeName);

    /* link  and error check */
    glLinkProgram(program);

    GLint  linked;
    glGetProgramiv( program, GL_LINK_STATUS, &linked );
    if ( !linked ) {
      std::cerr << "Shader program failed to link" << std::endl;
      GLint  logSize;
      glGetProgramiv( program, GL_INFO_LOG_LENGTH, &logSize);
      char* logMsg = new char[logSize];
      glGetProgramInfoLog( program, logSize, NULL, logMsg );
      std::cerr << logMsg << std::endl;
      delete [] logMsg;

      exit( EXIT_FAILURE );
    }

    /* use program object */
    glUseProgram(program);

    return program;
}


// Loading shader function
GLhandleARB loadShader(char* filename, unsigned int type)
{
  FILE *pfile;
  GLhandleARB handle;
  const GLcharARB* files[1];

  // shader Compilation variable
  GLint result;				// Compilation code result
  GLint errorLoglength ;
  char* errorLogText;
  GLsizei actualErrorLogLength;

  char buffer[400000];
  memset(buffer,0,400000);

  // This will raise a warning on MS compiler
  pfile = fopen(filename, "rb");
  if(!pfile)
  {
    printf("Sorry, can't open file: '%s'.\n", filename);
    exit(0);
  }

  fread(buffer,sizeof(char),400000,pfile);
  //printf("%s\n",buffer);


  fclose(pfile);

  handle = glCreateShaderObjectARB(type);
  if (!handle)
  {
    //We have failed creating the vertex shader object.
    printf("Failed creating vertex shader object from file: %s.",filename);
    exit(0);
  }

  files[0] = (const GLcharARB*)buffer;
  glShaderSourceARB(
    handle, //The handle to our shader
    1, //The number of files.
    files, //An array of const char * data, which represents the source code of theshaders
    NULL);

  glCompileShaderARB(handle);

  //Compilation checking.
  glGetObjectParameterivARB(handle, GL_OBJECT_COMPILE_STATUS_ARB, &result);

  // If an error was detected.
  if (!result)
  {
    //We failed to compile.
    printf("Shader '%s' failed compilation.\n",filename);

    //Attempt to get the length of our error log.
    glGetObjectParameterivARB(handle, GL_OBJECT_INFO_LOG_LENGTH_ARB, &errorLoglength);

    //Create a buffer to read compilation error message
    errorLogText = (char*)malloc(sizeof(char) * errorLoglength);

    //Used to get the final length of the log.
    glGetInfoLogARB(handle, errorLoglength, &actualErrorLogLength, errorLogText);

    // Display errors.
    printf("%s\n",errorLogText);

    // Free the buffer malloced earlier
    free(errorLogText);
  }

  return handle;
}



void loadShadowShader()
{
  GLhandleARB vertexShaderHandle;
  GLhandleARB fragmentShaderHandle;

  vertexShaderHandle   = loadShader("shadow.vert",GL_VERTEX_SHADER);
  fragmentShaderHandle = loadShader("shadow.frag",GL_FRAGMENT_SHADER);

  shadowShaderId = glCreateProgramObjectARB();

  glAttachObjectARB(shadowShaderId,vertexShaderHandle);
  glAttachObjectARB(shadowShaderId,fragmentShaderHandle);
  glLinkProgramARB(shadowShaderId);

  shadowMapUniform = glGetUniformLocationARB(shadowShaderId,"ShadowMap");
  shadowMapStepXUniform = glGetUniformLocationARB(shadowShaderId,"xPixelOffset");
  shadowMapStepYUniform = glGetUniformLocationARB(shadowShaderId,"yPixelOffset");

  projectionMatrixLocation = glGetUniformLocation(shadowShaderId, "projectionMatrix"); // Get the location of our projection matrix in the shader
  viewMatrixLocation = glGetUniformLocation(shadowShaderId, "viewMatrix"); // Get the location of our view matrix in the shader
  modelMatrixLocation = glGetUniformLocation(shadowShaderId, "modelMatrix"); // Get the location of our model matrix in the shader

  previousProjectionMatrixLocation = glGetUniformLocation(shadowShaderId, "previousProjectionMatrix"); // Get the location of our projection matrix in the shader
  previousViewMatrixLocation = glGetUniformLocation(shadowShaderId, "previousViewMatrix"); // Get the location of our view matrix in the shader


}

void generateShadowFBO()
{
  int shadowMapWidth = RENDER_WIDTH * SHADOW_MAP_RATIO;
  int shadowMapHeight = RENDER_HEIGHT * SHADOW_MAP_RATIO;

  //GLfloat borderColor[4] = {0,0,0,0};

  GLenum FBOstatus;

  // Try to use a texture depth component
  glActiveTexture(GL_TEXTURE7);
  glGenTextures(1, &depthTextureId);
  glBindTexture(GL_TEXTURE_2D, depthTextureId);

  // GL_LINEAR does not make sense for depth texture. However, next tutorial shows usage of GL_LINEAR and PCF
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  // Remove artefact on the edges of the shadowmap
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
  glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
  //glTexParameterfv( GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor );



  // No need to force GL_DEPTH_COMPONENT24, drivers usually give you the max precision if available 
  glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowMapWidth, shadowMapHeight, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);
  glBindTexture(GL_TEXTURE_2D, 0);

  // create a framebuffer object
  glGenFramebuffersEXT(1, &fboId);
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboId);

  // Instruct openGL that we won't bind a color texture with the currently binded FBO
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);

  // attach the texture to FBO depth attachment point
  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,GL_TEXTURE_2D, depthTextureId, 0);

  // check FBO status
  FBOstatus = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
  if(FBOstatus != GL_FRAMEBUFFER_COMPLETE_EXT)
    printf("GL_FRAMEBUFFER_COMPLETE_EXT failed, CANNOT use FBO\n");

  // switch back to window-system-provided framebuffer
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

void setupBlurFBO()
{

  glActiveTexture(GL_TEXTURE6);
  glGenTextures(1, &blurTextureId);
  glBindTexture(GL_TEXTURE_2D, blurTextureId);

  // GL_LINEAR does not make sense for depth texture. However, next tutorial shows usage of GL_LINEAR and PCF
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  // Remove artefact on the edges of the shadowmap
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
  glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
  //glTexParameterfv( GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor );



  // No need to force GL_DEPTH_COMPONENT24, drivers usually give you the max precision if available 
  glTexImage2D( GL_TEXTURE_2D, 0, 4, RENDER_WIDTH, RENDER_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
  glBindTexture(GL_TEXTURE_2D, 0);



  glGenFramebuffersEXT(1, &blurFboId);
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, blurFboId);
  glGenRenderbuffers(1, &depthrenderbuffer);
  glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, RENDER_WIDTH, RENDER_HEIGHT);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);

  // attach the texture to FBO depth attachment point
  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,GL_TEXTURE_2D, blurTextureId, 0);
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

void setupMatrices(float position_x,float position_y,float position_z,float lookAt_x,float lookAt_y,float lookAt_z, bool light)
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  modelMatrix = glm::mat4();
  gluPerspective(95,RENDER_WIDTH/RENDER_HEIGHT,10,40000); //This is used in the default pipeline --render from light's POV
  //TODO: eliminate last line.
  projectionMatrix = glm::perspective(45.0f, (float)RENDER_WIDTH/(float)RENDER_HEIGHT, 10.0f, 40000.0f); //this is used in modified pipeline -- our new render
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(position_x,position_y,position_z,lookAt_x,lookAt_y,lookAt_z,0,1,0);
  if(!light)
    viewMatrix = glm::lookAt(glm::vec3(position_x, position_y, position_z), glm::vec3(lookAt_x,lookAt_y,lookAt_z), glm::vec3(0.0f,1.0f,0.0f));
}


// This update only change the position of the light.

void setTextureMatrix(void)
{
  static double modelView[16];
  static double projection[16];

  // This is matrix transform every coordinate x,y,z
  // x = x* 0.5 + 0.5 
  // y = y* 0.5 + 0.5 
  // z = z* 0.5 + 0.5 
  // Moving from unit cube [-1,1] to [0,1]  
  const GLdouble bias[16] = {	
    0.5, 0.0, 0.0, 0.0, 
    0.0, 0.5, 0.0, 0.0,
    0.0, 0.0, 0.5, 0.0,
    0.5, 0.5, 0.5, 1.0};

  // Grab modelview and transformation matrices
  glGetDoublev(GL_MODELVIEW_MATRIX, modelView);
  glGetDoublev(GL_PROJECTION_MATRIX, projection);


  glMatrixMode(GL_TEXTURE);
  glActiveTextureARB(GL_TEXTURE7);

  glLoadIdentity();	
  glLoadMatrixd(bias);

  // concatating all matrice into one.
  glMultMatrixd (projection);
  glMultMatrixd (modelView);

  // Go back to normal matrix mode
  glMatrixMode(GL_MODELVIEW);
}

// During translation, we also have to maintain the GL_TEXTURE8, used in the shadow shader
// to determine if a vertex is in the shadow.
//TODO: change startRotate and startScale too. 
void startTranslate(float x,float y,float z)
{
  modelMatrixStack.push(modelMatrix);
  modelMatrix = glm::translate(modelMatrix, glm::vec3(x,y,z));
  glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]); // Send our model matrix to the shader
  glPushMatrix();
  glTranslatef(x,y,z);
  glMatrixMode(GL_TEXTURE);
  glActiveTextureARB(GL_TEXTURE7);
  glPushMatrix();
  glTranslatef(x,y,z);

  glMatrixMode(GL_MODELVIEW);
}

void startRotate(float angle, float x,float y,float z)
{
  modelMatrixStack.push(modelMatrix);
  modelMatrix = glm::rotate(modelMatrix,angle, glm::vec3(x,y,z));
  glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]); // Send our model matrix to the shader
  glPushMatrix();
  glRotatef(angle, x,y,z);

  glMatrixMode(GL_TEXTURE);
  glActiveTextureARB(GL_TEXTURE7);
  glPushMatrix();
  glRotatef(angle, x,y,z);

  glMatrixMode(GL_MODELVIEW);
}

void startScale(float sx, float sy, float sz)
{
  modelMatrixStack.push(modelMatrix);
  modelMatrix = glm::scale(modelMatrix, glm::vec3(sx,sy,sz));
  glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]); // Send our model matrix to the shader
  glPushMatrix();
  glScalef(sx, sy, sz);

  glMatrixMode(GL_TEXTURE);
  glActiveTextureARB(GL_TEXTURE7);
  glPushMatrix();
  glScalef(sx, sy, sz);

  glMatrixMode(GL_MODELVIEW);
}

void endTransformation(void)
{
  glMatrixMode(GL_TEXTURE);
  glActiveTextureARB(GL_TEXTURE7);
  glPopMatrix();

  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
  modelMatrix = modelMatrixStack.top();
  glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]); // Send our model matrix to the shader
  modelMatrixStack.pop();
}

//void setupCameraMatrices()
//{
//  setupMatrices(p_camera[0], p_camera[1], p_camera[2],l_camera[0], l_camera[1], l_camera[2]);
//}
//
//void setupLightMatrices()
//{
//  setupMatrices(p_light[0], p_light[1], p_light[2],l_light[0], l_light[1], l_light[2]);
//}
//




#endif


