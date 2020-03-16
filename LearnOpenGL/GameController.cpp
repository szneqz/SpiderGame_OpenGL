#include "GameController.h"

GameController::GameController(int number, MapGenerator* map, Model* myModel, Model* bazookaModel)
{
	for (int i = 0; i < number; i++)
	{
		spiders.push_back(new Spider);
		spiders[i]->mainMap = map;
		spiders[i]->mainScale = glm::vec3(0.2f, 0.2f, 0.2f);
		spiders[i]->mainModel = myModel;	//referencja na model

		bazookas.push_back(new Bazooka(bazookaModel, spiders[i]));
	}

	randomPositioning();
}

GameController::~GameController()
{

}

void GameController::Draw(Shader* shader, glm::mat4 VP, float deltaTime, Camera* camera)
{
	for (int i = 0; i < spiders.size(); i++)
	{
		//bazooka
		glm::mat4 model(1.0f);

		if(i == activeSpider)
			model = bazookas[i]->getModel(-camera->Front);	//aktualnie grajacy ma zwrocona bazooke wraz z kamera
		else
			model = bazookas[i]->getModel(spiders[i]->forward);	//pasywni gracze maja bazooki wzdluz ciala

		glm::mat4 MVP = VP * model;
		shader->setMat4("MVP", MVP);
		shader->setMat4("M_matrix", model);
		glm::mat4 matr_normals_cube = glm::mat4(glm::transpose(glm::inverse(model)));
		shader->setMat4("normals_matrix", matr_normals_cube);

		bazookas[i]->model->draw(shader->ID);

		//pajunk
		model = spiders[i]->getModel(spiders[i]->position, spiders[i]->forward, spiders[i]->mainScale, deltaTime);

		MVP = VP * model;
		shader->setMat4("MVP", MVP);
		shader->setMat4("M_matrix", model);
		matr_normals_cube = glm::mat4(glm::transpose(glm::inverse(model)));
		shader->setMat4("normals_matrix", matr_normals_cube);

		//ustawienia animacji zaleznie od tego czy biegaja czy nie
		if (!spiders[i]->isRunning)
			spiders[i]->mainModel->setCurrentAnimation(0);
		else
			spiders[i]->mainModel->setCurrentAnimation(1);

		spiders[i]->mainModel->draw(shader->ID);
	}

	//jezeli umre podczas biegania to juz koniec
	if (spiders[activeSpider]->isDead)
		nextSpider();
}

int GameController::getAcitveSpider()
{
	return activeSpider;
}

void GameController::randomPositioning()
{
	for (int i = 0; i < spiders.size(); i++)
	{
		spiders[i]->position = glm::vec3((((float)rand() / RAND_MAX) * (bounds[1].x - bounds[0].x) + bounds[0].x), 0.0f, (((float)rand() / RAND_MAX) * (bounds[1].y - bounds[0].y) + bounds[0].y));
		spiders[i]->getModel(spiders[i]->position, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(spiders[i]->mainScale), 1.0f);
	}
}

void GameController::nextSpider()
{
	for (int i = 0; i < spiders.size(); i++)
	{
		activeSpider++;
		if (activeSpider >= spiders.size())
		{
			activeSpider = 0;
		}

		if(spiders[activeSpider]->isDead)
			continue;

		break;
	}

}