#pragma once

#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "glm\glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

#include "Shader.h"
#include "Particle.h"
#include "stb_image.h"

class ParticleSys
{
public:
	float spawnDelay;
	glm::vec3 position;
	float minSize;
	float maxSize;
	glm::vec3 velocity;
	float error;
	glm::vec3 gravity;
	float rotation;
	float maxLifeTime;
	bool isActive = true;

	ParticleSys(float delay, glm::vec3 pos, float mSize, float mxSize, glm::vec3 vel, float err, glm::vec3 grav, float rot, float maxLife, Shader* sh, int tTil, int nrTil);
	~ParticleSys();

	void singleUse();
	void Update(float deltaTime);
	void init(const char* image_path);
	void Draw(glm::mat4 V, glm::mat4 P, glm::vec3 camPos);

private:
	float actTime = 0.0f;
	std::vector<Particle*> particles;
	unsigned int texture = 0;
	int texTiles = 0;
	int nrTiles = 0;
	unsigned int VAO = 0;
	Shader* shader;

	//tablica verticow
	float vertices[20] = {
		-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
		-0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
	};

	unsigned int indices[6]{
		0, 1, 2,
		2, 3, 0
	};
};