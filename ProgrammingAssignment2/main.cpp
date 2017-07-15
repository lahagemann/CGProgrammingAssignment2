#include <Windows.h>
#include <glut.h>
#include <glu.h>
#include <glui.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <vector>

#include "Model.h"
#include "Camera.h"
#include "matrix4x4f.h"

#define PI 3.141592654

struct ARGBf {
	float r, g, b, a;
};

struct Light {
	float att[3];
	vector4f direction;
	float ambient[4];
	Model::Material mat;
};

// TWEAKABLE PARAMETERS

// Camera Position
float g_CamPosition[] = { 0.0f, 0.0f, 5.0f };

// Camera Translation
float g_CamTranslation[] = { 0.0f, 0.0f, 0.0f };

// Camera Rotation
float g_CamRotation[] = { 0.0f, 0.0f, 0.0f };

// Clipping Plane
float g_ZFar = 2000.0f;
float g_ZNear = 1.0f;

// Field of View
float g_FOVX = 60.0f;
float g_FOVY = 60.0f;

// Material Color
float g_MatAmbient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
float g_MatDiffuse[] = { 0.5f, 0.0f, 0.0f, 1.0f };

// Light parameter
float g_LightMultiplier = 1.0f;
float g_LightDirection[] = { 1.1547f, -1.1547f, 1.1547f };
Light light;

// Booleans
int g_LookAtObject = 1;
int g_PerformBFCulling = 0;
int g_LightsOn = 1;

// Options
int g_DrawingMode = 0;
int g_VertexOrientation = 0;
int g_Shading = 0;
char filename[256] = "cube.in";

// other variables
Model m;
Camera g_cam;
int mainWindow, close2GLWindow;

// BUFFERS
// buffers must be a 1 dimension array/pointer, as screen can change size.
double *z_buffer;
ARGBf *color_buffer;

int width = 600;
int height = 600;
/*
	You can easily set up the viewing frustum using vertical field of view and screen's aspect ratio.
		aspect ratio = width/height
		top = tan(FOV/2) * near
		bottom = -top
		right = top * aspect ratio
		left = bottom = - top * aspect ratio
*/
//near far
float n = g_ZNear, f = g_ZFar; 



// WINDOW SIZE DEFINITION
float rv = 600.0f, lv = 0.0f, tv = 0.0f, bv = 600.0f; //window right left top bottom

matrix4x4f modelview;
matrix4x4f projection;
matrix4x4f viewport;

// OBJECT CENTER IN WCS
float obj_center[] = { 0.0f, 0.0f, 0.0f };


void update_modelview_matrix()
{
	matrix4x4f *m = &modelview;
	m->m[0] = g_cam.m[0];	m->m[4] = g_cam.m[4];	m->m[8] = g_cam.m[8];	m->m[12] = g_cam.m[12];
	m->m[1] = g_cam.m[1];	m->m[5] = g_cam.m[5];	m->m[9] = g_cam.m[9];	m->m[13] = g_cam.m[13];
	m->m[2] = g_cam.m[2];	m->m[6] = g_cam.m[6];	m->m[10] = g_cam.m[10];	m->m[14] = g_cam.m[14];
	m->m[3] = g_cam.m[3]; 	m->m[7] = g_cam.m[7]; 	m->m[11] = g_cam.m[11];	m->m[15] = g_cam.m[15];
}

void update_projection_matrix() 
{
	// FRUSTUM DEFINITION: right left top bottom
	
	float t = tan(g_FOVY * PI / 360.0f) * n;
	float b = -t;
	float r = tan(g_FOVX * PI/ 360.0f) * n;
	float l = -r;

	matrix4x4f *m = &projection;
	m->m[0] = 2 * n / (r - l);	m->m[4] = 0;				m->m[8] = (r + l) / (r - l);	m->m[12] = 0;
	m->m[1] = 0;				m->m[5] = 2 * n / (t - b);	m->m[9] = (t + b) / (t - b);	m->m[13] = 0;
	m->m[2] = 0;				m->m[6] = 0;				m->m[10] = -(f + n) / (f - n);	m->m[14] = -(2 * f*n) / (f - n);
	m->m[3] = 0; 				m->m[7] = 0; 				m->m[11] = -1;					m->m[15] = 0;
}

void update_viewport_matrix(double lv, double rv, double bv, double tv)
{
	matrix4x4f *m = &viewport;
	m->m[0] = (rv - lv) / 2;	m->m[4] = 0;				m->m[8] = 0;	m->m[12] = (rv + lv) / 2;
	m->m[1] = 0;				m->m[5] = (tv - bv) / 2;	m->m[9] = 0;	m->m[13] = (tv + bv) / 2;
	m->m[2] = 0;				m->m[6] = 0;				m->m[10] = 1;	m->m[14] = 0;
	m->m[3] = 0; 				m->m[7] = 0; 				m->m[11] = 0;	m->m[15] = 1;
}

void set_light()
{
	light.direction.x = -g_LightDirection[0];
	light.direction.y = -g_LightDirection[1];
	light.direction.z = -g_LightDirection[2];
	light.direction.w = 0.0f;

	light.mat.mat_ambient[0] = light.mat.mat_ambient[1] = light.mat.mat_ambient[2] = g_LightMultiplier*0.2f;
	light.mat.mat_ambient[3] = 1.0f;

	light.mat.mat_diffuse[0] = light.mat.mat_diffuse[1] = light.mat.mat_diffuse[2] = g_LightMultiplier*0.8f;
	light.mat.mat_diffuse[3] = 1.0f;

	light.mat.mat_specular[0] = light.mat.mat_specular[1] = light.mat.mat_specular[2] = 1.0f;

	light.att[0] = 1.0f;
	light.att[1] = light.att[2] = 0.0f;

	light.ambient[0] = light.ambient[1] = light.ambient[2] = 0.0f;
	light.ambient[3] = 1.0f;
}

