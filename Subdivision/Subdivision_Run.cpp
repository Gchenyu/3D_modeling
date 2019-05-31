#include"Display.h"
#include"Init.h"
int main(int argc, char **argv)
{
	Display display1;

	Init();
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(600, 400);
	glutCreateWindow("œ∏∑÷À„∑®");
	display1.Init();
	glutReshapeFunc(reshape);
	glutDisplayFunc(RenderScene);
	glutKeyboardFunc(KeyBoard);
	glutSpecialFunc(SpecialKeys);
	glutMainLoop();

	return 0;
}