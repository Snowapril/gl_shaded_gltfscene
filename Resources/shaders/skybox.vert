#version 450

layout(location = 0) in vec3 inPos;
layout(location = 0) out vec3 outWorldPos;

// Camera UBO
layout(std140, binding = 0) uniform UBOCamera
{
	mat4 projection; //  64
	mat4 view;		 // 128
	mat4 viewProj;	 // 192
	vec3 camPos;	 // 208
} uboCamera;

void main()
{
  vec4 pos      = vec4(inPos.xyz, 1.0);
  gl_Position   = uboCamera.projection * pos;
  gl_Position.z = 0.5; // always draw
  mat4 m        = inverse(uboCamera.view);
  m[3][0]       = 0.0;
  m[3][1]       = 0.0;
  m[3][2]       = 0.0;
  outWorldPos   = vec3(m * pos).xyz;
}
