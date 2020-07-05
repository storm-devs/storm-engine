#include "bsps.h"
#include "..\common_h\object.h"

BSPS_PS::BSPS_PS()
{
	api = (VAPI *)_CORE_API;
	
	TechniqueName = 0;

	ParticleColor = 0xffffffff;

	bTrackAngle = 0;

	l_PTR = 0; 
	r_PTR = 0;
	
	bLinkEmitter = false;

	RenderService = 0;
	ParticlesNum = 0;
	TexturesNum = 0;
	Particle = 0;
	VBuffer = 0;
	
	Emitter.x = Emitter.y = Emitter.z = 0;
	Camera_EmitterPos.x = Camera_EmitterPos.y = Camera_EmitterPos.z = 0;
	EmitterDirection.x = EmitterDirection.y = EmitterDirection.z = 0;
	DirectionDeviation = 0.0f;

	bColorInverse = false;
	bUniformEmit = false;	
	bRandomDirection = false;
	bRepeat = false;
	bComplete = false;
	
	ESpace = 0;

	EmitIndex = 0;
	EmitTimeDelta = 0;
	Delay = 0;

	DeltaTimeSLE = 0;
	nEmitted = 0;
	nSystemLifeTime = 0;

	vWindDirection.x = 0;
	vWindDirection.y = 0;
	vWindDirection.z = 0.0;
	fWindPower = 0;
	fWindEffect = 0;

	pFlowTrack = 0;
	nFlowTrackSize = 0;
	bUseFlowTrack = false;
	bLayOnSurface = false;
}

void BSPS_PS::Reset()
{
	float ChaosVal = 0.0001f;

	memset(Particle,0,ParticlesNum*sizeof(PARTICLE));
	
	for(int n=0;n<ParticlesNum;n++)
	{
		Particle[n].pos.x = Emitter.x + ESpace*(0.5f - (float)rand()/RAND_MAX);
		Particle[n].pos.y = Emitter.y + ESpace*(0.5f - (float)rand()/RAND_MAX);
		Particle[n].pos.z = Emitter.z + ESpace*(0.5f - (float)rand()/RAND_MAX);
		//Particle[n].size = 1.0f;
		Particle[n].size = 0.0f;
		Particle[n].color = 0xffffffff;
		Particle[n].color = ParticleColor;

		Particle[n].weight = Weight + WeightDeviation*(0.5f - (float)rand()/RAND_MAX);
		
		Particle[n].speedVal = Inispeed + SpeedDeviation*(0.5f - (float)rand()/RAND_MAX);
		Particle[n].speed = 0;//Particle[n].speedVal;

		if(bRandomDirection)
		{
			Particle[n].ang.x = (0.5f - (float)rand()/RAND_MAX);
			Particle[n].ang.y = (0.5f - (float)rand()/RAND_MAX);
			Particle[n].ang.z = (0.5f - (float)rand()/RAND_MAX);
		}
		else
		{
			Particle[n].ang.x = EmitterDirection.x + DirectionDeviation*(0.5f - (float)rand()/RAND_MAX);
			Particle[n].ang.y = EmitterDirection.y + DirectionDeviation*(0.5f - (float)rand()/RAND_MAX);
			Particle[n].ang.z = EmitterDirection.z + DirectionDeviation*(0.5f - (float)rand()/RAND_MAX);
		}
		Particle[n].ang = !Particle[n].ang;


		Particle[n].chaos.x = ChaosVal*(0.5f - (float)rand()/RAND_MAX);
		Particle[n].chaos.y = ChaosVal*(0.5f - (float)rand()/RAND_MAX);
		Particle[n].chaos.z = ChaosVal*(0.5f - (float)rand()/RAND_MAX);
		Particle[n].speed_chaos = 1.0f - 0.1f*((float)rand()/RAND_MAX);
		

		Particle[n].v = Particle[n].ang * Particle[n].speed;
		Particle[n].lifetime = Lifetime;
		
		Particle[n].time = 0;
		Particle[n].live = false;
		Particle[n].done = false;
		//if(bUniformEmit) Particle[n].time = -n*(Lifetime/ParticlesNum);
		//else Particle[n].time = -EmitterIniTime * rand()/RAND_MAX;

		Particle[n].spinVal = Spin + SpinDeviation * (0.5f - (float)rand()/RAND_MAX);
		Particle[n].spin = Particle[n].spinVal;
	}

	//EmitIndex = 0;
	//EmitTimeDelta = 0;
	//Delay = 0;

	//DeltaTimeSLE = 0;
	//nEmitted = 0;
	//nSystemLifeTime = 0;
}

