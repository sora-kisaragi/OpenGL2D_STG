#version 410

layout(location=0) in vec4 inColor;
layout(location=1) in vec2 inTexCoord;
layout(location=2) flat in int inBlendMode;

uniform sampler2D colorSampler;

out vec4 fragColor;

void main()
{
  vec4 texColor = texture(colorSampler, inTexCoord);
  if (inBlendMode == 0) {
    fragColor = texColor * inColor;
  } else if (inBlendMode == 1) {
    fragColor.rgb = texColor.rgb + inColor.rgb;
	fragColor.a = texColor.a * inColor.a;
  } else {
    fragColor.rgb = texColor.rgb - inColor.rgb;
	fragColor.a = texColor.a * inColor.a;
  }
}