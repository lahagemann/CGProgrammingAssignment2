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
float g_MatAmbient[] = { 0.5f, 0.0f, 0.0f, 1.0f };
float g_MatDiffuse[] = { 0.2f, 0.2f, 0.2f, 1.0f };

// Light parameter
float g_LightMultiplier = 1.0f;
float g_LightDirection[] = { 1.1547f, -1.1547f, 1.1547f };

// Booleans
int g_LookAtObject = 1;
int g_PerformBFCulling = 0;

// Options
int g_DrawingMode = 0;
char filename[256] = "cube.in";

// other variables
Model m;
Camera g_cam;
int mainWindow, close2GLWindow;
long int frameCounter, frameCounter2, fps, fps2;

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

void updateFPS(int value) {
	fps = frameCounter;
	frameCounter = 0;

	glutTimerFunc(1000/*1sec*/, updateFPS, 0);
}

void updateFPS2(int value) {
	fps2 = frameCounter2;
	frameCounter2 = 0;

	glutTimerFunc(1000/*1sec*/, updateFPS2, 0);
}

//Draw model
void Draw(Model m)
{
	if (g_DrawingMode == 0)
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
	else if (g_DrawingMode == 1)
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

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(modelview.m); // load OpenGL’s modelview matrix
}

void DrawClose2GLTriangle(vector4f v0, vector4f v1, vector4f v2)
{
	if (g_DrawingMode == 0)
	{
		glColor3f(g_MatDiffuse[0], g_MatDiffuse[1], g_MatDiffuse[2]);
		glBegin(GL_TRIANGLES);
		glVertex2f(v0.x, v0.y);
		glVertex2f(v1.x, v1.y);
		glVertex2f(v2.x, v2.y);
		glEnd();
	}
	else if (g_DrawingMode == 1)
	{
		glColor3f(g_MatDiffuse[0], g_MatDiffuse[1], g_MatDiffuse[2]);
		glBegin(GL_LINES);
		glVertex2f(v0.x, v0.y);
		glVertex2f(v1.x, v1.y);
		glVertex2f(v2.x, v2.y);
		glEnd();
	}
	else
	{
		glColor3f(g_MatDiffuse[0], g_MatDiffuse[1], g_MatDiffuse[2]);
		glBegin(GL_POINTS);
		glVertex2f(v0.x, v0.y);
		glVertex2f(v1.x, v1.y);
		glVertex2f(v2.x, v2.y);
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

	//for each vertex, multiply it by model + projected matrixes to obtain projected vertexes.
	for (int i = 0; i < m.numTriangles; i++)
	{
		//by modelview
		vector4f v0 = vector4f(m.triangles[i].v0.x, m.triangles[i].v0.y, m.triangles[i].v0.z, 1);
		modelview.transformVector(&v0);

		vector4f v1 = vector4f(m.triangles[i].v1.x, m.triangles[i].v1.y, m.triangles[i].v1.z, 1);
		modelview.transformVector(&v1);

		vector4f v2 = vector4f(m.triangles[i].v2.x, m.triangles[i].v2.y, m.triangles[i].v2.z, 1);
		modelview.transformVector(&v2);

		//by projection matrix
		projection.transformVector(&v0);
		projection.transformVector(&v1);
		projection.transformVector(&v2);
	

		/*	Clipping against the normalized perspective view volume is trivial. Points inside
			the view volume are defined by abs(x), abs(y), abs(z) ≤ abs(w).Why ?
			(to simplify your task, you can clip the whole triangle if at least one of its vertices
			fall outside the view volume). 
		*/

		// clip
		
		double w1 = abs(v0.w);
		double w2 = abs(v1.w);
		double w3 = abs(v2.w);

		if (abs(v0.x) <= w1 &&
			abs(v0.y) <= w1 &&
			abs(v0.z) <= w1 &&

			abs(v1.x) <= w2 &&
			abs(v1.y) <= w2 &&
			abs(v1.z) <= w2 &&

			abs(v2.x) <= w3 &&
			abs(v2.y) <= w3 &&
			abs(v2.z) <= w3)
		{
			//perform persp division then viewport mapping

			v0 = v0 / v0.w;
			v1 = v1 / v1.w;
			v2 = v2 / v2.w;

			viewport.transformVector(&v0);
			viewport.transformVector(&v1);
			viewport.transformVector(&v2);

			DrawClose2GLTriangle(v0, v1, v2);
		}
		
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
	//glScalef(g_Zoom, g_Zoom, g_Zoom);
	//if (g_CurrentShading == SHADING_SOLID)
	//	glutSolidTeapot(1.0);
	//else
	//	glutWireTeapot(1.0);
	Draw(m);
	glPopMatrix();

	// Present frame buffer
	glutSwapBuffers();

	// Recall Display at next frame
	glutSetWindow(mainWindow);
	glutPostRedisplay();
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
	GLUI *glui = GLUI_Master.create_glui("", 0, 100 + width + 10, 500 - 30);

	GLUI_Panel *mp = glui->add_panel("Model");
	GLUI_Panel *mpm = glui->add_panel_to_panel(mp, "");
	glui->add_edittext_to_panel(mpm, "File name:", GLUI_EDITTEXT_TEXT, filename);
	glui->add_button_to_panel(mpm, "Load", 0, (GLUI_Update_CB)LoadModel);

	//GLUI_Listbox *orientations = glui->add_listbox_to_panel(mp, "V. Orientation: ", &orientationOpt, 0, updateSettings);
	//orientations->add_item(0, "CW");
	//orientations->add_item(1, "CCW");

	GLUI_Listbox *models = glui->add_listbox_to_panel(mp, "Drawing Mode: ", &g_DrawingMode, 0, updateSettings);
	models->add_item(0, "Solid");
	models->add_item(1, "Wire");
	models->add_item(2, "Points");
	glui->add_checkbox_to_panel(mp, "Backface Culling?", &g_PerformBFCulling, 0, updateSettings);

	GLUI_Panel *mpc = glui->add_panel_to_panel(mp, "Coloring");
	GLUI_Spinner *rSpin = glui->add_spinner_to_panel(mpc, "R:", GLUI_SPINNER_FLOAT, &g_MatDiffuse[0]);
	rSpin->set_float_limits(0., 1., GLUI_LIMIT_CLAMP);
	GLUI_Spinner *gSpin = glui->add_spinner_to_panel(mpc, "G:", GLUI_SPINNER_FLOAT, &g_MatDiffuse[1]);
	gSpin->set_float_limits(0., 1., GLUI_LIMIT_CLAMP);
	GLUI_Spinner *bSpin = glui->add_spinner_to_panel(mpc, "B:", GLUI_SPINNER_FLOAT, &g_MatDiffuse[2]);
	bSpin->set_float_limits(0., 1., GLUI_LIMIT_CLAMP);

	GLUI_Panel *cp = glui->add_panel("Camera");

	GLUI_Panel *cptr = glui->add_panel_to_panel(cp, "Movement");
	glui->add_checkbox_to_panel(mp, "Look at object?", &g_LookAtObject, 0, updateSettings);
	glui->add_spinner_to_panel(cptr, "translation u:", GLUI_SPINNER_FLOAT, &g_CamTranslation[0]);
	glui->add_spinner_to_panel(cptr, "translation v:", GLUI_SPINNER_FLOAT, &g_CamTranslation[1]);
	glui->add_spinner_to_panel(cptr, "translation n:", GLUI_SPINNER_FLOAT, &g_CamTranslation[2]);
	glui->add_button_to_panel(cptr, "Translate", 0, (GLUI_Update_CB)TranslateCam);
	glui->add_spinner_to_panel(cptr, "rotation u:", GLUI_SPINNER_FLOAT, &g_CamRotation[0]);
	glui->add_spinner_to_panel(cptr, "rotation v:", GLUI_SPINNER_FLOAT, &g_CamRotation[1]);
	glui->add_spinner_to_panel(cptr, "rotation n:", GLUI_SPINNER_FLOAT, &g_CamRotation[2]);
	glui->add_button_to_panel(cptr, "Rotate", 0, (GLUI_Update_CB)RotateCam);
	
	GLUI_Panel *cpo = glui->add_panel_to_panel(cp, "Other");
	glui->add_spinner_to_panel(cpo, "fovx:", GLUI_SPINNER_FLOAT, &g_FOVX, 0, (GLUI_Update_CB)ReshapeBothWindows);
	glui->add_spinner_to_panel(cpo, "fovy:", GLUI_SPINNER_FLOAT, &g_FOVY, 0, (GLUI_Update_CB)ReshapeBothWindows);
	glui->add_spinner_to_panel(cpo, "Near Clip:", GLUI_SPINNER_FLOAT, &g_ZNear, 0, (GLUI_Update_CB)ReshapeBothWindows);
	glui->add_spinner_to_panel(cpo, "Far Clip:", GLUI_SPINNER_FLOAT, &g_ZFar, 0, (GLUI_Update_CB)ReshapeBothWindows);
	glui->add_button_to_panel(cpo, "Reset position", 0, (GLUI_Update_CB)ResetCam);

	glui->add_button("EXIT", 0, exit);
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