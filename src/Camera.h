#pragma once

#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/quaternion.hpp"

namespace Core
{
	glm::mat4 createPerspectiveMatrix(float zNear = 0.1f, float zFar = 100.0f);
	glm::mat4 createViewMatrix(glm::vec3 position, float yaw, float pitch, float roll);
	glm::mat4 createViewMatrixQuat(glm::vec3 position, glm::quat rotation);
}

class Camera
{
public:
	static float yaw, pitch, roll;
};
