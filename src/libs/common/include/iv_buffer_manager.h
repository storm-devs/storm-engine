#pragma once

#include "dx9render.h"

///////////////////////////////////////////////////////////////////
// CLASS DEFINITION
///////////////////////////////////////////////////////////////////
class IVBufferManager
{
  public:
    IVBufferManager(VDX9RENDER *renderer_, int32_t vertex_type, int vertex_size, size_t index_count, size_t vertex_count,
                    size_t max_size);
    virtual ~IVBufferManager();

    int32_t ReserveElement();
    void FreeElement(size_t i);
    void FreeAll();
    void LockBuffers();
    void UnlockBuffers();
    void GetPointers(int32_t _i, uint16_t **iPointer, void **vPointer, int32_t *vOffset = nullptr) const;
    void DrawBuffers(const char *_technique) const;

  private:
    VDX9RENDER *const renderer_;
    const size_t max_size_;
    const renderer_handle index_buffer_, vertex_buffer_;
    const size_t vertex_type_, vertex_size_, index_count_, vertex_count_;

    size_t actual_size_;
    bool locked_;
    uint16_t *indexes_;
    void *vertices_;

    std::vector<bool> used_;
};