void clear_buffers()
{
	for (int i = 0; i < width*height; i++) {
		z_buffer[i] = INT_MAX - 1;
		color_buffer[i] = { 0, 0, 0, 1 };
	}
}

void allocate_buffers()
{
	free(color_buffer);
	free(z_buffer);
	
	color_buffer = (ARGBf*) calloc(height*width,sizeof(ARGBf));
	
	z_buffer = (double*) calloc(height*width, sizeof(double));
}

//Draw model
void Draw(Model m)
{
	if (g_DrawingMode == 0)
	{
		for (int i = 0; i < m.numTriangles; i++)
		{
			glColor3f(g_MatDiffuse[0], g_MatDiffuse[1], g_MatDiffuse[2]);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glBegin(GL_TRIANGLES);
			glNormal3f(m.triangles[i].v[0].normal.x, m.triangles[i].v[0].normal.y, m.triangles[i].v[0].normal.z);
			glVertex3f(m.triangles[i].v[0].position.x, m.triangles[i].v[0].position.y, m.triangles[i].v[0].position.z);
			glNormal3f(m.triangles[i].v[1].normal.x, m.triangles[i].v[1].normal.y, m.triangles[i].v[1].normal.z);
			glVertex3f(m.triangles[i].v[1].position.x, m.triangles[i].v[1].position.y, m.triangles[i].v[1].position.z);
			glNormal3f(m.triangles[i].v[2].normal.x, m.triangles[i].v[2].normal.y, m.triangles[i].v[2].normal.z);
			glVertex3f(m.triangles[i].v[2].position.x, m.triangles[i].v[2].position.y, m.triangles[i].v[2].position.z);
			glEnd();
		}
	}
	else if (g_DrawingMode == 1)
	{
		for (int i = 0; i < m.numTriangles; i++)
		{
			glColor3f(g_MatDiffuse[0], g_MatDiffuse[1], g_MatDiffuse[2]);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glBegin(GL_TRIANGLES);
			glNormal3f(m.triangles[i].v[0].normal.x, m.triangles[i].v[0].normal.y, m.triangles[i].v[0].normal.z);
			glVertex3f(m.triangles[i].v[0].position.x, m.triangles[i].v[0].position.y, m.triangles[i].v[0].position.z);
			glNormal3f(m.triangles[i].v[1].normal.x, m.triangles[i].v[1].normal.y, m.triangles[i].v[1].normal.z);
			glVertex3f(m.triangles[i].v[1].position.x, m.triangles[i].v[1].position.y, m.triangles[i].v[1].position.z);
			glNormal3f(m.triangles[i].v[2].normal.x, m.triangles[i].v[2].normal.y, m.triangles[i].v[2].normal.z);
			glVertex3f(m.triangles[i].v[2].position.x, m.triangles[i].v[2].position.y, m.triangles[i].v[2].position.z);
			glEnd();
		}
	}
	else
	{
		for (int i = 0; i < m.numTriangles; i++)
		{
			glColor3f(g_MatDiffuse[0], g_MatDiffuse[1], g_MatDiffuse[2]);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glBegin(GL_POINTS);
			glNormal3f(m.triangles[i].v[0].normal.x, m.triangles[i].v[0].normal.y, m.triangles[i].v[0].normal.z);
			glVertex3f(m.triangles[i].v[0].position.x, m.triangles[i].v[0].position.y, m.triangles[i].v[0].position.z);
			glNormal3f(m.triangles[i].v[1].normal.x, m.triangles[i].v[1].normal.y, m.triangles[i].v[1].normal.z);
			glVertex3f(m.triangles[i].v[1].position.x, m.triangles[i].v[1].position.y, m.triangles[i].v[1].position.z);
			glNormal3f(m.triangles[i].v[2].normal.x, m.triangles[i].v[2].normal.y, m.triangles[i].v[2].normal.z);
			glVertex3f(m.triangles[i].v[2].position.x, m.triangles[i].v[2].position.y, m.triangles[i].v[2].position.z);
			glEnd();
		}
	}
	return;
}

void set_camera_position_auto()
{
	obj_center[0] = (m.max.x + m.min.x) / 2;
	obj_center[1] = (m.max.y + m.min.y) / 2;
	obj_center[2] = (m.max.z + m.min.z) / 2;

	g_cam.set({ obj_center[0], obj_center[1], (obj_center[0] + (m.max.x - m.min.x) / 2) }, 
				{ obj_center[0], obj_center[1], obj_center[2] },
				{ 0, 1, 0 });

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(modelview.m); // load OpenGL’s modelview matrix
}

