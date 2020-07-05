#ifndef _SEAFOAM_PS_H_
#define _SEAFOAM_PS_H_

#include "..\common_h\vmodule_api.h"
#include "..\common_h\dx8render.h"
#include "..\common_h\geometry.h"
#include "..\common_h\vfile_service.h"
#include "..\common_h\vparticle_system.h"
#include "..\common_h\matrix.h"


#define MAX_PS_TEXTURES		8
#define VERTEXS_ON_PARTICLE	6

#define	PSKEY_TEXTURE			"texture"
#define	PSKEY_PNUM				"particles_num"
#define PSKEY_EMISSIONTIME		"emissiontime"
#define PSKEY_EMISSIONTIMERAND	"emissiontime_rand"
#define PSKEY_TECHNIQUE			"technique"

#define PSKEY_SURFACEOFFSET		"surfaceoffset"

#define PSKEY_WINDEFFECT		"windeffect"
#define PSKEY_DDEVIATION		"ddeviation"
#define PSKEY_GRAVITY			"gravity"
#define PSKEY_INISPEED			"speed"
#define PSKEY_SDEVIATION		"speed_deviation"
#define PSKEY_LIFETIME			"lifetime"
#define PSKEY_COLORINVERSE		"inversecolor"
#define PSKEY_SPIN				"spin"
#define PSKEY_SPINDEV			"spin_deviation"
#define PSKEY_EMITTERINITIME	"emitter_initime"
#define PSKEY_UNIFORMEMIT		"uniformemit"
#define PSKEY_WEIGHT			"weight"
#define PSKEY_WEIGHTDEVIATION	"weight_deviation"
#define PSKEY_RANDOMDIRECTION	"randomdirection"
#define PSKEY_NONSTOPEMIT		"nonstopemit"
#define PSKEY_EMITDELTA			"emitdelta"
#define PSKEY_DOUBLE			"double"
#define PSKEY_EMITRADIUS		"emit_radius"
#define PSKEY_TRACKPOINTRADIUS	"trackpoint_radius"

#define PSKEY_ALPHAKEY		"key_alpha"
#define PSKEY_PSIZEKEY		"key_psize"
#define PSKEY_PSPEEDKEY		"key_pspeed"
#define PSKEY_PSPINKEY		"key_spin"
#define PSKEY_PANGLEKEY		"key_angle"
#define PSKEY_WINDEFFECTKEY	"key_windeffect"

struct PARTICLE_VERTEX
{
	CVECTOR pos;
	DWORD color;
	float tu,tv;
};

#define PARTICLE_FVF (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1|D3DFVF_TEXTUREFORMAT2)

struct PARTICLE
{
	CVECTOR pos;
	CVECTOR ang;
	CVECTOR v;
	CVECTOR chaos;
	CVECTOR track_dest;
	
	float size;
	float weight;

	float spin;
	float spinVal;

	float angle;
	float scale;
	
	float speed;
	float speed_chaos;
	float speedVal;

	long  lifetime;
	long  time;
	dword color;
	bool  live;
	bool  done;

	dword flow_track_index;
};

#define TRACK_EVENT_MAX	16

struct TRACK_EVENT
{
	long  time;
	float value;
};

class PARTICLES;

class SEAFOAM_PS : VPARTICLE_SYSTEM
{
	friend PARTICLES;
	TRACK_EVENT Visibility[TRACK_EVENT_MAX];
	TRACK_EVENT ParticleSize[TRACK_EVENT_MAX];
	TRACK_EVENT ParticleSpeed[TRACK_EVENT_MAX];
	TRACK_EVENT ParticleSpin[TRACK_EVENT_MAX];
	TRACK_EVENT ParticleAngle[TRACK_EVENT_MAX];
	TRACK_EVENT WindEffect[TRACK_EVENT_MAX];

	bool bTrackAngle;

	VAPI * api;
	VDX8RENDER * RenderService;
	VGEOMETRY * gs;

