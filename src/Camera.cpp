#include "Camera.h"

float Camera::yaw = 0;
float Camera::pitch = 0;
float Camera::roll = 0;

glm::mat4 Core::createPerspectiveMatrix(float zNear, float zFar)
{
	const float frustumScale = 1.1f;
	glm::mat4 perspective;
	perspective[0][0] = frustumScale;
	perspective[1][1] = frustumScale;
	perspective[2][2] = (zFar + zNear) / (zNear - zFar);
	perspective[3][2] = (2 * zFar * zNear) / (zNear - zFar);
	perspective[2][3] = -1;
	perspective[3][3] = 0;

	return perspective;
}

glm::mat4 Core::createViewMatrix(glm::vec3 position, float yaw, float pitch, float roll)
{
	glm::mat4 Rx, Ry, Rz, M, T;
	glm::mat4 Id = glm::mat4();

	glm::quat qPitch = glm::angleAxis(pitch, glm::vec3(1, 0, 0));
	glm::quat qYaw = glm::angleAxis(yaw, glm::vec3(0, 1, 0));
	glm::quat qRoll = glm::angleAxis(roll, glm::vec3(0, 0, 1));

	glm::quat rotQuat = qPitch * qYaw * qRoll;

	glm::mat4 RotMatrix = glm::mat4_cast(rotQuat);

	T = glm::translate(Id, -position - 5.0f);
	M = RotMatrix * T;

	return M;
}


glm::mat4 Core::createViewMatrixQuat(glm::vec3 position, glm::quat rotation)
{
	glm::mat4 cameraTranslation;
	cameraTranslation[3] = glm::vec4(-position, 1.0f);

	return glm::mat4_cast(rotation) * cameraTranslation;
}
