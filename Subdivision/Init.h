#pragma once
#include"MeshStructure.h"
int Init()
{
	Mesh mesh1;
	char key;
	cout << "��������ѡ�õ�ϸ���㷨��ccϸ�֣�c;loopϸ��:l" << endl;
	cin >> key;
	if (key == 'c')
	{
		mesh1.loadFile("cube.obj");
	}
	else if (key == 'l')
	{
		mesh1.loadFile("cube_loop.obj");
	}
	else
	{

		cout << "�������" << endl;
		system("pause");
		return 0;
	}

	myMesh.push_back(mesh1);
	setVertexNormal(myMesh[0]);
	
}