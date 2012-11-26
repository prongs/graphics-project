// Used for shadow lookup


uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;



varying vec4 ShadowCoord;
varying vec3 normal;
varying vec3 vert_to_light;
varying vec3 eyeVector;
varying vec4 pos;

void main()
{


     	ShadowCoord= gl_TextureMatrix[7] * gl_Vertex;
		pos = modelMatrix * gl_Vertex;
		gl_Position = projectionMatrix * viewMatrix * pos;

		normal = gl_NormalMatrix * gl_Normal;

		vec4 vertex_in_modelview_space = gl_ModelViewMatrix * gl_Vertex;
 
    	vert_to_light = vec3(gl_LightSource[0].position - vertex_in_modelview_space);

    	eyeVector = -vec3(vertex_in_modelview_space);

		gl_FrontColor = gl_Color;
}

