#version 410 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 textcoord;

out vec2 TexCoord;
out vec3 fragPos;
out vec4 Normal;
out vec4 fragPosLightSpace;
out vec4 fragmentPosEyeSpace;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceTrMatrix;

void main() {
	mat3 normalMatrix = transpose(inverse(mat3(model)));
	Normal =vec4(normalMatrix * vertexNormal,0.0f);
    TexCoord = textcoord;
	fragPosLightSpace = lightSpaceTrMatrix * model * vec4(vertexPosition,1.0);
	fragmentPosEyeSpace=view*model*vec4(vertexPosition,1.0);
	fragPos=(model*vec4(vertexPosition,1.0)).xyz;
    gl_Position = projection * view * model*vec4(vertexPosition,1.0);
}