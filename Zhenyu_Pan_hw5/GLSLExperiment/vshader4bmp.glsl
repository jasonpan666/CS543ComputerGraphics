#version 150

uniform mat4 projection;
uniform mat4 model_matrix;

in  vec4 vPosition;
in  vec4 vTexCoord;
out vec2 texCoord;

void main() 
{
  texCoord = vTexCoord.st;
  gl_Position = projection*vPosition;
} 
