#version 410

layout(location=0) in vec3 vPosition;
layout(location=1) in vec2 vTexCoord;
layout(location=2) in vec4 vColor;

layout(location=0) out vec3 outTexCoord;
layout(location=1) out vec4 outColor;

void main()
{
  outTexCoord = vec3(vTexCoord.xy, vPosition.z);
  outColor = vColor;
  gl_Position = vec4(vPosition.xy, 0, 1);
}