#include "Camera.h"
#include <math.h>
#include <cmath>

CCamera::CCamera() {
}

// Camera is initialized with a position and direction
CCamera::CCamera(glm::vec3 pos, glm::vec3 dir,
	float scrWidth, float scrHeight,
	float fovy, float nearPlane, float farPlane)
{
	position = pos;
	direction = dir;
	screenWidth = scrWidth; screenHeight = scrHeight;
	fov = fovy;
	nearDist = nearPlane;
	farDist = farPlane;

	// find up direction
	glm::vec3 right = glm::cross(direction, glm::vec3(0.0, 1.0, 0.0));
	up = glm::normalize(glm::cross(right, direction));
}

// returns fov in radians
// fov is in y direction
float CCamera::getFieldOfView() {
	return fov * (3.141592653 / 180.0);
}

float CCamera::getAspectRatio() {
	return screenWidth / screenHeight;
}

// set up the projection matrix
void CCamera::set() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, screenWidth, screenHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fov, screenWidth / screenHeight, nearDist, farDist);

	float px = position.x, py = position.y, pz = position.z;
	gluLookAt(px, py, pz,
		px + direction.x, py + direction.y, pz + direction.z,
		up.x, up.y, up.z);
}

// restore matrix to identity
void CCamera::unset() {
	glPopMatrix();
	glLoadIdentity();
}

void CCamera::resize(float width, float height) {
	screenWidth = width;
	screenHeight = height;
}

glm::vec3 CCamera::getPosition() {
	return position;
}

// convert screen mouse coordinates to a direction in world coordinates
glm::vec3 CCamera::castRayFromScreen(double mx, double my) {
	// Does not work correctly when camera is rolled
	/*
	// normalize mouse coordinates to [-1,1] (clip space)
	float x = (2.0*mx / screenWidth) - 1.0;
	float y = 1.0f - (2.0*my) / screenHeight;
	float z = -1.0f;
	glm::vec4 rayClip = glm::vec4(x, y, z, 1.0);

	// to eye coordinates
	glm::vec4 rayEye = rayClip * glm::inverse(getProjectionMatrix());
	rayEye = glm::vec4(rayEye.x, rayEye.y, 1.0, 0.0);

	// to world coordinates
	glm::vec4 rayWorld = rayEye * glm::inverse(getViewMatrix());
	glm::vec3 rayDir = glm::normalize(glm::vec3(rayWorld));
	*/

	// Following code section is from:
	// http://www.bfilipek.com/2012/06/select-mouse-opengl.html
	double matModelView[16], matProjection[16];
	int viewport[4];

	// get matrix and viewport:
	glGetDoublev(GL_MODELVIEW_MATRIX, matModelView);
	glGetDoublev(GL_PROJECTION_MATRIX, matProjection);
	glGetIntegerv(GL_VIEWPORT, viewport);

	mx = mx;
	my = viewport[3] - my;

	double x1, y1, z1 = 0;
	double x2, y2, z2 = 0;

	// get point on the 'near' plane (third param is set to 0.0)
	gluUnProject(mx, my, 0.0, matModelView, matProjection,
		viewport, &x1, &y1, &z1);

	// get point on the 'far' plane (third param is set to 1.0)
	gluUnProject(mx, my, 1.0, matModelView, matProjection,
		viewport, &x2, &y2, &z2);

	glm::vec3 start = glm::vec3(x1, y1, z1);
	glm::vec3 end = glm::vec3(x2, y2, z2);
	glm::vec3 dir = glm::normalize(end - start);

	return dir;
}

// projection matrix in row order
glm::mat4 CCamera::getProjectionMatrix() {
	float aspect = screenWidth / screenHeight;
	float n = nearDist;
	float f = farDist;
	float fv = 2 * fov * (3.141592653 / 180.0);
	return glm::transpose(glm::mat4(
		fv / aspect, 0, 0, 0,
		0, fv, 0, 0,
		0, 0, (f + n) / (n - f), (2 * f*n) / (n - f),
		0, 0, -1, 0));
}

// view matrix in row order
glm::mat4 CCamera::getViewMatrix() {
	glm::vec3 right = glm::cross(direction, glm::vec3(0.0, 1.0, 0.0));
	return glm::transpose(glm::mat4(
		right.x, up.x, direction.x, position.x,
		right.y, up.y, direction.y, position.y,
		right.z, up.z, direction.z, position.z,
		0.0, 0.0, 0.0, 1.0
	));
}

// convert world position p to screen coordinates
glm::vec3 CCamera::worldToScreenCoordinates(glm::vec3 p) {
	GLdouble scrX, scrY, scrZ;
	int viewport[4];
	double mvmatrix[16], projmatrix[16];

	// identity
	for (int i = 0; i<16; i++) {
		mvmatrix[i] = 0.0;
	}
	mvmatrix[0] = 1.0;
	mvmatrix[5] = 1.0;
	mvmatrix[10] = 1.0;
	mvmatrix[15] = 1.0;

	glGetDoublev(GL_PROJECTION_MATRIX, projmatrix);
	glGetIntegerv(GL_VIEWPORT, viewport);

	gluProject((GLdouble)p.x, (GLdouble)p.y, (GLdouble)p.z,
		mvmatrix, projmatrix, viewport,
		&scrX, &scrY, &scrZ);

	return glm::vec3((float)scrX, (float)viewport[3] - (float)scrY - 1.0, 0.0);
}

