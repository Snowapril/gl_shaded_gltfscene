#version 450 core

precision highp float;
precision highp int;

layout ( location = 0 ) out VSOUT{
	vec2 texCoord;
} vs_out;

void main() {
	vec2 pos = vec2(gl_VertexID & 1, (gl_VertexID & 2) >> 1) * 2 - 1;
	vs_out.texCoord = pos.xy * 0.5 + 0.5;
	gl_Position = vec4(pos.xy, 0, 1);
}