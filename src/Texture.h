#pragma once

#include "glew.h"
#include "freeglut.h"

namespace Core
{
	GLuint LoadTexture(const char * filepath);
	GLuint LoadTextureCM(const char * filepath);
	GLuint setupCubeMap(const char *xpos, const char *xneg, const char *ypos, const char *yneg, const char *zpos, const char *zneg);

	
	void SetActiveTexture(GLuint textureID, const char * shaderVariableName, GLuint programID, int textureUnit);
}