void CCamera::moveForward(float val) {
	position = position + val * direction;
}

void CCamera::moveBackward(float val) {
	position = position - val * direction;
}

void CCamera::moveRight(float val) {
	glm::vec3 right = glm::cross(direction, up);
	position = position + val * right;
}

void CCamera::moveLeft(float val) {
	glm::vec3 left = -glm::cross(direction, up);
	position = position + val * left;
}

void CCamera::moveUp(float val) {
	position = position + val * up;
}

void CCamera::moveDown(float val) {
	position = position - val * up;
}

void CCamera::rotateRight(float rad) {
	direction = glm::rotate(direction, rad, -up);
}

void CCamera::rotateLeft(float rad) {
	direction = glm::rotate(direction, -rad, -up);
}

void CCamera::rotateUp(float rad) {
	glm::vec3 right = glm::cross(direction, up);
	up = glm::rotate(up, rad, right);
	direction = glm::rotate(direction, rad, right);
}

void CCamera::rotateDown(float rad) {
	glm::vec3 right = glm::cross(direction, up);
	up = glm::rotate(up, -rad, right);
	direction = glm::rotate(direction, -rad, right);
}

void CCamera::rollRight(float rad) {
	up = glm::rotate(up, rad, -direction);
}

void CCamera::rollLeft(float rad) {
	up = glm::rotate(up, -rad, -direction);
}

void CCamera::initializeOrientation() {
	position = glm::vec3(0.0, 0.0, 0.0);
	direction = glm::vec3(1.0, 0.0, 0.0);
	up = glm::vec3(0.0, 1.0, 0.0);
}

void CCamera::setRotation(glm::mat4 rotMatrix) {
	initializeOrientation();
	glm::vec4 dir4 = rotMatrix * glm::vec4(direction.x,
		direction.y,
		direction.z, 1.0);
	glm::vec4 up4 = rotMatrix * glm::vec4(up.x, up.y, up.z, 1.0);

	direction = glm::vec3(dir4.x, dir4.y, dir4.z);
	up = glm::vec3(up4.x, up4.y, up4.z);
}

void CCamera::setPosition(glm::vec3 pos) {
	position = pos;
}

/*
	CCamera1
*/
CCamera1::CCamera1()
{

}

void CCamera1::SetOrthographic(bool value)
{
	if (value == this->orthographic)
	{
		return;
	}

	this->orthographic = value;
	refreshProjectionMatrix = true;
	refreshInverseProjectionViewMatrix = true;
}

void CCamera1::SetFrustumLeft(float left)
{
	if (this->fl == left) {
		return;
	}
	this->fl = left;
	refreshProjectionMatrix = true;
	refreshInverseProjectionViewMatrix = true;
}
void CCamera1::SetFrustumRight(float right)
{
	if (this->fr == right) {
		return;
	}
	this->fr = right;
	refreshProjectionMatrix = true;
	refreshInverseProjectionViewMatrix = true;
}
void CCamera1::SetFrustumBottom(float bottom)
{
	if (this->fb == bottom) {
		return;
	}
	this->fb = bottom;
	refreshProjectionMatrix = true;
	refreshInverseProjectionViewMatrix = true;
}

void CCamera1::SetFrustumTop(float top)
{
	if (this->ft == top) {
		return;
	}
	this->ft = top;
	refreshProjectionMatrix = true;
	refreshInverseProjectionViewMatrix = true;
}

void CCamera1::SetFrustumNear(float near)
{
	if (this->fn == near) {
		return;
	}
	this->fn = near;
	refreshProjectionMatrix = true;
	refreshInverseProjectionViewMatrix = true;
}

void CCamera1::SetFrustumFar(float far)
{
	if (this->ff == far) {
		return;
	}
	this->ff = far;
	refreshProjectionMatrix = true;
	refreshInverseProjectionViewMatrix = true;
}

void CCamera1::SetFrustumPerspective(float fovY, float aspect, float near, float far)
{
	SetFrustumPerspective(fovY, aspect, near, far, 1, 1, 0, 0);
}

void CCamera1::SetFrustumPerspective(float fovY, float aspect, float near, float far,
	int tilesX, int tilesY, int tileX, int tileY)
{
	SetOrthographic(false);
	// degree to radian conversion
	float h = (float)tan(fovY * 0.0174533 * 0.5f) * near;
	float w = aspect;
	float left = -w + (float)tileX / tilesX * 2.0f * w;
	float right = left + (float) 1.0f / tilesX * 2.0f * w;
	float bottom = -h + (float)tileY / tilesY * 2.0f * h;
	float top = bottom + (float) 1.0f / tilesY * 2.0f * h;

	SetFrustumLeft(left);
	SetFrustumRight(right);
	SetFrustumBottom(bottom);
	SetFrustumTop(top);
	SetFrustumNear(near);
	SetFrustumFar(far);

}

