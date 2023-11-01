/*
	fragmentShader.glsl
	author: Telo PHILIPPE

	Use shell texturing to create a grass effect
*/

#version 330 core

out vec4 outColor;	  // Shader output: the color response attached to this fragment

in vec3 vertexNormal;  // Input from the vertex shader
in vec3 worldPos;
in vec2 textureUV;

uniform vec3 u_cameraPosition;

uniform sampler2D u_texture;

uniform float u_height;

float density = 1000;
vec3 lightDir = normalize(vec3(1.0f, 1.0f, 1.0f));

float rand2D(in vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main() {
	ivec2 seed = ivec2(textureUV * density);
	float val = rand2D(seed) * 0.5f + 0.5f;
	bool isGrass = val > u_height;

	// In local space
	vec2 localPos = fract(textureUV * density);
	localPos = localPos * 2.0f - 1.0f;
	float dist = length(localPos);
	if (!isGrass || dist > (val - u_height)) {
		discard;
	}
	vec3 objColor = isGrass ? vec3(0.0f, 1.0f, 0.0f) : vec3(0.0f);

	// Add half lambert shading
	float lambert = dot(vertexNormal, lightDir) * 0.5f + 0.5f;
	objColor *= lambert;
	objColor *= u_height * 0.3f + 0.7f;

	outColor = vec4(objColor, 1.0f);
}
