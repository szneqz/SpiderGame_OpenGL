#include "Arrow.h"

Arrow::Arrow(unsigned int width, unsigned int height)
{
	scrW = width;
	scrH = height;
	calcVect();
}

Arrow::~Arrow()
{

}

void Arrow::calcVect()
{	//losowy wektor i losowa sila
	targVer = glm::normalize(glm::vec3(rand() - RAND_MAX / 2, rand() - RAND_MAX / 2, rand() - RAND_MAX / 2));
	str = (float)((float)rand() / (float)RAND_MAX) * 3.0f;
}

glm::mat4 Arrow::getModel()
{
	forward = targVer * str;	//sila wiatru!

	glm::mat4 model(1.0f);
	glm::vec3 position = mainCamera->Position + (mainCamera->Front * 0.8f);
	model = glm::lookAt(position, position - targVer, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::inverse(model);
	model = glm::scale(model, glm::vec3(0.2f));

	return model;
}

void Arrow::draw(glm::mat4 model, glm::mat4 MV, Shader* shader)
{
	glViewport((unsigned int)(scrW * 0.8f), (unsigned int)(scrH * 0.8f), (unsigned int)(scrW * 0.2f), (unsigned int)(scrH * 0.2f));

	model = getModel();

	shader->setMat4("MVP", MV * model);
	shader->setMat4("M_matrix", model);
	glm::mat4 matr_normals_cube = glm::mat4(glm::transpose(glm::inverse(model)));
	shader->setMat4("normals_matrix", matr_normals_cube);

	mainModel->draw(shader->ID);

	glViewport(0, 0, scrW, scrH);
}