BSPS_PS::~BSPS_PS()
{
	long n;
	if(VBuffer) VBuffer->Release();
	if(RenderService) 
	{
		for(n=0;n<TexturesNum;n++) RenderService->TextureRelease(TextureID[n]);
		api->FreeService("dx8render");
	}
	if(Particle) delete Particle; Particle = 0;
	if(pFlowTrack) delete pFlowTrack; pFlowTrack = 0;
	if(TechniqueName) delete TechniqueName; TechniqueName = 0;

}

// node ----------------------------------------------------------------------------
BSPS_PS * BSPS_PS::GetLeftNode() { return l_PTR; }
BSPS_PS * BSPS_PS::GetRightNode() {	return r_PTR; }
void BSPS_PS::SetLeftNode(BSPS_PS * node) {	l_PTR = node; }
void BSPS_PS::SetRightNode(BSPS_PS * node) { r_PTR = node; }
void BSPS_PS::Attach(BSPS_PS * * Root,BSPS_PS * * Top)
{
	if(*Root == 0) 
	{
		*Root = this;
		*Top = this;
		return;
	}
	(*Top)->SetRightNode(this);
	SetLeftNode(*Top);
	*Top = this;
}
void BSPS_PS::AttachTo(BSPS_PS * la_PTR,BSPS_PS * * Root,BSPS_PS * * Top)
{
	BSPS_PS * t_PTR;
	t_PTR = la_PTR->GetRightNode();
	la_PTR->SetRightNode(this);
	SetRightNode(t_PTR);
	SetLeftNode(la_PTR);
	if(t_PTR)
	{
		t_PTR->SetLeftNode(this);
	} else *Top = this;
}

void BSPS_PS::Deattach(BSPS_PS * * Root,BSPS_PS * * Top)
{
	if(l_PTR) l_PTR->SetRightNode(r_PTR);
	if(r_PTR) r_PTR->SetLeftNode(l_PTR);
	if(*Root == this) *Root = r_PTR;
	if(*Top == this) *Top = l_PTR;
}

void BSPS_PS::ProcessOrder(BSPS_PS * * Root,BSPS_PS * * Top)
{
	if(r_PTR) 
	{
		//if(Camera_EmitterPos.z > r_PTR->Camera_EmitterPos.z)
		if(Camera_EmitterPosA.z < r_PTR->Camera_EmitterPosA.z)
		{
			Deattach(Root,Top);
			AttachTo(r_PTR,Root,Top);
		}
	}
}

//----------------------------------------------------------------------------------

