#version 330 core

in vec3 FragPos;
in vec2 texCoord0;
in vec2 texCoord1;

out vec4 FragColor;

uniform float factor;

uniform sampler2D texture1;

void main()
{
	FragColor = mix(texture(texture1, texCoord0), texture(texture1, texCoord1), factor);
}