#include "glew.h"
#include "glm.hpp"
#include "ext.hpp"
#include <iostream>
#include <cmath>
#include <windows.h>
#include <string.h>
#include <cmath>
#include <vector>
#include "Shader_Loader.h"
#include "Render_Utils.h"
#include "Camera.h"
#include "Texture.h"
#include "main.h"
#include "cubeArr.hpp"

#define min(a,b)            (((a) < (b)) ? (a) : (b))
#define max(a,b)            (((a) > (b)) ? (a) : (b))

glm::quat rotation = glm::quat(1, 0, 0, 0);
glm::vec3 rotationChangeXYZ = glm::vec3(0, 0, 0);
glm::vec3 cameraSide;


class Particla
{
private:
	float width;
	float height;
	float depth;
	glm::vec3 shipColor = glm::vec3(0.5, 0.5, 0.5);
	static int counterID;
public:
	glm::vec3 ParticlaDir;
	float bornTime = 0;
	float ParticlaLife = 2000;
	int partID;
	glm::vec3 pos;
	glm::vec3 vel;
	glm::quat rot;

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

void handleCollisions();
bool FPS = true;

GLuint 
	programColor,
	programTexture,
	programSkybox,
	programDepth,
	programShadow,
	programTextureNorm,
	programTextureBasic,
	depthTexture,
	textureEarth,
	textureEarthNormal,
	textureAsteroid,
	cubemapTexture,
	FramebufferObject;


const int font = (int)GLUT_BITMAP_9_BY_15;
int killed = 0, w, h, cubeMapID, sign = 1, pointCounter = 0, indexPath = 0;//t % shipPath.size();

char s[30];
double t;

std::vector<float> tangent(1203);
std::vector<glm::vec4> planets;
std::vector<glm::vec3> coins, shipPath;
bool state = true;

Core::Shader_Loader shaderLoader;


obj::Model 
	shipModel,
	sphereModel,
	renderModel,
	coinModel;


//Camera
float cameraAngleX = 4.7, cameraAngleY = 5,
	yaw = 0.0, pitch = 0.0, roll = 0.0, 
	oldX = 0.0, oldY = 0.0, cameraAngle = 0.0;

glm::vec3 cameraPos(-5, -1, 10), cameraDir(0.0f),
	ship_pos,
	points[220],
	lightDir = glm::normalize(glm::vec3(1.0f, -0.9f, -1.0f)),
	circle_points[220];//wektor punkt�w do naszej krzywej


glm::mat4 cameraMatrix(0.0f),
	lightProjection, 
	lightView,
	perspectiveMatrix(0.0f), 
	rotations[220]; //macierz rotacji BNT - wzd�u� krzywej



//nasze pi�kne cz�steczki
struct Particle {
	glm::vec3 pos;
	glm::quat rot;

	glm::vec3 vel; //linMom??
	glm::vec3 angVel; //AngMom??

	glm::vec3 force;
	glm::vec3 torque;

