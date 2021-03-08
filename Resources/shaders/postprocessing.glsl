#version 450 core

uniform sampler2D color;
uniform sampler2D depth;

out vec4 fragColor;

void main()
{
	ivec2 texCoord = ivec2(gl_FragCoord.xy);

	vec3 color = texelFetch(color, texCoord, 0).rgb;
	fragColor = vec4(color, 1.0f);
}