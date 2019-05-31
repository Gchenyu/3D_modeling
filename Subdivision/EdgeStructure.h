#pragma once//边结构定义
class Edge
{
public:
	int id, pair_id, next_id, vertex_id, face_id, nextEdgeVertex, nextCCVertex;
	Edge(int _id = -1, int vertex = -1, int face = -1, int pair = -1)
	{
		id = _id;
		vertex_id = vertex;
		nextEdgeVertex = -1;
		nextCCVertex = -1;
		face_id = face;
		pair_id = pair;
	}
	~Edge() {}
};