	glm::mat4 shipDepthModel;
};

std::vector<Particle> spaceships;

std::vector<Particla> spaceshipa;
std::vector<Particla> explodeParticlas;
std::vector<Particla> bullets;


void mouseMove(int x, int y)
{
	float sen = 0.05;
	float margin = 5;

	if (x - margin > oldX)
	{
		yaw += sen;
		cameraAngleX += sen;
	}
	else if (x + margin < oldX)
	{
		yaw -= sen;
		cameraAngleX -= sen;
	}

	if (y - margin > oldY)
	{
		pitch += sen;
		cameraAngleY += sen;
	}
	else if (y + margin < oldY)
	{
		pitch -= sen;
		cameraAngleY -= sen;
	}

	rotationChangeXYZ.y = 0.1f*(x - oldX);
	rotationChangeXYZ.x = 0.1f*(y - oldY);

	oldX = x;
	oldY = y;
}

void keyboard(unsigned char key, int x, int y)
{
	float angleSpeed = 0.1f;
	float moveSpeed = 0.1f;
	float moveSpeedUpDown = 0.3f;
	switch(key)
	{
	case 'p': FPS = !FPS; break;
	case 'z': cameraAngle -= angleSpeed; rotationChangeXYZ.z += 0.1f; break;
	case 'x': cameraAngle += angleSpeed; rotationChangeXYZ.z -= 0.1f; break;
	case 'w': cameraPos += cameraDir * moveSpeed; break;
	case 's': cameraPos -= cameraDir * moveSpeed; break;
	case 'd': cameraPos += glm::cross(cameraDir, glm::vec3(0,1,0)) * moveSpeed; break;
	case 'a': cameraPos -= glm::cross(cameraDir, glm::vec3(0,1,0)) * moveSpeed; break;
	case 'c': cameraPos += glm::cross(cameraDir, glm::vec3(1, 0, 0)) * moveSpeedUpDown; break;
	case 'v': cameraPos -= glm::cross(cameraDir, glm::vec3(1, 0, 0)) * moveSpeedUpDown; break;
	case (char)32:
		int shipWidth = 1;
		int shipHeight = 1;
		int shipDepth = 1;
		Particla bullet(cameraDir, cameraPos + (cameraDir * 3), shipWidth, shipHeight, shipDepth);
		bullets.push_back(bullet);
		break;
	}
}


glm::mat4 createCameraMatrix()
{
	if (FPS) {

		glm::quat rotationChange = glm::quat(rotationChangeXYZ);
		rotation = rotationChange * rotation;
		rotation = glm::normalize(rotation);
		rotationChangeXYZ = glm::vec3(0);
		cameraDir = glm::inverse(rotation) * glm::vec3(0, 0, -1);
		cameraSide = glm::inverse(rotation) * glm::vec3(1, 0, 0);

		return Core::createViewMatrixQuat(cameraPos, rotation);
	}
	else {
		cameraDir = glm::vec3(cosf(cameraAngleX), 0.0f, sinf(cameraAngleX));
		
		return Core::createViewMatrix(cameraPos, yaw, pitch, roll);
	}
	
}

void drawObjectColor(obj::Model * model, glm::mat4 modelMatrix, glm::vec3 color)
{
	GLuint program = programColor;

	glUseProgram(program);

	glUniform3f(glGetUniformLocation(program, "objectColor"), color.x, color.y, color.z);
	glUniform3f(glGetUniformLocation(program, "lightDir"), lightDir.x, lightDir.y, lightDir.z);

	glm::mat4 transformation = perspectiveMatrix * cameraMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "modelViewProjectionMatrix"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);

	Core::DrawModel(model);

	glUseProgram(0);
}
void drawObjectTexture(obj::Model * model, glm::mat4 modelMatrix, GLuint textureId)
{
	GLuint program = programTexture;

	glUseProgram(program);

	glUniform3f(glGetUniformLocation(program, "lightDir"), lightDir.x, lightDir.y, lightDir.z);
	glUniform3f(glGetUniformLocation(program, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);
	Core::SetActiveTexture(textureId, "textureSampler", program, 0);

	glm::mat4 transformation = perspectiveMatrix * cameraMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "modelViewProjectionMatrix"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);

	Core::DrawModel(model);

	glUseProgram(0);
}
void drawObjectDepth(obj::Model * model, glm::mat4 modelMatrix, glm::mat4 projMatrix, glm::mat4 inverseLightMatrix)
{
	GLuint program = programDepth;

	glUseProgram(program);

	glm::mat4 transformation = projMatrix * inverseLightMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "depthMVP"), 1, GL_FALSE, (float*)&transformation);

	Core::DrawModel(model);

	glUseProgram(0);
}
void drawObjectShadow(obj::Model * model, glm::mat4 modelMatrix, glm::mat4 projMatrix, glm::mat4 lightMatrix, GLuint textureId, GLuint depthTexture)
{
	GLuint program = programShadow;

	glUseProgram(program);

	glUniform3f(glGetUniformLocation(program, "lightDir"), lightDir.x, lightDir.y, lightDir.z);
	glUniform3f(glGetUniformLocation(program, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);
	Core::SetActiveTexture(textureId, "textureSampler", program, 0);
	Core::SetActiveTexture(depthTexture, "shadowMap", program, 1);

	glm::mat4 transformation = perspectiveMatrix * cameraMatrix * modelMatrix;
	glm::mat4 lightTransformation = projMatrix * lightMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "modelViewProjectionMatrix"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);
	glUniformMatrix4fv(glGetUniformLocation(program, "lightMatrix"), 1, GL_FALSE, (float*)&lightTransformation);

	Core::DrawModel(model);

	glUseProgram(0);
}
void drawObjectTextureBasic(obj::Model * model, glm::mat4 modelMatrix, GLuint textureId)
{
	GLuint program = programTextureBasic;

	glUseProgram(program);

	glUniform3f(glGetUniformLocation(program, "lightDir"), lightDir.x, lightDir.y, lightDir.z);
	glUniform3f(glGetUniformLocation(program, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);
	Core::SetActiveTexture(textureId, "textureSampler", program, 0);

	glm::mat4 transformation = perspectiveMatrix * cameraMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "modelViewProjectionMatrix"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);

	Core::DrawModel(model);

	glUseProgram(0);
}
void drawObjectTextureNormal(obj::Model * model, glm::mat4 modelMatrix, GLuint textureId, GLuint normalMap)
{
	GLuint program = programTextureNorm;

	glUseProgram(program);

	glUniform3f(glGetUniformLocation(program, "lightDir"), lightDir.x, lightDir.y, lightDir.z);
	glUniform3f(glGetUniformLocation(program, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);
	Core::SetActiveTexture(textureId, "textureSampler", program, 0);
	Core::SetActiveTexture(normalMap, "normalMap", program, 1);

	glm::mat4 transformation = perspectiveMatrix * cameraMatrix * modelMatrix;
	//glm::mat4 lightTransformation = projMatrix * lightMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "modelViewProjectionMatrix"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);
	//glUniformMatrix4fv(glGetUniformLocation(program, "lightMatrix"), 1, GL_FALSE, (float*)&lightTransformation);

	Core::DrawModelNormal(model);

	glUseProgram(0);
}
void drawSkybox(GLuint cubemapID) 
{
	GLuint program = programSkybox;

	glUseProgram(program);

	Core::SetActiveTexture(cubemapID, "skybox", program, 0);

	glm::mat4 transformation = perspectiveMatrix * cameraMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "modelViewProjectionMatrix"), 1, GL_FALSE, (float*)&transformation);


	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapID);

	Core::DrawVertexArray(cubeVertices, 36, 4);

	glUseProgram(0);
}

