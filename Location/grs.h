//============================================================================================
//	Spirenkov Maxim, 2002
//--------------------------------------------------------------------------------------------
//	
//--------------------------------------------------------------------------------------------
//	grs
//--------------------------------------------------------------------------------------------
//	������ ����� grs
//============================================================================================


#ifndef _GRS_H_
#define _GRS_H_

//============================================================================================

#pragma pack(push, 1)

//============================================================================================
//
//		GRSHeader
//		GRSMiniMapElement[miniZ][miniX]
//		GRSMapElement[numElements]
//
//============================================================================================


#define GRASS_ID	'ssrG'
#define GRASS_VER	'00.1'


#define GRASS_BLK_DST	1.0f							//������ �����
#define GRASS_CNT_MIN	16								//����������� ���������� �������� � �����
#define GRASS_CNT_DLT	8								//������� � ���������� ��������
#define GRASS_STEP		(GRASS_BLK_DST/GRASS_BLK_SIZE)	//��� ����� �����
#define GRASS_BLK_POW	5								//������� ������� ����� 2^N
#define GRASS_BLK_SIZE	(1 << GRASS_BLK_POW)			//������ ����� ����� M = 2^N
#define GRASS_BLK_MASK	(GRASS_BLK_SIZE - 1)			//����� ������� �����


struct GRSHeader
{
	unsigned long id;		//�������������
	unsigned long ver;		//������
	long miniX, miniZ;		//������� ���������
	float startX, startZ;	//������� �����
	long numElements;		//���������� ��������� (��������)
};

struct GRSMiniMapElement
{
	long start;			//������ �����
	long num[4];		//���������� ����� � �����
	float minHeight;	//����������� ������ � �����
	float maxHeight;	//������������ ������ � �����
};

struct GRSMapElement
{
	unsigned char x, z;			//������� �������� � �����
	unsigned char w : 4, h : 4;	//������ � ������
	unsigned char frame : 4;	//����
	unsigned char ang : 4;		//��� ��������
	float y;					//������ � ������� �����������
};


//============================================================================================

#pragma pack(pop)

//============================================================================================

#endif
