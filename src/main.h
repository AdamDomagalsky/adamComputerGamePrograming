#pragma once

glm::mat4 createRotationMatrix(float time) {
	glm::mat4 rotationMatrix;
	rotationMatrix[0][0] = cos(time);
	rotationMatrix[0][2] = -sin(time);
	rotationMatrix[2][0] = sin(time);
	rotationMatrix[2][2] = cos(time);
	return rotationMatrix;
}

glm::mat4 createTranslationMatrixXYZ(float X, float Y, float Z) {
	glm::mat4 translationMatrix;
	translationMatrix[3][0] = X;
	translationMatrix[3][1] = Y;
	translationMatrix[3][2] = Z;
	return translationMatrix;
}

//find distance between two points
float find_distance(glm::vec3 A, glm::vec3 B) {
	float dx, dy, dz, d;
	dx = A.x - B.x;
	dy = A.y - B.y;
	dz = A.z - B.z;
	d = pow(dx, 2) + pow(dy, 2) + pow(dz, 2);
	return sqrt(d);
}

std::vector<glm::vec3> generatePoints(glm::vec4 v1, glm::vec4 v2, glm::vec4 v3, glm::vec4 v4, float diff) {
	std::vector<glm::vec3> points;
	for (float i = 0; i < 1; i += diff) {
		glm::vec3 catmullVec = glm::catmullRom(v1, v2, v3, v4, i);
		points.push_back(catmullVec);
		//printf("hermite vect %f %f %f\n", catmullVec.x, catmullVec.y, catmullVec.z);
	}
	printf("size %d generated", points.size());
	return points;
}