//wyznaczenie punkt�w na okr�gu
void drawCircle(GLfloat x, GLfloat y, GLfloat z, GLfloat radius, GLint numberOfSides)
{
	GLint numberOfVertices = numberOfSides + 1;

	GLfloat doublePi = 2.0f * 3.14f;

	GLfloat* circleVerticesX = new GLfloat[numberOfVertices];
	GLfloat* circleVerticesY = new GLfloat[numberOfVertices];
	GLfloat* circleVerticesZ = new GLfloat[numberOfVertices];

	for (int i = 0; i < numberOfVertices; i++)
	{
		circleVerticesX[i] = x + (radius * cos(i * doublePi / numberOfSides));
		circleVerticesY[i] = y;
		circleVerticesZ[i] = z + (radius * sin(i * doublePi / numberOfSides));
	}

	for (int i = 0; i < numberOfVertices; i++)
	{
		circle_points[i].x = circleVerticesX[i];
		circle_points[i].y = circleVerticesY[i];
		circle_points[i].z = circleVerticesZ[i];
	}
}

//wyznaczenie wektor�w BNT
void parallel_transport() {
	glm::vec3 tangent[220];
	glm::vec3 normal[220];
	glm::vec3 binormal[220];
	glm::vec3 T, N, B;
	for (int i = 0; i < 219; i++) {
		T = glm::normalize(circle_points[i + 1] - circle_points[i]);
		tangent[i] = T;
	}
	T = glm::normalize(circle_points[1] - circle_points[219]);
	tangent[219] = T;
	N = (tangent[0] / glm::length(tangent[0]));
	normal[0] = N;

	for (int i = 0; i < 219; i++)
	{
		B = glm::cross(tangent[i], tangent[i + 1]);
		if (glm::length(B) == 0)
			normal[i + 1] = normal[i];
		else {
			B = (B / glm::length(B));
			float theta = acosf(glm::dot(tangent[i],tangent[i+1]));
			normal[i + 1] =  glm::vec4(normal[i], 0.0f) * glm::rotate(theta, B);
		}
		binormal[i] = glm::cross(tangent[i], normal[i]);
		binormal[i] = glm::normalize(binormal[i]);
	}
	binormal[219] = glm::cross(tangent[219], normal[219]);
	binormal[219] = glm::normalize(binormal[219]);

	glm::mat3 BNT;
	for (int i = 0; i < 220; i++) {
		BNT[0] = binormal[i];
		BNT[1] = normal[i];
		BNT[2] = tangent[i];
		rotations[i] = BNT;
	}
}

void setOrthographicProjection() {
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, w, 0, h);
	glScalef(1, -1, 1);
	glTranslatef(0, -h, 0);
	glMatrixMode(GL_MODELVIEW);
}
void resetPerspectiveProjection() {
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}
void renderBitmapString(float x, float y, void *font, const char *string) {
	const char *c;
	glRasterPos2f(x, y);
	for (c = string; *c != '\0'; c++) {
		glutBitmapCharacter(font, *c);
	}
}

static void resize(int width, int height)
{
	const float ar = (float)width / (float)height;
	w = width;
	h = height;
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-ar, ar, -1.0, 1.0, 2.0, 100.0);     glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void update(int value) {
	t = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
	int time = (int)t;
	sprintf(s, "TIME : %2d Sec", time);
	glutTimerFunc(1000, update, 0);
	glutPostRedisplay();
}

