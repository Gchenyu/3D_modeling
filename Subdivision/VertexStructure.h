#pragma once
#include"_3DStructure.h"//顶点结构定义
class Vertex
{
public:
	_3DStructure point;
	_3DStructure normal;

	int id, edge_id; 

	bool isNormal, isBoundary;
	Vertex(float x = 0.0, float y = 0.0, float z = 0.0, int _id = 0, int edge = -1)
	{
		id = _id;
		edge_id = edge;
		_3DStructure _x(x, y, z);
		point = _x;
		isNormal = false;
		isBoundary = false;
	}

};