bool BSPS_PS::Init(INIFILE * ini, char * psname)
{
	GUARD(BSPS_PS::Init)
	if(!ini) return false;
	long n;
	bool bRes;

	// load render service -----------------------------------------------------
	RenderService = (VDX8RENDER *)api->CreateService("dx8render");
	if(!RenderService)	_THROW("No service: dx8render");
	
	gs = (VGEOMETRY *)_CORE_API->CreateService("geometry");
	//if(!gs) return false;

	// read textures ------------------------------------------------------------
	char string[MAX_PATH];

	TexturesNum = 0;
	for(n=0;n<MAX_PS_TEXTURES;n++)
	{
		if(n == 0)
			bRes = ini->ReadString(psname,PSKEY_TEXTURE,string,sizeof(string),"");
		else
			bRes = ini->ReadStringNext(psname,PSKEY_TEXTURE,string,sizeof(string));

		if(bRes)
		{
			TextureID[n] = RenderService->TextureCreate(string);
			if(TextureID[n] >= 0) TexturesNum++;
		} else break;
	}
	
	if(!ini->ReadString(psname,PSKEY_TECHNIQUE,string,sizeof(string),""))
	{
		_CORE_API->Trace("Particle system: %s",psname);
		_THROW(no technique for particle system);
	}
	TechniqueName = NEW char[strlen(string) + 1];
	strcpy(TechniqueName,string);

	// configure particles
	ParticlesNum = ini->GetLong(psname,PSKEY_PNUM,32);
	EmissionTime = ini->GetFloat(psname,PSKEY_EMISSIONTIME,0);
	DeltaTimeSLE = (long) EmissionTime;
	EmissionTimeRand = ini->GetFloat(psname,PSKEY_EMISSIONTIMERAND,0);
	CurrentEmissionTimeRand = (float)EmissionTimeRand*rand()/RAND_MAX;
	fSurfaceOffset = ini->GetFloat(psname,PSKEY_SURFACEOFFSET,0);
	ParticleColor = ini->GetLong(psname,"color",0xffffffff);
	
	fWindEffect = ini->GetFloat(psname,PSKEY_WINDEFFECT,0.0f);
	
	DirectionDeviation = ini->GetFloat(psname,PSKEY_DDEVIATION,0.0f);
	Gravity = ini->GetFloat(psname,PSKEY_GRAVITY,0.0f);
	Inispeed = ini->GetFloat(psname,PSKEY_INISPEED,0.0f);
	SpeedDeviation = ini->GetFloat(psname,PSKEY_SDEVIATION,0.0f);
	Lifetime = ini->GetLong(psname,PSKEY_LIFETIME,1000);
	Spin = ini->GetFloat(psname,PSKEY_SPIN,0.0f);
	SpinDeviation = ini->GetFloat(psname,PSKEY_SPINDEV,0.0f);
	EmitterIniTime = ini->GetLong(psname,PSKEY_EMITTERINITIME,0);
	Weight = ini->GetFloat(psname,PSKEY_WEIGHT,0.0f);
	WeightDeviation = ini->GetFloat(psname,PSKEY_WEIGHTDEVIATION,0.0f);
	Emitdelta = ini->GetLong(psname,PSKEY_EMITDELTA,0);
	ESpace = ini->GetFloat(psname,PSKEY_EMITRADIUS,0);
	fTrackPointRadius = ini->GetFloat(psname,PSKEY_TRACKPOINTRADIUS,1.0f);
	
	
	if(ini->TestKey(psname,PSKEY_COLORINVERSE,0)) bColorInverse = true;
	else bColorInverse = false;

	if(ini->TestKey(psname,PSKEY_UNIFORMEMIT,0)) bUniformEmit = true;
	else bUniformEmit = false;

	if(ini->TestKey(psname,PSKEY_RANDOMDIRECTION,0)) bRandomDirection = true;
	else bRandomDirection = false;
	
	if(ini->TestKey(psname,PSKEY_NONSTOPEMIT,0)) bRepeat = true;
	else bRepeat = false;

	
	
	float ChaosVal;
	ChaosVal = 0.0001f;
	

	Particle = (PARTICLE *) NEW char[ParticlesNum*sizeof(PARTICLE)];
	if(Particle == 0) _THROW(mem error);

	memset(Particle,0,ParticlesNum*sizeof(PARTICLE));
	
	for(n=0;n<ParticlesNum;n++)
	{
		Particle[n].pos.x = Emitter.x + ESpace*(0.5f - (float)rand()/RAND_MAX);
		Particle[n].pos.y = Emitter.y + ESpace*(0.5f - (float)rand()/RAND_MAX);
		Particle[n].pos.z = Emitter.z + ESpace*(0.5f - (float)rand()/RAND_MAX);
		//Particle[n].size = 1.0f;
		Particle[n].size = 0.0f;
		Particle[n].color = 0xffffffff;
		Particle[n].color = ParticleColor;

		Particle[n].weight = Weight + WeightDeviation*(0.5f - (float)rand()/RAND_MAX);
		
		Particle[n].speedVal = Inispeed + SpeedDeviation*(0.5f - (float)rand()/RAND_MAX);
		Particle[n].speed = 0;//Particle[n].speedVal;

		if(bRandomDirection)
		{
			Particle[n].ang.x = (0.5f - (float)rand()/RAND_MAX);
			Particle[n].ang.y = (0.5f - (float)rand()/RAND_MAX);
			Particle[n].ang.z = (0.5f - (float)rand()/RAND_MAX);
		}
		else
		{
			Particle[n].ang.x = EmitterDirection.x + DirectionDeviation*(0.5f - (float)rand()/RAND_MAX);
			Particle[n].ang.y = EmitterDirection.y + DirectionDeviation*(0.5f - (float)rand()/RAND_MAX);
			Particle[n].ang.z = EmitterDirection.z + DirectionDeviation*(0.5f - (float)rand()/RAND_MAX);
		}
		Particle[n].ang = !Particle[n].ang;


		Particle[n].chaos.x = ChaosVal*(0.5f - (float)rand()/RAND_MAX);
		Particle[n].chaos.y = ChaosVal*(0.5f - (float)rand()/RAND_MAX);
		Particle[n].chaos.z = ChaosVal*(0.5f - (float)rand()/RAND_MAX);
		Particle[n].speed_chaos = 1.0f - 0.1f*((float)rand()/RAND_MAX);
		

		Particle[n].v = Particle[n].ang * Particle[n].speed;
		Particle[n].lifetime = Lifetime;
		
		Particle[n].time = 0;
		Particle[n].live = false;
		Particle[n].done = false;
		//if(bUniformEmit) Particle[n].time = -n*(Lifetime/ParticlesNum);
		//else Particle[n].time = -EmitterIniTime * rand()/RAND_MAX;

		Particle[n].spinVal = Spin + SpinDeviation * (0.5f - (float)rand()/RAND_MAX);
		Particle[n].spin = Particle[n].spinVal;
	}
	
	// build tracks
	BuildTrack(ini,Visibility,psname,PSKEY_ALPHAKEY);
	BuildTrack(ini,ParticleSize,psname,PSKEY_PSIZEKEY);
	BuildTrack(ini,ParticleSpeed,psname,PSKEY_PSPEEDKEY);
	BuildTrack(ini,ParticleSpin,psname,PSKEY_PSPINKEY);
	BuildTrack(ini,WindEffect,psname,PSKEY_WINDEFFECTKEY);
	if(BuildTrack(ini,ParticleAngle,psname,PSKEY_PANGLEKEY)) bTrackAngle = true;
	else bTrackAngle = false;
	
	

	
	// create vertex buffer
	RenderService->CreateVertexBuffer(sizeof(PARTICLE_VERTEX)*VERTEXS_ON_PARTICLE*ParticlesNum, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, PARTICLE_FVF, D3DPOOL_SYSTEMMEM, &VBuffer);
	if(VBuffer == 0) _THROW(vbuffer error);

	UpdateVertexBuffer();

	UNGUARD
	return true;
}

