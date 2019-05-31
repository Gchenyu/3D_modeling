#pragma once


#include"VertexStructure.h"
#include"Edgestructure.h"
#include"FaceStructure.h"
#define PI 3.14159265
using namespace std;

//网格结构定义

class Mesh
{
public:
	vector<Face> faces;
	vector<Vertex> vertices;
	vector<Edge> edges;
	int type;  // 0 代表 Loop 细分, 1 代表 Catmul-Clark 细分
	Mesh() {}
	void loadFile(char *fname);
	void displayMesh();
	void setFaceNormal();
	int previousEdge(int edge);
	vector<int> edgesOfVertex(int vertex);
	void setFaceCentroid();
};




/*********************求顶点以及面的法向量***************************///向量归一化
_3DStructure normalize(_3DStructure& v) {
	float d = v.Length();
	if (d == 0) {
		d = 0.1f;
	}
	v.x /= d;
	v.y /= d;
	v.z /= d;
	return v;
}

//两个向量角度计算
float degree(_3DStructure  v1, _3DStructure  v2)
{
	float cos = (v1^v2) / (v1.Length()*v2.Length());
	float result = acos(cos) * 180.0 / PI;
	return result;

}int Mesh::previousEdge(int edge)//求当前边的前一条边，由于是封闭的，不断找下一条边，直到转一圈
{
	Edge next;
	next = edges[edges[edge].next_id];
	do {
		next = edges[next.next_id];

	} while (next.next_id != edge);
	return next.id;
}

void setPairEdge(Mesh & mesh)//设置对边
{
	for (unsigned int i = 0; i<mesh.edges.size(); ++i)
	{
		if (mesh.edges[i].pair_id == -1)
		{
			for (unsigned int j = 0; j<mesh.edges.size(); ++j)
			{
				if (mesh.edges[i].vertex_id == mesh.edges[mesh.previousEdge(j)].vertex_id && mesh.edges[j].vertex_id == mesh.edges[mesh.previousEdge(i)].vertex_id)
				{
					mesh.edges[i].pair_id = j;
					mesh.edges[j].pair_id = i;
					break;
				}
			}
			//判断是否是边界点 没有对边的边对应的点是边界点
			if (mesh.edges[i].pair_id == -1)
			{
				mesh.vertices[mesh.edges[i].vertex_id].isBoundary = true;
				mesh.vertices[mesh.edges[mesh.previousEdge(i)].vertex_id].isBoundary = true;
			}
		}
	}
}vector<int> Mesh::edgesOfVertex(int vertex)
{
	Edge edge;
	vector<int> temp;
	edge = edges[vertices[vertex].edge_id];
	if (vertices[vertex].isBoundary)
	{
		for (unsigned int i = 0; i<edges.size(); ++i)
		{
			if (edges[i].vertex_id == vertex)
				temp.push_back(edges[i].next_id);
		}
		return temp;
	}
	do
	{
		edge = edges[edges[edge.pair_id].next_id];
		temp.push_back(edge.id);

	} while (edge.id != edges[vertices[vertex].edge_id].id);
	return temp;
}//求每个面的法向量
void Mesh::setFaceNormal()
{
	_3DStructure edge1, edge2, normal;
	for (unsigned int i = 0; i<faces.size(); ++i)
	{
		edge1 = vertices[faces[i].ver_id[1]].point - vertices[faces[i].ver_id[0]].point;
		edge2 = vertices[faces[i].ver_id[2]].point - vertices[faces[i].ver_id[0]].point;
		normal = edge1*edge2;//叉乘
		faces[i].faceNormal = normalize(normal);
		faces[i].isNormal = true;
	}
}//点的法向量是以该点为顶点的所有三角形面的法向量之和
void setVertexNormal(Mesh & mesh)
{
	setPairEdge(mesh);
	mesh.setFaceNormal();//求面的法向量
	vector<int> neighbor;
	for (unsigned int i = 0; i<mesh.vertices.size(); ++i)
	{
		//get neighbor face
		neighbor.clear();
		neighbor = mesh.edgesOfVertex(i);
		_3DStructure temp; //初始化为（0.0，0.0，0.0）
		for (unsigned int j = 0; j<neighbor.size(); ++j)
		{
			int k = (j + 1) % neighbor.size();
			if (mesh.vertices[i].isBoundary)
			{
				temp = temp + mesh.faces[mesh.edges[neighbor[j]].face_id].faceNormal;//边界点的法向量
			}
			else
			{
				_3DStructure edge1 = mesh.vertices[mesh.edges[neighbor[j]].vertex_id].point - mesh.vertices[mesh.edges[mesh.edges[neighbor[j]].pair_id].vertex_id].point;
				_3DStructure edge2 = mesh.vertices[mesh.edges[neighbor[k]].vertex_id].point - mesh.vertices[mesh.edges[mesh.edges[neighbor[k]].pair_id].vertex_id].point;
				float weight = degree(edge1, edge2) / 360;
				temp = temp + weight*mesh.faces[mesh.edges[neighbor[j]].face_id].faceNormal;
			}
		}
		mesh.vertices[i].normal = normalize(temp);//单位法向量
		mesh.vertices[i].isNormal = true;
	}

}

//字符串分割
void StringSplit(string str, string separator, vector<string>* results) {
	int found;
	found = str.find_first_of(separator);
	while (found != string::npos) {
		if (found > 0) {
			results->push_back(str.substr(0, found));
		}
		str = str.substr(found + 1);
		found = str.find_first_of(separator);
	}
	if (str.length() > 0) {
		results->push_back(str);
	}
}


void Mesh::loadFile(char* fileName)
{
	ifstream input(fileName);
	string line;
	vector<string> temp;
	if (input.is_open())
	{
		if (fileName == "cube.obj")
			type = 1;
		if (fileName == "cube_loop.obj")
			type = 0;

		while (getline(input, line))
		{
			temp.clear();
			StringSplit(line, " ", &temp);
			if (temp[0] == "#")
				continue;
			if (temp[0] == "v")
			{
				if (temp.size() == 4)
				{
					Vertex tempVertex(atof(temp[1].c_str()), atof(temp[2].c_str()), atof(temp[3].c_str()), vertices.size());
					vertices.push_back(tempVertex);
				}
				else
				{
					cout << "文件中的顶点坐标存储格式错误！" << endl;
				}

			}
			if (temp[0] == "f")
			{
				if (temp.size() >= 4)
				{
					vector<int> tempVertices;
					tempVertices.clear();
					Face tempFace(faces.size());
					for (unsigned int i = 1; i<temp.size(); ++i)
					{
						tempVertices.push_back(atoi(temp[i].c_str()) - 1);
						Edge tempEdge(edges.size(), atoi(temp[i].c_str()) - 1, faces.size());
						if (i == temp.size() - 1)
						{
							tempEdge.next_id = edges.size() - temp.size() + 2;
							vertices[atoi(temp[i].c_str()) - 1].edge_id = edges.size() - temp.size() + 2;
							tempFace.edge_id = edges.size();
						}
						else
						{
							vertices[atoi(temp[i].c_str()) - 1].edge_id = edges.size() + 1;
							tempEdge.next_id = edges.size() + 1;
						}
						edges.push_back(tempEdge);
					}
					tempFace.ver_id = tempVertices;
					faces.push_back(tempFace);

				}
				else
				{
					cout << "文件中的面的存储格式错误！" << endl;
				}

			}

		}
		cout << "成功打开文件， " << vertices.size() << " 个顶点， " << faces.size() << " 个面， " << edges.size() << " 条边。" << endl;
	}
	else
	{
		cout << "文件打开失败！" << endl;
	}
}

