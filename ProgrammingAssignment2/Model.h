#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>

#define MAX_MATERIAL_COUNT 3;

class Model
{
public:
	Model();
	Model(char *Filename);
	~Model();

	void load(char* Filename);
	void print();

	typedef struct Normal {
		float x;
		float y;
		float z;
	};

	typedef struct Vertex {
		float x;
		float y;
		float z;
	};

	typedef struct Vector3f {
		float x;
		float y;
		float z;
	};

	typedef struct Color {
		int r;
		int g;
		int b;
	};

	typedef struct Triangle {
		Vertex v0;
		Vertex v1;
		Vertex v2;
		Normal normal[3];
		Vertex face_normal;
		Color color;
	};

	std::vector<Triangle> triangles;
	int numTriangles;
	Vector3f min;
	Vector3f max;


};

