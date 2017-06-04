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
	vector3f minpoint = { 10000.0f, 10000.0f, 10000.0f };
	vector3f maxpoint = { -10000.0f, -10000.0f, -10000.0f };

	vector3f ambient[3], diffuse[3], specular[3];
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
		Triangle4f tri;

		fscanf(fp, "v0 %f %f %f %f %f %f %d\n",
			&(tri.v[0].position.x), &(tri.v[0].position.y), &(tri.v[0].position.z),
			&(tri.v[0].normal.x), &(tri.v[0].normal.y), &(tri.v[0].normal.z),
			&(color_index[0]));
		fscanf(fp, "v1 %f %f %f %f %f %f %d\n",
			&(tri.v[1].position.x), &(tri.v[1].position.y), &(tri.v[1].position.z),
			&(tri.v[1].normal.x), &(tri.v[1].normal.y), &(tri.v[1].normal.z),
			&(color_index[1]));
		fscanf(fp, "v2 %f %f %f %f %f %f %d\n",
			&(tri.v[2].position.x), &(tri.v[2].position.y), &(tri.v[2].position.z),
			&(tri.v[2].normal.x), &(tri.v[2].normal.y), &(tri.v[2].normal.z),
			&(color_index[2]));
		fscanf(fp, "face normal %f %f %f\n", &(tri.normal.x), &(tri.normal.y), &(tri.normal.z));
		//

		tri.v[0].position.w = 1;
		tri.v[1].position.w = 1;
		tri.v[2].position.w = 1;

		if (tri.v[0].position.x < minpoint.x) { minpoint.x = tri.v[0].position.x; }
		if (tri.v[0].position.y < minpoint.y) { minpoint.y = tri.v[0].position.y; }
		if (tri.v[0].position.z < minpoint.z) { minpoint.z = tri.v[0].position.z; }
		if (tri.v[0].position.x > maxpoint.x) { maxpoint.x = tri.v[0].position.x; }
		if (tri.v[0].position.y > maxpoint.y) { maxpoint.y = tri.v[0].position.y; }
		if (tri.v[0].position.z > maxpoint.z) { maxpoint.z = tri.v[0].position.z; }

		if (tri.v[1].position.x < minpoint.x) { minpoint.x = tri.v[1].position.x; }
		if (tri.v[1].position.y < minpoint.y) { minpoint.y = tri.v[1].position.y; }
		if (tri.v[1].position.z < minpoint.z) { minpoint.z = tri.v[1].position.z; }
		if (tri.v[1].position.x > maxpoint.x) { maxpoint.x = tri.v[1].position.x; }
		if (tri.v[1].position.y > maxpoint.y) { maxpoint.y = tri.v[1].position.y; }
		if (tri.v[1].position.z > maxpoint.z) { maxpoint.z = tri.v[1].position.z; }

		if (tri.v[2].position.x < minpoint.x) { minpoint.x = tri.v[2].position.x; }
		if (tri.v[2].position.y < minpoint.y) { minpoint.y = tri.v[2].position.y; }
		if (tri.v[2].position.z < minpoint.z) { minpoint.z = tri.v[2].position.z; }
		if (tri.v[2].position.x > maxpoint.x) { maxpoint.x = tri.v[2].position.x; }
		if (tri.v[2].position.y > maxpoint.y) { maxpoint.y = tri.v[2].position.y; }
		if (tri.v[2].position.z > maxpoint.z) { maxpoint.z = tri.v[2].position.z; }

		tri.v[0].color[0] = (unsigned char)(int)(255 * (diffuse[color_index[0]].x));
		tri.v[0].color[1] = (unsigned char)(int)(255 * (diffuse[color_index[0]].y));
		tri.v[0].color[2] = (unsigned char)(int)(255 * (diffuse[color_index[0]].z));

		tri.v[1].color[0] = (unsigned char)(int)(255 * (diffuse[color_index[1]].x));
		tri.v[1].color[1] = (unsigned char)(int)(255 * (diffuse[color_index[1]].y));
		tri.v[1].color[2] = (unsigned char)(int)(255 * (diffuse[color_index[1]].z));

		tri.v[2].color[0] = (unsigned char)(int)(255 * (diffuse[color_index[2]].x));
		tri.v[2].color[1] = (unsigned char)(int)(255 * (diffuse[color_index[2]].y));
		tri.v[2].color[2] = (unsigned char)(int)(255 * (diffuse[color_index[2]].z));

		triangles.push_back(tri);
	}
	fclose(fp);

	printf("minpoint: %f %f %f\n", minpoint.x, minpoint.y, minpoint.z);
	printf("maxpoint: %f %f %f\n", maxpoint.x, maxpoint.y, maxpoint.z);

	min = minpoint;
	max = maxpoint;
}
