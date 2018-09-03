#version 150

in  vec4 vPosition;
in  vec4 vNormal;
flat out vec4 flatColor;
out vec3 pos;
out vec3 fL;
out vec3 fN;

uniform mat4 projectionMatrix;
uniform mat4 modelMatrix;
uniform vec4 ambient, diffuse, specular;
uniform vec4 lightPos;
uniform vec4 lightDir;
uniform float lightFalloff;
uniform float lightCutoff;
uniform float shininess;

void main() 
{

  //Calculate needed vectors
  pos = (modelMatrix * vPosition).xyz;
  fL = normalize(lightPos.xyz - pos);
  vec3 fE = normalize(-pos);
  vec3 fH = normalize(fL + fE);
  fN = normalize(modelMatrix * vec4(vNormal)).xyz;

  //Find intensity  
  float lightPosAngle = acos(dot(fL, normalize(lightDir.xyz)));
  float intensity = 0;
  if(lightPosAngle < lightCutoff) {
	intensity = pow(cos(lightPosAngle), lightFalloff) * 3;
  }

  //Calculate flat color terms
  vec4 fAmbient = ambient * intensity;
  
  float cosTheta = max(dot(fL, fN), 0.0);
  vec4 fDiffuse = cosTheta * diffuse * intensity;

  float cosPhi = pow(max(dot(fN, fH), 0.0), shininess);
  vec4 fSpecular = cosPhi * specular * intensity;

  if(dot(fL, fN) < 0.0) {
	fSpecular = vec4(0.0, 0.0, 0.0, 0.0);
  }

  gl_Position = projectionMatrix * modelMatrix * vPosition;
  flatColor = fAmbient + fDiffuse + fSpecular;
  flatColor.a = 1.0f;
} 
