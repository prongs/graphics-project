

uniform mat4 previousProjectionMatrix;
uniform mat4 previousViewMatrix;
uniform int RENDER_WIDTH;
uniform int RENDER_HEIGHT;
uniform bool first_frame;
uniform sampler2D frameBuf;
varying vec4 pos;
void main()
{
    int numSamples = 10;
    vec4 prevPos = previousProjectionMatrix * previousViewMatrix * pos;
    if(prevPos.s<0&&prevPos.t<0)
	{
        gl_FragColor = vec4(1,1,0,1);
    }

	prevPos = prevPos/prevPos.w;
    float prevPos_x = ((prevPos.s+1.0)/2.0);
    float prevPos_y = ((prevPos.t+1.0)/2.0);
    vec2 prev = vec2(prevPos_x,prevPos_y);
    vec2 samplePos = vec2(gl_FragCoord.s/RENDER_WIDTH,gl_FragCoord.t/RENDER_HEIGHT);
    vec2 samplePos_first = vec2(gl_FragCoord.s/RENDER_WIDTH,gl_FragCoord.t/RENDER_HEIGHT);
    vec2 velocity = (samplePos-prev)/5.0;
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


