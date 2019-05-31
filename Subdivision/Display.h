#pragma once



#include "CCsubdivision.h"
#include "Loopsubdivision.h"

#define kRotationStep 10.0f
#define kTranslationStep 0.5f
#define BUFFER_LENGTH 64 

class Display
{
public:
	void initLights(void);
	void SetCamera(void);
	void Init();
	void displayMesh();
	void updateMesh();

protected:
	int mode = 0;
	int last_x, last_y, spin_x, spin_y;
	int animate = 0;
	char titleString[150];

};
Display temp;
static int du = 90, oldmx = -1, oldmy = -1;
static float r = 1.5f, h = 0.0f;
int currentMesh = 0;
vector<Mesh> myMesh;
Mesh mesh;
GLfloat ambient[] = { 0.2, 0.2, 0.2, 1.0 };
GLfloat position[] = { 0.0, 0.0, 2.0, 1.0 };
GLfloat mat_diffuse[] = { 0.6, 0.6, 0.6, 1.0 };
GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat mat_shininess[] = { 50.0 };
GLfloat camRotX, camRotY, camPosX, camPosY, camPosZ;
GLboolean isSmooth;
GLboolean showLine;

GLint viewport[4];
GLdouble modelview[16];
GLdouble projection[16];

GLuint pickedObj = -1;
void Display::initLights(void)
{
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_POSITION, position);

	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
}

void Display::SetCamera(void)
{
	glTranslatef(0, 0, camPosZ);
	glRotatef(camRotX, 1, 0, 0);
	glRotatef(camRotY, 0, 1, 0);
}

void Display:: Init()
{
	

	glClearColor(0.0, 0.0, 1.0, 0.0);//修改背景颜色值
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);

	initLights();

	camRotX = 380.0f;
	camRotY = 700.0f;
	camPosX = 0.0f;
	camPosY = 0.0f;
	camPosZ = -10.5f;

	isSmooth = true;
}


//网格显示
void Display::displayMesh()
{
	Mesh mesh;
	mesh = myMesh[currentMesh];

	for (unsigned int i = 0; i<mesh.faces.size(); ++i)
	{
				glBegin(GL_LINE_LOOP);
		for (unsigned int j = 0; j<mesh.faces[i].ver_id.size(); ++j)
		{
			glVertex3f(mesh.vertices[mesh.faces[i].ver_id[j]].point.x, mesh.vertices[mesh.faces[i].ver_id[j]].point.y, mesh.vertices[mesh.faces[i].ver_id[j]].point.z);
		}
		glEnd();
	}

}

int n = 0;

void Display::updateMesh()
{
	Mesh newMesh;

	if (currentMesh == (int)myMesh.size())
	{
		if (myMesh[currentMesh - 1].type == 0)
		{
			n++;
			newMesh = getLoopSub(myMesh[currentMesh - 1]);
			cout << endl;
			cout << "第" << n << "次 Loop 细分 "  << endl;
			cout << endl;
		}

		else
		{
			n++;
			newMesh = getCCSub(myMesh[currentMesh - 1]);
			cout << endl;
			cout << "第" << n << "次 Catmull Clark 细分" << endl;
			cout << endl;
		}
		setVertexNormal(newMesh);
		myMesh.push_back(newMesh);

	}
}

void RenderScene(void)
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();
	{

		temp.SetCamera();


		glRotatef(1.0, 1.0, 1.0, 1.0);

		temp.displayMesh();

		glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
		glGetDoublev(GL_PROJECTION_MATRIX, projection);
		glGetIntegerv(GL_VIEWPORT, viewport);

	}
	glPopMatrix();

	glFlush();
}

void SpecialKeys(int key, int x, int y)
{
	if (key == GLUT_KEY_UP) {
		camRotX += kRotationStep;
	}
	if (key == GLUT_KEY_DOWN) {
		camRotX -= kRotationStep;
	}
	if (key == GLUT_KEY_LEFT) {
		camRotY += kRotationStep;
	}
	if (key == GLUT_KEY_RIGHT) {
		camRotY -= kRotationStep;
	}
	glutPostRedisplay();
}

void KeyBoard(unsigned char key, int x, int y)
{
	if (key == '-')
	{
		if (currentMesh >= 1)
			currentMesh--;
	}
	if (key == '+')
		currentMesh++;
	if (key == 'z' || key == 'Z') {
		camPosZ += kTranslationStep;
	}
	if (key == 'x' || key == 'X') {
		camPosZ -= kTranslationStep;
	}
	if (key == '`') 
		exit(0);


	temp.updateMesh();

	glutPostRedisplay();
}


void reshape(int w, int h)
{


	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();


	gluPerspective(45.0f, (GLfloat)w / (GLfloat)h, 1.0f, 100.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

