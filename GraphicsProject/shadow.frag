uniform sampler2DShadow ShadowMap;
uniform sampler2D pervFrame;
uniform mat4 previousProjectionMatrix;
uniform mat4 previousViewMatrix;
uniform int RENDER_WIDTH;
uniform int RENDER_HEIGHT;

varying vec4 ShadowCoord;
varying vec3 normal;
varying vec3 vert_to_light;
varying vec3 eyeVector;
varying vec4 pos;

uniform float xPixelOffset ;
uniform float yPixelOffset ;
	
float lookup( vec2 offSet)
{
	// Values are multiplied by ShadowCoord.w because shadow2DProj does a W division for us.
	return shadow2DProj(ShadowMap, ShadowCoord + vec4(offSet.x * xPixelOffset * ShadowCoord.w, offSet.y * yPixelOffset * ShadowCoord.w, 0.05, 0.0) ).z;
}
void main()
{	

	int numSamples = 1;
	vec4 prevPos = previousProjectionMatrix * previousViewMatrix * pos;
	prevPos = prevPos/prevPos.w;
	int prevPos_x = int((prevPos.s+1)*RENDER_WIDTH/2.0);
	int prevPos_y = int((prevPos.t+1)*RENDER_HEIGHT/2.0);
	
	vec2 velocity = vec2(prevPos_x-gl_FragCoord.x, prevPos_y-gl_FragCoord.y)/2.0;
	vec2 samplePos = gl_FragCoord.st+velocity;
	vec4 blurColor = vec4(0,0,0,0);
	for(int i = 0; i < numSamples-1; i++, samplePos+=velocity)
	{
		blurColor += texture2D(pervFrame, samplePos);
	}
	//if(dot(blurColor, blurColor)<64.9)
	//	numSamples=1;
/*
	if(prevPos_y<100.0)
		{
			gl_FragColor = vec4(1.0,0.0,0.0,1.0);
			return;
		}

*/
	vec4 shadowCoordinateWdivide = ShadowCoord / ShadowCoord.w ;
	
	// Used to lower moiré pattern and self-shadowing
//	shadowCoordinateWdivide.z += 0.0005;
	
	
//	float distanceFromLight = texture2D(ShadowMap,shadowCoordinateWdivide.st).z;
	
	
 //	float shadow = 1.0;
 //	if (ShadowCoord.w > 0.0)
 //		shadow = distanceFromLight < shadowCoordinateWdivide.z ? 0.5 : 1.0 ;

	float shadow;
		
	// Avoid counter shadow
	if (ShadowCoord.w > 1.0)
	{
		float x,y;
		for (y = -1.5 ; y <=1.5 ; y+=1.0)
			for (x = -1.5 ; x <=1.5 ; x+=1.0)
				shadow += lookup(vec2(x,y));
		
		shadow /= 16.0 ;

	}
  	
  	vec3 N = normalize(normal);
  	vec3 L = normalize(vert_to_light);
  	vec3 E = normalize(eyeVector);
    float diffuse_term = clamp(dot(N, L), 0.0, 1.0);

    vec3 H = normalize(E + L);
    float specular_term = clamp(dot(N, H), 0.0, 1.0);

    vec4 ambient_col = vec4(gl_LightSource[0].ambient * gl_FrontMaterial.ambient);

    vec4 diffuse_col = vec4(gl_LightSource[0].diffuse * gl_FrontMaterial.diffuse);

    vec4 specular_col = vec4(gl_LightSource[0].specular * gl_FrontMaterial.specular);

	
  	vec4 col =	 (ambient_col + diffuse_col*diffuse_term + specular_col*pow(specular_term, gl_FrontMaterial.shininess) * shadow);
	
	vec4 curColor =	 vec4(vec3(col)*(shadow + 0.2),1.0);

	gl_FragColor = (numSamples==1)?curColor:(blurColor + curColor)/numSamples;
  
}
