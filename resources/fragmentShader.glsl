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

float density = 100;

float rand2D(in vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main() {
	ivec2 seed = ivec2(textureUV * density);
	float val = rand2D(seed);
	bool isGrass = val > u_height;
	vec3 objColor = isGrass ? vec3(0.0f, 1.0f, 0.0f) : vec3(0.0f);
	outColor = vec4(objColor, 1.0f);
}
