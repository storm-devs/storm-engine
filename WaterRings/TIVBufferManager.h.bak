#ifndef _TIVBUFFERMANAGER_H_
#define _TIVBUFFERMANAGER_H_

#include "..\common_h\dx8render.h"
#include "..\common_h\geos.h"
#include "..\common_h\exs.h"

///////////////////////////////////////////////////////////////////
// CLASS DEFINITION
///////////////////////////////////////////////////////////////////
class TIVBufferManager  
{
public:
	TIVBufferManager(VDX8RENDER *_renderer, long _vType, long _vSize, long _iCount, long _vCount, long _count);
	virtual ~TIVBufferManager();

	long ReserveElement();
	void FreeElement(long _i);
	void FreeAll();
	void LockBuffers();
	void UnlockBuffers();
	void GetPointers(long _i, WORD **iPointer, void **vPointer, long *vOffset = 0);
	void DrawBuffers(char *_technique);

private:
	VDX8RENDER *renderer;
	long vBuffer, iBuffer;
	long vType, vSize, iCount, vCount, elementsCount;
	bool *used;
	bool locked;
	WORD *indexes;
	void *vertices;
	long count;
};

#endif
