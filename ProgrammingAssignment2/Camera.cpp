#include "Camera.h"


Camera::Camera()
{
	set({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f });
}



Camera::~Camera()
{
}

void Camera::setModelViewMatrix(void)
{ // load modelview matrix with existing camera values
	//float m[16];
	Vector3f eVec = { eye.x, eye.y, eye.z }; // a vector version of eye

	m[0] = u.x;		m[4] = u.y;		m[8] = u.z;		m[12] = -dot(eVec, u);
	m[1] = v.x;		m[5] = v.y;		m[9] = v.z;		m[13] = -dot(eVec, v);
	m[2] = n.x;		m[6] = n.y;		m[10] = n.z;	m[14] = -dot(eVec, n);
	m[3] = 0;		m[7] = 0;		m[11] = 0;		m[15] = 1.0;

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(m); // load OpenGL�s modelview matrix
}

void Camera::set(Point Eye, Point look, Vector3f up)
{ // create a modelview matrix and send it to OpenGL
	eye = Eye; // store the given eye position
	n = { eye.x - look.x, eye.y - look.y, eye.z - look.z }; // make n

	//vector n must be normalized before computing the cross product.
	normalize(&n);
	normalize(&u); // make them unit length

	u = cross(up, n); // make u = up X n
	v = cross(n, u); // make v = n X u

	setModelViewMatrix(); // tell OpenGL
}

void Camera::slide(float delU, float delV, float delN)
{
	Point e = eye;

	e.x += delU * u.x;
	e.y += delV * v.y;
	e.z += delN * n.z;

	eye = e;
	setModelViewMatrix();
}

// Rotation operations

// yaw the camera through angle degrees
void Camera::rotateU(float angle)
{ 
	Vector3f newN, newV;
	float cs = cos(3.14159265 / 180 * angle);
	float sn = sin(3.14159265 / 180 * angle);

	newV = { cs*v.x - sn*n.x, cs*v.y - sn*n.y, cs*v.z - sn*n.z };
	newN = { sn*v.x + cs*n.x, sn*v.y + cs*n.y, sn*v.z + cs*n.z };
	v = newV; 
	n = newN;
	
	setModelViewMatrix();
}

// pitch the camera through angle degrees
void Camera::rotateV(float angle)
{ 
	Vector3f newU, newN;
	float cs = cos(3.14159265 / 180 * angle);
	float sn = sin(3.14159265 / 180 * angle);

	newU = { cs*u.x + sn*n.x, cs*u.y + sn*n.y, cs*u.z + sn*n.z };
	newN = { -sn*u.x + cs*n.x, -sn*u.y + cs*n.y, -sn*u.z + cs*n.z };
	u = newU;
	n = newN;

	setModelViewMatrix();
}

// roll the camera through angle degrees
void Camera::rotateN(float angle)
{ 
	Vector3f newU, newV;
	float cs = cos(3.14159265 / 180 * angle);
	float sn = sin(3.14159265 / 180 * angle);

	newU = { cs*u.x - sn*v.x, cs*u.y - sn*v.y, cs*u.z - sn*v.z };
	newV = { sn*u.x + cs*v.x, sn*u.y + cs*v.y, sn*u.z + cs*v.z };
	u = newU;
	v = newV;

	setModelViewMatrix();
}

// Vector operations

Camera::Vector3f Camera::cross(Vector3f a, Vector3f b)
{
	Vector3f result;
	result.x = a.y * b.z - b.y * a.z;
	result.y = a.z * b.x - b.z * a.x;
	result.z = a.x * b.y - b.x * a.y;

	return result;
}

float Camera::dot(Vector3f a, Vector3f b)
{
	// compute
	float product = 0.0f;
	product += a.x * b.x;
	product += a.y * b.y;
	product += a.z * b.z;

	return product;
}

void Camera::normalize(Vector3f *vec)
{
	vec->x = vec->x / sqrt(pow(vec->x, 2) + pow(vec->y, 2) + pow(vec->z, 2));
	vec->y = vec->y / sqrt(pow(vec->x, 2) + pow(vec->y, 2) + pow(vec->z, 2));
	vec->z = vec->z / sqrt(pow(vec->x, 2) + pow(vec->y, 2) + pow(vec->z, 2));
}