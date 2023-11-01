/*
	fragmentShader.glsl
	author: Telo PHILIPPE
*/

#version 330 core

out vec4 outColor;	  // Shader output: the color response attached to this fragment

in vec3 vertexNormal;  // Input from the vertex shader
in vec3 worldPos;
in vec2 textureUV;

uniform vec3 u_cameraPosition;

uniform sampler2D u_texture;

float rand2D(in vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main() {
	//vec3 objColor = texture(u_texture, textureUV).xyz;
	vec3 objColor = vec3(rand2D(textureUV));
	outColor = vec4(objColor, 1.0f);
}
