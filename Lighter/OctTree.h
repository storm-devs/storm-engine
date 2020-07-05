//============================================================================================
//	Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//	
//--------------------------------------------------------------------------------------------
//	OctTree
//--------------------------------------------------------------------------------------------
//	
//============================================================================================

#ifndef _OctTree_H_
#define _OctTree_H_

#include "LGeometry.h"

class OctTree  
{

	struct OTNode
	{
		OTNode(CVECTOR & _min, CVECTOR & _max);
		~OTNode();
		OTNode * node[8];		//����
		CVECTOR min;			//����������� ��������
		CVECTOR max;			//������������ ��������
		Vertex ** vrt;			//������� ������
		long num;				//���������� ������
	};

//--------------------------------------------------------------------------------------------
//���������������, ������������������
//--------------------------------------------------------------------------------------------
public:
	OctTree();
	virtual ~OctTree();

	//���������������� ������
	void Init(LGeometry * g);	
	//����� ������� � �������� �������
	void FindVerts(CVECTOR & pos, float r);


	OctFndVerts * verts;
	long numVerts;
	long maxVerts;

//--------------------------------------------------------------------------------------------
//������������
//--------------------------------------------------------------------------------------------
private:
	//���������� ������
	bool AddVertex(OTNode * node, Vertex * v);
	//����������� ������
	void Optimize(OTNode * node);
	//�����
	void FindVerts(OTNode * node);

	long Check(OTNode * node, Vertex * v, long num);

private:
	Vertex * vrt;
	long numVrt;
	OTNode * root;	
	CVECTOR vertsPos, vertsPosMin, vertsPosMax;
	float vertsR;
};

#endif

