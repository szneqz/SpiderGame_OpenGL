#pragma once

#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "glm\glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Model.h"
#include "Shader.h"
#include "Spider.h"
#include "MapGenerator.h"
#include "Bazooka.h"
#include "Camera.h"

#include <vector>

class GameController
{
public:
	std::vector<Spider*> spiders;	//Tablica pajakow
	std::vector<Bazooka*> bazookas;	//Tablica broni

	GameController(int number, MapGenerator* map, Model* myModel, Model* bazookaModel);
	~GameController();
	int getAcitveSpider();
	void nextSpider();
	void Draw(Shader* shader, glm::mat4 VP, float deltaTime, Camera* camera);

private:
	glm::vec2 bounds[2] = 
	{
		glm::vec2(10.0f, 10.0f),
		glm::vec2(90.0f, 90.0f)
	};
	int activeSpider = 0;

	void randomPositioning();
};