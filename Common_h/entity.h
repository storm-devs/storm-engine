#ifndef _ENTITY_H_
#define _ENTITY_H_

#include "d_types.h"
#include "entity_id.h"
#include "attributes.h"
#include "key_codes.h"
#include "message.h"
#include "entity_state.h"
#include "vsystem_api.h"
#include "exs.h"

#include "..\ENGINE\PROGRAM\NET\net_defines.h"

//-------------------------------------------------------------------------------------------------
// Pop operation (from run stack) for object constructor perform engine code
// Push and Pop operation for object destructor perform engine code
// Return code for function dont have any meaning for core in this version of engine

#define ENGINE_VERSION_MAJOR		0x2
#define ENGINE_VERSION_MINOR		0x0

#define ENGINE_VERSION				(ENGINE_VERSION_MAJOR<<16)|(ENGINE_VERSION_MINOR)
#define MODULE_API_VERSION			0x0001
#define RUN_OK						0x0

// LayeR flags
#define LRFLAG_EXECUTE		0x1
#define LRFLAG_REALIZE		0x2
#define LRFLAG_SYS_MESSAGES	0x4
#define LRFLAG_FROZEN		0x8

class CORE;

extern VSYSTEM_API * _VSYSTEM_API;
class VDX8RENDER;
class VGEOMETRY;
class COLLIDE;

class ENTITY 
{
friend CORE;
private: 
	ENTITY_ID Entity_ID;
	bool bServer, bFirstTestServer;
	VGEOMETRY * __pGeoService;
	VDX8RENDER * __pRenderService;
	COLLIDE * __pCollideService;
	dword dwNetID;

public:
	ATTRIBUTES * AttributesPointer;
	inline ENTITY_ID GetID(){return Entity_ID;}
	ENTITY()
	{
		AttributesPointer = null;
		_VSYSTEM_API->SetEntityPointer(this);
		Entity_ID = _VSYSTEM_API->GetEntityID();
		_VSYSTEM_API->Push(Entity_ID.pointer,Entity_ID.class_code);
	
		__pGeoService = null;
		__pRenderService = null;
		__pCollideService = null;
		bServer = false;
		bFirstTestServer = true;
		dwNetID = DST_INVALID;
	};
	virtual ~ENTITY()
	{
		_VSYSTEM_API->Pop(Entity_ID.pointer);
	};
	virtual bool Init()											{return true;};
	virtual void Execute(dword Delta_Time)						{};
	virtual void Realize(dword Delta_Time)						{};
	virtual void ProcessMessage(dword,dword,dword)				{};
	virtual dword _cdecl ProcessMessage(MESSAGE & message)		{return 0;};
	virtual bool LoadState(ENTITY_STATE * state)				{return true;};
	virtual bool CreateState(ENTITY_STATE_GEN * state_gen)		{return true;};
	virtual dword AttributeChanged(ATTRIBUTES *)				{return 0;};

	virtual VDX8RENDER & Render() { if (!__pRenderService) __pRenderService = (VDX8RENDER*)_VSYSTEM_API->CreateService("dx8render"); return *__pRenderService; };
	virtual VGEOMETRY & Geometry() { if (!__pGeoService) __pGeoService = (VGEOMETRY *)_VSYSTEM_API->CreateService("geometry"); return *__pGeoService; };
	virtual COLLIDE & Collide() { if (!__pCollideService) __pCollideService = (COLLIDE *)_VSYSTEM_API->CreateService("coll"); return *__pCollideService; };

	virtual void SetServer(bool bServer) { this->bServer = bServer; bFirstTestServer = false; };
	virtual bool IsServer() 
	{ 
		if (!bFirstTestServer) return bServer; 
		Assert(AttributesPointer);
		Assert(AttributesPointer->FindAClass(AttributesPointer, "Server"));
		bServer = AttributesPointer->GetAttributeAsDword("Server") != 0;
		bFirstTestServer = false;
		return bServer;
	};
	virtual bool IsClient() { return !IsServer(); };

	virtual dword GetNetID() { return dwNetID; };
	virtual void SetNetID(dword dwNetID) { this->dwNetID = dwNetID; };

	virtual void LostRender() {};
	virtual void RestoreRender() {};
};

#endif