void CCamera1::SetDirection(glm::vec3 direction)
{
	if (this->direction == direction)
	{
		return;
	}
	this->direction = direction;
	refreshViewMatrix = true;
	refreshInverseProjectionViewMatrix = true;
}

void CCamera1::DoRefreshViewMatrix()
{
	tmp0 = direction;
	tmp0 = glm::normalize(tmp0);
	tmp1 = up;
	tmp1 = glm::normalize(tmp1);
	
	// CHECK
	tmp1 = glm::cross(tmp0, tmp1);
	tmp2 = glm::cross(tmp1, tmp0);

	viewMatrix = glm::mat4(
		tmp1.x, tmp1.y, tmp1.z,
		-tmp1.x * position.x - tmp1.y * position.y - tmp1.z * position.z,

		tmp2.x, tmp2.y, tmp2.z,
		-tmp2.x * position.x - tmp2.y * position.y - tmp2.z * position.z,

		tmp0.x, tmp0.y, tmp0.z,
		-tmp0.x * position.x - tmp0.y * position.y - tmp0.z * position.z,

		0.0f, 0.0f, 0.0f, 1.0f
	);

	refreshViewMatrix = false;
}

void CCamera1::SetPosition(glm::vec3 pos)
{
	if (this->position == pos)
	{
		return;
	}

	this->position = pos;
	refreshViewMatrix = true;
	refreshInverseProjectionViewMatrix = true;
}

void CCamera1::SetUp(glm::vec3 pos)
{
	if (this->up == pos)
	{
		return;
	}

	this->up = pos;
	refreshViewMatrix = true;
	refreshInverseProjectionViewMatrix = true;
}

glm::mat4 CCamera1::GetProjectionMatrix()
{
	if (refreshProjectionMatrix)
	{
		DoRefreshProjectionMatrix();
	}
	return projectionMatrix;
}

void CCamera1::DoRefreshProjectionMatrix()
{
	if (!orthographic)
	{
		projectionMatrix = glm::mat4(
			2.0f * fn / (fr - fl), 0.0f, (fr + fl) / (fr - fl), 0.0f,
			0.0f, 2.0f * fn / (ft - fb), (ft + fb) / (ft - fb), 0.0f,
			0.0f, 0.0f, -(ff + fn) / (ff - fn), -2.0f * ff * fn / (ff - fn),
			0.0f, 0.0f, -1.0f, 0.0f
		);
	}
	else
	{
		projectionMatrix = glm::mat4(
			2.0f / (fr - fl), 0.0f, 0.0f, -(fr + fl) / (fr - fl),
			0.0f, 2.0f / (ft - fb), 0.0f, -(ft + fb) / (ft - fb),
			0.0f, 0.0f, -2.0f / (ff - fn), -(ff + fn) / (ff - fn),
			0.0f, 0.0f, 0.0f, 1.0f
		);
	}
}

glm::mat4 CCamera1::GetViewMatrix()
{
	if (refreshViewMatrix)
	{
		DoRefreshViewMatrix();
	}
	return viewMatrix;
}
	
void CCamera1::DoRefreshInverseProjectionViewMatrix()
{
	invViewProjectionMatrix = GetProjectionMatrix();
	invViewProjectionMatrix = invViewProjectionMatrix * GetViewMatrix();
	invViewProjectionMatrix = glm::inverse(invViewProjectionMatrix);
	refreshInverseProjectionViewMatrix = false;
}

glm::mat4 CCamera1::GetInverseProjectionViewMatrix()
{
	if (refreshViewMatrix)
	{
		DoRefreshInverseProjectionViewMatrix();
	}
	return invViewProjectionMatrix;
}

glm::vec3 CCamera1::GetEyeRay(float x, float y)
{
	tmp3 = glm::vec4(x, y, 0.0f, 1.0f);
	glm::mat4 invMat = GetInverseProjectionViewMatrix();

	//	TODO - Scaling -- Is this just normalising
	// cause the source is just scaling by w
	tmp3 = glm::normalize(tmp3);

	tmp0 = glm::vec3(tmp3.x, tmp3.y, tmp3.z);
	glm::vec3 res = tmp0 - position;

	return res;
}

void CCamera1::SetLookAt(glm::vec3 position, glm::vec3 lookAt, glm::vec3 up)
{
	SetPosition(position);
	tmp0 = up;
	tmp0 = glm::normalize(tmp0);
	tmp1 = lookAt;
	tmp1 = tmp1 - position;
	tmp1 = glm::normalize(tmp1);
	SetDirection(tmp1);
	right = glm::cross(tmp1, tmp0);
	right = glm::normalize(right);
	tmp0 = glm::cross(right, tmp1);
	SetUp(tmp0);
}