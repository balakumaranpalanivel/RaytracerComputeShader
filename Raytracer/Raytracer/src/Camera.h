#pragma once

#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include <GL/glew.h>

class CCamera
{
public:
	CCamera();
	CCamera(glm::vec3 pos, glm::vec3 dir,
		float scrWidth, float scrHeight,
		float fovy, float nearPlane, float farPlane);

	glm::mat4 getViewMatrix();
	glm::mat4 getProjectionMatrix();
	glm::vec3 worldToScreenCoordinates(glm::vec3 p);
	glm::vec3 getPosition();
	glm::vec3 castRayFromScreen(double mx, double my);

	glm::vec3 position;
	glm::vec3 direction;
	glm::vec3 up;          // assume <0,1,0> on initialization
	float screenWidth;
	float screenHeight;

	float getFieldOfView();
	float getAspectRatio();
	void set();
	void unset();
	void resize(float width, float height);
	void moveForward(float val);    // move along direction vector
	void moveBackward(float val);
	void moveRight(float val);      // move side to side
	void moveLeft(float val);
	void moveUp(float val);         // move along up vector
	void moveDown(float val);
	void rotateRight(float rad);    // rotaties about up vector
	void rotateLeft(float rad);
	void rotateUp(float rad);       // rotate about right/left vector
	void rotateDown(float rad);
	void rollRight(float rad);      // roll about direction vector
	void rollLeft(float rad);
	void initializeOrientation();
	void setRotation(glm::mat4 rotMatrix);
	void setPosition(glm::vec3 pos);

private:
	float fov;
	float nearDist;
	float farDist;
};

class CCamera1
{
private:
	float fn = 0.001f;
	float ff = 10.0f;
	float fl = -0.05f;
	float fr = 0.05f;
	float ft = 0.05f;
	float fb = -0.05f;

	bool refreshViewMatrix = true;
	bool refreshProjectionMatrix = true;
	bool refreshInverseProjectionViewMatrix = true;

	glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 direction = glm::vec3(0.0f, 0.0f, -1.0f);

	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

	glm::vec3 right = glm::vec3(1.0f, 0.0f, 0.0f);

	glm::mat4 projectionMatrix;

	glm::mat4 viewMatrix;

	glm::mat4 invViewProjectionMatrix;

	/**
	* Orthographic projection is inherently different from perspective
	* projection.
	*/
	bool orthographic;

	glm::vec3 tmp0;
	glm::vec3 tmp1;
	glm::vec3 tmp2;
	glm::vec4 tmp3;

public:
	CCamera1();

	void SetFrustumPerspective(float fovY, float aspect, float near, float far);
	void SetFrustumPerspective(float fovY, float aspect, float near, float far,
		int tilesX, int tilesY, int tileX, int tileY);

	void SetOrthographic(bool value);
	void SetDirection(glm::vec3 direction);

	void SetFrustumLeft(float left);
	void SetFrustumRight(float right);
	void SetFrustumBottom(float bottom);
	void SetFrustumTop(float top);
	void SetFrustumNear(float near);
	void SetFrustumFar(float far);
	void SetLookAt(glm::vec3 position, glm::vec3 lookAt, glm::vec3 up);

	void DoRefreshViewMatrix();

	inline glm::vec3 GetPosition() { return position;  }
	void SetPosition(glm::vec3 pos);
	inline glm::vec3 GetUp() { return up; }
	void SetUp(glm::vec3 pos);
	inline glm::vec3 GetRight() { return right; }

	glm::mat4 GetProjectionMatrix();
	void DoRefreshProjectionMatrix();
	glm::mat4 GetViewMatrix();
	void DoRefreshInverseProjectionViewMatrix();
	glm::mat4 GetInverseProjectionViewMatrix();

	glm::vec3 GetEyeRay(float x, float y);
};

