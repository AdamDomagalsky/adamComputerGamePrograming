#pragma once

#include <glm.hpp>
#include <algorithm>

class Particla
{
private:
	float width;
	float height;
	float depth;
	glm::vec3 shipColor = glm::vec3(0.5, 0.5, 0.5);
	static int idCounter;
	
public:
	glm::vec3 ParticlaDir;
	float bornTime = 0;
	float ParticlaLife = 2000;
	int partID;
	glm::vec3 pos;
	glm::vec3 vel;
	//glm::quat rot;
	glm::vec3 rot;

	Particla(glm::vec3 pos, float width, float height, float depth);
	Particla(glm::vec3 ParticlaDir, glm::vec3 pos, float width, float height, float depth);
	glm::vec3 getShipColor();
	glm::vec3 getParticlaDir();
	float getMinX();

	float getMinY();

	float getMinZ();

	float getMaxX();

	float getMaxY();

	float getMaxZ();

	void setColorColided(bool isColliding);

	bool intersect(Particla  b);
};