void phong_illumination_model(Model::Vertex4f *v)
{
	double I, att, distance, kd;
	Model::Material mat = m.material;
	vector4f N = v->normal;
	
	//N.z = -N.z;
	N.normalize();
	for (int channel = 0; channel < 3; channel++) {
		I = 0;
		I = light.ambient[channel] * /*mat.mat_ambient[channel]*/ g_MatDiffuse[channel];
		if (g_LightsOn) {
			static vector4f L, R, V;
			static vector3f view;
			static double NL_dot, RV_dot;
			// ambient part
			I += light.mat.mat_ambient[channel] * /*mat.mat_ambient[channel]*/ g_MatDiffuse[channel];
			// attenuation
			distance = distanceV(light.direction, v->position);
			att = 1.0 / (light.att[0] + distance*light.att[1] + distance*distance*light.att[2]);
			if (att > 1)
				att = 1;
			// diffuse part
			L = light.direction - v->position;
			L.normalize();
			NL_dot = dotProduct(N, L);
			if (NL_dot > 0) {
				I += att * NL_dot * light.mat.mat_diffuse[channel] * /*mat.mat_diffuse[channel]*/ g_MatDiffuse[channel];
				// specular part if specular element present
				R = 2 * dotProduct(N, L)*N - L;
				R.normalize();
				view = g_cam.eye - v->position.toVector3f();
				view.normalize();
				RV_dot = dotProduct(view, R.toVector3f());
				if (RV_dot > 0)
					I += att * light.mat.mat_specular[channel] * /*mat.mat_specular[channel]*/ g_MatDiffuse[channel] * pow(RV_dot, mat.shine);
			}
		}
		v->color[channel] = I;
	}
}

// RASTERIZATION FUNCTIONS
ARGBf interpolate_colors(float c1[3], float c2[3], float position)
{
	return {position*c1[0] + (1-position)*c2[0],
			position*c1[1] + (1 - position)*c2[1],
			position*c1[2] + (1 - position)*c2[2],
			1};
}

ARGBf interpolate_colors(ARGBf c1, ARGBf c2, float position)
{
	return {position*c1.r + (1 - position)*c2.r,
			position*c1.g + (1 - position)*c2.g,
			position*c1.b + (1 - position)*c2.b,
			1};
}

ARGBf average_color(float c1[3], float c2[3], float c3[3])
{
	return {(c1[0] + c2[0] + c3[0])/3,
			(c1[1] + c2[1] + c3[1])/3,
			(c1[2] + c2[2] + c3[2])/3,
			1};
}

ARGBf average_color(ARGBf c1, ARGBf c2, ARGBf c3)
{
	return	{(c1.r + c2.r + c3.r)/3,
			 (c1.g + c2.g + c3.g)/3,
			 (c1.b + c2.b + c3.b)/3, 
			 1};
}

float interpolate_depths(float z1, float z2, float position)
{
	return (1-position)*z1 + position*z2;
}

bool test_z_buffer(int x, int y, float z)
{
	if (z_buffer[x + y*width] > z) {
		z_buffer[x + y*width] = z;
		return true;
	}
	else
		return false;
}

void set_to_color_buffer(int x, int y, ARGBf color)
{
	color_buffer[x + y*width] = color;
}

