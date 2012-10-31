uniform sampler2D ShadowMap;

varying vec4 ShadowCoord;
varying vec3 normal;
varying vec3 vert_to_light;
varying vec3 eyeVector;

void main()
{	
	vec4 shadowCoordinateWdivide = ShadowCoord / ShadowCoord.w ;
	
	// Used to lower moiré pattern and self-shadowing
	shadowCoordinateWdivide.z += 0.0005;
	
	
	float distanceFromLight = texture2D(ShadowMap,shadowCoordinateWdivide.st).z;
	
	
 	float shadow = 1.0;
 	if (ShadowCoord.w > 0.0)
 		shadow = distanceFromLight < shadowCoordinateWdivide.z ? 1.0 : 1.0 ;
  	
  	vec3 N = normalize(normal);
  	vec3 L = normalize(vert_to_light);
    float diffuse_term = clamp(dot(N, L), 0.0, 1.0);

    vec3 H = normalize(N + L);
    float specular_term = clamp(dot(N, H), 0.0, 1.0);

    vec4 ambient_col = vec4(gl_LightSource[0].ambient * gl_FrontMaterial.ambient);

    vec4 diffuse_col = vec4(gl_LightSource[0].diffuse * gl_FrontMaterial.diffuse);

    vec4 specular_col = vec4(gl_LightSource[0].specular * gl_FrontMaterial.specular);
	
  	gl_FragColor =	 (ambient_col + diffuse_col*diffuse_term + specular_col*pow(specular_term, gl_FrontMaterial.shininess) * shadow);
	
	// gl_FragColor =	 gl_Color * shadow;

  
}
