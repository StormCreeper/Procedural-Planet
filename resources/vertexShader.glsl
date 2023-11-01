/*
	vertexShader.glsl
	author: Telo PHILIPPE
*/

#version 330 core            // Minimal GL version support expected from the GPU

layout(location=0) in vec3 vPosition;
layout(location=1) in vec3 vNormal;
layout(location=2) in vec2 vUV;

uniform mat4 u_viewMat, u_projMat, u_modelMat;

uniform float u_height;

out vec3 vertexNormal;
out vec3 worldPos;
out vec2 textureUV;

void main() {
	float heightFactor = 0.3f;
	gl_Position =  u_projMat * u_viewMat * u_modelMat * vec4(vPosition + u_height * heightFactor * vNormal, 1.0);
	
	vec4 worldPos_Homo = u_modelMat * vec4(vPosition, 1.0);
	worldPos = worldPos_Homo.xyz / worldPos_Homo.w;

	vec4 normal_Homo = transpose(inverse(u_modelMat)) * vec4(vNormal, 1.0);
	vertexNormal = normalize(normal_Homo.xyz);

	textureUV = vUV;
}