void rasterize_triangle(Model::Triangle4f tri)
{
	switch (g_DrawingMode)
	{
		case 0:
		{
			Model::Vertex4f V1, V2, V3, left, right, bottom;
			double dx1, dx2, dx3, dy1, dy2, dy3;
			double incx1, incx2, incz1, incz2;
			double height, y;
			double limit_left, limit_right;
			float depth1, depth2, depth12;
			ARGBf color1, color2, color12;

			bool inverted_triangle = false, upright_triangle = false;

			// select V1 as the topmost vertex + detect special cases.
			if (tri.v[0].position.y >= tri.v[1].position.y && tri.v[0].position.y >= tri.v[2].position.y)
			{
				V1 = tri.v[0];
				if (tri.v[1].position.y > tri.v[2].position.y) { V3 = tri.v[1];	V2 = tri.v[2]; }
				else { V3 = tri.v[2]; V2 = tri.v[1]; }
			}
			else if (tri.v[1].position.y > tri.v[0].position.y && tri.v[1].position.y >= tri.v[2].position.y)
			{
				V1 = tri.v[1];
				if (tri.v[0].position.y > tri.v[2].position.y) { V3 = tri.v[0];	V2 = tri.v[2]; }
				else { V3 = tri.v[2]; V2 = tri.v[0]; }
			}
			else
			{
				V1 = tri.v[2];
				if (tri.v[0].position.y > tri.v[1].position.y) { V3 = tri.v[0];	V2 = tri.v[1]; }
				else { V3 = tri.v[1]; V2 = tri.v[0]; }
			}

			if (round(V1.position.y) == round(V2.position.y) || round(V1.position.y) == round(V3.position.y))
				inverted_triangle = true;
			else if (round(V2.position.y) == round(V3.position.y))
				upright_triangle = true;


			if (upright_triangle)
			{
				// put first vertex in the color/z buffer.
				if (test_z_buffer(V1.position.x, V1.position.y, (float)V1.position.z))
				{
					ARGBf V1color;
					V1color.r = V1.color[0];
					V1color.g = V1.color[1];
					V1color.b = V1.color[2];
					V1color.a = 1;
					set_to_color_buffer(V1.position.x, V1.position.y, V1color);
				}

				// select pair of active edges as V1V2 (1) and V1V3 (2).
				dx1 = V2.position.x - V1.position.x;
				dy1 = V1.position.y - V2.position.y;

				dx2 = V3.position.x - V1.position.x;
				dy2 = V1.position.y - V3.position.y;

				height = dy1; // doesn't matter which since V2.y = V3.y
				incx1 = dx1 / dy1;
				incx2 = dx2 / dy2;

				y = V1.position.y;

				// incrementing y one at a time, rasterize each line.
				for (float n = 1; n <= height; n += 0.5f)
				{
					limit_left = V1.position.x + n * incx1;
					limit_right = V1.position.x + n * incx2;

					// calculate colors and depths at the right/left limits (along the edges): 
					color1 = interpolate_colors(V2.color, V1.color, (float)(n / height));
					color2 = interpolate_colors(V3.color, V1.color, (float)(n / height));

					depth1 = interpolate_depths(V2.position.z, V1.position.z, (float)(n / height));
					depth2 = interpolate_depths(V3.position.z, V1.position.z, (float)(n / height));

					for (int x = limit_left; x <= limit_right; x++)
					{
						int posx = (int)round(x);
						int posy = (int)round(y);

						// interpolate colors and depth between the lines set by V1V2 and V1V3
						color12 = interpolate_colors(color1, color2, (float)((x - limit_left) / (limit_right - limit_left)));
						depth12 = interpolate_depths(depth1, depth2, (float)((x - limit_left) / (limit_right - limit_left)));

						if (test_z_buffer(posx, posy, depth12))
						{
							// z buffer test came back positive. pixel is visible.
							if (g_Shading == 1)
								set_to_color_buffer(posx, posy, color12);
							else
								set_to_color_buffer(posx, posy, average_color(V1.color, V2.color, V3.color));
						}

					}
					y -= 0.5;
				}
			}
			else if (inverted_triangle)
			{
				// put first vertex in the color/z buffer.
				if (test_z_buffer(V2.position.x, V2.position.y, (float)V2.position.z))
				{
					ARGBf V2color;
					V2color.r = V2.color[0];
					V2color.g = V2.color[1];
					V2color.b = V2.color[2];
					V2color.a = 1;
					set_to_color_buffer(V2.position.x, V2.position.y, V2color);
				}

				// select pair of active edges as V2V1 (1) and V2V3 (2).
				dx1 = V2.position.x - V1.position.x;
				dy1 = V2.position.y - V1.position.y;

				dx2 = V2.position.x - V3.position.x;
				dy2 = V2.position.y - V3.position.y;

				height = -dy1; // doesn't matter which since V1.y = V3.y
				incx1 = dx1 / dy1;
				incx2 = dx2 / dy2;

				y = V2.position.y;

				// incrementing y one at a time, rasterize each line.
				for (float n = 1; n <= height; n += 0.5f)
				{
					if (V1.position.x <= V3.position.x)
					{
						limit_left = V2.position.x + n * incx1;
						limit_right = V2.position.x + n * incx2;
					}
					else
					{
						limit_left = V2.position.x + n * incx2;
						limit_right = V2.position.x + n * incx1;
					}

					// calculate colors and depths at the right/left limits (along the edges): 
					color1 = interpolate_colors(V1.color, V2.color, (float)(n / height));
					color2 = interpolate_colors(V3.color, V2.color, (float)(n / height));

					depth1 = interpolate_depths(V1.position.z, V2.position.z, (float)(n / height));
					depth2 = interpolate_depths(V3.position.z, V2.position.z, (float)(n / height));

					for (int x = limit_left; x <= limit_right; x++)
					{
						int posx = (int)round(x);
						int posy = (int)round(y);

						// interpolate colors and depth between the lines set by V1V2 and V1V3
						color12 = interpolate_colors(color1, color2, (float)((x - limit_left) / (limit_right - limit_left)));
						depth12 = interpolate_depths(depth1, depth2, (float)((x - limit_left) / (limit_right - limit_left)));

						if (test_z_buffer(posx, posy, depth12))
						{
							// z buffer test came back positive. pixel is visible.
							if (g_Shading == 1)
								set_to_color_buffer(posx, posy, color12);
							else
								set_to_color_buffer(posx, posy, average_color(V1.color, V2.color, V3.color));
						}

					}
					y += 0.5;
				}
			}
			else // generic, two-step rasterization.
			{
				/*
					GENERIC CASE : first render everything from B to A height, then render the rest.
						 V1
					    / \
					   V2  \
					        V3
				*/

				// put first vertex in the color/z buffer.
				if (test_z_buffer(V1.position.x, V1.position.y, (float)V1.position.z))
				{
					ARGBf V1color;
					V1color.r = V1.color[0];
					V1color.g = V1.color[1];
					V1color.b = V1.color[2];
					V1color.a = 1;
					set_to_color_buffer(V1.position.x, V1.position.y, V1color);
				}

				// select pair of active edges as V1V2 (1) and V1V3 (2).
				dx1 = V2.position.x - V1.position.x;
				dy1 = V1.position.y - V2.position.y;

				dx2 = V3.position.x - V1.position.x;
				dy2 = V1.position.y - V3.position.y;

				incx1 = dx1 / dy1;
				incx2 = dx2 / dy2;

				//V2 is always lower than V3.
				height = dy2;

				y = V1.position.y;
				// incrementing y one at a time, rasterize each line.
				for (float n = 1; n <= height; n += 0.5f)
				{
					if (V2.position.x <= V3.position.x)
					{
						limit_left = V1.position.x + n * incx1;
						limit_right = V1.position.x + n * incx2;
					}
					else
					{
						limit_left = V1.position.x + n * incx2;
						limit_right = V1.position.x + n * incx1;
					}

					// calculate colors and depths at the right/left limits (along the edges): 
					color1 = interpolate_colors(V2.color, V1.color, (float)(n / height));
					color2 = interpolate_colors(V3.color, V1.color, (float)(n / height));

					depth1 = interpolate_depths(V2.position.z, V1.position.z, (float)(n / height));
					depth2 = interpolate_depths(V3.position.z, V1.position.z, (float)(n / height));

					for (int x = limit_left; x <= limit_right; x++)
					{
						int posx = (int)round(x);
						int posy = (int)round(y);

						// interpolate colors and depth between the lines set by V1V2 and V1V3
						color12 = interpolate_colors(color1, color2, (float)((x - limit_left) / (limit_right - limit_left)));
						depth12 = interpolate_depths(depth1, depth2, (float)((x - limit_left) / (limit_right - limit_left)));

						if (test_z_buffer(posx, posy, depth12))
						{
							// z buffer test came back positive. pixel is visible.
							if (g_Shading == 1)
								set_to_color_buffer(posx, posy, color12);
							else
								set_to_color_buffer(posx, posy, average_color(V1.color, V2.color, V3.color));
						}

					}
					y -= 0.5;
				}
				
				// now render the rest of the triangle.
				// set right left and bottom accordingly so that
				/*
						 /   \
						left  \ (right)
							  bottom
				*/

				// V2 is always the bottom vertex.
				bottom = V2;
				if (V1.position.x >= V3.position.x) { left = V3; right = V1; } 
				else { left = V1; right = V3; }

				height = V3.position.y - V2.position.y;

				// put first vertex in the color/z buffer.
				if (test_z_buffer(bottom.position.x, bottom.position.y, (float)bottom.position.z))
				{
					ARGBf V2color;
					V2color.r = bottom.color[0];
					V2color.g = bottom.color[1];
					V2color.b = bottom.color[2];
					V2color.a = 1;
					set_to_color_buffer(bottom.position.x, bottom.position.y, V2color);
				}

				// select pair of active edges as V2V1 (1) and V2V3 (2).
				dx1 = bottom.position.x - left.position.x;
				dy1 = bottom.position.y - left.position.y;

				dx2 = bottom.position.x - right.position.x;
				dy2 = bottom.position.y - right.position.y;

				incx1 = dx1 / dy1;
				incx2 = dx2 / dy2;

				y = bottom.position.y;

				// incrementing y one at a time, rasterize each line.
				for (float n = 1; n <= height; n += 0.5)
				{
					limit_left = bottom.position.x + n * incx1;
					limit_right = bottom.position.x + n * incx2;


					// calculate colors and depths at the right/left limits (along the edges): 
					color1 = interpolate_colors(left.color, bottom.color, (float)(n / height));
					color2 = interpolate_colors(right.color, bottom.color, (float)(n / height));

					depth1 = interpolate_depths(left.position.z, bottom.position.z, (float)(n / height));
					depth2 = interpolate_depths(right.position.z, bottom.position.z, (float)(n / height));

					for (int x = limit_left; x <= limit_right; x++)
					{
						int posx = (int)round(x);
						int posy = (int)round(y);

						// interpolate colors and depth between the lines set by V1V2 and V1V3
						color12 = interpolate_colors(color1, color2, (float)((x - limit_left) / (limit_right - limit_left)));
						depth12 = interpolate_depths(depth1, depth2, (float)((x - limit_left) / (limit_right - limit_left)));

						if (test_z_buffer(posx, posy, depth12))
						{
							// z buffer test came back positive. pixel is visible.
							if (g_Shading == 1)
								set_to_color_buffer(posx, posy, color12);
							else
								set_to_color_buffer(posx, posy, average_color(V1.color, V2.color, V3.color));
						}

					}
					y += 0.5;
				}
			}
		}
		case 1:
		{

		}
		case 2:
		{

		}
	}
	



}

