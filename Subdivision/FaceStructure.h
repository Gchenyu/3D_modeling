#pragma once

#include"Edgestructure.h"
#include<vector>
//��ṹ����
class Face {
public:
	vector<int> ver_id;
	int id, edge_id;
	Edge faceEdge; 
	_3DStructure faceNormal;
	_3DStructure faceCentroid;//����
	bool isNormal;

	Face(int d = 0)
	{
		id = d;
	}
};