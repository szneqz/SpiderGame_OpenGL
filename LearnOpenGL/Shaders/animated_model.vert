#version 330 core

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_text_coords;
layout(location = 3) in ivec4 bone_ids;     // INT pointer
layout(location = 4) in vec4 weights;

out vec3 FragPos;  
out vec3 Normal;  
out vec2 TexCoords;

uniform mat4 MVP;
uniform mat4 M_matrix;
uniform mat4 normals_matrix;

const int MAX_BONES = 100;
uniform mat4 bones[MAX_BONES];

void main()
{
	mat4 bone_transform = bones[bone_ids[0]] * weights[0];
		bone_transform += bones[bone_ids[1]] * weights[1];
		bone_transform += bones[bone_ids[2]] * weights[2];
		bone_transform += bones[bone_ids[3]] * weights[3];
			
	vec4 boned_position = bone_transform * vec4(in_position, 1.0); // transformed by bones

	Normal = normalize(vec3(normals_matrix * (bone_transform * vec4(in_normal, 0.0))));

	FragPos = vec3(M_matrix * boned_position);
	TexCoords = in_text_coords;

	gl_Position = MVP * boned_position;
}