void DrawColorBuffer()
{
	glutSetWindow(close2GLWindow);
	glDrawPixels(width, height, GL_RGBA, GL_FLOAT, color_buffer);
}

void DrawClose2GLTriangle(Model::Triangle4f tri)
{
	if (g_DrawingMode == 0)
	{
		glColor3f(g_MatDiffuse[0], g_MatDiffuse[1], g_MatDiffuse[2]);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glBegin(GL_TRIANGLES);
		glVertex2f(tri.v[0].position.x, tri.v[0].position.y);
		glVertex2f(tri.v[1].position.x, tri.v[1].position.y);
		glVertex2f(tri.v[2].position.x, tri.v[2].position.y);
		glEnd();
	}
	else if (g_DrawingMode == 1)
	{
		glColor3f(g_MatDiffuse[0], g_MatDiffuse[1], g_MatDiffuse[2]);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glBegin(GL_TRIANGLES);
		glVertex2f(tri.v[0].position.x, tri.v[0].position.y);
		glVertex2f(tri.v[1].position.x, tri.v[1].position.y);
		glVertex2f(tri.v[2].position.x, tri.v[2].position.y);
		glEnd();
	}
	else
	{
		glColor3f(g_MatDiffuse[0], g_MatDiffuse[1], g_MatDiffuse[2]);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glBegin(GL_POINTS);
		glVertex2f(tri.v[0].position.x, tri.v[0].position.y);
		glVertex2f(tri.v[1].position.x, tri.v[1].position.y);
		glVertex2f(tri.v[2].position.x, tri.v[2].position.y);
		glEnd();
	}
}

