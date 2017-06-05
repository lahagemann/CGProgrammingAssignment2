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
//float g_MatAmbient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
float g_MatDiffuse[] = { 0.5f, 0.0f, 0.0f, 1.0f };

// Light parameter
float g_LightMultiplier = 1.0f;
float g_LightDirection[] = { 1.1547f, -1.1547f, 1.1547f };

// Booleans
int g_LookAtObject = 1;
int g_PerformBFCulling = 0;
int g_VertexOrientation = 0;

// Options
int g_DrawingMode = 0;
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

float g_Zoom = 1.0f;


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
	if (z_buffer[x + y*width] > z) 
	{
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


void RasterizeTriangle(Model::Triangle4f tri) 
{
	switch (g_DrawingMode)
	{
		case 0: // solid
		{
			double dx0, dy0, dx1, dy1, dx2, dy2;
			double incx0, incx1, incx2, newincxA, newincxB;
			double y, x1, x2, raster_limit_left, raster_limit_right;
			double dy, starting_x;
			double depthBA, depthBC;
			ARGBf colorBA, colorBC;
			int rasterX, rasterY;

			Model::Vertex4f A, B, C, bottom, newA, newB;

			/*
				Order vertexes by coordinate y
				start by upper vertex
					    B
					   / \
					  A   \
						   C
			*/

			if ((tri.v[0].position.y > tri.v[1].position.y) && (tri.v[0].position.y > tri.v[2].position.y))
			{
				B = tri.v[0];
				if ((tri.v[1].position.x < tri.v[2].position.x))
				{
					A = tri.v[1];
					C = tri.v[2];
				}
				else
				{
					C = tri.v[1];
					A = tri.v[2];
				}
			}
			else if ((tri.v[1].position.y > tri.v[0].position.y) && (tri.v[1].position.y > tri.v[2].position.y))
			{
				B = tri.v[1];
				if ((tri.v[0].position.x < tri.v[2].position.x))
				{
					A = tri.v[0];
					C = tri.v[2];
				}
				else
				{
					C = tri.v[0];
					A = tri.v[2];
				}
			}
			else
			{
				B = tri.v[2];
				if ((tri.v[0].position.x < tri.v[1].position.x))
				{
					A = tri.v[0];
					C = tri.v[1];
				}
				else
				{
					C = tri.v[0];
					A = tri.v[1];
				}
			}
				
			dx0 = B.position.x - A.position.x;
			dx1 = C.position.x - B.position.x;
			dx2 = A.position.x - C.position.x;

			dy0 = B.position.y - A.position.y;
			dy1 = B.position.y - C.position.y;
			dy2 = A.position.y - C.position.y;

			/* Phase 1: rasterize from top to next vertex sorted by y position
				   ( B ) 
				  ( / \ )
				 ( A   \ )
				        \
				         C
			*/
			if (dy2 > 0) // A > C, stops at B-A.
				dy = dy0;
			else      // A < C, stops at B-C.
				dy = dy1;

			incx0 = dx0 / dy0;
			incx1 = dx1 / dy1;
			incx2 = dx2 / dy2;

			y = B.position.y;
			
			// now rasterize each line.
			for (int n = 0; n <= dy; n++)
			{
				/*
						 bx
					   x1  x2
				*/
				x1 = B.position.x - n*incx0;
				x2 = B.position.x + n*incx1;
				y--;

				if (g_Shading == 1) //GOURAUD 
				{
					// interpolate colors in active edges B-A and B-C in current position n/dy
					colorBA = interpolate_colors(B.color, A.color, (float)n/dy);
					colorBC = interpolate_colors(B.color, C.color, (float)n/dy);
				}

				//interpolate zs of active edges B-A and B-C
				depthBA = interpolate_depths(B.position.z, A.position.z, (float)n/dy);
				depthBC = interpolate_depths(B.position.z, A.position.z, (float)n/dy);

				if (x1 < x2)
				{
					raster_limit_left = x1;
					raster_limit_right = x2;
				}
				else
				{
					raster_limit_left = x2;
					raster_limit_right = x1;
				}

				// now fill the line interpolating the colors
				for (int x = raster_limit_left; x <= raster_limit_right; x++)
				{
					rasterX = (int) round(x);
					rasterY = (int) round(y);
					float current_depth = interpolate_depths(depthBA, depthBC, (float)(x-raster_limit_left)/(raster_limit_right-raster_limit_left));
				
					//test z buffer
					if (test_z_buffer(rasterX, rasterY, current_depth))
					{
						if (g_Shading == 1) // gouraud
							set_to_color_buffer(rasterX, rasterY, interpolate_colors(colorBA, colorBC, (float)(x - raster_limit_left) / (raster_limit_right - raster_limit_left)));
						else //flat
							set_to_color_buffer(rasterX, rasterY, average_color(B.color, A.color, C.color));
					}
				}
			}

			//Phase 2: rasterize the bottom of the triangle
			// Rasterize Phase 2 - bottom part
			/*
					     B  
					    / \ 
					  ( A  new B )
					 (	    \	  )
					(		 C	   )
				*/

			if (dy2 > 0) { // A > C, C bellow A
				dy = dy1 - dy0; //B-C - B-A (what's left)
				newincxA = incx2;
				newincxB = -incx1;
				bottom = C;
				newA = A;
				newB = B;
			}
			else { //A < C, A bellow C
				dy = dy0 - dy1; // B-A - B-C (what's left)
				newincxA = incx0;
				newincxB = incx2;
				bottom = A;
				newA = B;
				newB = C;
			}

			starting_x = bottom.position.x;
			y = bottom.position.y;
			for (int n = 0; n <= dy; n++) 
			{
				x1 = starting_x + n*newincxA;
				x2 = starting_x + n*newincxB;
				y++;

				if (g_Shading == 1) //GOURAUD 
				{
					// interpolate colors in active edges C-A and C-newB in current position n/dy
					colorBA = interpolate_colors(bottom.color, newA.color, (float)n/dy);
					colorBC = interpolate_colors(bottom.color, newB.color, (float)n/dy);
				}

				//interpolate zs of active edges C-A and C-newB
				depthBA = interpolate_depths(bottom.position.z, newA.position.z, (float)n/dy);
				depthBC = interpolate_depths(bottom.position.z, newB.position.z, (float)n/dy);

				if (x1 < x2)
				{
					raster_limit_left = x1;
					raster_limit_right = x2;
				}
				else
				{
					raster_limit_left = x2;
					raster_limit_right = x1;
				}

				// now fill the line interpolating the colors
				for (int x = raster_limit_left; x <= raster_limit_right; x++)
				{
					rasterX = (int)round(x);
					rasterY = (int)round(y);
					float current_depth = interpolate_depths(depthBA, depthBC, (float)(x - raster_limit_left) / (raster_limit_right - raster_limit_left));

					//test z buffer
					if (test_z_buffer(rasterX, rasterY, current_depth))
					{
						if (g_Shading == 1) // gouraud
							set_to_color_buffer(rasterX, rasterY, interpolate_colors(colorBA, colorBC, (float)(x - raster_limit_left) / (raster_limit_right - raster_limit_left)));
						else //flat
							set_to_color_buffer(rasterX, rasterY, average_color(B.color, A.color, C.color));
					}
				}
			}
		}
		break;
		case 1: // wire
		{

		}
		break;
		case 2: //points
		{

		}
		break;
	}
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
	update_modelview_matrix();

	// Clear frame buffer
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_NORMALIZE);

	std::vector<Model::Triangle4f> triangles = m.triangles;
	//for each vertex, multiply it by model + projected matrixes to obtain projected vertexes.
	for (int i = 0; i < m.numTriangles; i++)
	{
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

		DrawClose2GLTriangle(triangles[i]);
	}
	

	// Present frame buffer
	glutSwapBuffers();
	//glFlush();

	// Recall Display at next frame
	//glutPostRedisplay();
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

	GLUI_Panel *mp = glui->add_panel("model");
	GLUI_Panel *mpm = glui->add_panel_to_panel(mp, "");
	glui->add_edittext_to_panel(mpm, "file name:", GLUI_EDITTEXT_TEXT, filename);
	glui->add_button_to_panel(mpm, "load", 0, (GLUI_Update_CB)LoadModel);

	GLUI_Listbox *models = glui->add_listbox_to_panel(mp, "drawing Mode: ", &g_DrawingMode, 0, updateSettings);
	models->add_item(0, "solid");
	models->add_item(1, "wire");
	models->add_item(2, "points");
	
	glui->add_checkbox_to_panel(mp, "backface culling?", &g_PerformBFCulling, 0, updateSettings);
	
	GLUI_Listbox *orientations = glui->add_listbox_to_panel(mp, "vertex orientation: ", &g_VertexOrientation, 0, updateSettings);
	orientations->add_item(0, "CW");
	orientations->add_item(1, "CCW");

	GLUI_Listbox *shading = glui->add_listbox_to_panel(mp, "shading: ", &g_Shading, 0, updateSettings);
	orientations->add_item(0, "flat");
	orientations->add_item(1, "gouraud");

	GLUI_Panel *mpc = glui->add_panel_to_panel(mp, "coloring");
	GLUI_Spinner *rSpin = glui->add_spinner_to_panel(mpc, "R:", GLUI_SPINNER_FLOAT, &g_MatDiffuse[0]);
	rSpin->set_float_limits(0., 1., GLUI_LIMIT_CLAMP);
	GLUI_Spinner *gSpin = glui->add_spinner_to_panel(mpc, "G:", GLUI_SPINNER_FLOAT, &g_MatDiffuse[1]);
	gSpin->set_float_limits(0., 1., GLUI_LIMIT_CLAMP);
	GLUI_Spinner *bSpin = glui->add_spinner_to_panel(mpc, "B:", GLUI_SPINNER_FLOAT, &g_MatDiffuse[2]);
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
	float axis[] = { 0.7f, 0.7f, 0.0f }; // initial model rotation
	float angle = 0.8f;

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