// Used for shadow lookup
varying vec4 ShadowCoord;
varying vec3 normal;
varying vec3 vert_to_light;
varying vec3 eyeVector;


void main()
{


     	ShadowCoord= gl_TextureMatrix[7] * gl_Vertex;
  
		gl_Position = ftransform();

		normal = gl_NormalMatrix * gl_Normal;

		vec4 vertex_in_modelview_space = gl_ModelViewMatrix * gl_Vertex;
 
    	vert_to_light = vec3(gl_LightSource[0].position - vertex_in_modelview_space);

    	eyeVector = -vec3(vertex_in_modelview_space);

		gl_FrontColor = gl_Color;
}

