#pragma once

#include <glm.hpp>
#include <algorithm>

class Particle2
{
private:
	float width;
	float height;
	float depth;
	glm::vec3 shipColor = glm::vec3(0.5, 0.5, 0.5);
	static int idCounter;
	
public:
	glm::vec3 Particle2Dir;
	float bornTime = 0;
	float Particle2Life = 2000;
	int partID;
	glm::vec3 pos;
	glm::vec3 vel;
	//glm::quat rot;
	glm::vec3 rot;

	Particle2(glm::vec3 pos, float width, float height, float depth);
	Particle2(glm::vec3 Particle2Dir, glm::vec3 pos, float width, float height, float depth);
	glm::vec3 getShipColor();
	glm::vec3 getParticle2Dir();
	float getMinX();

	float getMinY();

	float getMinZ();

	float getMaxX();

	float getMaxY();

	float getMaxZ();

	void setColorColided(bool isColliding);

	bool intersect(Particle2  b);
};