void BSPS_PS::UpdateVertexBuffer()
{
	CVECTOR ipos[4];
	CVECTOR rpos[4];
	CVECTOR pos;
	CVECTOR local_pos;
	PARTICLE_VERTEX * pVertex;
	long n,i;
	long index;
	float halfsize;
	CMatrix RMatrix;

	Camera_EmitterPosA.x = Camera_EmitterPosA.y = Camera_EmitterPosA.z = 0;
	
	RenderService->VBLock(VBuffer, 0, sizeof(PARTICLE_VERTEX)*VERTEXS_ON_PARTICLE*ParticlesNum,(byte**)&pVertex, 0);
	for(n=0;n<ParticlesNum;n++)
	{
		index = n*VERTEXS_ON_PARTICLE;

		// RenderService->GetTransform(D3DTS_VIEW,Matrix); set for lock particles in screen zero axis
		local_pos = Matrix * Particle[n].pos;

		Camera_EmitterPosA += local_pos;

		halfsize = Particle[n].size/2.0f;

		ipos[0].x = -halfsize;
		ipos[0].y = halfsize;
		ipos[0].z = 0;

		ipos[1].x = -halfsize;
		ipos[1].y = -halfsize;
		ipos[1].z = 0;

		ipos[2].x = halfsize;
		ipos[2].y = -halfsize;
		ipos[2].z = 0;

		ipos[3].x = halfsize;
		ipos[3].y = halfsize;
		ipos[3].z = 0;

		
		RMatrix.BuildRotateZ(Particle[n].angle);
		
		rpos[0] = RMatrix * ipos[0];
		rpos[1] = RMatrix * ipos[1];
		rpos[2] = RMatrix * ipos[2];
		rpos[3] = RMatrix * ipos[3];

		/*rpos[0] = ipos[0];
		rpos[1] = ipos[1];
		rpos[2] = ipos[2];
		rpos[3] = ipos[3];*/

		
		// first & second left up 
		pos.x = local_pos.x + rpos[0].x;// - halfsize;
		pos.y = local_pos.y + rpos[0].y;//halfsize;
		pos.z = local_pos.z;
		pVertex[index].pos = pos;
		pVertex[index + 3].pos = pos;

		pVertex[index].tu = 0.0f;
		pVertex[index].tv = 0.0f;
		pVertex[index + 3].tu = 0.0f;
		pVertex[index + 3].tv = 0.0f;

		// first left down 
		pos.x = local_pos.x + rpos[1].x;// -halfsize;
		pos.y = local_pos.y + rpos[1].y;// -halfsize;
		pos.z = local_pos.z;
		pVertex[index + 1].pos = pos;

		pVertex[index + 1].tu = 0.0f;
		pVertex[index + 1].tv = 1.0f;


		// first & second right down 
		pos.x = local_pos.x + rpos[2].x;//halfsize;
		pos.y = local_pos.y + rpos[2].y;//-halfsize;
		pos.z = local_pos.z;
		pVertex[index + 2].pos = pos;
		pVertex[index + 4].pos = pos;

		pVertex[index + 2].tu = 1.0f;
		pVertex[index + 2].tv = 1.0f;
		pVertex[index + 4].tu = 1.0f;
		pVertex[index + 4].tv = 1.0f;

		// second right up 
		pos.x = local_pos.x + rpos[3].x;//halfsize;
		pos.y = local_pos.y + rpos[3].y;//halfsize;
		pos.z = local_pos.z;
		pVertex[index + 5].pos = pos;

		pVertex[index + 5].tu = 1.0f;
		pVertex[index + 5].tv = 0.0f;

		for(i=index;i<(index + VERTEXS_ON_PARTICLE);i++)
		{
			pVertex[i].color = Particle[n].color;
		}

	}
	RenderService->VBUnlock(VBuffer);
	if(ParticlesNum) 
	{
		Camera_EmitterPosA.x =  Camera_EmitterPosA.x/ParticlesNum;
		Camera_EmitterPosA.y =  Camera_EmitterPosA.y/ParticlesNum;
		Camera_EmitterPosA.z =  Camera_EmitterPosA.z/ParticlesNum;
	}
}