	long TextureID[MAX_PS_TEXTURES];
	long TexturesNum;

	long ParticlesNum;
	PARTICLE * Particle;

	IDirect3DVertexBuffer9 * VBuffer;

	CVECTOR Camera_EmitterPos;
	CVECTOR Camera_EmitterPosA;
	CVECTOR Emitter;
	CVECTOR EmitterDirection;
	float   DirectionDeviation;
	long	EmitterIniTime;

	
	CVECTOR vWindDirection;
	float fWindPower;
	float fWindEffect;
	float Gravity;
	float Inispeed;
	float SpeedDeviation;
	long  Lifetime;
	long  Emitdelta;
	float Spin;
	float SpinDeviation;
	float Weight;
	float WeightDeviation;
	float ESpace;
	long  Delay;

//---------------------------------

	bool  enableEmit;
	dword nEmitted;
	float EmissionTime;	// time for emitting one particle
	long  DeltaTimeSLE; // SinceLastEmission
	bool  EmitParticle();
	long  nSystemLifeTime;
	void  AddTrackPoint(CVECTOR pos);
	CVECTOR * pFlowTrack;
	dword nFlowTrackSize;
	bool  bUseFlowTrack;
	void SetFlowTrack(dword index);
	float fTrackPointRadius;
	float EmissionTimeRand;
	float CurrentEmissionTimeRand;
	bool bLayOnSurface;
	void LayOnSurface(dword index);
	ENTITY_ID SurfaceID;
	void UseSurface(ENTITY_ID surface_id);
	float fSurfaceOffset;
	char * TechniqueName;
	dword ParticleColor;

//---------------------------------

	CVECTOR CameraPos;
	CVECTOR CameraAng;
	float Perspective;

	CMatrix Matrix;

	bool bComplete;

	bool bRepeat;
	bool bColorInverse;
	bool bUniformEmit;
	bool bLinkEmitter;
	bool bRandomDirection;
	
	

	SEAFOAM_PS * l_PTR;
	SEAFOAM_PS * r_PTR;

	ENTITY_ID LinkObject;
	CVECTOR LinkPos;
	CVECTOR LinkDir;
	CVECTOR LinkDirPos;


	long EmitIndex;
	long EmitTimeDelta;

public:
	 SEAFOAM_PS();
	~SEAFOAM_PS();
	bool Init(INIFILE * ini, char * psname);
	void UpdateVertexBuffer();
	void Realize(dword DeltaTime);
	void Execute(dword DeltaTime);
	void ProcessParticles(dword DeltaTime);
	bool Complete();
	void Stop() { bComplete = true; };

	void SetParticlesTracks(dword DeltaTime);

	float GetTrackValue(TRACK_EVENT * Track, long Time);
	bool  BuildTrack(INIFILE * ini, TRACK_EVENT * Track, char * psname, char * key_name);
	void  SetEmitter(CVECTOR p, CVECTOR a);
	void  LinkToObject(ENTITY_ID id, CVECTOR _LinkPos);
	void  SetDelay(long _delay);
	void  EnableEmit (bool _enableEmit) { enableEmit = _enableEmit; }

	// nodes --------------------------------------------------------
	SEAFOAM_PS * GetLeftNode();
	SEAFOAM_PS * GetRightNode();
	void SetLeftNode(SEAFOAM_PS * node);
	void SetRightNode(SEAFOAM_PS * node);
	void AttachTo(SEAFOAM_PS * link_after_PTR,SEAFOAM_PS * * Root,SEAFOAM_PS * * Top);
	void Attach(SEAFOAM_PS * * Root,SEAFOAM_PS * * Top);
	void Deattach(SEAFOAM_PS * * Root,SEAFOAM_PS * * Top);
	void ProcessOrder(SEAFOAM_PS * * Root,SEAFOAM_PS * * Top);
	//---------------------------------------------------------------

	void  SetLifeTime(dword time) {nSystemLifeTime = time;}

};

#endif