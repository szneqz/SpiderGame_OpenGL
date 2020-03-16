#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 texCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec2 texTile0;
uniform vec2 texTile1;

out vec3 FragPos;
out vec2 texCoord0;
out vec2 texCoord1;

void main()
{
	texCoord0 = vec2(texCoord.x + texTile0.x, texCoord.y + texTile0.y);
	texCoord1 = vec2(texCoord.x + texTile1.x, texCoord.y + texTile1.y);
    FragPos = vec3(model * vec4(aPos, 1.0));
	gl_Position = projection * view * vec4(FragPos, 1.0);
}