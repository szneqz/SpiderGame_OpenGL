#pragma once

#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "glm\glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <math.h>

#include "MapGenerator.h"
#include "Model.h"

class Spider
{
public:
	glm::vec3 position = glm::vec3(0.0f);
	glm::vec3 forward = glm::vec3(0.0f);
	glm::vec3 upward = glm::vec3(0.0f);
	glm::vec3 right = glm::vec3(0.0f);
	glm::vec3 mainScale = glm::vec3(0.0f);
	//Bazooka* bazooka = nullptr;
	MapGenerator* mainMap;
	Model* mainModel;
	float accel = 4.0f;
	float maxVelocity = 5.0f;
	bool isActive = false;
	bool isRunning = false;
	bool isDead = false;

	Spider();
	~Spider();
	
	glm::mat4 getModel(glm::vec3 pos, glm::vec3 targVec, glm::vec3 scale, float deltaTime);
	void move(int dir, float deltaTime, glm::vec3 camForw, glm::vec3 camRight);

private:
	float actVel = 0.0f;
	glm::vec3 actNormal = glm::vec3(0.0f, 1.0f, 0.0f);

	glm::mat4 spiderLookAt(glm::vec3 targetPos, glm::vec3 WorldUpVec);
};