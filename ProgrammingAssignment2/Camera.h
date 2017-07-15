#pragma once
#include <glut.h>
#include <glu.h>
#include <math.h>
#include <stdio.h>

#include "vector3f.h"

class Camera
{
public:
	Camera();
	~Camera();

	typedef struct Point {
		float x;
		float y;
		float z;
	};

	vector3f eye;
	vector3f u, v, n;

	float m[16];

	float viewAngle, aspect, nearDist, farDist; // view volume shape
	void setModelViewMatrix(); // tell OpenGL where the camera is

	void set(vector3f eye, Point look, vector3f up); // like gluLookAt()
	void rotateU(float angle); // yaw it
	void rotateV(float angle); // pitch it
	void rotateN(float angle); // roll it
	void slide(float delU, float delV, float delN); // slide it
	void setShape(float vAng, float asp, float nearD, float farD);

};