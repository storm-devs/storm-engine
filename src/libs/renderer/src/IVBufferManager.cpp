#include "IVBufferManager.h"

//--------------------------------------------------------------------
IVBufferManager::IVBufferManager(VDX9RENDER *renderer_, long vertex_type, int vertex_size, size_t index_count,
                                 size_t vertex_count, size_t max_size)
    : renderer_(renderer_), max_size_(max_size),
      index_buffer_(renderer_->CreateIndexBuffer(max_size * index_count * sizeof(uint16_t))),
      vertex_buffer_(
          renderer_->CreateVertexBuffer(vertex_type, max_size * vertex_count * vertex_size, D3DUSAGE_WRITEONLY)),
      vertex_type_(vertex_type), vertex_size_(vertex_size), index_count_(index_count), vertex_count_(vertex_count),
      actual_size_(0)
{
    used_.resize(max_size, false);
}

//--------------------------------------------------------------------
IVBufferManager::~IVBufferManager()
{
    if (vertex_buffer_)
    {
        renderer_->ReleaseVertexBuffer(vertex_buffer_);
    }
    if (index_buffer_)
    {
        renderer_->ReleaseIndexBuffer(index_buffer_);
    }
}

//--------------------------------------------------------------------
long IVBufferManager::ReserveElement()
{
    for (size_t i = 0; i < max_size_; i++)
    {
        if (!used_[i])
        {
            used_[i] = true;
            if (actual_size_ == i)
            {
                ++actual_size_;
            }
            return i;
        }
    }

    return -1;
}

//--------------------------------------------------------------------
void IVBufferManager::FreeElement(size_t i)
{
    used_[i] = false;
}

//--------------------------------------------------------------------
void IVBufferManager::FreeAll()
{
    std::fill(std::begin(used_), std::end(used_), false);
}

//--------------------------------------------------------------------
void IVBufferManager::LockBuffers()
{
    indexes_ = static_cast<uint16_t *>(renderer_->LockIndexBuffer(index_buffer_));
    vertices_ = renderer_->LockVertexBuffer(vertex_buffer_);
    locked_ = true;
}

//--------------------------------------------------------------------
void IVBufferManager::UnlockBuffers()
{
    renderer_->UnLockIndexBuffer(index_buffer_);
    renderer_->UnLockVertexBuffer(vertex_buffer_);
    locked_ = false;
}

//--------------------------------------------------------------------
void IVBufferManager::GetPointers(long _i, uint16_t **iPointer, void **vPointer, long *vOffset /* = 0*/) const
{
    if ((_i < 0) || !locked_)
    {
        *iPointer = nullptr;
        *vPointer = nullptr;
        return;
    }

    *iPointer = indexes_ + _i * index_count_;
    *vPointer = static_cast<uint8_t *>(vertices_) + _i * vertex_size_ * vertex_count_;
    if (vOffset)
    {
        *vOffset = _i * vertex_count_;
    }
}

//--------------------------------------------------------------------
void IVBufferManager::DrawBuffers(const char *_technique) const
{
    if (locked_ || !actual_size_)
        return;

    renderer_->DrawBuffer(vertex_buffer_, vertex_size_, index_buffer_, 0, actual_size_ * vertex_count_, 0,
                          actual_size_ * index_count_ / 3, _technique);
}

//--------------------------------------------------------------------
