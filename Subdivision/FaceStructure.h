#pragma once

#include"Edgestructure.h"
#include<vector>
//面结构定义
class Face {
public:
	vector<int> ver_id;
	int id, edge_id;
	Edge faceEdge; 
	_3DStructure faceNormal;
	_3DStructure faceCentroid;//面心
	bool isNormal;

	Face(int d = 0)
	{
		id = d;
	}
};