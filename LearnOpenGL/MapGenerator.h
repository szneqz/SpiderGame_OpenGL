#pragma once

#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "glm\glm.hpp"
#include "stb_image.h"
#include "Shader.h"

#include <vector>
#include <math.h>

class MapGenerator
{
public:
	MapGenerator();
	MapGenerator(int tsize, float factor, float maxHeight, float minStep, float maxStep, int nrHills);
	~MapGenerator();

	void loadImageToTexture(const char* image_path_diff, const char* image_path_spec);
	void Draw();
	void initShader(Shader shader);
	float getHeight(glm::vec2 pos, glm::vec3 & normal);
	void makeHole(glm::vec3 pos, float radius);
	float isHit(glm::vec3 origin, glm::vec3 vector, float skip);

private:
	std::vector<float> heights;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec3> interNormals;
	std::vector<float> vertexData;
	unsigned int diffTexture = 0;
	unsigned int specTexture = 0;
	unsigned int VAO = 0;
	int size = 0;

	void calcStraightNormals();
	void AroundHill(int point, float step, float actHeight);
	void Border();
	void calcVertexData(float factor);
	glm::vec3 calcNormals(glm::vec3 vect1, glm::vec3 vect2, glm::vec3 vect3);
	void init();

};