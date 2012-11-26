uniform mat4 previousProjectionMatrix;
uniform mat4 previousViewMatrix;
uniform int RENDER_WIDTH;
uniform int RENDER_HEIGHT;
uniform bool first_frame;
uniform sampler2D frameBuf;


varying vec4 pos;

void main()
{	

	int numSamples = 9;
	vec4 prevPos = previousProjectionMatrix * previousViewMatrix * pos;
	prevPos = prevPos/prevPos.w;
	int prevPos_x = int((prevPos.s+1)*RENDER_WIDTH/2.0);
	int prevPos_y = int((prevPos.t+1)*RENDER_HEIGHT/2.0);

	float x = prevPos_x/RENDER_WIDTH;
	float y = prevPos_y/RENDER_HEIGHT;
	
	vec2 samplePos = vec2(gl_FragCoord.s/RENDER_WIDTH,gl_FragCoord.t/RENDER_HEIGHT);
	vec2 velocity = (vec2(x,y)-samplePos)/220.0;
	vec4 blurColor = vec4(0,0,0,0);
	for(int i = 0; i<numSamples; i++,samplePos+=velocity)
	{
		blurColor+= texture2D(frameBuf, samplePos);
	}
	if(blurColor.x==0)
	{
		gl_FragColor = vec4(1,0,0,1);
		//return;
	}
	blurColor/=numSamples;
	blurColor.w = 1;
	gl_FragColor = blurColor;
}

