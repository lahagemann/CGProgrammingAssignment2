#include <AntTweakBar.h>

#include <Windows.h>
#include <glut.h>
#include <glu.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <vector>

#include "Model.h"
#include "Camera.h"

// STRUCTURES
typedef struct Matrix {
	float m[16];
};

typedef struct Vec {
	float v[4];
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
float g_FOVX = 128.0f;
float g_FOVY = 72.0f;

// Material Color
float g_MatAmbient[] = { 0.5f, 0.0f, 0.0f, 1.0f };
float g_MatDiffuse[] = { 0.2f, 0.2f, 0.2f, 1.0f };

// Light parameter
float g_LightMultiplier = 1.0f;
float g_LightDirection[] = { 1.1547f, -1.1547f, 1.1547f };

// Booleans
int g_LookAtObject = 1;
int g_PerformBFCulling = 0;

// Options
// This example displays one of the following shapes
typedef enum { SHADING_POINTS = 1, SHADING_WIRE, SHADING_SOLID } Shading;
#define NUM_SHADING 3
Shading g_CurrentShading = SHADING_SOLID;

char filename[256] = "cow_up.in";

// other variables

Model m;
Camera g_cam;

int width = 1280;
int height = 720;
/*
	You can easily set up the viewing frustum using vertical field of view and screen's aspect ratio.
		aspect ratio = width/height
		top = tan(FOV/2) * near
		bottom = -top
		right = top * aspect ratio
		left = bottom = - top * aspect ratio
*/
//near far
float n = 1.0f, f = 2000.0f; 

// FRUSTUM DEFINITION: right left top bottom
float t = tan(g_FOVY / 2.0f) * n;
float b = -t;
float r = t * (width / height);
float l = -r;

// WINDOW SIZE DEFINITION
float rv = 720.0f, lv = 0.0f, tv = 0.0f, bv = 720.0f; //window right left top bottom

// MATRIXES
/*
	mat setup:
		m[0]	m[4]	m[8]	m[12]	
		m[1]	m[5]	m[9]	m[13]
		m[2]	m[6]	m[10]	m[14]
		m[3]	m[7]	m[11]	m[15]
*/
float ModelViewMat[16];
float ProjectionMat[16] = 
{	
	2*n/(r-l),		0.0f,			0.0f,				0.0f,
	0.0f,			2*n/(t-b),		0.0f,				0.0f,	
	(r+l)/(r-l),	(t+b)/(t-b),	-((f+n)/(f-n)),		-1.0f,
	0.0f,			0.0f,			-((2*f*n)/(f-n)),	0.0f 
};

float ViewPortMat[16] = 
{
	(rv-lv)/2,		0.0f,		0.0f,	0.0f,
	0.0f,			(tv-bv)/2,	0.0f,	0.0f,
	0.0f,			0.0f,		1.0f,	0.0f,
	(rv + lv)/2,	(tv+bv)/2,	0.0f,	1.0f
};


// OBJECT CENTER IN WCS
float obj_center[] = { 0.0f, 0.0f, 0.0f };

float g_Zoom = 1.0f;



//Draw model
void Draw(Model m)
{
	if (g_CurrentShading == SHADING_SOLID)
	{
		for (int i = 0; i < m.numTriangles; i++)
		{
			glColor4f(g_MatDiffuse[0], g_MatDiffuse[1], g_MatDiffuse[2], g_MatDiffuse[3]);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glBegin(GL_TRIANGLES);
			glNormal3f(m.triangles[i].normal[0].x, m.triangles[i].normal[0].y, m.triangles[i].normal[0].z);
			glVertex3f(m.triangles[i].v0.x, m.triangles[i].v0.y, m.triangles[i].v0.z);
			glNormal3f(m.triangles[i].normal[1].x, m.triangles[i].normal[1].y, m.triangles[i].normal[1].z);
			glVertex3f(m.triangles[i].v1.x, m.triangles[i].v1.y, m.triangles[i].v1.z);
			glNormal3f(m.triangles[i].normal[2].x, m.triangles[i].normal[2].y, m.triangles[i].normal[2].z);
			glVertex3f(m.triangles[i].v2.x, m.triangles[i].v2.y, m.triangles[i].v2.z);
			glEnd();
		}
	}
	else if (g_CurrentShading == SHADING_WIRE)
	{
		for (int i = 0; i < m.numTriangles; i++)
		{
			glColor4f(g_MatDiffuse[0], g_MatDiffuse[1], g_MatDiffuse[2], g_MatDiffuse[3]);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glBegin(GL_TRIANGLES);
			glNormal3f(m.triangles[i].normal[0].x, m.triangles[i].normal[0].y, m.triangles[i].normal[0].z);
			glVertex3f(m.triangles[i].v0.x, m.triangles[i].v0.y, m.triangles[i].v0.z);
			glNormal3f(m.triangles[i].normal[1].x, m.triangles[i].normal[1].y, m.triangles[i].normal[1].z);
			glVertex3f(m.triangles[i].v1.x, m.triangles[i].v1.y, m.triangles[i].v1.z);
			glNormal3f(m.triangles[i].normal[2].x, m.triangles[i].normal[2].y, m.triangles[i].normal[2].z);
			glVertex3f(m.triangles[i].v2.x, m.triangles[i].v2.y, m.triangles[i].v2.z);
			glEnd();
		}
	}
	else
	{
		for (int i = 0; i < m.numTriangles; i++)
		{
			glColor4f(g_MatDiffuse[0], g_MatDiffuse[1], g_MatDiffuse[2], g_MatDiffuse[3]);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glBegin(GL_POINTS);
			glNormal3f(m.triangles[i].normal[0].x, m.triangles[i].normal[0].y, m.triangles[i].normal[0].z);
			glVertex3f(m.triangles[i].v0.x, m.triangles[i].v0.y, m.triangles[i].v0.z);
			glNormal3f(m.triangles[i].normal[1].x, m.triangles[i].normal[1].y, m.triangles[i].normal[1].z);
			glVertex3f(m.triangles[i].v1.x, m.triangles[i].v1.y, m.triangles[i].v1.z);
			glNormal3f(m.triangles[i].normal[2].x, m.triangles[i].normal[2].y, m.triangles[i].normal[2].z);
			glVertex3f(m.triangles[i].v2.x, m.triangles[i].v2.y, m.triangles[i].v2.z);
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
}

void multiply_matrixes(float m1[16], float m2[16], Matrix *res)
{
	/*
		mat setup:
		m[0]	m[4]	m[8]	m[12]
		m[1]	m[5]	m[9]	m[13]
		m[2]	m[6]	m[10]	m[14]
		m[3]	m[7]	m[11]	m[15]
	*/

	res->m[0] = m1[0] * m2[0] + m1[4] * m2[1] + m1[8] * m2[2] + m1[12] * m2[3];
	//res[1] = m1[1 5 9 13] x m2[0 1 2 3]
	res->m[1] = m1[1] * m2[0] + m1[5] * m2[1] + m1[9] * m2[2] + m1[13] * m2[3];
	//res[2] = m1[2 6 10 14] x m2[0 1 2 3]
	res->m[2] = m1[2] * m2[0] + m1[6] * m2[1] + m1[10] * m2[2] + m1[14] * m2[3];
	//res[3] = m1[3 7 11 15] x m2[0 1 2 3]
	res->m[3] = m1[3] * m2[0] + m1[7] * m2[1] + m1[11] * m2[2] + m1[15] * m2[3];

	//res[4] = m1[0 4 8 12] x m2[4 5 6 7]
	res->m[4] = m1[0] * m2[4] + m1[4] * m2[5] + m1[8] * m2[6] + m1[12] * m2[7];
	//res[5] = m1[1 5 9 13] x m2[4 5 6 7]
	res->m[5] = m1[1] * m2[4] + m1[5] * m2[5] + m1[9] * m2[6] + m1[13] * m2[7];
	//res[6] = m1[2 6 10 14] x m2[4 5 6 7]
	res->m[6] = m1[2] * m2[4] + m1[6] * m2[5] + m1[10] * m2[6] + m1[14] * m2[7];
	//res[7] = m1[3 7 11 15] x m2[4 5 6 7]
	res->m[7] = m1[3] * m2[4] + m1[7] * m2[5] + m1[11] * m2[6] + m1[15] * m2[7];

	//res[8] = m1[0 4 8 12] x m2[8 9 10 11]
	res->m[8] = m1[0] * m2[8] + m1[4] * m2[9] + m1[8] * m2[10] + m1[12] * m2[11];
	//res[9] = m1[1 5 9 13] x m2[8 9 10 11]
	res->m[9] = m1[1] * m2[8] + m1[5] * m2[9] + m1[9] * m2[10] + m1[13] * m2[11];
	//res[10] = m1[2 6 10 14] x m2[8 9 10 11]
	res->m[10] = m1[2] * m2[8] + m1[6] * m2[9] + m1[10] * m2[10] + m1[14] * m2[11];
	//res[11] = m1[3 7 11 15]
	res->m[11] = m1[3] * m2[8] + m1[7] * m2[9] + m1[11] * m2[10] + m1[15] * m2[11];

	//res[12] = m1[0 4 8 12] x m2[12 13 14 15]
	res->m[12] = m1[0] * m2[12] + m1[4] * m2[13] + m1[8] * m2[14] + m1[12] * m2[15];
	//res[13] = m1[1 5 9 13] x m2[12 13 14 15]
	res->m[13] = m1[1] * m2[12] + m1[5] * m2[13] + m1[9] * m2[14] + m1[13] * m2[15];
	//res[14] = m1[2 6 10 14] x m2[12 13 14 15]
	res->m[14] = m1[2] * m2[12] + m1[6] * m2[13] + m1[10] * m2[14] + m1[14] * m2[15];
	//res[15] = m1[3 7 11 15] x m2[12 13 14 15]
	res->m[15] = m1[3] * m2[12] + m1[7] * m2[13] + m1[11] * m2[14] + m1[15] * m2[15];
}

void multiply_vec_by_matrix(float vec[4], float m[16], Vec *res)
{
	/*
		mat setup:
		m[0]	m[4]	m[8]	m[12]
		m[1]	m[5]	m[9]	m[13]
		m[2]	m[6]	m[10]	m[14]
		m[3]	m[7]	m[11]	m[15]
	*/

	res->v[0] = m[0] * vec[0] + m[4] * vec[1] + m[8] * vec[2] + m[12] * vec[3];
	res->v[1] = m[1] * vec[0] + m[5] * vec[1] + m[9] * vec[2] + m[13] * vec[3];
	res->v[2] = m[2] * vec[0] + m[6] * vec[1] + m[10] * vec[2] + m[14] * vec[3];
	res->v[3] = m[3] * vec[0] + m[7] * vec[1] + m[11] * vec[2] + m[15] * vec[3];
}

void DrawClose2GLTriangles()
{
	
}

void DisplayClose2GL(void)
{
	float v[4]; // will be used to set light parameters

	// Clear frame buffer
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// viewport and frustum setup
	// set up viewport and projection matrixes.

	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(g_FOVY, (double)g_FOVX / g_FOVY, g_ZNear, g_ZFar);

	//compute modelviewproj (MP) matrix
	Matrix PM;
	multiply_matrixes(ProjectionMat, g_cam.m, &PM);

	//for each vertex, multiply it by PM matriz to obtain projected vertexes.
	for (int i = 0; i < m.numTriangles; i++)
	{
		Vec v0, v1, v2;
		
		float v[4] = { m.triangles[i].v0.x, m.triangles[i].v0.y, m.triangles[i].v0.z, 1 };
		multiply_vec_by_matrix(v, PM.m, &v0);

		v[0] = m.triangles[i].v1.x; v[1] = m.triangles[i].v1.y; v[2] = m.triangles[i].v1.z;
		multiply_vec_by_matrix(v, PM.m, &v1);

		v[0] = m.triangles[i].v2.x; v[1] = m.triangles[i].v2.y; v[2] = m.triangles[i].v2.z;
		multiply_vec_by_matrix(v, PM.m, &v2);
	}

	//then clip

	//then multiply it by the viewport matrix to obtain pixel coordinates
	
	//then draw it on screen.
	

	// Set light
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	v[0] = v[1] = v[2] = g_LightMultiplier*0.4f; v[3] = 1.0f;
	glLightfv(GL_LIGHT0, GL_AMBIENT, v);
	v[0] = v[1] = v[2] = g_LightMultiplier*0.8f; v[3] = 1.0f;
	glLightfv(GL_LIGHT0, GL_DIFFUSE, v);
	v[0] = -g_LightDirection[0]; v[1] = -g_LightDirection[1]; v[2] = -g_LightDirection[2]; v[3] = 0.0f;
	glLightfv(GL_LIGHT0, GL_POSITION, v);

	// Set material
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, g_MatAmbient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, g_MatDiffuse);

	// Rotate and draw shape
	glPushMatrix();
	glScalef(g_Zoom, g_Zoom, g_Zoom);
	Draw(m);
	glPopMatrix();

	// Draw tweak bars
	TwDraw();

	// Present frame buffer
	glutSwapBuffers();

	// Recall Display at next frame
	glutPostRedisplay();
}


// Callback function called by GLUT to render screen
void Display(void)
{
	float v[4]; // will be used to set light parameters

	// Clear frame buffer
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// viewport and frustum setup
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(g_FOVY, (double)g_FOVX / g_FOVY, g_ZNear, g_ZFar);

	// camera setup
	if (g_LookAtObject)
	{
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(g_CamPosition[0] + g_CamTranslation[0], g_CamPosition[1] + g_CamTranslation[1],
			g_CamPosition[2] + g_CamTranslation[2], obj_center[0], obj_center[1], obj_center[2], 0, 1, 0);
	}
	else
	{
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(g_CamPosition[0] + g_CamTranslation[0], g_CamPosition[1] + g_CamTranslation[1], g_CamPosition[2] + g_CamTranslation[2],
			obj_center[0] + g_CamTranslation[0], obj_center[1] + g_CamTranslation[1], obj_center[2] + g_CamTranslation[2],
			0, 1, 0);
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
	v[0] = v[1] = v[2] = g_LightMultiplier*0.8f; v[3] = 1.0f;
	glLightfv(GL_LIGHT0, GL_DIFFUSE, v);
	v[0] = -g_LightDirection[0]; v[1] = -g_LightDirection[1]; v[2] = -g_LightDirection[2]; v[3] = 0.0f;
	glLightfv(GL_LIGHT0, GL_POSITION, v);

	// Set material
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, g_MatAmbient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, g_MatDiffuse);

	// Rotate and draw shape
	glPushMatrix();
	glScalef(g_Zoom, g_Zoom, g_Zoom);
	//if (g_CurrentShading == SHADING_SOLID)
	//	glutSolidTeapot(1.0);
	//else
	//	glutWireTeapot(1.0);
	Draw(m);
	glPopMatrix();

	// Draw tweak bars
	TwDraw();

	// Present frame buffer
	glutSwapBuffers();

	// Recall Display at next frame
	glutPostRedisplay();
}

// Callback function called by GLUT when window size changes
void Reshape(int width, int height)
{
	// Set OpenGL viewport and camera
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(40, (double)width / height, 1, 10);
	//glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();
	//gluLookAt(g_CamPosition[0], g_CamPosition[1], g_CamPosition[2], 0, 0, 0, 0, 1, 0);
	glTranslatef(0, 0.6f, -1);

	// Send the new window size to AntTweakBar
	TwWindowSize(width, height);
}

// Function called at exit
void Terminate(void)
{
	TwTerminate();
}

//  Callback function called when the 'LookAtObject' variable value of the tweak bar has changed
void TW_CALL SetLookAtObjectCB(const void *value, void *clientData)
{
	(void)clientData; // unused

	g_LookAtObject = *(const int *)value; // copy value to LookAtObject
}

//  Callback function called by the tweak bar to get the 'LookAtObject' value
void TW_CALL GetLookAtObjectCB(void *value, void *clientData)
{
	(void)clientData; // unused
	*(int *)value = g_LookAtObject; // copy LookAtObject to value
}

//  Callback function called when the 'BFCulling' variable value of the tweak bar has changed
void TW_CALL SetBFCullingCB(const void *value, void *clientData)
{
	(void)clientData; // unused

	g_PerformBFCulling = *(const int *)value; // copy value to g_PerformBFCulling
}

//  Callback function called by the tweak bar to get the 'BFCulling' value
void TW_CALL GetBFCullingCB(void *value, void *clientData)
{
	(void)clientData; // unused
	*(int *)value = g_PerformBFCulling; // copy g_PerformBFCulling to value
}

void TW_CALL ResetCam(void * /*clientData*/)
{
	set_camera_position_auto();

	g_CamTranslation[0] = 0.0f;
	g_CamTranslation[1] = 0.0f;
	g_CamTranslation[2] = 0.0f;

	g_CamRotation[0] = 0.0f;
	g_CamRotation[1] = 0.0f;
	g_CamRotation[2] = 0.0f;
}

void TW_CALL TranslateCB(void * /*clientData*/)
{
	g_cam.slide(g_CamTranslation[0], g_CamTranslation[1], g_CamTranslation[2]);

	// must be vec up = {0,1,0}, otherwise it won't work ==> WHY?
	if (g_LookAtObject)
		g_cam.set(g_cam.eye, { obj_center[0], obj_center[1], obj_center[2] }, { 0, 1, 0 });

}

void TW_CALL RotateCB(void * /*clientData*/)
{
	g_cam.rotateU(g_CamRotation[0]);
	g_cam.rotateV(g_CamRotation[1]);
	g_cam.rotateN(g_CamRotation[2]);
	
	// must be vec up = {0,1,0}, otherwise it won't work ==> WHY?
	if (g_LookAtObject)
		g_cam.set(g_cam.eye, { obj_center[0], obj_center[1], obj_center[2] }, { 0, 1, 0 });

}

void TW_CALL LoadModel(void * /*clientData*/)
{
	m = Model(filename);
	//set_camera_position_auto();
}

// Main
int main(int argc, char *argv[])
{
	TwBar *bar; // Pointer to the tweak bar
	float axis[] = { 0.7f, 0.7f, 0.0f }; // initial model rotation
	float angle = 0.8f;

	m = Model(filename);
	set_camera_position_auto();

	// Initialize GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(1280, 720);
	glutCreateWindow("Programming Assignment 2");
	glutCreateMenu(NULL);

	// Set GLUT callbacks
	glutDisplayFunc(DisplayClose2GL);
	glutReshapeFunc(Reshape);
	atexit(Terminate);  // Called after glutMainLoop ends

	// Initialize AntTweakBar
	TwInit(TW_OPENGL, NULL);

	// Set GLUT event callbacks
	// - Directly redirect GLUT mouse button events to AntTweakBar
	glutMouseFunc((GLUTmousebuttonfun)TwEventMouseButtonGLUT);
	// - Directly redirect GLUT mouse motion events to AntTweakBar
	glutMotionFunc((GLUTmousemotionfun)TwEventMouseMotionGLUT);
	// - Directly redirect GLUT mouse "passive" motion events to AntTweakBar (same as MouseMotion)
	glutPassiveMotionFunc((GLUTmousemotionfun)TwEventMouseMotionGLUT);
	// - Directly redirect GLUT key events to AntTweakBar
	glutKeyboardFunc((GLUTkeyboardfun)TwEventKeyboardGLUT);
	// - Directly redirect GLUT special key events to AntTweakBar
	glutSpecialFunc((GLUTspecialfun)TwEventSpecialGLUT);
	// - Send 'glutGetModifers' function pointer to AntTweakBar;
	//   required because the GLUT key event functions do not report key modifiers states.
	TwGLUTModifiersFunc(glutGetModifiers);

	// Create a tweak bar
	bar = TwNewBar("Assignment 1");
	TwDefine(" GLOBAL help='This example shows how to integrate AntTweakBar with GLUT and OpenGL.' "); // Message added to the help bar.
	TwDefine(" TweakBar size='200 700' color='96 216 224' "); // change default tweak bar size and color

	//cam position
	TwAddVarRW(bar, "CamPosition", TW_TYPE_DIR3F, &g_cam.eye,
		" label='Camera Position' opened=false help='Change the camera position.' ");

	// cam translation
	TwAddVarRW(bar, "CamTranslation", TW_TYPE_DIR3F, &g_CamTranslation,
		" label='Camera Translation' opened=true help='Translate camera along u v or n.' ");

	// translate confirmation
	TwAddButton(bar, "Translate", TranslateCB, NULL, " label='Translate' ");

	// cam rotation
	TwAddVarRW(bar, "CamRotation", TW_TYPE_DIR3F, &g_CamRotation,
		" label='Camera Rotation' opened=true help='Rotate camera along u v or n.' ");

	// rotate confirmation
	TwAddButton(bar, "Rotate", RotateCB, NULL, " label='Rotate' ");

	// Look at object?
	TwAddVarCB(bar, "LookAtObject", TW_TYPE_BOOL32, SetLookAtObjectCB, GetLookAtObjectCB, NULL,
		" label='Look at object' key=space help='Toggle look at object.' ");
	// ...
	TwAddButton(bar, "Reset", ResetCam, NULL, " label='Reset' ");

	// clipping plane
	// z far
	TwAddVarRW(bar, "Z Far", TW_TYPE_FLOAT, &g_ZFar,
		" min=0.0 max=4000.0 step=0.5 keyIncr=f keyDecr=F help='Rotate camera along z axis ' ");

	// z near
	TwAddVarRW(bar, "Z Near", TW_TYPE_FLOAT, &g_ZNear,
		" min=0.0 max=100.0 step=0.5 keyIncr=n keyDecr=N help='Rotate camera along z axis ' ");

	// fov x
	TwAddVarRW(bar, "FOV x", TW_TYPE_FLOAT, &g_FOVX,
		" min=0.0 max=500.0 step=0.5 keyIncr=f keyDecr=F help='Rotate camera along z axis ' ");

	// fov y
	TwAddVarRW(bar, "FOV y", TW_TYPE_FLOAT, &g_FOVY,
		" min=0.0 max=500.0 step=0.5 keyIncr=n keyDecr=N help='Rotate camera along z axis ' ");

	// object/model variables

	// shading mode
	// (before adding an enum variable, its enum type must be declared to AntTweakBar as follow)
	{
		// ShapeEV associates Shape enum values with labels that will be displayed instead of enum values
		TwEnumVal shadingEV[NUM_SHADING] = { { SHADING_POINTS, "Points" }, { SHADING_WIRE, "Wire" }, { SHADING_SOLID, "Solid" } };
		// Create a type for the enum shapeEV
		TwType shadingType = TwDefineEnum("ShapeType", shadingEV, NUM_SHADING);
		// add 'g_CurrentShape' to 'bar': this is a variable of type ShapeType. Its key shortcuts are [<] and [>].
		TwAddVarRW(bar, "Shading", shadingType, &g_CurrentShading, " keyIncr='<' keyDecr='>' help='Change object shape.' ");
	}

	// Back Face Culling?
	TwAddVarCB(bar, "BFCulling", TW_TYPE_BOOL32, SetBFCullingCB, GetBFCullingCB, NULL,
		" label='Back Face Culling' key=enter help='Toggle back face culling.' ");

	// Add 'g_MatAmbient' to 'bar': this is a variable of type TW_TYPE_COLOR3F (3 floats color, alpha is ignored)
	// and is inserted into a group named 'Material'.
	TwAddVarRW(bar, "Ambient", TW_TYPE_COLOR3F, &g_MatAmbient, " group='Material' ");

	// Add 'g_MatDiffuse' to 'bar': this is a variable of type TW_TYPE_COLOR3F (3 floats color, alpha is ignored)
	// and is inserted into group 'Material'.
	TwAddVarRW(bar, "Diffuse", TW_TYPE_COLOR3F, &g_MatDiffuse, " group='Material' ");

	// load .obj file
	TwAddVarRW(bar, "Filename", TW_TYPE_CSSTRING(sizeof(filename)), filename, " label='file' group=CDString help='type file name.' ");

	// ...
	TwAddButton(bar, "Load", LoadModel, NULL, " label='Load Model' ");

	// Call the GLUT main loop
	glutMainLoop();

	return 0;
}