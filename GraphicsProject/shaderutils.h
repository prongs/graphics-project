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

glm::mat4 projectionMatrix;
glm::mat4 viewMatrix;
glm::mat4 modelMatrix;

glm::mat4 previousProjectionMatrix;
glm::mat4 previousViewMatrix;

stack<glm::mat4> modelMatrixStack;




int projectionMatrixLocation;
int viewMatrixLocation;
int modelMatrixLocation;

int blurProjectionMatrixLocation;
int blurViewMatrixLocation;
int blurModelMatrixLocation;

int previousProjectionMatrixLocation;
int previousViewMatrixLocation;

int cubeMapLocation;
int readFromTextureLocation;
int textureLocation;
int showShadowsLocation;



int firstFrameBoolLoc;
int frameBufferLoc;
int prevFrameLocation;
int renderWidthLocation;
int renderHeightLocation;
int numSamplesLoc;

float p_camera[3] = {32,20,0};
float l_camera[3] = {2,0,-10};

float p_light[3] = {3,20,0};
float l_light[3] = {0,0,-1};

GLuint fboId;
GLuint blurFboId;
// ID of texture(motion blur)
GLuint textureId;

// Z values will be rendered to this texture when using fboId framebuffer
GLuint depthTextureId;
GLuint blurTextureId;
GLuint depthrenderbuffer;

// Use to activate/disable shadowShader
GLhandleARB shadowShaderId;
GLhandleARB blurShaderId;
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

    glBindFragDataLocation(program, 0, outputAttributeName);

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

    glUseProgram(program);

    return program;
}


GLhandleARB loadShader(char* filename, unsigned int type)
{
  FILE *pfile;
  GLhandleARB handle;
  const GLcharARB* files[1];

  GLint result;
  GLint errorLoglength ;
  char* errorLogText;
  GLsizei actualErrorLogLength;

  char buffer[400000];
  memset(buffer,0,400000);

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
    printf("Failed creating vertex shader object from file: %s.",filename);
    exit(0);
  }

  files[0] = (const GLcharARB*)buffer;
  glShaderSourceARB(
    handle,
    1,
    files,
    NULL);

  glCompileShaderARB(handle);

  glGetObjectParameterivARB(handle, GL_OBJECT_COMPILE_STATUS_ARB, &result);

  if (!result)
  {
    printf("Shader '%s' failed compilation.\n",filename);
    glGetObjectParameterivARB(handle, GL_OBJECT_INFO_LOG_LENGTH_ARB, &errorLoglength);
    errorLogText = (char*)malloc(sizeof(char) * errorLoglength);
    glGetInfoLogARB(handle, errorLoglength, &actualErrorLogLength, errorLogText);

    printf("%s\n",errorLogText);
    free(errorLogText);
  }
  return handle;
}



void loadAllShaders()
{
  GLhandleARB vertexShaderHandle;
  GLhandleARB fragmentShaderHandle;
  GLhandleARB blurVertexShaderHandle;
  GLhandleARB blurFragmentShaderHandle;

  vertexShaderHandle   = loadShader("shadow.vert",GL_VERTEX_SHADER);
  fragmentShaderHandle = loadShader("shadow.frag",GL_FRAGMENT_SHADER);


  blurVertexShaderHandle   = loadShader("blur.vert",GL_VERTEX_SHADER);
  blurFragmentShaderHandle = loadShader("blur.frag",GL_FRAGMENT_SHADER);

  shadowShaderId = glCreateProgramObjectARB();
  blurShaderId = glCreateProgramObjectARB();

  glAttachObjectARB(shadowShaderId,vertexShaderHandle);
  glAttachObjectARB(shadowShaderId,fragmentShaderHandle);
  glLinkProgramARB(shadowShaderId);

  glAttachObjectARB(blurShaderId,blurVertexShaderHandle);
  glAttachObjectARB(blurShaderId,blurFragmentShaderHandle);
  glLinkProgramARB(blurShaderId);


  shadowMapUniform = glGetUniformLocationARB(shadowShaderId,"ShadowMap");
  shadowMapStepXUniform = glGetUniformLocationARB(shadowShaderId,"xPixelOffset");
  shadowMapStepYUniform = glGetUniformLocationARB(shadowShaderId,"yPixelOffset");

  projectionMatrixLocation = glGetUniformLocation(shadowShaderId, "projectionMatrix"); 
  viewMatrixLocation = glGetUniformLocation(shadowShaderId, "viewMatrix"); 
  modelMatrixLocation = glGetUniformLocation(shadowShaderId, "modelMatrix");

  cubeMapLocation = glGetUniformLocation(shadowShaderId, "cubeMap");
  readFromTextureLocation = glGetUniformLocation(shadowShaderId, "readFromTexture");
  textureLocation = glGetUniformLocation(shadowShaderId, "tex");
  showShadowsLocation = glGetUniformLocation(shadowShaderId, "showShadows");

  blurProjectionMatrixLocation = glGetUniformLocation(blurShaderId, "projectionMatrix"); 
  blurViewMatrixLocation = glGetUniformLocation(blurShaderId, "viewMatrix"); 
  blurModelMatrixLocation = glGetUniformLocation(blurShaderId, "modelMatrix");

  previousProjectionMatrixLocation = glGetUniformLocation(blurShaderId, "previousProjectionMatrix"); 
  previousViewMatrixLocation = glGetUniformLocation(blurShaderId, "previousViewMatrix"); 
  renderWidthLocation = glGetUniformLocation(blurShaderId, "RENDER_WIDTH"); 
  renderHeightLocation = glGetUniformLocation(blurShaderId, "RENDER_HEIGHT"); 
  firstFrameBoolLoc = glGetUniformLocation(blurShaderId, "first_frame"); 
  frameBufferLoc = glGetUniformLocation(blurShaderId, "frameBuf"); 
  numSamplesLoc = glGetUniformLocation(blurShaderId, "num_samples");


}

