#include "Model.h"


Model::Model()
{
	//load("cow_up.in");
	//mostNegativeX = INT_MAX;
	//mostPositiveX = INT_MIN;
	//minpoint = { 10000.0f, 10000.0f, 10000.0f };
	//maxpoint = { -10000.0f, -10000.0f, -10000.0f };
}

Model::Model(char *Filename)
{
	load(Filename);

	//print();
}


Model::~Model()
{
}

void Model::load(char *FileName)
{
	//double cx = 0.0, cy = 0.0, cz = 0.0;
	Vector3f minpoint = { 10000.0f, 10000.0f, 10000.0f };
	Vector3f maxpoint = { -10000.0f, -10000.0f, -10000.0f };

	Vertex ambient[3], diffuse[3], specular[3];
	float shine[3];
	int material_count, color_index[3], i;
	char ch;
	FILE* fp = fopen(FileName, "r");
	if (fp == NULL) { printf("ERROR: unable to open TriObj [%s]!\n", FileName); exit(1); }
	fscanf(fp, "%c", &ch);
	while (ch != '\n') // skip the first line – object’s name 
		fscanf(fp, "%c", &ch);
	//
	fscanf(fp, "# triangles = %d\n", &numTriangles); // read # of triangles
	fscanf(fp, "Material count = %d\n", &material_count); // read material count
	//
	for (i = 0; i<material_count; i++) {
		fscanf(fp, "ambient color %f %f %f\n", &(ambient[i].x), &(ambient[i].y), &(ambient[i].z));
		fscanf(fp, "diffuse color %f %f %f\n", &(diffuse[i].x), &(diffuse[i].y), &(diffuse[i].z));
		fscanf(fp, "specular color %f %f %f\n", &(specular[i].x), &(specular[i].y), &(specular[i].z));
		fscanf(fp, "material shine %f\n", &(shine[i]));
	}
	//
	fscanf(fp, "%c", &ch);
	while (ch != '\n') // skip documentation line
		fscanf(fp, "%c", &ch);
	//
	// allocate triangles for tri model
	//
	printf("Reading in %s (%d triangles). . .\n", FileName, numTriangles);
	//Model::Triangle Tris = new <triangle data struct>[NumTris];
	//
	for (i = 0; i<numTriangles; i++) // read triangles
	{
		Triangle Tris;
		fscanf(fp, "v0 %f %f %f %f %f %f %d\n",
			&(Tris.v0.x), &(Tris.v0.y), &(Tris.v0.z),
			&(Tris.normal[0].x), &(Tris.normal[0].y), &(Tris.normal[0].z),
			&(color_index[0]));
		fscanf(fp, "v1 %f %f %f %f %f %f %d\n",
			&(Tris.v1.x), &(Tris.v1.y), &(Tris.v1.z),
			&(Tris.normal[1].x), &(Tris.normal[1].y), &(Tris.normal[1].z),
			&(color_index[1]));
		fscanf(fp, "v2 %f %f %f %f %f %f %d\n",
			&(Tris.v2.x), &(Tris.v2.y), &(Tris.v2.z),
			&(Tris.normal[2].x), &(Tris.normal[2].y), &(Tris.normal[2].z),
			&(color_index[2]));
		fscanf(fp, "face normal %f %f %f\n", &(Tris.face_normal.x), &(Tris.face_normal.y),
			&(Tris.face_normal.z));
		//

		if (Tris.v0.x < minpoint.x) { minpoint.x = Tris.v0.x; }
		if (Tris.v0.y < minpoint.y) { minpoint.y = Tris.v0.y; }
		if (Tris.v0.z < minpoint.z) { minpoint.z = Tris.v0.z; }
		if (Tris.v0.x > maxpoint.x) { maxpoint.x = Tris.v0.x; }
		if (Tris.v0.y > maxpoint.y) { maxpoint.y = Tris.v0.y; }
		if (Tris.v0.z > maxpoint.z) { maxpoint.z = Tris.v0.z; }

		if (Tris.v1.x < minpoint.x) { minpoint.x = Tris.v1.x; }
		if (Tris.v1.y < minpoint.y) { minpoint.y = Tris.v1.y; }
		if (Tris.v1.z < minpoint.z) { minpoint.z = Tris.v1.z; }
		if (Tris.v1.x > maxpoint.x) { maxpoint.x = Tris.v1.x; }
		if (Tris.v1.y > maxpoint.y) { maxpoint.y = Tris.v1.y; }
		if (Tris.v1.z > maxpoint.z) { maxpoint.z = Tris.v1.z; }

		if (Tris.v2.x < minpoint.x) { minpoint.x = Tris.v2.x; }
		if (Tris.v2.y < minpoint.y) { minpoint.y = Tris.v2.y; }
		if (Tris.v2.z < minpoint.z) { minpoint.z = Tris.v2.z; }
		if (Tris.v2.x > maxpoint.x) { maxpoint.x = Tris.v2.x; }
		if (Tris.v2.y > maxpoint.y) { maxpoint.y = Tris.v2.y; }
		if (Tris.v2.z > maxpoint.z) { maxpoint.z = Tris.v2.z; }

		Tris.color.r = (unsigned char)(int)(255 * (diffuse[color_index[0]].x));
		Tris.color.g = (unsigned char)(int)(255 * (diffuse[color_index[0]].y));
		Tris.color.b = (unsigned char)(int)(255 * (diffuse[color_index[0]].z));

		triangles.push_back(Tris);
	}
	fclose(fp);

	printf("minpoint: %f %f %f\n", minpoint.x, minpoint.y, minpoint.z);
	printf("maxpoint: %f %f %f\n", maxpoint.x, maxpoint.y, maxpoint.z);

	min = minpoint;
	max = maxpoint;
}

void Model::print()
{
	for (int i = 0; i < numTriangles; i++)
	{
		printf("triangle %d\n", i);
		printf("v0: %f %f %f\n", triangles[i].v0.x, triangles[i].v0.y, triangles[i].v0.z);
		printf("v1: %f %f %f\n", triangles[i].v1.x, triangles[i].v1.y, triangles[i].v1.z);
		printf("v2: %f %f %f\n", triangles[i].v2.x, triangles[i].v2.y, triangles[i].v2.z);
		printf("color: %d %d %d\n", triangles[i].color.r, triangles[i].color.g, triangles[i].color.b);
	}
}