void BSPS_PS::Execute(dword DeltaTime)
{
	/*if(Delay > 0) {	Delay = Delay - DeltaTime;	return;	}

	if(bLinkEmitter)
	{
		COLLISION_OBJECT * pLink;
		pLink = (COLLISION_OBJECT *)api->GetEntityPointer(&LinkObject);
		if(pLink)
		{
			Emitter = pLink->mtx * LinkPos;
			EmitterDirection = pLink->mtx * LinkDirPos;
			EmitterDirection = EmitterDirection - Emitter;
			EmitterDirection = !EmitterDirection;
		}
	}
	ProcessParticles(DeltaTime);
	SetParticlesTracks(DeltaTime);
	UpdateVertexBuffer();*/
}

void BSPS_PS::LayOnSurface(dword index)
{
	COLLISION_OBJECT * pLink;
	CVECTOR from,to;
	float dist;
	pLink = (COLLISION_OBJECT *)api->GetEntityPointer(&SurfaceID);
	if(pLink == null) return;
	from = Particle[index].pos;
	to = from;
	from.y =100.0f;
	to.y = -100.0f;
	dist = pLink->Trace(from,to);
	Particle[index].pos.y = from.y + dist*(to.y - from.y) + fSurfaceOffset;
}

void BSPS_PS::Realize(dword DeltaTime)
{
	if(Delay > 0) {	Delay = Delay - DeltaTime;	return;	}

	if(bLinkEmitter)
	{
		COLLISION_OBJECT * pLink;
		pLink = (COLLISION_OBJECT *)api->GetEntityPointer(&LinkObject);
		if(pLink)
		{
			Emitter = pLink->mtx * LinkPos;
			EmitterDirection = pLink->mtx * LinkDirPos;
			EmitterDirection = EmitterDirection - Emitter;
			EmitterDirection = !EmitterDirection;

		}
	}

	CVECTOR p,a;

	p.x = p.y = p.z = 0; p.z = 0;
	a.z = a.y = a.z = 0;
	
	RenderService->GetTransform(D3DTS_VIEW,Matrix);

	//Camera_EmitterPos = Matrix * Emitter;

	RenderService->GetCamera(CameraPos,CameraAng,Perspective);
	
	CMatrix IMatrix;
	RenderService->SetTransform(D3DTS_VIEW,IMatrix);
	RenderService->SetTransform(D3DTS_WORLD,IMatrix);
	ProcessParticles(DeltaTime);
	SetParticlesTracks(DeltaTime);
	UpdateVertexBuffer();

	RenderService->TextureSet(0, TextureID[0]);

	RenderService->SetVertexShader(PARTICLE_FVF);
	RenderService->SetStreamSource(0, VBuffer, sizeof(PARTICLE_VERTEX));
	RenderService->SetIndices(0, 0);
	
	bool bDraw;
	//if(bColorInverse)bDraw = RenderService->TechniqueExecuteStart("particles_inv");
	//else bDraw = RenderService->TechniqueExecuteStart("particles");

	bDraw = RenderService->TechniqueExecuteStart(TechniqueName);
	if (bDraw) 
		do 
		{
			RenderService->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 2 * ParticlesNum);
		}
		while (RenderService->TechniqueExecuteNext());

	RenderService->SetTransform(D3DTS_VIEW,Matrix);
}


