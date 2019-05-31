#pragma once
#include "MeshStructure.h"
/*
	Step 1�Գ�ʼ�����ÿ�����㰴�չ�ʽ�����¶����λ�ã���������ڱ߽�
���ϣ����ձ߽��ߵļ��ι�����д���;

    Step 2��ÿ����ʼ�߽���ϸ�֣��õ��±ߵ㣬������Ǳ߽��ߣ����ձ߽�
�ߵļ��ι�����д���;

    Step 3�����¶��������ڽӱߵ��±ߵ㣬�����±ߵ��������ڱߵ��±ߵ�;
  
    Step 4���½���������Ϣ;
    
    Step 5���¼���ÿ������ķ���ʸ��;
    
    Step 6���¼���ÿ���¶���ķ���ʸ��;
    
    Step 7�㷨������*/
//����ÿ������¶���
vector<Vertex> getLoopVertices(Mesh mesh)
{
	vector<int> neighbor;
	vector<Vertex> newVertices;
	Vertex newVertex;
	float beta;
	for (unsigned int i = 0; i<mesh.vertices.size(); ++i)
	{
		neighbor.clear();
		_3DStructure newPoint;
		neighbor = mesh.edgesOfVertex(i);//�ߵ�vector
		if (mesh.vertices[i].isBoundary)
		{
			newVertex.isBoundary = true;
			if (neighbor.size() == 1)
			{//2.����߽�V-����λ�ã���߽綥��v0���������ڵ�Ϊv1��v2����ö�����º�λ��Ϊv = 3 / 4 * v0 + 1 / 8 * (v1 + v2)��
				beta = 0.125;
				newPoint = 0.75*mesh.vertices[i].point;
				_3DStructure neighborPoint = mesh.vertices[mesh.edges[neighbor[0]].vertex_id].point;
				newPoint.display();
				newPoint = newPoint + beta*neighborPoint;
				newPoint.display();
				Edge edge = mesh.edges[mesh.edges[neighbor[0]].next_id];
				newPoint = newPoint + beta*mesh.vertices[edge.vertex_id].point;
			}
			else
			{//�����ڲ�V-����λ�ã����ڲ�����v0�����ڵ�Ϊv1��v2������vn����ö�����º�λ��Ϊ�����С�
				int n = neighbor.size() + 1;
				if (n == 3)
					beta = 0.1875;//beta=1/n[5/8-(3/8+1/4*cos(2/3*PI))^2] ��n=3
				else
					beta = 3.0 / (8.0*(float)n); //beat=3/(8*n)
				newPoint = (1 - n*beta)*mesh.vertices[i].point;
				for (unsigned int j = 0; j<n - 1; ++j)
				{
					Edge edge = mesh.edges[neighbor[j]];
					Edge edge1 = mesh.edges[edge.next_id];
					Edge edge2 = mesh.edges[edge1.next_id];
					if (edge2.pair_id == -1)
					{
						newPoint = newPoint + beta*mesh.vertices[edge1.vertex_id].point;
					}
					newPoint = newPoint + beta*mesh.vertices[edge.vertex_id].point;

				}//v=��1-n*beta��v0+beta*��v1+v2+v3+...+vn��

			}
		}
		else
		{
			newVertex.isBoundary = false;
			int n = neighbor.size();
			if (n == 3)
				beta = 0.1875;
			else
				beta = 3.0 / (8.0*(float)n);
			newPoint = (1 - n*beta)*mesh.vertices[i].point;
			for (unsigned int j = 0; j<n; ++j)
				newPoint = newPoint + beta*mesh.vertices[mesh.edges[neighbor[j]].vertex_id].point;
		}
		newVertex.point = newPoint;
		newVertex.id = i;
		newVertex.isNormal = false;
		newVertices.push_back(newVertex);

	}
	return newVertices;
}


//���ӱߵ�
Vertex getLoopEdgeVertex(Mesh mesh, int edge_id)
{
	Vertex newVertex;
	_3DStructure point;
	Edge edge = mesh.edges[edge_id];
	newVertex.isNormal = false;
	if (edge.pair_id == -1)
	{//����߽�E-����λ�ã���߽�ߵ������˵�Ϊv0��v1���������ӵĶ���λ��Ϊv = 1/2*(v0 + v1)
		newVertex.isBoundary = true;
		point = point + 0.5*mesh.vertices[edge.vertex_id].point;
		edge = mesh.edges[edge.next_id];
		edge = mesh.edges[edge.next_id];
		point = point + 0.5*mesh.vertices[edge.vertex_id].point;

	}
	else
	{//�����ڲ�E-����λ�ã����ڲ��ߵ������˵�Ϊv0��v1����Ե���������Ϊv2��v3���������ӵĶ���λ��Ϊv = 3/8*(v0 + v1) + 1/8*(v2 + v3)
		newVertex.isBoundary = false;
		Edge pair = mesh.edges[edge.pair_id];
		Edge edge_next = mesh.edges[edge.next_id];
		Edge pair_next = mesh.edges[pair.next_id];
		point = point + 0.375*mesh.vertices[edge.vertex_id].point;
		point = point + 0.375*mesh.vertices[pair.vertex_id].point;
		point = point + 0.125*mesh.vertices[edge_next.vertex_id].point;
		point = point + 0.125*mesh.vertices[pair_next.vertex_id].point;
	}
	newVertex.point = point;
	return newVertex;
}


