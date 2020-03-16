#include "Particle.h"

Particle::Particle(glm::vec3 pos, float siz, glm::vec3 vel, glm::vec3 grav, float rot, float maxLife)
{
	position = pos;
	size = siz;
	velocity = vel;
	gravity = grav;
	rotation = rot;
	maxLifeTime = maxLife;
}

Particle::~Particle()
{

}

void Particle::Update(float deltaTime)
{
	velocity = velocity + gravity * deltaTime;
	position = position + velocity * deltaTime;
	actRotation = actRotation + rotation * deltaTime;
	lifeTime += deltaTime;
}