// Fragment shader for motion blur shader

uniform mat4 previousProjectionMatrix;
uniform mat4 previousViewMatrix;
uniform int RENDER_WIDTH;
uniform int RENDER_HEIGHT;
uniform bool first_frame;
uniform sampler2D frameBuf;
uniform int num_samples;
varying vec4 pos;
void main()
{
    int numSamples = num_samples;
    vec4 prevPos = previousProjectionMatrix * previousViewMatrix * pos;

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

	blurColor/=numSamples;
    blurColor.w = 1;
    gl_FragColor = blurColor;
}


