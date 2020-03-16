#pragma once

#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "glm\glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Model.h"
#include "Spider.h"

class Bazooka
{
public:
	glm::vec3 position = glm::vec3(0.0f);
	glm::vec3 mainScale = glm::vec3(0.0f);
	Model* model = nullptr;

	Bazooka();
	Bazooka(Model* Bmodel, Spider* Bspider);
	~Bazooka();

	glm::mat4 getModel(glm::vec3 targVer);

private:
	Spider* spider = nullptr;

	glm::mat4 bazookaLookAt(glm::vec3 targetVec, glm::vec3 WorldUpVec);

};