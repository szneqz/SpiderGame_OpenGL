#include "Rocket.h"

Rocket::Rocket()
{

}

Rocket::~Rocket()
{

}

void Rocket::shoot(glm::vec3 forward)
{	//wystrzeliwuje
	if (isFlying == false && actWaitTime == 0)
	{
		isFlying = true;
		velocity = forward * str;
		smoke->isActive = true;

		gameContr->nextSpider();	//po strzale mam nowego gracza
	}
}

void Rocket::setPosition(Bazooka* target)
{
	bazooka = target;
}

glm::mat4 Rocket::getModel(float deltaTime, glm::vec3 forward)
{
	if (bazooka != gameContr->bazookas[gameContr->getAcitveSpider()])
	{
		bazooka = gameContr->bazookas[gameContr->getAcitveSpider()];
		actWaitTime = waitTime;
	}

	glm::vec3 actScale = bazooka->mainScale;

	if (isFlying)	//obliczamy kolejna pozycje poprzez deltaTime, gdy leci
	{
		position = calcNextPos(deltaTime);
		forward = glm::normalize(velocity);

		checkForGround();
	}
	else
	{
		if (actWaitTime == 0.0f)
		{
			position = bazooka->position;
		}
		else
		{
			forward = glm::normalize(velocity);
			actScale = glm::vec3(0.0f, 0.0f, 0.0f);
			wait(deltaTime);
		}
	}

	glm::mat4 model(1.0f);
	model = glm::lookAt(position, position - forward, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::inverse(model);
	model = glm::scale(model, actScale);

	return model;
}

void Rocket::checkForGround()
{
	glm::vec3 normal;
	float h = map->getHeight(glm::vec2(position.x, position.z), normal);
	if (position.y <= h || position.y <= -0.1f)
	{
		actWaitTime = waitTime;	//ustawiam oczekwianie na koniec ruchu
		isFlying = false;
		map->makeHole(glm::vec3(position.x, h, position.z), radius);
		arrow->calcVect();
		smoke->isActive = false;	//wylacz smoke

		lightStr = 1.0f;	//blysk

		explosion->position = position;
		explosion->singleUse();
		explosion->singleUse();
		explosion->singleUse();		//potrójny wybuch, bo ³adnie wygl¹da XD
	}
}

glm::vec3 Rocket::calcNextPos(float deltaTime)
{
	glm::vec3 gravity(0.0f, -4.9f, 0.0f);
	velocity = (velocity + gravity * deltaTime + arrow->forward * deltaTime);

	return position + (velocity * deltaTime);
}

void Rocket::wait(float deltaTime)
{
	actWaitTime -= deltaTime;
	if (actWaitTime < 0.0f)
		actWaitTime = 0.0f;
}

void Rocket::decLightStr(float deltaTime, float factor)
{	//blysk przy wybuchu
	lightStr -= deltaTime * factor;

	if (lightStr < 0.0f)
		lightStr = 0.0f;
}