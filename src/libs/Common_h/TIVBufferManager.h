#ifndef _TIVBUFFERMANAGER_H_
#define _TIVBUFFERMANAGER_H_

#include "dx9render.h"

///////////////////////////////////////////////////////////////////
// CLASS DEFINITION
///////////////////////////////////////////////////////////////////
class TIVBufferManager
{
  public:
    TIVBufferManager(VDX9RENDER *_renderer, long _vType, long _vSize, long _iCount, long _vCount, long _count);
    virtual ~TIVBufferManager();

    long ReserveElement();
    void FreeElement(long _i);
    void FreeAll() const;
    void LockBuffers();
    void UnlockBuffers();
    void GetPointers(long _i, uint16_t **iPointer, void **vPointer, long *vOffset = nullptr) const;
    void DrawBuffers(const char *_technique) const;

  private:
    VDX9RENDER *renderer;
    long vBuffer, iBuffer;
    long vType, vSize, iCount, vCount, elementsCount;
    bool *used;
    bool locked;
    uint16_t *indexes;
    void *vertices;
    long count;
    long *ivIndexes;
};

#endif
