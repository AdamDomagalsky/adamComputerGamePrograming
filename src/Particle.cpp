#include "Particle.h"


inline Particle2::Particle2(glm::vec3 pos, float width, float height, float depth) {
	this->pos = pos;
	this->width = width;
	this->height = height;
	this->depth = depth;
	
	this->partID = this->idCounter++;
}

inline Particle2::Particle2(glm::vec3 Particle2Dir, glm::vec3 pos, float width, float height, float depth) {
	this->pos = pos;
	this->width = width;
	this->height = height;
	this->depth = depth;
	this->Particle2Dir = Particle2Dir;
	this->partID = this->idCounter++;
}

inline glm::vec3 Particle2::getShipColor() {
	return shipColor;
}

inline glm::vec3 Particle2::getParticle2Dir() {
	return Particle2Dir;
}

inline float Particle2::getMinX() {
	return std::min(pos.x, pos.x + width);
}

inline float Particle2::getMinY() {
	return std::min(pos.y, pos.y + height);
}

inline float Particle2::getMinZ() {
	return std::min(pos.z, pos.z + depth);
}

inline float Particle2::getMaxX() {
	return std::max(pos.x, pos.x + width);
}

inline float Particle2::getMaxY() {
	return std::max(pos.y, pos.y + height);
}

inline float Particle2::getMaxZ() {
	return std::max(pos.z, pos.z + depth);
}

inline void Particle2::setColorColided(bool isColliding) {
	if (isColliding)
		shipColor = glm::vec3(1.0, 0, 0);
	else
		shipColor = glm::vec3(0.5, 0.5, 0.5);
}

inline bool Particle2::intersect(Particle2 that) {
	//printf("A X%d\n", pos.x);
	//printf("A Y%d\n", pos.y);

	return (getMinX() <= that.getMaxX() && getMaxX() >= that.getMinX()) &&
		(getMinY() <= that.getMaxY() && getMaxY() >= that.getMinY()) &&
		(getMinZ() <= that.getMaxZ() && getMaxZ() >= that.getMinZ());
}

int Particle2::idCounter = 0;
