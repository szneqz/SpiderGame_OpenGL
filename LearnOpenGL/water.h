#pragma once

#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "glm\glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "stb_image.h"
#include "Shader.h"

#include <vector>
#include <math.h>

class water
{
public:
	water(Shader* shad, const char* image_path, const char* image_path2, float texSiz, float center, float velocity);
	~water();

	void Draw(glm::mat4 V, glm::mat4 P, float deltaTime);

private:
	//tablica verticow
	float vertices[32] = {
		-0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		-0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f
	};

	unsigned int indices[6]{
		0, 1, 2,
		2, 3, 0
	};

	float waterVelocity = 0.0f;
	float actTime = 0.0f;
	float centerLoc = 0.0f;
	float texSize = 1.0f;
	unsigned int texture = 0;
	unsigned int texture2 = 0;
	unsigned int VAO = 0;
	Shader* shader = nullptr;

	void init(const char* image_path, const char* image_path2);

};