//�����һ����loopϸ��
Mesh getLoopSub(Mesh mesh)   
{
	Mesh newMesh;
	vector<Vertex> newVertices = getLoopVertices(mesh);
	vector<Face> newFaces;
	vector<Edge> newEdges;
	int pushTime;
	for (unsigned int i = 0; i<mesh.faces.size(); ++i)
	{
		pushTime = 0;
		//��ȡ������Ե����
		Edge currentEdge1 = mesh.edges[mesh.faces[i].edge_id];
		Vertex newVertex1;
		Vertex newVertex2;
		Vertex newVertex3;
		if (currentEdge1.nextEdgeVertex == -1)
		{
			newVertex1 = getLoopEdgeVertex(mesh, currentEdge1.id);
			newVertex1.id = newVertices.size();
			pushTime++;
			if (currentEdge1.pair_id != -1)
				mesh.edges[currentEdge1.pair_id].nextEdgeVertex = newVertex1.id;
		}
		else
		{
			newVertex1 = newVertices[currentEdge1.nextEdgeVertex];
		}
		Edge currentEdge2 = mesh.edges[currentEdge1.next_id];
		if (currentEdge2.nextEdgeVertex == -1)
		{
			newVertex2 = getLoopEdgeVertex(mesh, currentEdge2.id);
			newVertex2.id = newVertices.size() + pushTime;
			pushTime++;
			if (currentEdge2.pair_id != -1)
				mesh.edges[currentEdge2.pair_id].nextEdgeVertex = newVertex2.id;
		}
		else
		{
			newVertex2 = newVertices[currentEdge2.nextEdgeVertex];
		}
		Edge currentEdge3 = mesh.edges[currentEdge2.next_id];
		if (currentEdge3.nextEdgeVertex == -1)
		{
			newVertex3 = getLoopEdgeVertex(mesh, currentEdge3.id);
			newVertex3.id = newVertices.size() + pushTime;
			if (currentEdge3.pair_id != -1)
				mesh.edges[currentEdge3.pair_id].nextEdgeVertex = newVertex3.id;

		}
		else
		{
			newVertex3 = newVertices[currentEdge3.nextEdgeVertex];
		}
		Vertex newConerVertex1 = newVertices[currentEdge1.vertex_id];
		Vertex newConerVertex2 = newVertices[currentEdge2.vertex_id];
		Vertex newConerVertex3 = newVertices[currentEdge3.vertex_id];

		//�����м��������
		Face newFace;
		newFace.id = newFaces.size();
		newFace.ver_id.push_back(newVertex1.id);
		newFace.ver_id.push_back(newVertex2.id);
		newFace.ver_id.push_back(newVertex3.id);
		//�±�
		Edge newEdge1, newEdge2, newEdge3;
		newEdge1.vertex_id = newVertex1.id;
		newEdge2.vertex_id = newVertex2.id;
		newEdge3.vertex_id = newVertex3.id;
		newEdge1.face_id = newFace.id;
		newEdge2.face_id = newFace.id;
		newEdge3.face_id = newFace.id;
		newEdge1.id = newEdges.size();
		newEdge2.id = newEdges.size() + 1;
		newEdge3.id = newEdges.size() + 2;
		newEdge1.next_id = newEdge2.id;
		newEdge2.next_id = newEdge3.id;
		newEdge3.next_id = newEdge1.id;
		newFace.edge_id = newEdge1.id;
		if (currentEdge1.nextEdgeVertex == -1)
		{
			newVertex1.edge_id = newEdge2.id;
			newVertices.push_back(newVertex1);
		}
		if (currentEdge2.nextEdgeVertex == -1)
		{
			newVertex2.edge_id = newEdge3.id;
			newVertices.push_back(newVertex2);

		}
		if (currentEdge3.nextEdgeVertex == -1)
		{
			newVertex3.edge_id = newEdge1.id;
			newVertices.push_back(newVertex3);

		}
		//���������α�
		//1 face
		Face conerFace1;
		conerFace1.id = newFaces.size() + 1;
		conerFace1.ver_id.push_back(newVertex1.id);
		conerFace1.ver_id.push_back(newConerVertex1.id);
		conerFace1.ver_id.push_back(newVertex2.id);
		//1 edges
		Edge newConer1Edge1, newConer1Edge2, newConer1Edge3;
		newConer1Edge1.vertex_id = newVertex1.id;
		newConer1Edge2.vertex_id = newConerVertex1.id;
		newConer1Edge3.vertex_id = newVertex2.id;
		newConer1Edge1.face_id = conerFace1.id;
		newConer1Edge2.face_id = conerFace1.id;
		newConer1Edge3.face_id = conerFace1.id;
		newConer1Edge1.id = newEdges.size() + 3;
		newConer1Edge2.id = newEdges.size() + 4;
		newConer1Edge3.id = newEdges.size() + 5;
		newConer1Edge1.next_id = newConer1Edge2.id;
		newConer1Edge2.next_id = newConer1Edge3.id;
		newConer1Edge3.next_id = newConer1Edge1.id;
		conerFace1.edge_id = newConer1Edge1.id;
		newVertices[currentEdge1.vertex_id].edge_id = newConer1Edge2.id;

		//2 face
		Face conerFace2;
		conerFace2.id = newFaces.size() + 2;
		conerFace2.ver_id.push_back(newVertex2.id);
		conerFace2.ver_id.push_back(newConerVertex2.id);
		conerFace2.ver_id.push_back(newVertex3.id);
		//2 edge
		Edge newConer2Edge1, newConer2Edge2, newConer2Edge3;
		newConer2Edge1.vertex_id = newVertex2.id;
		newConer2Edge2.vertex_id = newConerVertex2.id;
		newConer2Edge3.vertex_id = newVertex3.id;
		newConer2Edge1.face_id = conerFace2.id;
		newConer2Edge2.face_id = conerFace2.id;
		newConer2Edge3.face_id = conerFace2.id;
		newConer2Edge1.id = newEdges.size() + 6;
		newConer2Edge2.id = newEdges.size() + 7;
		newConer2Edge3.id = newEdges.size() + 8;
		newConer2Edge1.next_id = newConer2Edge2.id;
		newConer2Edge2.next_id = newConer2Edge3.id;
		newConer2Edge3.next_id = newConer2Edge1.id;
		conerFace2.edge_id = newConer2Edge1.id;
		newVertices[currentEdge2.vertex_id].edge_id = newConer2Edge2.id;
		//3 face
		Face conerFace3;
		conerFace3.id = newFaces.size() + 3;
		conerFace3.ver_id.push_back(newVertex3.id);
		conerFace3.ver_id.push_back(newConerVertex3.id);
		conerFace3.ver_id.push_back(newVertex1.id);
		//3 edge
		Edge newConer3Edge1, newConer3Edge2, newConer3Edge3;
		newConer3Edge1.vertex_id = newVertex3.id;
		newConer3Edge2.vertex_id = newConerVertex3.id;
		newConer3Edge3.vertex_id = newVertex1.id;
		newConer3Edge1.face_id = conerFace3.id;
		newConer3Edge2.face_id = conerFace3.id;
		newConer3Edge3.face_id = conerFace3.id;
		newConer3Edge1.id = newEdges.size() + 9;
		newConer3Edge2.id = newEdges.size() + 10;
		newConer3Edge3.id = newEdges.size() + 11;
		newConer3Edge1.next_id = newConer3Edge2.id;
		newConer3Edge2.next_id = newConer3Edge3.id;
		newConer3Edge3.next_id = newConer3Edge1.id;
		conerFace3.edge_id = newConer3Edge1.id;
		newVertices[currentEdge3.vertex_id].edge_id = newConer3Edge2.id;
		//������
		newEdge1.pair_id = newConer3Edge1.id;
		newConer3Edge1.pair_id = newEdge1.id;
		newEdge2.pair_id = newConer1Edge1.id;
		newConer1Edge1.pair_id = newEdge2.id;
		newEdge3.pair_id = newConer2Edge1.id;
		newConer2Edge1.pair_id = newEdge3.id;
		
		//����
		newFaces.push_back(newFace);
		newFaces.push_back(conerFace1);
		newFaces.push_back(conerFace2);
		newFaces.push_back(conerFace3);
		//�±�
		newEdges.push_back(newEdge1);
		newEdges.push_back(newEdge2);
		newEdges.push_back(newEdge3);
		newEdges.push_back(newConer1Edge1);
		newEdges.push_back(newConer1Edge2);
		newEdges.push_back(newConer1Edge3);
		newEdges.push_back(newConer2Edge1);
		newEdges.push_back(newConer2Edge2);
		newEdges.push_back(newConer2Edge3);
		newEdges.push_back(newConer3Edge1);
		newEdges.push_back(newConer3Edge2);
		newEdges.push_back(newConer3Edge3);
	}
	newMesh.vertices = newVertices;
	newMesh.faces = newFaces;
	newMesh.edges = newEdges;
	newMesh.type = 0;
	return newMesh;
}

//��ÿ���������
void Mesh::setFaceCentroid()
{
	for (unsigned int i = 0; i<faces.size(); ++i)
	{
		_3DStructure faceCentroid;
		for (unsigned int j = 0; j<faces[i].ver_id.size(); ++j)
		{
			float n = faces[i].ver_id.size();
			faceCentroid = faceCentroid + (1 / n)*vertices[faces[i].ver_id[j]].point;//����Ķ���֮�͵�ƽ��ֵ��Ϊ����

		}
		faces[i].faceCentroid = faceCentroid;
	}
}