#pragma once
#include <glut.h>
#include <glu.h>
#include <math.h>
#include <stdio.h>

class Camera
{
public:
	Camera();
	~Camera();

	typedef struct Vector3f {
		float x;
		float y;
		float z;
	};

	typedef struct Point {
		float x;
		float y;
		float z;
	};

	Point eye;
	Vector3f u, v, n;

	float m[16];

	float viewAngle, aspect, nearDist, farDist; // view volume shape
	void setModelViewMatrix(); // tell OpenGL where the camera is

	void set(Point eye, Point look, Vector3f up); // like gluLookAt()
	void rotateU(float angle); // yaw it
	void rotateV(float angle); // pitch it
	void rotateN(float angle); // roll it
	void slide(float delU, float delV, float delN); // slide it
	void setShape(float vAng, float asp, float nearD, float farD);

	Vector3f cross(Vector3f a, Vector3f b);
	float dot(Vector3f a, Vector3f b);
	void normalize(Vector3f *vec);

};