bool BSPS_PS::EmitParticle()
{
	long n;
	for(n=0;n<ParticlesNum;n++)
	{
		if(Particle[n].live) continue;	// search dead particle
		if(Particle[n].done && (!bRepeat)) continue;
		// emit new
		
		Particle[n].time = 0;
		Particle[n].flow_track_index = 0;
		Particle[n].pos.x = Emitter.x + ESpace*(0.5f - (float)rand()/RAND_MAX);
		Particle[n].pos.y = Emitter.y + ESpace*(0.5f - (float)rand()/RAND_MAX);
		Particle[n].pos.z = Emitter.z + ESpace*(0.5f - (float)rand()/RAND_MAX);


		Particle[n].speed = 0;//Inispeed + SpeedDeviation*(0.5f - (float)rand()/RAND_MAX);

		if(bRandomDirection)
		{
			Particle[n].ang.x = (0.5f - (float)rand()/RAND_MAX);
			Particle[n].ang.y = (0.5f - (float)rand()/RAND_MAX);
			Particle[n].ang.z = (0.5f - (float)rand()/RAND_MAX);
		}
		else
		{
			Particle[n].ang.x = EmitterDirection.x + DirectionDeviation*(0.5f - (float)rand()/RAND_MAX);
			Particle[n].ang.y = EmitterDirection.y + DirectionDeviation*(0.5f - (float)rand()/RAND_MAX);
			Particle[n].ang.z = EmitterDirection.z + DirectionDeviation*(0.5f - (float)rand()/RAND_MAX);
		}
		Particle[n].ang = !Particle[n].ang;
		Particle[n].v = Particle[n].ang * Particle[n].speed;
		Particle[n].live = true;
		if(bLayOnSurface) LayOnSurface(n);

		return true;
	}
	return false;
}

void BSPS_PS::ProcessParticles(dword DeltaTime)
{
	long n;

	if(nSystemLifeTime > 0)
	{
		nSystemLifeTime = nSystemLifeTime - DeltaTime;
		if(nSystemLifeTime <= 0) bRepeat = false;
	}

	for(n=0;n<ParticlesNum;n++)
	{
		if(!Particle[n].live) continue;

		
		Particle[n].time += DeltaTime;
		if(Particle[n].time > Lifetime)
		{
			// particle done
			Particle[n].live = false;
			if(!bRepeat) Particle[n].done = true;
			Particle[n].size = 0;
			Particle[n].color = 0;
			Particle[n].color = 0xffffff;
			Particle[n].color = ParticleColor;
			if(ParticleColor != 0xffffffff)
			{
				Particle[n].color &= 0xff000000;
				Particle[n].color |= 0xffffff*rand()/RAND_MAX;

			}
			
			Particle[n].time = 0;
			continue;
		}

		// update particle parameters
		if(bUseFlowTrack)
		{
			Particle[n].pos.x += emitterDelta.x + DeltaTime*(Particle[n].v.x + Particle[n].chaos.x);
			Particle[n].pos.y += emitterDelta.y + DeltaTime*(Particle[n].v.y + Particle[n].chaos.y);
			Particle[n].pos.z += emitterDelta.z + DeltaTime*(Particle[n].v.z + Particle[n].chaos.z);

			//Particle[n].angle += DeltaTime * Particle[n].spin;
			SetFlowTrack(n);
			
			//Particle[n].pos.y -= Particle[n].weight*Gravity*DeltaTime;
			
			Particle[n].speed *= Particle[n].speed_chaos;
			
			Particle[n].v = Particle[n].ang * Particle[n].speed;
		}
		else
		{
			Particle[n].pos.x += emitterDelta.x + DeltaTime*(Particle[n].v.x + Particle[n].chaos.x);
			Particle[n].pos.y += emitterDelta.y + DeltaTime*(Particle[n].v.y + Particle[n].chaos.y);
			Particle[n].pos.z += emitterDelta.z + DeltaTime*(Particle[n].v.z + Particle[n].chaos.z);
			Particle[n].angle += DeltaTime * Particle[n].spin;
			Particle[n].pos.y -= Particle[n].weight*Gravity*DeltaTime;
			Particle[n].speed *= Particle[n].speed_chaos;
			Particle[n].v = Particle[n].ang * Particle[n].speed;
			/*
			// alpha ----------------------------------------------
			float val = GetTrackValue(&Visibility[0],Particle[n].time);
			dword alpha = (dword)((float)0xff * val);
			Particle[n].color = 0xffffff | (alpha << 24);
			*/
		}

		/*if(fWindEffect != 0.0f && fWindPower != 0.0f)
		{
			Particle[n].pos += (DeltaTime*fWindEffect*fWindPower)*vWindDirection;
		}*/
		//bComplete = false;	// still have particles to run

	}

	//_CORE_API->Trace("Delta: %d",DeltaTime);

	DeltaTimeSLE += DeltaTime;
	if(DeltaTimeSLE >= (EmissionTime + CurrentEmissionTimeRand))
	{
		// burn new particle
		if(EmitParticle()) 
		{
			DeltaTimeSLE = 0;//DeltaTimeSLE - EmissionTime;
		}
		CurrentEmissionTimeRand = (float)EmissionTimeRand*rand()/RAND_MAX;
	}

	if(!bRepeat)
	{
		nEmitted++;
		if(nEmitted > ParticlesNum) 
		{
			bComplete = true;
			for(n=0;n<ParticlesNum;n++)
			{
				if(Particle[n].done) continue;
				bComplete = false;
				break;
			}
		}
	}
}

