#pragma once
#include "MeshStructure.h"/*算法步骤
Step  1对初始网格的每个控制顶点按照公式(3)计算新顶点的位置，如
果顶点在边界线上，按照边界顶点的计算公式(5)进行处理;

Step 2对每个原始面按照公式(1)计算新面点;

Step 3对每一条原始边按照公式(2)计算新边点，如果是边界线，则按
照边界上的新边点计算公式(4)进行计算;

Step 4连接新面点到对应多边形的各边新边点，连接新顶点到它的各邻接边
的新边点;

Step 5重新建立连接信息;

Step 6重新计算每个新面的法向矢量;

Step 7重新计算每个新顶点的法向矢量;

Step 8算法结束。
*/
//顶点更新
_3DStructure ccVertexPoint(Mesh mesh, int vertex_id)
{
	_3DStructure result;
	vector<int> neighbor = mesh.edgesOfVertex(vertex_id);
	int n = neighbor.size();
	if (mesh.vertices[vertex_id].isBoundary)
	{
		int boundary = 0;
		for (unsigned int i = 0; i<n; ++i)
		{
			if (mesh.edges[neighbor[i]].pair_id == -1)
				boundary++;
			if (mesh.edges[mesh.previousEdge(neighbor[i])].pair_id == -1)
				boundary++;
		}
		if (boundary == 2)
		{//网格边界V-顶点位置：设边界顶点v0的两个相邻点为v1、v2，则该顶点更新后位置为v = 3 / 4 * v0 + 1 / 8 * (v1 + v2)。
			result = 0.75*mesh.vertices[vertex_id].point;
			for (unsigned int i = 0; i<n; ++i)
			{
				Edge previous = mesh.edges[mesh.previousEdge(neighbor[i])];
				if (mesh.edges[neighbor[i]].pair_id == -1)
					result = result + 0.125*mesh.vertices[mesh.edges[neighbor[i]].vertex_id].point;
				if (previous.pair_id == -1)
				{
					previous = mesh.edges[mesh.previousEdge(previous.id)];
					result = result + 0.125*mesh.vertices[previous.vertex_id].point;

				}

			}

		}
		if (boundary>2)
			result = mesh.vertices[vertex_id].point;

	}
	else
	{//网格内部V-顶点位置：设内部顶点v0的相邻点为v1、v2，…，v2n，则该顶点更新后位置为 ，其中α、β、γ分别为α = 1 - β - γ。
	 //d=（n-2）/n=α，c=1/（n*n）=β=γ
		float d = (n - 2.) / n;
		result = d*mesh.vertices[vertex_id].point;
		float c = 1.0 / (n*n);
		for (unsigned int i = 0; i<n; ++i)
		{
			result = result + c*mesh.vertices[mesh.edges[neighbor[i]].vertex_id].point;
			result = result + c*mesh.faces[mesh.edges[neighbor[i]].face_id].faceCentroid;
		}
	}
	return result;

}


