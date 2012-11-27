// Vertex shader for the blur shader program


uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
varying vec4 pos;
void main()
{
    pos = modelMatrix * gl_Vertex;
    gl_Position = projectionMatrix * viewMatrix * pos;
    gl_FrontColor = gl_Color;
}


