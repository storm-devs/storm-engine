#include "vcollide.h"

LCOLL::LCOLL(const char *layerName, VAPI &_api) : api(_api)
{
	walker = api.LayerGetWalker((char*)layerName);
	col = (COLLIDE*)api.CreateService("coll");
	if(!col)	_THROW("No service: collide");
}

LCOLL::~LCOLL()
{
	delete walker;
}

#define REALLOC_QUANT 1024	//mast be power of to due to AND

VAPI *GlobApi;
long addedFaces;
long *sVrt;
CVECTOR *addVerts;

bool AddPolyColl(const CVECTOR *vr, long nverts)
{
	//start vertex of face, max faces is REALLOC_QUANT
	if((addedFaces&(REALLOC_QUANT-1))==0)
		sVrt = (long*)GlobApi->MemReallocate(sVrt, sizeof(long)*(addedFaces + REALLOC_QUANT));

	if(addedFaces==0)	sVrt[addedFaces] = nverts;
	else	sVrt[addedFaces] = sVrt[addedFaces-1] + nverts;

	//F0(v0,v1,v2), F1(v0,v1,v2,v3)...
	if(addedFaces==0 || (sVrt[addedFaces-1]&(REALLOC_QUANT-1))+nverts>REALLOC_QUANT )
		addVerts = (CVECTOR*)GlobApi->MemReallocate(addVerts, sizeof(long)*(sVrt[addedFaces]/REALLOC_QUANT + REALLOC_QUANT));

	for(long v=0; v<nverts; v++)
	{
		addVerts[sVrt[addedFaces]-nverts+v].x = vr[v].x;
		addVerts[sVrt[addedFaces]-nverts+v].y = vr[v].x;
		addVerts[sVrt[addedFaces]-nverts+v].z = vr[v].x;
	}

	addedFaces++;
	return true;
}

long LCOLL::SetBox(const CVECTOR &boxSize, const CMatrix &transform, bool testOnly)
{
	//create box
	PLANE clip_p[6];
	memset(clip_p, 0, sizeof(plane));
	clip_p[0].Nx = clip_p[2].Ny = clip_p[4].Nz = 1.0f;
	clip_p[1].Nx = clip_p[3].Ny = clip_p[5].Nz = -1.0f;
	clip_p[0].D = boxSize.x;	clip_p[1].D = -boxSize.x;
	clip_p[2].D = boxSize.y;	clip_p[3].D = -boxSize.y;
	clip_p[4].D = boxSize.z;	clip_p[5].D = -boxSize.z;

	//transform planes
	for(long p=0; p<6; p++)
	{
		float x = clip_p[p].D*clip_p[p].Nx - transform.m[3][0];
		float y = clip_p[p].D*clip_p[p].Ny - transform.m[3][1];
		float z = clip_p[p].D*clip_p[p].Nz - transform.m[3][2];
		float Nx = transform.m[0][0]*clip_p[p].Nx + transform.m[0][1]*clip_p[p].Ny + transform.m[0][2]*clip_p[p].Nz;
		float Ny = transform.m[1][0]*clip_p[p].Nx + transform.m[1][1]*clip_p[p].Ny + transform.m[1][2]*clip_p[p].Nz;
		float Nz = transform.m[2][0]*clip_p[p].Nx + transform.m[2][1]*clip_p[p].Ny + transform.m[2][2]*clip_p[p].Nz;
		float lx = transform.m[0][0]*x + transform.m[0][1]*y + transform.m[0][2]*z;
		float ly = transform.m[1][0]*x + transform.m[1][1]*y + transform.m[1][2]*z;
		float lz = transform.m[2][0]*x + transform.m[2][1]*y + transform.m[2][2]*z;
		plane[p].Nx = Nx;
		plane[p].Ny = Ny;
		plane[p].Nz = Nz;
		plane[p].D = (Nx*lx + Ny*ly + Nz*lz)*transform.m[3][3];
	}

	boxCenter = transform.Pos();
	boxRadius = sqrtf(~boxSize);

	
	//we needs api to have resize function
	GlobApi = &api;
	//added faces
	addedFaces = 0;
	//start vertex of face
	sVrt = 0;
	//F0(v0,v1,v2), F1(v0,v1,v2,v3)...
	addVerts = 0;

	col->Clip(*walker, &plane[0], 6, boxCenter, boxRadius, AddPolyColl, 0, 0);
	return 0;
}

const CVECTOR *LCOLL::GetFace(long &numVertices)
{
	numVertices = 0;
	return 0;
}

float LCOLL::Trace(const CVECTOR &src, const CVECTOR &dst)
{
	return 2.0f;
}