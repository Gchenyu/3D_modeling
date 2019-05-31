#pragma once
#include "MeshStructure.h"/*�㷨����
Step  1�Գ�ʼ�����ÿ�����ƶ��㰴�չ�ʽ(3)�����¶����λ�ã���
�������ڱ߽����ϣ����ձ߽綥��ļ��㹫ʽ(5)���д���;

Step 2��ÿ��ԭʼ�水�չ�ʽ(1)���������;

Step 3��ÿһ��ԭʼ�߰��չ�ʽ(2)�����±ߵ㣬����Ǳ߽��ߣ���
�ձ߽��ϵ��±ߵ���㹫ʽ(4)���м���;

Step 4��������㵽��Ӧ����εĸ����±ߵ㣬�����¶��㵽���ĸ��ڽӱ�
���±ߵ�;

Step 5���½���������Ϣ;

Step 6���¼���ÿ������ķ���ʸ��;

Step 7���¼���ÿ���¶���ķ���ʸ��;

Step 8�㷨������
*/
//�������
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
		{//����߽�V-����λ�ã���߽綥��v0���������ڵ�Ϊv1��v2����ö�����º�λ��Ϊv = 3 / 4 * v0 + 1 / 8 * (v1 + v2)��
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
	{//�����ڲ�V-����λ�ã����ڲ�����v0�����ڵ�Ϊv1��v2������v2n����ö�����º�λ��Ϊ �����Ц����¡��÷ֱ�Ϊ�� = 1 - �� - �á�
	 //d=��n-2��/n=����c=1/��n*n��=��=��
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


//���ӱߵ�
_3DStructure ccEdgePoint(Mesh mesh, int edge_id)
{
	_3DStructure result;
	Edge edge = mesh.edges[edge_id];
	if (edge.pair_id == -1)
	{//����߽�E-����λ�ã���߽�ߵ������˵�Ϊv0��v1���������ӵĶ���λ��Ϊv = 1 / 2 * (v0 + v1)��
		result = result + 0.5*mesh.vertices[edge.vertex_id].point;
		Edge previous = mesh.edges[mesh.previousEdge(edge_id)];
		result = result + 0.5*mesh.vertices[previous.vertex_id].point;
	}
	else
	{//�����ڲ�E-����λ�ã����ڲ��ߵ������˵�Ϊv0��v1����ñ����ڵ������ı��ζ���ֱ�Ϊv0��v1��v2��v3��v0��v1��v4��v5���������ӵĶ���λ��Ϊv = 1 / 4 * (v0 + v1 + vf1 + vf2) = 3 / 8 * (v0 + v1) + 1 / 16 * (v2 + v3 + v4 + v5)��
		result = result + 0.25*mesh.vertices[edge.vertex_id].point;
		result = result + 0.25*mesh.vertices[mesh.edges[edge.pair_id].vertex_id].point;
		result = result + 0.25*mesh.faces[mesh.edges[edge_id].face_id].faceCentroid;
		result = result + 0.25*mesh.faces[mesh.edges[mesh.edges[edge_id].pair_id].face_id].faceCentroid;
	}
	return result;
}
//ִ��ccϸ��
Mesh getCCSub(Mesh mesh)
{
	//��һ�λ�ȡ����
	mesh.setFaceCentroid();
	vector<Vertex> newVertices;
	vector<Face> newFaces;
	vector<Edge> newEdges;
	//���
	for (unsigned int i = 0; i<mesh.faces.size(); ++i)
	{
		Vertex faceCentroid;
		faceCentroid.point = mesh.faces[i].faceCentroid;
		faceCentroid.id = newVertices.size();
		faceCentroid.isBoundary = false;
		newVertices.push_back(faceCentroid);
	}
	//���¶���
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
	//�����ߵ�
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
	//��������
	for (unsigned int i = 0; i<mesh.faces.size(); ++i)
	{
		Edge edge = mesh.edges[mesh.faces[i].edge_id];
		for (unsigned int j = 0; j<mesh.faces[i].ver_id.size(); ++j)
		{
			Face newFace(newFaces.size());
			Edge newEdge1, newEdge2, newEdge3, newEdge4;
			newFace.ver_id.push_back(i); //������
			newEdge1.vertex_id = i;
			newEdge1.face_id = newFace.id;
			newEdge1.id = newEdges.size();

			newFace.ver_id.push_back(edge.nextCCVertex);  //�ױ�
			newEdge2.vertex_id = edge.nextCCVertex;
			newEdge2.face_id = newFace.id;
			newEdge2.id = newEdges.size() + 1;

			newFace.ver_id.push_back(mesh.faces.size() + edge.vertex_id);
			newEdge3.vertex_id = mesh.faces.size() + edge.vertex_id;
			newEdge3.face_id = newFace.id;
			newEdge3.id = newEdges.size() + 2;

			edge = mesh.edges[edge.next_id];
			newFace.ver_id.push_back(edge.nextCCVertex);  //�ױ�
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