//增加边点
_3DStructure ccEdgePoint(Mesh mesh, int edge_id)
{
	_3DStructure result;
	Edge edge = mesh.edges[edge_id];
	if (edge.pair_id == -1)
	{//网格边界E-顶点位置：设边界边的两个端点为v0、v1，则新增加的顶点位置为v = 1 / 2 * (v0 + v1)。
		result = result + 0.5*mesh.vertices[edge.vertex_id].point;
		Edge previous = mesh.edges[mesh.previousEdge(edge_id)];
		result = result + 0.5*mesh.vertices[previous.vertex_id].point;
	}
	else
	{//网格内部E-顶点位置：设内部边的两个端点为v0、v1，与该边相邻的两个四边形顶点分别为v0、v1、v2、v3和v0、v1、v4、v5，则新增加的顶点位置为v = 1 / 4 * (v0 + v1 + vf1 + vf2) = 3 / 8 * (v0 + v1) + 1 / 16 * (v2 + v3 + v4 + v5)。
		result = result + 0.25*mesh.vertices[edge.vertex_id].point;
		result = result + 0.25*mesh.vertices[mesh.edges[edge.pair_id].vertex_id].point;
		result = result + 0.25*mesh.faces[mesh.edges[edge_id].face_id].faceCentroid;
		result = result + 0.25*mesh.faces[mesh.edges[mesh.edges[edge_id].pair_id].face_id].faceCentroid;
	}
	return result;
}
//执行cc细分
Mesh getCCSub(Mesh mesh)
{
	//第一次获取顶点
	mesh.setFaceCentroid();
	vector<Vertex> newVertices;
	vector<Face> newFaces;
	vector<Edge> newEdges;
	//面点
	for (unsigned int i = 0; i<mesh.faces.size(); ++i)
	{
		Vertex faceCentroid;
		faceCentroid.point = mesh.faces[i].faceCentroid;
		faceCentroid.id = newVertices.size();
		faceCentroid.isBoundary = false;
		newVertices.push_back(faceCentroid);
	}
	//更新顶点
	for (unsigned int i = 0; i<mesh.vertices.size(); ++i)
	{
		Vertex newVertex;
		newVertex.id = newVertices.size();
		newVertex.point = ccVertexPoint(mesh, i);
		if (mesh.vertices[i].isBoundary)
			newVertex.isBoundary = true;
		else
			newVertex.isBoundary = false;
		newVertices.push_back(newVertex);
	}
	//新增边点
	for (unsigned int i = 0; i<mesh.edges.size(); ++i)
	{
		Vertex edgeV;
		if (mesh.edges[i].nextCCVertex == -1)
		{
			edgeV.point = ccEdgePoint(mesh, i);
			edgeV.id = newVertices.size();
			mesh.edges[i].nextCCVertex = edgeV.id;
			if (mesh.edges[i].pair_id != -1)
			{
				mesh.edges[mesh.edges[i].pair_id].nextCCVertex = edgeV.id;
				edgeV.isBoundary = false;

			}
			else
				edgeV.isBoundary = true;
			newVertices.push_back(edgeV);
		}
	}
	//构建新面
	for (unsigned int i = 0; i<mesh.faces.size(); ++i)
	{
		Edge edge = mesh.edges[mesh.faces[i].edge_id];
		for (unsigned int j = 0; j<mesh.faces[i].ver_id.size(); ++j)
		{
			Face newFace(newFaces.size());
			Edge newEdge1, newEdge2, newEdge3, newEdge4;
			newFace.ver_id.push_back(i); //面质心
			newEdge1.vertex_id = i;
			newEdge1.face_id = newFace.id;
			newEdge1.id = newEdges.size();

			newFace.ver_id.push_back(edge.nextCCVertex);  //首边
			newEdge2.vertex_id = edge.nextCCVertex;
			newEdge2.face_id = newFace.id;
			newEdge2.id = newEdges.size() + 1;

			newFace.ver_id.push_back(mesh.faces.size() + edge.vertex_id);
			newEdge3.vertex_id = mesh.faces.size() + edge.vertex_id;
			newEdge3.face_id = newFace.id;
			newEdge3.id = newEdges.size() + 2;

			edge = mesh.edges[edge.next_id];
			newFace.ver_id.push_back(edge.nextCCVertex);  //首边
			newEdge4.vertex_id = edge.nextCCVertex;
			newEdge4.face_id = newFace.id;
			newEdge4.id = newEdges.size() + 3;

			newEdge1.next_id = newEdge2.id;
			newEdge2.next_id = newEdge3.id;
			newEdge3.next_id = newEdge4.id;
			newEdge4.next_id = newEdge1.id;
			newEdges.push_back(newEdge1);
			newEdges.push_back(newEdge2);
			newEdges.push_back(newEdge3);
			newEdges.push_back(newEdge4);
			newVertices[i].edge_id = newEdge2.id;
			newVertices[newEdge2.vertex_id].edge_id = newEdge3.id;
			newVertices[newEdge3.vertex_id].edge_id = newEdge4.id;
			newVertices[newEdge4.vertex_id].edge_id = newEdge1.id;

			newFace.edge_id = newEdge1.id;
			newFace.isNormal = false;
			newFaces.push_back(newFace);
		}
	}
	Mesh newMesh;
	newMesh.type = 1;
	newMesh.vertices = newVertices;
	newMesh.edges = newEdges;
	newMesh.faces = newFaces;
	return newMesh;
}