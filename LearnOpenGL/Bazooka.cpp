#include "Bazooka.h"


Bazooka::Bazooka()
{

}

Bazooka::Bazooka(Model* Bmodel, Spider* Bspider)
{
	model = Bmodel;
	spider = Bspider;
}

Bazooka::~Bazooka()
{

}

glm::mat4 Bazooka::getModel(glm::vec3 targVer)
{
	glm::mat4 model(1.0f);
	position = spider->position + spider->upward * 0.25f;
	model = glm::lookAt(position, position + targVer, spider->upward);
	model = glm::inverse(model);
	model = glm::scale(model, spider->mainScale);
	mainScale = spider->mainScale;

	return model;
}

glm::mat4 Bazooka::bazookaLookAt(glm::vec3 targetVec, glm::vec3 WorldUpVec)
{	//prywatny lookAt
	targetVec = glm::normalize(targetVec);
	glm::vec3 rightVec = glm::normalize(glm::cross(glm::normalize(WorldUpVec), targetVec));	//wektor prawy kamery
	glm::vec3 upVec = glm::cross(targetVec, rightVec);										//wektor gory kamery
	glm::mat4 lookAt = glm::mat4(1.0f);														//macierz poszczegolnych obrotow
	lookAt[0][0] = rightVec.x;	//pierwsza kolumna, pierwszy rzad
	lookAt[1][0] = rightVec.y;	//druga kolumna, pierwszy rzad
	lookAt[2][0] = rightVec.z;	//trzecia kolumna, pierwszy rzad
	lookAt[0][1] = upVec.x;		//pierwsza kolumna, drugi rzad
	lookAt[1][1] = upVec.y;
	lookAt[2][1] = upVec.z;
	lookAt[0][2] = targetVec.x;	//pierwsza kolumna trzeci rzad
	lookAt[1][2] = targetVec.y;
	lookAt[2][2] = targetVec.z;

	return lookAt;
}