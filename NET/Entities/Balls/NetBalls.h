#ifndef NET_BALLS_HPP
#define NET_BALLS_HPP

#include "..\..\common.h"
#include "..\ship\ShipLights.h"

class NetBalls : public ENTITY
{
private:
	struct BALL_PARAMS
	{
		CVECTOR				vFirstPos, vPos;	// first and current ball position
		VPARTICLE_SYSTEM	* pParticle;
		word				wBallOwner;			// ball owner(character index)
		word				wBallIndex;			// ball index
		float				fTime;				// ball time: in seconds
		float				fSpeedV0;			// initial speed: in m/s
		float				fDirX, fDirZ;		// X/Z identity moving(cos(dir),sin(dir))

		float				fSinAngle;			// initial angle: sin(a)
		float				fCosAngle;			// initial angle: cos(a)
	};

	// ============================================================================
	// One ball type, contain common parameters
	// ============================================================================
	struct BALL_TYPE
	{
		string				sName;				// ball name
		string				sParticleName;		// particle name
		dword				dwSubTexIndex;		// 
		dword				dwGoodIndex;		//
		float				fSize;				// ball size(sprite size in meters)
		array<BALL_PARAMS>	Balls;				// container with current balls 

		// constructor for initialization
		BALL_TYPE() : Balls(_FL_, 64) {};
	};

	CANNON_TRACE_BASE	* pSail, * pSea, * pFort, * pIsland, * pNetFort;
	NetShipLights		* pShipsLights;

	word				wBallIndex;
	float				fDeltaTimeMultiplyer;
	float				fBallFlySoundDistance;
	float				fBallFlySoundStereoMultiplyer;

	string				sTextureName;				// texture name
	long				iTextureIndex;				// texture index
	dword				dwSubTexX, dwSubTexY;		// all balls must be in one texture
	VIDWALKER			* pVWForts, * pVWShips;		// 

	array<BALL_TYPE>	aBallTypes;					// Balls types container
	array<RS_RECT>		aBallRects;					// Balls container for render

	void AddBall(ATTRIBUTES * pABall);
	void DelBall(dword dwBallIndex);

public:
	NetBalls();
	~NetBalls();

	bool Init();

	void Execute(dword Delta_Time);
	void Realize(dword Delta_Time);

	dword AttributeChanged(ATTRIBUTES * pA);

};

#endif