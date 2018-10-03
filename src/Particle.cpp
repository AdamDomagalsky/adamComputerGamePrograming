#include "Particle.h"


inline Particla::Particla(glm::vec3 pos, float width, float height, float depth) {
	this->pos = pos;
	this->width = width;
	this->height = height;
	this->depth = depth;
	
	this->partID = this->idCounter++;
}

inline Particla::Particla(glm::vec3 ParticlaDir, glm::vec3 pos, float width, float height, float depth) {
	this->pos = pos;
	this->width = width;
	this->height = height;
	this->depth = depth;
	this->ParticlaDir = ParticlaDir;
	this->partID = this->idCounter++;
}

inline glm::vec3 Particla::getShipColor() {
	return shipColor;
}

inline glm::vec3 Particla::getParticlaDir() {
	return ParticlaDir;
}

inline float Particla::getMinX() {
	return std::min(pos.x, pos.x + width);
}

inline float Particla::getMinY() {
	return std::min(pos.y, pos.y + height);
}

inline float Particla::getMinZ() {
	return std::min(pos.z, pos.z + depth);
}

inline float Particla::getMaxX() {
	return std::max(pos.x, pos.x + width);
}

inline float Particla::getMaxY() {
	return std::max(pos.y, pos.y + height);
}

inline float Particla::getMaxZ() {
	return std::max(pos.z, pos.z + depth);
}

inline void Particla::setColorColided(bool isColliding) {
	if (isColliding)
		shipColor = glm::vec3(1.0, 0, 0);
	else
		shipColor = glm::vec3(0.5, 0.5, 0.5);
}

inline bool Particla::intersect(Particla that) {
	//printf("A X%d\n", pos.x);
	//printf("A Y%d\n", pos.y);

	return (getMinX() <= that.getMaxX() && getMaxX() >= that.getMinX()) &&
		(getMinY() <= that.getMaxY() && getMaxY() >= that.getMinY()) &&
		(getMinZ() <= that.getMaxZ() && getMaxZ() >= that.getMinZ());
}

int Particla::idCounter = 0;
