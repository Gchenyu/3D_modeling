#pragma once
#include"MeshStructure.h"
int Init()
{
	Mesh mesh1;
	char key;
	cout << "请输入想选用的细分算法，cc细分：c;loop细分:l" << endl;
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

		cout << "输入错误" << endl;
		system("pause");
		return 0;
	}

	myMesh.push_back(mesh1);
	setVertexNormal(myMesh[0]);
	
}