void BSPS_PS::SetDelay(long _delay)
{
	long n;
	Delay = _delay;
	if(Delay > 0)
	{
		for(n=0;n<ParticlesNum;n++) 
		{
			Particle[n].color = Particle[n].color & 0xffffff;
		}
	}
}

void BSPS_PS::SetParticlesTracks(dword DeltaTime)
{
	dword color;
	dword alpha;
	float val;
	long n;

	for(n=0;n<ParticlesNum;n++) 
	{
		if(!Particle[n].live) continue;
		// alpha ----------------------------------------------
		val = GetTrackValue(&Visibility[0],Particle[n].time);
		alpha = (dword)((float)0xff * val);
		color = Particle[n].color & (0xffffff);

		//if(bColorInverse) {black = alpha; color = (black<<16)|(black<<8)|black;}

		color = ((alpha<<24)&0xff000000)|color;
		

		Particle[n].color = color;


		// size -----------------------------------------------
		Particle[n].size = GetTrackValue(ParticleSize,Particle[n].time);

		// speed ----------------------------------------------
		Particle[n].speed = Particle[n].speedVal * GetTrackValue(ParticleSpeed,Particle[n].time);

		// spin -----------------------------------------------
		Particle[n].spin = Particle[n].spinVal * GetTrackValue(ParticleSpin,Particle[n].time);

		// angle ----------------------------------------------
		if(bTrackAngle) Particle[n].angle = GetTrackValue(ParticleAngle,Particle[n].time);

		// wind effect ----------------------------------------
		if(fWindEffect != 0.0f && fWindPower != 0.0f)
		{
			val = GetTrackValue(WindEffect,Particle[n].time) * fWindEffect;
			Particle[n].pos += (DeltaTime*val*fWindPower)*vWindDirection;
		}

	}
}


float BSPS_PS::GetTrackValue(TRACK_EVENT * Track, long Time)
{
	long n;
	float v1,v2;
	long  t1,t2;

	if(Time < 0) return 0;

	for(n=0;n<TRACK_EVENT_MAX;n++)
	{
		// if time - return value
		if(Time == Track[n].time) return Track[n].value;

		if(Track[n].time < 0) 
		{
			// if no more keys - return previous value
			if(n == 0) return 0;
			return Track[n - 1].value;
		}
		// skip already processed keys
		if(Time > Track[n].time) continue;
		
		// set from value and time
		if(n == 0)
		{
			v1 = 0;
			t1 = 0;
		}
		else
		{
			v1 = Track[n-1].value;
			t1 = Track[n-1].time;
		}

		// set to value and time
		v2 = Track[n].value;
		t2 = Track[n].time;

		if(t1 == t2) return Track[n].value;	// input error, double key

		return v1 + (Time - t1)*(v2 - v1)/(t2 - t1);
		
	}
	return 0;
}