int shipWidth = 1, shipHeight = 1, shipDepth = 1;
float WDH = 1.0f;

void initialise_particles(int qty)
{
	for (int i = 1; i <= 10; i++) {
		Particla enemy(glm::vec3(1 + i, i + 1, 1 + i), WDH, WDH, WDH);
		spaceshipa.push_back(enemy);
	}

	for (int i = 0; i < qty; i++) {

		Particle x;
		x.pos.x = -2.0 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (2.0 - (-2.0))));
		x.pos.y = -2.0 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (2.0 - (-2.0))));
		x.pos.z = -2.0 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (2.0 - (-2.0))));
		x.vel = glm::vec3(0, 0, 0);
		spaceships.push_back(x);
	}
}


int oldTimeSinceStart = 0;

void renderScene()
{
	glm::vec3 sum = glm::vec3(0);

	if (state && coins.size() != 0) {
		float time = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
		// Aktualizacja macierzy widoku i rzutowania. Macierze sa przechowywane w zmiennych globalnych, bo uzywa ich funkcja drawObject.
		// (Bardziej elegancko byloby przekazac je jako argumenty do funkcji, ale robimy tak dla uproszczenia kodu.
		//  Jest to mozliwe dzieki temu, ze macierze widoku i rzutowania sa takie same dla wszystkich obiektow!)
		cameraMatrix = createCameraMatrix();
		perspectiveMatrix = Core::createPerspectiveMatrix();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.0f, 0.3f, 0.3f, 1.0f);

		glColor3d(0.0, 0.0, 0.0);
		setOrthographicProjection();
		glPushMatrix();
		glLoadIdentity();
		//renderBitmapString(100, 100, (void *)font, "Coins: ");
		std::stringstream ss;
		ss << "Score: " << 10-coins.size() + killed << "/" << 10;
		std::string s = ss.str();
		const char * c = s.c_str();
		renderBitmapString(100, 100, (void *)font, c);
		glPopMatrix();
		resetPerspectiveProjection();


		// Macierz statku "przyczepia" go do kamery. Warto przeanalizowac te linijke i zrozumiec jak to dziala.
		if (indexPath == 0) {
			sign = 1;
			//printf("%s\n", "true");
		}
		else if (indexPath == shipPath.size() - 1) {
			//printf("%s\n", "false");
			sign = -1;
		}
		glm::mat4 mainShipModelMatrix = glm::translate(cameraPos + cameraDir * 0.5f + glm::vec3(5, 4.6, 4.7)) * glm::rotate(-cameraAngle + glm::radians(180.0f), glm::vec3(0, 1, 0)) * glm::scale(glm::vec3(0.25f));
		glm::vec3 mainShipPosition = mainShipModelMatrix[3];
		shipPath = generatePoints(glm::vec4(0, 0, 0, 0), glm::vec4(5, 0, 5, 0), glm::vec4(22, 0, 22, 0), glm::vec4(25, 0, 25, 0), 0.001);

		ship_pos = glm::vec3(circle_points[pointCounter % 220].x, circle_points[pointCounter % 220].y, circle_points[pointCounter % 220].z);
		//glm::mat4 shipModelMatrix = glm::translate(glm::vec3(ship_pos.x, ship_pos.y, ship_pos.z)) * rotations[pointCounter % 220];
		glm::mat4 shipModelMatrix = glm::translate(shipPath[indexPath]) * glm::rotate(glm::radians(90.0f), glm::vec3(0, 1, 0)) * glm::scale(glm::vec3(0.25f));

		drawObjectColor(&shipModel, shipModelMatrix, glm::vec3(0.7f, 0.0f, 0.0f));
		drawObjectColor(&shipModel, mainShipModelMatrix, glm::vec3(0.7f, 0.7f, 0.7f));
		pointCounter++;
		//Sleep(50);

		//dodanie naszych monetek
		for (int i = 0; i < coins.size(); i++)
		{
			glm::mat4 coinModelMatrix = glm::translate(coins[i]) * createRotationMatrix(time / 2) * glm::translate(glm::vec3(-3, 0, 0)) * glm::scale(glm::vec3(0.1f));
			float d = findDistance(coins[i], mainShipPosition + glm::vec3(0, -2.5, 0));
			if (d < 1)
				coins.erase(std::find(coins.begin(), coins.end(), coins[i]));
			else
				drawObjectColor(&coinModel, coinModelMatrix, glm::vec3(1.0f, 1.0f, 1.0f));
		}

		//przemieszczanie stateczk�w
		for (int i = 0; i < spaceships.size(); i++)
		{
			glm::mat4 shipModelMatrix = glm::translate(spaceships[i].pos) * rotations[pointCounter % 220] * glm::scale(glm::vec3(0.10f));
			spaceships[i].shipDepthModel = shipModelMatrix;
			drawObjectColor(&shipModel, shipModelMatrix, glm::vec3(0.0f, 0.0f, 0.7f));


			glm::vec3 v1, v2, v3;
			float m1 = 5;
			float m2 = 3;
			float m3 = 0.05;
			//v1 = rule1: centre_of_mass = ship_pos;
			v1 = glm::normalize(ship_pos - spaceships[i].pos);
			v1 = m1 * v1;
			//v2 = rule2: keep a distance away from other objects
			if (glm::length(ship_pos - spaceships[i].pos) < 2.5)
				spaceships[i].pos -= (ship_pos - spaceships[i].pos);
			for (int j = 0; j < spaceships.size(); j++) {
				v2 = glm::vec3(0, 0, 0);
				if (j != i) //boid which is near by
					if (glm::length(spaceships[j].pos - spaceships[i].pos) < 2)
						v2 -= (spaceships[j].pos - spaceships[i].pos);
			}
			v2 = m2 * v2;
			//v3 = rule3: match velocity with near boids
			for (int j = 0; j < spaceships.size(); j++) {
				if (j != i)
					v3 += spaceships[j].vel;
			}
			v3 = v3 / (spaceships.size() - 1);
			v3 = (v3 - spaceships[i].vel) / 8;
			v3 = m3 * v3;

			spaceships[i].vel += v1 + v2 + v3;
			//limiting the speed
			int vlim = 1;
			if (glm::length(spaceships[i].vel) > vlim)
				spaceships[i].vel = (spaceships[i].vel / glm::length(spaceships[i].vel)) * vlim;

			spaceships[i].pos += spaceships[i].vel;
		}
		//planets with shadows
		glm::mat4 planetModelMatrix = glm::translate(glm::vec3(sin(-time), 2.0, cos(-time))) * glm::scale(glm::vec3(3.0));
		glm::vec3 planetPosition = planetModelMatrix[3];

		glm::mat4 renderTarget = glm::translate(glm::vec3(0, -5, 0)) * glm::rotate(-1.56f, glm::vec3(1, 0, 0)) * glm::rotate(1.56f, glm::vec3(0, 1, 0)) * glm::scale(glm::vec3(7, 14, 14));

		glBindFramebuffer(GL_FRAMEBUFFER, FramebufferObject);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		lightProjection = glm::ortho(-30.0f, 30.0f, -30.0f, 30.0f, -30.0f, 30.0f);
		lightView = glm::lookAt(-lightDir,
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f));

		drawObjectDepth(&sphereModel, planetModelMatrix, lightProjection, lightView);
		drawObjectDepth(&renderModel, renderTarget, lightProjection, lightView);
		for (int i = 0; i < spaceships.size(); i++)
		{
			drawObjectDepth(&shipModel, spaceships[i].shipDepthModel, lightProjection, lightView);
		}
		drawObjectDepth(&shipModel, shipModelMatrix, lightProjection, lightView);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//planet with normal mapping
		drawObjectTextureNormal(&sphereModel, planetModelMatrix, textureEarth, textureEarthNormal);

		drawObjectShadow(&sphereModel, planetModelMatrix, lightProjection, lightView, textureEarth, depthTexture);
		drawObjectShadow(&renderModel, renderTarget, lightProjection, lightView, textureAsteroid, depthTexture);

		//planets with reflexes (from old code)
		for (int i = 0; i < 3; i++)
		{
			glm::mat4 planetModelMatrix = glm::translate(glm::vec3(planets[i])) * glm::scale(glm::vec3(planets[i].w));
			drawObjectTexture(&sphereModel, planetModelMatrix, cubeMapID);
		}
		drawSkybox(cubeMapID);

		//collision detection
		float dp = findDistance(planetPosition, mainShipPosition);
		float ds = findDistance(ship_pos, mainShipPosition);
		if (dp < 4 || ds < 2)
			state = false;
		for (int i = 0; i < spaceships.size(); i++) {
			float dss = findDistance(spaceships[i].pos, mainShipPosition);
			if (dss < 1)
				state = false;
		}
		for (int i = 0; i < 3; i++) {
			float dps = findDistance(planets[i], mainShipPosition);
			if (dps < 2)
				state = false;
		}
	} else 
	if (coins.size() == 0) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.0f, 0.3f, 0.3f, 1.0f);

		glColor3d(1.0, 1.0, 1.0);
		setOrthographicProjection();
		glPushMatrix();
		glLoadIdentity();
		renderBitmapString(450, 512, (void *)font, "WON!!!");
		glPopMatrix();
		resetPerspectiveProjection();
	} else {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glClearColor(0.0f, 0.3f, 0.3f, 1.0f);
		glColor3d(1.0, 1.0, 1.0);
		setOrthographicProjection();
		glPushMatrix();
		glLoadIdentity();
		renderBitmapString(450, 512, (void *)font, "GAME OVER");
		glPopMatrix();
		resetPerspectiveProjection();
	}


	for (int j = 0; j < spaceships.size(); j++)
	{
		sum += spaceships[j].vel;
	}
	



	int timeSinceStart = glutGet(GLUT_ELAPSED_TIME);
	int deltaTime = timeSinceStart - oldTimeSinceStart;
	oldTimeSinceStart = timeSinceStart;
	for (int j = 0; j < bullets.size(); j++) {
		bullets[j].pos += bullets[j].ParticlaDir * 0.05 * deltaTime;
	}
	for (int j = 0; j < explodeParticlas.size(); j++) {
		explodeParticlas[j].pos += explodeParticlas[j].ParticlaDir * 0.0005 * deltaTime;
	}

	for (int i = 0; i < spaceshipa.size(); i++)
	{
		//float weightV1 = 0.00001;
		//float weightV2 = 0.0011;
		//float weightV3 = 0.001

		float weightV1 = 0.01;
		float weightV2 = 0.002;
		float weightV3 = 0.01;
		glm::vec3 v3 = glm::vec3(0);

		glm::vec3 v1Attract = glm::normalize(shipPath[indexPath] - spaceshipa[i].pos);
		//glm::vec3 v2Separation = separationV2(spaceshipa[i]);
		glm::vec3 v3Alignment = ((sum / spaceshipa.size()) - spaceshipa[i].vel);

		spaceshipa[i].vel += (weightV1 * v1Attract) /*+ (weightV2 * v2Separation) + (weightV3 * v3Alignment)*/;
		spaceships[i].vel += (weightV1 * v1Attract) + /*(weightV2 * v2Separation) +*/ (weightV3 * v3Alignment);

		spaceshipa[i].pos += spaceshipa[i].vel;
		//printf("x%d\n", spaceshipa[i].pos.x);
		//printf("y%d\n", spaceshipa[i].pos.y);
	}


	handleCollisions();

	indexPath += sign;

	glutSwapBuffers();
}

