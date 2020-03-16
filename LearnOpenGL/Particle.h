#pragma once

#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "glm\glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Particle
{
public:
	glm::vec3 position;
	float size;
	glm::vec3 velocity;
	glm::vec3 gravity;
	float rotation;
	float maxLifeTime;
	float lifeTime = 0.0f;	//musze to sprawdzac z zewnatrz

	float actRotation = 0.0f;

	Particle(glm::vec3 pos, float siz, glm::vec3 vel, glm::vec3 grav, float rot, float maxLife);
	~Particle();

	void Update(float deltaTime);

private:

};