void DisplayClose2GL(void)
{
	glutSetWindow(close2GLWindow);
	float v[4]; // will be used to set light parameters
	
	clear_buffers();

	update_modelview_matrix();

	// Clear frame buffer
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_NORMALIZE);

	std::vector<Model::Triangle4f> triangles = m.triangles;
	//for each vertex, multiply it by model + projected matrixes to obtain projected vertexes.
	for (int i = 0; i < m.numTriangles; i++)
	{
		if (g_LightsOn)
		{
			phong_illumination_model(&triangles[i].v[0]);
			phong_illumination_model(&triangles[i].v[1]);
			phong_illumination_model(&triangles[i].v[2]);
		}
			
		//by modelview
		modelview.transformVector(&triangles[i].v[0].position);
		modelview.transformVector(&triangles[i].v[1].position);
		modelview.transformVector(&triangles[i].v[2].position);

		//by projection matrix
		projection.transformVector(&triangles[i].v[0].position);
		projection.transformVector(&triangles[i].v[1].position);
		projection.transformVector(&triangles[i].v[2].position);
	}

	std::vector<Model::Triangle4f> temp = triangles;

	for (int i = 0; i < triangles.size(); i++)
	{

		/*	Clipping against the normalized perspective view volume is trivial. Points inside
		the view volume are defined by abs(x), abs(y), abs(z) ≤ abs(w).Why ?
		(to simplify your task, you can clip the whole triangle if at least one of its vertices
		fall outside the view volume).
		*/

		// clip
		double w1 = abs(triangles[i].v[0].position.w);
		double w2 = abs(triangles[i].v[1].position.w);
		double w3 = abs(triangles[i].v[2].position.w);


		if (abs(triangles[i].v[0].position.x) <= w1 &&
			abs(triangles[i].v[0].position.y) <= w1 &&
			abs(triangles[i].v[0].position.z) <= w1 &&

			abs(triangles[i].v[1].position.x) <= w2 &&
			abs(triangles[i].v[1].position.y) <= w2 &&
			abs(triangles[i].v[1].position.z) <= w2 &&

			abs(triangles[i].v[2].position.x) <= w3 &&
			abs(triangles[i].v[2].position.y) <= w3 &&
			abs(triangles[i].v[2].position.z) <= w3) {

				temp.push_back(triangles[i]);
		}
	}

	// Do culling
	if (g_PerformBFCulling) {
		triangles.clear();
		for (unsigned int i = 0; i<temp.size(); i++) {

			vector3f face_normal = vector3f(temp[i].normal.x, temp[i].normal.y, temp[i].normal.z);

			switch (g_VertexOrientation) {
			case 0:	//cw
				if (dotProduct(-(g_cam.n), face_normal) > 0)
					triangles.push_back(temp[i]);
				break;
			case 1: //ccw
				if (dotProduct(-(g_cam.n), face_normal) < 0)
					triangles.push_back(temp[i]);
				break;
			}
		}
	}
	else
		triangles = temp;

	//perform persp division then viewport mapping

	for (int i = 0; i < triangles.size(); i++)
	{
		triangles[i].v[0].position = triangles[i].v[0].position / triangles[i].v[0].position.w;
		triangles[i].v[1].position = triangles[i].v[1].position / triangles[i].v[1].position.w;
		triangles[i].v[2].position = triangles[i].v[2].position / triangles[i].v[2].position.w;

		viewport.transformVector(&triangles[i].v[0].position);
		viewport.transformVector(&triangles[i].v[1].position);
		viewport.transformVector(&triangles[i].v[2].position);

		//DrawClose2GLTriangle(triangles[i]);
		rasterize_triangle(triangles[i]);
	}

	DrawColorBuffer();

	// Present frame buffer
	glutSwapBuffers();
}

// Callback function called by GLUT to render screen
void Display(void)
{
	glutSetWindow(mainWindow);

	float v[4]; // will be used to set light parameters

	// Clear frame buffer
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// viewport and frustum setup
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(g_FOVY, tan(g_FOVX*PI / 360) / tan(g_FOVY*PI / 360), g_ZNear, g_ZFar);

	//glMatrixMode(GL_MODELVIEW);
	//glLoadMatrixf(modelview.m); // load OpenGL’s modelview matrix

	// camera setup
	if (g_LookAtObject)
	{
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(	g_cam.eye.x, g_cam.eye.y, g_cam.eye.z,
					obj_center[0], obj_center[1], obj_center[2], 
					g_cam.v.x, g_cam.v.y, g_cam.v.z);

		//gluLookAt(	g_cam.eye.x, g_cam.eye.y, g_cam.eye.z,
		//			g_cam.eye.x + g_cam.n.x, g_cam.eye.y + g_cam.n.y, g_cam.eye.z + g_cam.n.z,
		//			g_cam.v.x, g_cam.v.y, g_cam.v.z);
	}
	else
	{
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(	g_cam.eye.x, g_cam.eye.y, g_cam.eye.z,
					obj_center[0] + g_CamTranslation[0], obj_center[1] + g_CamTranslation[1], obj_center[2] + g_CamTranslation[2],
					g_cam.v.x, g_cam.v.y, g_cam.v.z);
	}

	// culling setup (NOT WORKING?)
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	if (g_PerformBFCulling)
	{
		glFrontFace(GL_CW);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
	}
	else
		glDisable(GL_CULL_FACE);
	glEnable(GL_NORMALIZE);


	// Set light
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	v[0] = v[1] = v[2] = g_LightMultiplier*0.4f; v[3] = 1.0f;
	glLightfv(GL_LIGHT0, GL_AMBIENT, v);
	v[0] = v[1] = v[2] = g_LightMultiplier*0.4f; v[3] = 1.0f;
	glLightfv(GL_LIGHT0, GL_DIFFUSE, v);
	v[0] = -g_LightDirection[0]; v[1] = -g_LightDirection[1]; v[2] = -g_LightDirection[2]; v[3] = 0.0f;
	glLightfv(GL_LIGHT0, GL_POSITION, v);

	// Set material
	
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, g_MatDiffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, g_MatDiffuse);

	// Rotate and draw shape
	//glPushMatrix();
	Draw(m);
	//glPopMatrix();

	// Present frame buffer
	glutSwapBuffers();

	// Recall Display at next frame
	//glutSetWindow(mainWindow);
	//glutPostRedisplay();
}