void init()
{
	srand(time(0));
	glEnable(GL_DEPTH_TEST);
	programColor = shaderLoader.CreateProgram("shaders/shader_color.vert", "shaders/shader_color.frag");
	programTexture = shaderLoader.CreateProgram("shaders/shader_tex.vert", "shaders/shader_tex.frag");
	programTextureNorm = shaderLoader.CreateProgram("shaders/shader_norm.vert", "shaders/shader_norm.frag");
	programTextureBasic = shaderLoader.CreateProgram("shaders/shader_basic.vert", "shaders/shader_basic.frag");
	programDepth = shaderLoader.CreateProgram("shaders/shader_depth.vert", "shaders/shader_depth.frag");
	programShadow = shaderLoader.CreateProgram("shaders/shader_shadow.vert", "shaders/shader_shadow.frag");
	programSkybox = shaderLoader.CreateProgram("shaders/sky_box.vert", "shaders/sky_box.frag");
	sphereModel = obj::loadModelFromFile("models/sphere.obj");
	shipModel = obj::loadModelFromFile("models/spaceship.obj");
	textureAsteroid = Core::LoadTexture("textures/asteroid2.png");
	renderModel = obj::loadModelFromFile("models/render.obj");
	coinModel = obj::loadModelFromFile("models/oldcoin.obj");
	textureEarth = Core::LoadTexture("textures/earth.png");
	textureEarthNormal = Core::LoadTexture("textures/earth_normalmap.png");
	cubeMapID = Core::setupCubeMap("textures/xpos.png", "textures/xneg.png", "textures/ypos.png", "textures/yneg.png", "textures/zpos.png", "textures/zneg.png");

	//std::vector<float> unitY = { 0.0, 1.0, 0.0 };
	//std::vector<float> unitX = { 1.0, 0.0, 0.0 };

	glm::vec3 uY = { 0.0, -1.0, 0.0 };
	glm::vec3 uX = { -1.0, 0.0, 0.0 };

	glm::vec3 tang[1203];

	for (int i = 0; i < sphereModel.normal.size(); i += 3)
	{
		glm::vec3 normal = { sphereModel.normal[i + 0], sphereModel.normal[i + 1], sphereModel.normal[i + 2] };

		if (sphereModel.normal[i + 1] < 0.99 && sphereModel.normal[i + 1] > -0.99) tang[i] = glm::normalize(glm::cross(normal, uY));
		else tang[i] = glm::normalize(glm::cross(normal, uX));

		tangent[i + 0] = tang[i].x;
		tangent[i + 1] = tang[i].y;
		tangent[i + 2] = tang[i].z;
	}

	sphereModel.tangent = tangent;

	for (int i = 0; i < 3; i++)
	{
		glm::vec3 position = glm::ballRand(30.0f);
		float d = findDistance(glm::vec3(-1, 2, -1), position);
		while (d < 2) {
			position = glm::ballRand(30.0f);
			d = findDistance(glm::vec3(-1, 2, -1), position);
		}
		float scale = glm::linearRand(1.5f, 1.0f);
		planets.push_back(glm::vec4(position, scale));
	}

	for (int i = 0; i < 10; i++)
	{
		float xpos = glm::linearRand(-10.0f, 10.0f);
		float ypos = glm::linearRand(-4.0f, 10.0f);
		float zpos = glm::linearRand(-10.0f, 10.0f);
		glm::vec3 position = glm::vec3(xpos, ypos, zpos);
		float d = findDistance(glm::vec3(-1, 2, -1), position);
		while (d < 2) {
			xpos = glm::linearRand(-10.0f, 10.0f);
			ypos = glm::linearRand(-4.0f, 10.0f);
			zpos = glm::linearRand(-10.0f, 10.0f);
			position = glm::vec3(xpos, ypos, zpos);
			d = findDistance(glm::vec3(-1, 2, -1), position);
		}
		coins.push_back(position);
	}

	drawCircle(0, 3, 0, 5, 219);
	parallel_transport();
	initialise_particles(50);

	//Generate Framebuffer
	FramebufferObject = 0;
	glGenFramebuffers(1, &FramebufferObject);
	glBindFramebuffer(GL_FRAMEBUFFER, FramebufferObject);

	//Generate depth texture
	glGenTextures(1, &depthTexture);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	//Filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//Attach depth texture to frame buffer
	glBindFramebuffer(GL_FRAMEBUFFER, FramebufferObject);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void shutdown()
{
	shaderLoader.DeleteProgram(programColor);
	shaderLoader.DeleteProgram(programTexture);
	shaderLoader.DeleteProgram(programSkybox);
}

void idle()
{
	glutPostRedisplay();
}

int main(int argc, char ** argv)
{

	glutInit(&argc, argv);

	glutInitWindowPosition(0, 0);
	glutInitWindowSize(1024, 1024);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutCreateWindow("CGP");
	glutReshapeFunc(resize);
	glewInit();

	init();
	glutMotionFunc(mouseMove);
	glutKeyboardFunc(keyboard);
	glutDisplayFunc(renderScene);
	glutIdleFunc(idle);
	glutTimerFunc(25, update, 0);
	glutMainLoop();

	shutdown();

	return 0;
}

inline Particla::Particla(glm::vec3 pos, float width, float height, float depth) {
	this->pos = pos;
	this->width = width;
	this->height = height;
	this->depth = depth;
	this->partID = counterID++;
}

inline Particla::Particla(glm::vec3 ParticlaDir, glm::vec3 pos, float width, float height, float depth) {
	this->pos = pos;
	this->width = width;
	this->height = height;
	this->depth = depth;
	this->ParticlaDir = ParticlaDir;
	this->partID = counterID++;
}

inline glm::vec3 Particla::getShipColor() {
	return shipColor;
}

inline glm::vec3 Particla::getParticlaDir() {
	return ParticlaDir;
}

inline float Particla::getMinX() {
	return min(pos.x, pos.x + width);
}

inline float Particla::getMinY() {
	return min(pos.y, pos.y + height);
}

inline float Particla::getMinZ() {
	return min(pos.z, pos.z + depth);
}

inline float Particla::getMaxX() {
	return max(pos.x, pos.x + width);
}

inline float Particla::getMaxY() {
	return max(pos.y, pos.y + height);
}

inline float Particla::getMaxZ() {
	return max(pos.z, pos.z + depth);
}

inline void Particla::setColorColided(bool isColliding) {
	if (isColliding)
		shipColor = glm::vec3(1.0, 0, 0);
	else
		shipColor = glm::vec3(0.5, 0.5, 0.5);
}

inline bool Particla::intersect(Particla b) {
	//printf("A X%d\n", pos.x);
	//printf("A Y%d\n", pos.y);

	return (getMinX() <= b.getMaxX() && getMaxX() >= b.getMinX()) &&
		(getMinY() <= b.getMaxY() && getMaxY() >= b.getMinY()) &&
		(getMinZ() <= b.getMaxZ() && getMaxZ() >= b.getMinZ());
}

int Particla::counterID = 0;

void createExplosion(glm::vec3 accPos, glm::vec3 bulletDir) {
	for (int i = 0; i < 5; i++)
	{
		int randomNumberx = (rand() % 5);
		int randomNumbery = (rand() % 5);
		int randomNumberz = (rand() % 5);
		glm::vec3 partDir;
		partDir.x = bulletDir.x + randomNumberx;
		partDir.y = bulletDir.y + randomNumbery;
		partDir.z = bulletDir.z + randomNumberz;
		Particla partOfShip(partDir, accPos + (partDir * 3), 0, 0, 0);
		partOfShip.bornTime = oldTimeSinceStart;
		explodeParticlas.push_back(partOfShip);
	}
}
void handleCollisions() {
	std::vector<int> toRemoveIndexes;
	std::vector<int> bullettoRemoveIndexes;
	std::vector<int> parttoRemoveIndexes;

	for (int i = 0; i < spaceshipa.size(); i++) {
		Particla shipA = spaceshipa[i];
		int shipX = shipA.pos.x;
		int shipY = shipA.pos.y;
		int shipZ = shipA.pos.z;
		bool collidedEver = false;
		for (int j = 0; j < spaceshipa.size(); j++) {
			Particla shipB = spaceshipa[j];
			if (shipA.pos != shipB.pos) {
				bool colliding = shipA.intersect(shipB);
				if (colliding) {
					shipA.setColorColided(true);
					shipB.setColorColided(true);
					collidedEver = true;
				}
			}
		}
		
		for (int j = 0; j < bullets.size(); j++) {
			Particla bullet = bullets[j];
			if (shipA.pos != bullet.pos) {
				bool colliding = shipA.intersect(bullet);
				if (colliding) {
					printf("%s\n", "true");
					toRemoveIndexes.push_back(i);
					bullettoRemoveIndexes.push_back(j);
					createExplosion(shipA.pos, shipA.ParticlaDir);
					killed++;
				}
			}
		}
		
		if (!collidedEver) {
			shipA.setColorColided(false);
		}
		glm::mat4 enemyMatrix = glm::translate(shipA.pos) * glm::scale(glm::vec3(1.0f));
		drawObjectColor(&shipModel, enemyMatrix, shipA.getShipColor());
	}
	
	for (int j = 0; j < explodeParticlas.size(); j++) {
		Particla part = explodeParticlas[j];
		int diffTime = oldTimeSinceStart - part.bornTime;
		//1000 * part.ParticlaLife
		if (diffTime > part.ParticlaLife)
		{
			printf("%d \n", diffTime);
			parttoRemoveIndexes.push_back(part.partID);
		}
	}

	for (int i = 0; i < toRemoveIndexes.size(); i++) {
		int partID = toRemoveIndexes[i];
		for (int j = 0; j < spaceshipa.size(); j++) {
			if (partID == spaceshipa[j].partID)
			{
				//printf("%d %d %d %d %d \n", i, j, partID, toRemoveIndexes.size(), spaceshipa.size());
				spaceshipa.erase(spaceshipa.begin() + j);
			}
		}
	}
	for (int i = 0; i < bullettoRemoveIndexes.size(); i++) {
		int partID = bullettoRemoveIndexes[i];
		for (int j = 0; j < bullets.size(); j++) {
			if (partID == bullets[j].partID)
			{
				//printf("%d %d %d %d %d \n", i, j, partID, bullettoRemoveIndexes.size(), bullets.size());
				bullets.erase(bullets.begin() + j);
			}
		}
	}
	for (int i = 0; i < parttoRemoveIndexes.size(); i++) {
		int partID = parttoRemoveIndexes[i];
		for (int j = 0; j < explodeParticlas.size(); j++) {
			if (partID == explodeParticlas[j].partID)
			{
				//printf("%d %d %d %d %d \n", i, j, partID, parttoRemoveIndexes.size(),explodeParticlas.size());
				explodeParticlas.erase(explodeParticlas.begin() + j);
			}
		}
	}
	for (int j = 0; j < bullets.size(); j++) {
		Particla bullet = bullets[j];
		glm::mat4 bulletMatrix = glm::translate(bullet.pos) * glm::scale(glm::vec3(0.75f));
		drawObjectColor(&sphereModel, bulletMatrix, bullet.getShipColor());
	}

	for (int j = 0; j < explodeParticlas.size(); j++) {
		Particla part = explodeParticlas[j];
		glm::mat4 partMatrix = glm::translate(part.pos) * glm::scale(glm::vec3(0.2f));
		drawObjectColor(&sphereModel, partMatrix, part.getShipColor());
	}
	
}