#ifndef _ISLAND_H_
#define _ISLAND_H_

#include "..\..\common.h"
#include "..\..\..\sea_ai\AIFlowGraph.h"

class MapZipper
{
private: 
	dword	dwSizeX;
	dword	dwDX;
	dword	dwBlockSize, dwBlockShift;
	dword	dwShiftNumBlocksX;
	dword	dwNumRealBlocks;

	word	* pWordTable;
	byte	* pRealData;
public:
	MapZipper();
	~MapZipper(); 

	dword	GetSizeX() { return dwSizeX; };

	void	UnInit();
	void	DoZip(byte * pSrc, dword dwSizeX);
	byte	Get(dword dwX, dword dwY);

	bool	Save(string sFileName);
	bool	Load(string sFileName);

	bool	isLoaded() { return pRealData != null; }
};

class NetIsland : public ISLAND_BASE
{
private:
	string				sIslandName;
	array<ENTITY_ID>	aSpheres;
	array<ENTITY_ID>	aForts;
	AIFlowGraph			AIPath;

	FRECT				rIsland;
	bool				bForeignModels;
	bool				bDrawReflections;
	float				fStepDX, fStepDZ, fStep1divDX, fStep1divDZ;
	float				fShadowMapSize, fShadowMapStep;
	CVECTOR				vBoxSize, vBoxCenter, vRealBoxSize;
	dword				iDMapSize, iDMapSizeShift;
	ENTITY_ID			model_id, seabed_id;

	bool				bFirstRealize;

	float				fCurrentImmersion;

	char				cModelsDir[256], cModelsID[256], cFoamDir[256];
	
	float				fDepthHeight[256];

	MapZipper			mzShadow, mzDepth;

	byte				* pDepthMap;
	byte				* pShadowMap;

	VDX8RENDER			* pRS;
	VGEOMETRY			* pGS;
	COLLIDE				* pCollide;
	VIDWALKER			* pIslandTraceWalker;

	CMatrix				mIslandOld, mSeaBedOld;
	float				fImmersionDepth, fImmersionDistance;

	void	Blur8(byte * * pBuffer, dword dwSize);
	bool	SaveTga8(char * fname, byte * pBuffer, dword dwSizeX, dword dwSizeY);

	// shadow map section

	// depth map section
	bool	CreateHeightMap(char * pDir, char * pName);
	bool	ActivateCamomileTrace(CVECTOR & vSrc);
	inline float	GetDepthCheck(DWORD iX, DWORD iZ);
	inline float	GetDepthNoCheck(DWORD iX, DWORD iZ);

	bool	Mount(char *fname, char * fdir, ENTITY_ID *eID);
	void	Uninit();

	void	CalcBoxParameters(CVECTOR & vBoxCenter, CVECTOR & vBoxSize);

	void	CreateDirectories(char * pDir);

	void	SetName(char * pIslandName) { sIslandName = pIslandName; };
	char	* GetName() { return (char*)sIslandName.GetBuffer(); };

	void	AddLocationModel(ENTITY_ID & eid, char * pIDStr, char * pStr);

	// debug
	void	DoZapSuperGenerator();
	void	DoZapSuperGeneratorInnerDecodeFiles(char * sub_dir, char * mask);
	bool	DoZapSuperGeneratorDecodeFile(char * sname);

public:
	 NetIsland();
	~NetIsland();
	bool			Init();
	void			Realize(dword Delta_Time);
	dword _cdecl	ProcessMessage(MESSAGE & message);

	void			Move();
	void			SetDevice();

// inherit functions COLLISION_OBJECT
	float		Trace(const CVECTOR &src, const CVECTOR &dst);
	bool		Clip(const PLANE *planes, long nplanes, const CVECTOR &center, float radius, ADD_POLYGON_FUNC addpoly) { return false; };

	const char	*GetCollideMaterialName() { return 0; };
	bool		GetCollideTriangle(TRIANGLE &triangle) { return false; };

// inherit functions CANNON_TRACE_BASE
    float		Cannon_Trace(long iBallOwner, const CVECTOR &src, const CVECTOR &dst);

// inherit functions ISLAND_BASE
	bool		GetMovePoint(CVECTOR & vSrc, CVECTOR & vDst, CVECTOR & vRes);

	ENTITY_ID	GetModelEID() { return model_id; };
	ENTITY_ID	GetSeabedEID() { return seabed_id; };

	bool		Check2DBoxDepth(CVECTOR vPos, CVECTOR vSize, float fAngY, float fMinDepth);
	bool		GetDepth(float x, float z, float * fRes = 0);
	bool		GetDepthFast(float x, float z, float * fRes = 0);
	bool		GetDepth(FRECT * pRect, float * fMinH, float * fMaxH);

	bool		GetShadow(float x, float z, float * fRes = 0) { return false; };

	float		GetCurrentImmersion() { return fCurrentImmersion; };
};

#endif