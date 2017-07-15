#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>

#include "vector4f.h"

#define MAX_MATERIAL_COUNT 3;

class Model
{
public:
	Model();
	Model(char *Filename);
	~Model();

	void load(char* Filename);
	//void print();

	struct Vertex4f
	{
		vector4f position;
		vector4f normal;
		float color[3];
		float s, t;
	};

	struct Triangle4f
	{
		Vertex4f v[4];
		vector4f normal;
	};

	struct Material
	{
		float mat_ambient[3];
		float mat_diffuse[3];
		float mat_specular[3];
		float shine;
	};

	std::vector<Triangle4f> triangles;
	int numTriangles;
	vector3f min;
	vector3f max;
	Material material;
	


};