// Callback function called by GLUT when window size changes
void Reshape(int w, int h)
{
	glutSetWindow(mainWindow);
	
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(g_FOVY, tan(g_FOVX*PI / 360) / tan(g_FOVY*PI / 360), g_ZNear, g_ZFar);
	
	/*// Set OpenGL viewport and camera
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(40, (double)width / height, 1, 10);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//gluLookAt(g_CamPosition[0], g_CamPosition[1], g_CamPosition[2], 0, 0, 0, 0, 1, 0);
	//glTranslatef(0, 0.6f, -1);
	*/

	glMatrixMode(GL_MODELVIEW);

	glutPostRedisplay();
}

void ReshapeClose2GL(int w, int h)
{
	glutSetWindow(close2GLWindow);

	//allocMatrixes();

	glViewport(0, 0, (GLsizei)w, (GLsizei)h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0., width, 0., height);

	glMatrixMode(GL_MODELVIEW);

	update_projection_matrix();

	update_viewport_matrix(0., width, 0., height); //same params of gluOrtho2D

	glutPostRedisplay();
}

void updateWindows(int nil = 0)
{
	glutSetWindow(close2GLWindow);
	glutPostRedisplay();
	glutSetWindow(mainWindow);
	glutPostRedisplay();
}

void updateSettings(int nil)
{
	glutSetWindow(mainWindow);

	// OpenGL
	//switch (shadeOpt) {
	//case 0: glShadeModel(GL_SMOOTH); break;
	//case 1: glShadeModel(GL_FLAT); break;
	//}
	g_PerformBFCulling ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
	//switch (orientationOpt) {
	//case 0: glFrontFace(GL_CW); break;
	//case 1: glFrontFace(GL_CCW); break;
	//}

	updateWindows();
}

void TranslateCam() 
{
	g_cam.slide(g_CamTranslation[0], g_CamTranslation[1], g_CamTranslation[2]);

	// must be vec up = {0,1,0}, otherwise it won't work ==> WHY?
	if (g_LookAtObject)
		g_cam.set(g_cam.eye, { obj_center[0], obj_center[1], obj_center[2] }, { 0, 1, 0 });

	updateWindows();
}

void RotateCam()
{
	g_cam.rotateU(g_CamRotation[0]);
	g_cam.rotateV(g_CamRotation[1]);
	g_cam.rotateN(g_CamRotation[2]);

	// must be vec up = {0,1,0}, otherwise it won't work ==> WHY?
	if (g_LookAtObject)
		g_cam.set(g_cam.eye, { obj_center[0], obj_center[1], obj_center[2] }, { 0, 1, 0 });

	updateWindows();
}

void ResetCam()
{
	set_camera_position_auto();

	g_CamTranslation[0] = 0.0f;
	g_CamTranslation[1] = 0.0f;
	g_CamTranslation[2] = 0.0f;

	g_CamRotation[0] = 0.0f;
	g_CamRotation[1] = 0.0f;
	g_CamRotation[2] = 0.0f;

	updateWindows();
}

void LoadModel()
{
	m = Model(filename);
	set_camera_position_auto();

	updateWindows();
}

void ReshapeBothWindows()
{
	Reshape(width, height);
	ReshapeClose2GL(width, height);
}

