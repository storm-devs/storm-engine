#ifndef SEA_BALLS_AI_HPP
#define SEA_BALLS_AI_HPP

#include "..\common_h\vmodule_api.h"
#include "..\common_h\character.h"
#include "..\common_h\sd2_h\CannonTrace.h"
#include "..\common_h\messages.h"
#include "..\common_h\vparticle_system.h"
#include "AIHelper.h"

struct BALL_PARAMS
{
	CVECTOR				vFirstPos, vPos;	// first and current ball position
	VPARTICLE_SYSTEM	* pParticle;
	string				sBallEvent;
	long				iBallOwner;			// ball owner(character index)
	float				fTime;				// ball time: in seconds
	float				fSpeedV0;			// initial speed: in m/s
	float				fDirX, fDirZ;		// X/Z identity moving(cos(dir),sin(dir))

	float				fSinAngle;			// initial angle: sin(a)
	float				fCosAngle;			// initial angle: cos(a)

	float				fHeightMultiply;	// Height position multiply
	float				fSizeMultiply;		// Size of ball multiply
	float				fTimeSpeedMultiply;	// Time speed multiply
	float				fMaxFireDistance;
	dword				dwCannonType;		// Additional parameter
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
	float				fWeight;			// ball weight
	array<BALL_PARAMS>	Balls;				// container with current balls 

	// constructor for initialization
	BALL_TYPE() : Balls(_FL_, 64) {};
};

// ============================================================================
// Master class AIBalls
// Contains functions of balls : drawing, physics and collision
// ============================================================================
class AIBalls : public ENTITY
{
private:
	CANNON_TRACE_BASE	* pSail, * pSea, * pFort, * pIsland;

	float				fDeltaTimeMultiplyer;
	float				fBallFlySoundDistance;
	float				fBallFlySoundStereoMultiplyer;

	string				sTextureName;				// texture name
	dword				dwTextureIndex;				// texture index
	dword				dwSubTexX, dwSubTexY;		// all balls must be in one texture
	VIDWALKER			* pVWForts, * pVWShips;		// 
	dword				dwFireBallFromCameraTime;

	array<BALL_TYPE>	aBallTypes;					// Balls types container
	array<RS_RECT>		aBallRects;					// Balls container for render

	void	AddBall(ATTRIBUTES * pABall);

	// inherited functions from ENTITY
	bool	Init();
	void	SetDevice();

	void	Realize(dword Delta_Time);
	void	Execute(dword Delta_Time);
	
	void	FireBallFromCamera();

	dword	AttributeChanged(ATTRIBUTES * pAttributeChanged);

	dword _cdecl	ProcessMessage(MESSAGE & message);

public:
	static AIBalls * pAIBalls;

	AIBalls();
	~AIBalls();

	void Save(CSaveLoad * pSL);
	void Load(CSaveLoad * pSL);
};

#endif