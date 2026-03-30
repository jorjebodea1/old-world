#version 410 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 textcoord;
layout(location = 3) in ivec4 in_jointIndices;
layout(location = 4) in vec4 in_weights;

out vec2 TexCoord;
out vec3 fragPos;
out vec4 Normal;
out vec4 fragPosLightSpace;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceTrMatrix;

uniform samplerBuffer boneTexture;
mat4 getBoneMatrix(int jointIndex) {
    // A mat4 is 4 columns (pixels) wide in the texture
    int offset = jointIndex * 4;
    
    vec4 col1 = texelFetch(boneTexture, offset + 0);
    vec4 col2 = texelFetch(boneTexture, offset + 1);
    vec4 col3 = texelFetch(boneTexture, offset + 2);
    vec4 col4 = texelFetch(boneTexture, offset + 3);
    
    return mat4(col1, col2, col3, col4);
}
vec4 applyBoneTransform(vec4 p) {
    vec4 result = vec4(0.0);
    for (int i = 0; i < 4; ++i) {
        mat4 boneTransform = getBoneMatrix(in_jointIndices[i]);
        result += in_weights[i] * (boneTransform * p);
    }
    return result;
}


void main() {
    vec4 position = applyBoneTransform(vec4(vertexPosition, 1.0));
    vec4 normal = normalize(applyBoneTransform(vec4(vertexNormal, 0.0)));
	mat3 normalMatrix = transpose(inverse(mat3(model)));
    fragPos = vec3(model * position);
    fragPosLightSpace = lightSpaceTrMatrix * model * position;
	Normal=vec4(normalMatrix*normal.xyz,0.0f);
    TexCoord = textcoord;
    gl_Position = projection * view * model*position;

}