void createGuiWindow()
{
	GLUI *glui = GLUI_Master.create_glui("", 0, 100 + width, 0);

	GLUI_Panel *lp = glui->add_panel("light");
	glui->add_checkbox_to_panel(lp, "lights on?", &g_LightsOn, 0, updateSettings);
	GLUI_Panel *lpc = glui->add_panel_to_panel(lp, "light source color");
	GLUI_Spinner *lrSpin = glui->add_spinner_to_panel(lpc, "R:", GLUI_SPINNER_FLOAT, &light.ambient[0]);
	lrSpin->set_float_limits(0., 1., GLUI_LIMIT_CLAMP);
	GLUI_Spinner *lgSpin = glui->add_spinner_to_panel(lpc, "G:", GLUI_SPINNER_FLOAT, &light.ambient[1]);
	lgSpin->set_float_limits(0., 1., GLUI_LIMIT_CLAMP);
	GLUI_Spinner *lbSpin = glui->add_spinner_to_panel(lpc, "B:", GLUI_SPINNER_FLOAT, &light.ambient[2]);
	lbSpin->set_float_limits(0., 1., GLUI_LIMIT_CLAMP);


	GLUI_Panel *mp = glui->add_panel("model");
	GLUI_Panel *mpm = glui->add_panel_to_panel(mp, "");
	glui->add_edittext_to_panel(mpm, "file name:", GLUI_EDITTEXT_TEXT, filename);
	glui->add_button_to_panel(mpm, "load", 0, (GLUI_Update_CB)LoadModel);

	GLUI_Listbox *shading = glui->add_listbox_to_panel(mp, "shading mode: ", &g_Shading, 0, updateSettings);
	shading->add_item(0, "flat");
	shading->add_item(1, "gouraud");

	GLUI_Listbox *models = glui->add_listbox_to_panel(mp, "drawing mode: ", &g_DrawingMode, 0, updateSettings);
	models->add_item(0, "solid");
	models->add_item(1, "wire");
	models->add_item(2, "points");
	
	glui->add_checkbox_to_panel(mp, "backface culling?", &g_PerformBFCulling, 0, updateSettings);
	
	GLUI_Listbox *orientations = glui->add_listbox_to_panel(mp, "vertex orientation: ", &g_VertexOrientation, 0, updateSettings);
	orientations->add_item(0, "CW");
	orientations->add_item(1, "CCW");

	GLUI_Panel *mpc = glui->add_panel_to_panel(mp, "coloring");
	GLUI_Spinner *rSpin = glui->add_spinner_to_panel(mpc, "R:", GLUI_SPINNER_FLOAT, &m.material.mat_diffuse[0]);
	rSpin->set_float_limits(0., 1., GLUI_LIMIT_CLAMP);
	GLUI_Spinner *gSpin = glui->add_spinner_to_panel(mpc, "G:", GLUI_SPINNER_FLOAT, &m.material.mat_diffuse[1]);
	gSpin->set_float_limits(0., 1., GLUI_LIMIT_CLAMP);
	GLUI_Spinner *bSpin = glui->add_spinner_to_panel(mpc, "B:", GLUI_SPINNER_FLOAT, &m.material.mat_diffuse[2]);
	bSpin->set_float_limits(0., 1., GLUI_LIMIT_CLAMP);

	GLUI_Panel *cp = glui->add_panel("camera");
	GLUI_Panel *cptr = glui->add_panel_to_panel(cp, "movement");
	glui->add_checkbox_to_panel(cptr, "look at object?", &g_LookAtObject, 0, updateSettings);

	glui->add_spinner_to_panel(cptr, "translation u:", GLUI_SPINNER_FLOAT, &g_CamTranslation[0]);
	glui->add_spinner_to_panel(cptr, "translation v:", GLUI_SPINNER_FLOAT, &g_CamTranslation[1]);
	glui->add_spinner_to_panel(cptr, "translation n:", GLUI_SPINNER_FLOAT, &g_CamTranslation[2]);
	glui->add_button_to_panel(cptr, "translate", 0, (GLUI_Update_CB)TranslateCam);

	glui->add_spinner_to_panel(cptr, "rotation u:", GLUI_SPINNER_FLOAT, &g_CamRotation[0]);
	glui->add_spinner_to_panel(cptr, "rotation v:", GLUI_SPINNER_FLOAT, &g_CamRotation[1]);
	glui->add_spinner_to_panel(cptr, "rotation n:", GLUI_SPINNER_FLOAT, &g_CamRotation[2]);
	glui->add_button_to_panel(cptr, "rotate", 0, (GLUI_Update_CB)RotateCam);
	
	GLUI_Panel *cpo = glui->add_panel_to_panel(cp, "Other");
	glui->add_spinner_to_panel(cpo, "fovx:", GLUI_SPINNER_FLOAT, &g_FOVX, 0, (GLUI_Update_CB)ReshapeBothWindows);
	glui->add_spinner_to_panel(cpo, "fovy:", GLUI_SPINNER_FLOAT, &g_FOVY, 0, (GLUI_Update_CB)ReshapeBothWindows);
	glui->add_spinner_to_panel(cpo, "near clip:", GLUI_SPINNER_FLOAT, &g_ZNear, 0, (GLUI_Update_CB)ReshapeBothWindows);
	glui->add_spinner_to_panel(cpo, "far clip:", GLUI_SPINNER_FLOAT, &g_ZFar, 0, (GLUI_Update_CB)ReshapeBothWindows);
	glui->add_button_to_panel(cpo, "reset position", 0, (GLUI_Update_CB)ResetCam);

	glui->add_button("quit", 0, exit);
	glui->set_main_gfx_window(mainWindow);
}

// Main
int main(int argc, char *argv[])
{
	allocate_buffers();

	set_light();

	m = Model(filename);
	set_camera_position_auto();

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(width, height);
	//glutCreateWindow("OpenGL");

	//OpenGL Window
	mainWindow = glutCreateWindow("OpenGL");
	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);
	//GLUI_Master.set_glutIdleFunc(Display);
	
	glutInitWindowPosition(width + 260, 0);
	close2GLWindow = glutCreateWindow("Close2GL");
	glutDisplayFunc(DisplayClose2GL);
	glutReshapeFunc(ReshapeClose2GL);
	//GLUI_Master.set_glutIdleFunc(DisplayClose2GL);

	createGuiWindow();
	
	// Call the GLUT main loop
	glutMainLoop();

	return 0;
}