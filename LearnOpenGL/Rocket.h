#pragma once

#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "glm\glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Model.h"
#include "Shader.h"
#include "Bazooka.h"
#include "Arrow.h"
#include "ParticleSys.h"
#include "GameController.h"

class Rocket
{
public:
	Model* mainModel = nullptr;
	Arrow* arrow = nullptr;
	MapGenerator* map = nullptr;
	GameController* gameContr = nullptr;
	glm::vec3 position = glm::vec3(0.0f);
	ParticleSys* smoke = nullptr;
	ParticleSys* explosion = nullptr;
	float lightStr = 0.0f;

	Rocket();
	~Rocket();

	void shoot(glm::vec3 forward);
	void setPosition(Bazooka* target);
	glm::mat4 getModel(float deltaTime, glm::vec3 forward);
	void decLightStr(float deltaTime, float factor);

private:
	float str = 10.0f;
	float radius = 3.0f;
	float waitTime = 2.0f;
	float actWaitTime = 0.0f;
	glm::vec3 velocity = glm::vec3(0.0f);
	bool isFlying = false;
	Bazooka* bazooka = nullptr;

	void checkForGround();
	glm::vec3 calcNextPos(float deltaTime);
	void wait(float deltaTime);
};