bool BSPS_PS::BuildTrack(INIFILE * ini, TRACK_EVENT * Track, char * psname, char * key_name)
{
	long n,i;
	char buffer[MAX_PATH];
	bool bRes;
	bool bFound;
	
	bFound = false;

	for(n=0;n<TRACK_EVENT_MAX;n++)
	{
		Track[n].value = 0;
		Track[n].time = -1;

		if(n == 0)
		{
			bRes = ini->ReadString(psname,key_name,buffer,sizeof(buffer),"0,-1");
			if(bRes) bFound = true;
		}
		else
		{
			bRes = ini->ReadStringNext(psname,key_name,buffer,sizeof(buffer));
		}

		if(!bRes) { Track[n].time = -1; return bFound;}
		for(i=0;buffer[i];i++)
		{
			if(buffer[i] == ',') 
			{
				buffer[i] = 0;
				Track[n].value = (float)atof(buffer);
				Track[n].time = atol(&buffer[i+1]);
				break;
			}
		}
	}
	return bFound;
}

void BSPS_PS::SetEmitter(CVECTOR p, CVECTOR a)
{
	emitterDelta = p - Emitter;
	Emitter = p;
	EmitterDirection = !a;
	/*
	for(n=0;n<ParticlesNum;n++)
	{
		Particle[n].ang.x = EmitterDirection.x + DirectionDeviation*(0.5f - (float)rand()/RAND_MAX);
		Particle[n].ang.y = EmitterDirection.y + DirectionDeviation*(0.5f - (float)rand()/RAND_MAX);
		Particle[n].ang.z = EmitterDirection.z + DirectionDeviation*(0.5f - (float)rand()/RAND_MAX);
	}
	*/
}

void BSPS_PS::LinkToObject(ENTITY_ID id, CVECTOR _LinkPos)
{
	long n;
	bLinkEmitter = true;
	LinkObject = id;
	LinkPos = _LinkPos;
	LinkDir = EmitterDirection;
	LinkDirPos = LinkPos + LinkDir;
	
	COLLISION_OBJECT * pLink;
	pLink = (COLLISION_OBJECT *)api->GetEntityPointer(&LinkObject);
	if(pLink) Emitter = pLink->mtx * LinkPos;
	
	for(n=0;n<ParticlesNum;n++)
	{
		Particle[n].pos.x = Emitter.x + ESpace*(0.5f - (float)rand()/RAND_MAX);
		Particle[n].pos.y = Emitter.y + ESpace*(0.5f - (float)rand()/RAND_MAX);
		Particle[n].pos.z = Emitter.z + ESpace*(0.5f - (float)rand()/RAND_MAX);
	}
	UpdateVertexBuffer();
}

bool BSPS_PS::Complete()
{
	return bComplete;
}

void BSPS_PS::AddTrackPoint(CVECTOR pos)
{
	bUseFlowTrack = true;
	nFlowTrackSize++;
	pFlowTrack = (CVECTOR *)RESIZE(pFlowTrack,nFlowTrackSize*sizeof(CVECTOR));
	pFlowTrack[nFlowTrackSize - 1] = pos;

}

void BSPS_PS::SetFlowTrack(dword index)
{
	CVECTOR dest;
	if(Particle[index].flow_track_index >= nFlowTrackSize) return;
	dest = pFlowTrack[Particle[index].flow_track_index];
	dest = dest - Particle[index].pos; 
	Particle[index].ang = !dest;
	float dist = ~dest;
	if(dist < fTrackPointRadius)
	{
		Particle[index].flow_track_index++;
	}
	//if(index==0)_CORE_API->Trace("track: %d",Particle[index].flow_track_index);
}


void BSPS_PS::UseSurface(ENTITY_ID surface_id)
{
	bLayOnSurface = true;
	SurfaceID = surface_id;
}

void  BSPS_PS::TryEmitParticle()
{
	// burn new particle
	if(EmitParticle()) 
	{
		DeltaTimeSLE = 0;//DeltaTimeSLE - EmissionTime;
	}
	CurrentEmissionTimeRand = (float)EmissionTimeRand*rand()/RAND_MAX;
}
