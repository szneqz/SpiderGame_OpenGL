#pragma once

#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "glm\glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Model.h"
#include "Camera.h"
#include "Shader.h"

class Arrow
{
public:
	float str = 1.5f;
	glm::vec3 forward = glm::vec3(0.0f);	//sila wiatru (nieznormalizowana)

	Model* mainModel = nullptr;
	Camera* mainCamera = nullptr;

	unsigned int scrH = 0;
	unsigned int scrW = 0;

	Arrow(unsigned int width, unsigned int height);
	~Arrow();

	glm::mat4 getModel();
	void draw(glm::mat4 model, glm::mat4 MVP, Shader* shader);
	void calcVect();

private:
	glm::vec3 targVer = glm::vec3(0.0f);
};