void generateShadowFBO()
{
  int shadowMapWidth = RENDER_WIDTH * SHADOW_MAP_RATIO;
  int shadowMapHeight = RENDER_HEIGHT * SHADOW_MAP_RATIO;

  //GLfloat borderColor[4] = {0,0,0,0};

  GLenum FBOstatus;

  glActiveTexture(GL_TEXTURE7);
  glGenTextures(1, &depthTextureId);
  glBindTexture(GL_TEXTURE_2D, depthTextureId);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
  glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
  //glTexParameterfv( GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor );



  glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowMapWidth, shadowMapHeight, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);
  glBindTexture(GL_TEXTURE_2D, 0);

  glGenFramebuffersEXT(1, &fboId);
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboId);

  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);

  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,GL_TEXTURE_2D, depthTextureId, 0);

  FBOstatus = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
  if(FBOstatus != GL_FRAMEBUFFER_COMPLETE_EXT)
    printf("GL_FRAMEBUFFER_COMPLETE_EXT failed, CANNOT use FBO\n");

  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

void setupBlurFBO()
{

  glActiveTexture(GL_TEXTURE6);
  glGenTextures(1, &blurTextureId);
  glBindTexture(GL_TEXTURE_2D, blurTextureId);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
  glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
  //glTexParameterfv( GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor );

  glTexImage2D( GL_TEXTURE_2D, 0, 4, RENDER_WIDTH, RENDER_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
  glBindTexture(GL_TEXTURE_2D, 0);



  glGenFramebuffersEXT(1, &blurFboId);
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, blurFboId);
  glGenRenderbuffers(1, &depthrenderbuffer);
  glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, RENDER_WIDTH, RENDER_HEIGHT);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);
  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,GL_TEXTURE_2D, blurTextureId, 0);
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

void setupMatrices(float position_x,float position_y,float position_z,float lookAt_x,float lookAt_y,float lookAt_z, bool light)
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  modelMatrix = glm::mat4();

  //This is used in the default pipeline --render from light's POV
  gluPerspective(95,RENDER_WIDTH/RENDER_HEIGHT,10,40000);
  //TODO: eliminate last line.
  //this is used in modified pipeline -- our new render
  projectionMatrix = glm::perspective(45.0f, (float)RENDER_WIDTH/(float)RENDER_HEIGHT, 10.0f, 40000.0f);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(position_x,position_y,position_z,lookAt_x,lookAt_y,lookAt_z,0,1,0);
  if(!light)
    viewMatrix = glm::lookAt(glm::vec3(position_x, position_y, position_z), glm::vec3(lookAt_x,lookAt_y,lookAt_z), glm::vec3(0.0f,1.0f,0.0f));
}


void setTextureMatrix(void)
{
  static double modelView[16];
  static double projection[16];

  const GLdouble bias[16] = {	
    0.5, 0.0, 0.0, 0.0, 
    0.0, 0.5, 0.0, 0.0,
    0.0, 0.0, 0.5, 0.0,
    0.5, 0.5, 0.5, 1.0};

  glGetDoublev(GL_MODELVIEW_MATRIX, modelView);
  glGetDoublev(GL_PROJECTION_MATRIX, projection);


  glMatrixMode(GL_TEXTURE);
  glActiveTextureARB(GL_TEXTURE7);

  glLoadIdentity();	
  glLoadMatrixd(bias);
  glMultMatrixd (projection);
  glMultMatrixd (modelView);
  glMatrixMode(GL_MODELVIEW);
}

//TODO: change startRotate and startScale too. 
void startTranslate(float x,float y,float z)
{
  modelMatrixStack.push(modelMatrix);
  modelMatrix = glm::translate(modelMatrix, glm::vec3(x,y,z));
  glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]);
  glUniformMatrix4fv(blurModelMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]);
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
  glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]);
  glUniformMatrix4fv(blurModelMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]);
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
  glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]);
  glUniformMatrix4fv(blurModelMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]);
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
  glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]);
  glUniformMatrix4fv(blurModelMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]);
  modelMatrixStack.pop();
}

#endif