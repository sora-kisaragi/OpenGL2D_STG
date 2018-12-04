#version 410

layout(location=0) in vec3 vPosition;
layout(location=1) in vec4 vColor;
layout(location=2) in vec2 vTexCoord;
layout(location=3) in int vBlendMode;

layout(location=0) out vec4 outColor;
layout(location=1) out vec2 outTexCoord;
layout(location=2) flat out int outBlendMode;

uniform mat4x4 matMVP;

void main()
{
  outColor = vColor;
  outTexCoord = vTexCoord;
  outBlendMode = vBlendMode;
  gl_Position = matMVP * vec4(vPosition, 1.0);
}