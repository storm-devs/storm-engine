// Class for working with memory as with a file ...

#pragma once

#include <cstdint>
#include <cstring>
#include <storm_assert.h>

#define VFSEEK_SET (0x00000000)
#define VFSEEK_CURRENT (0x00000001)
#define VFSEEK_END (0x00000002)

class MemFile
{
    bool DataIsMy;

    uint8_t *Data;
    uint32_t MaxLen;
    uint32_t CurPos;
    uint32_t BiggestWritePos;

  public:
    MemFile()
    {
        DataIsMy = false;
        Data = nullptr;
        MaxLen = 0;
        CurPos = 0;
        BiggestWritePos = 0;
    }

    ~MemFile()
    {
        Close();
    }

    bool Compare(MemFile *pMemfile)
    {
        if (GetLength() != pMemfile->GetLength())
            return false;

        for (uint32_t i = 0; i < GetLength(); i++)
        {
            const auto Buffer1 = static_cast<uint8_t *>(GetBuffer());
            const auto Buffer2 = static_cast<uint8_t *>(pMemfile->GetBuffer());

            if (Buffer1[i] != Buffer2[i])
                return false;
        }

        return true;
    }

    void OpenRead(void *Data, uint32_t DataSize)
    {
        DataIsMy = false;
        this->Data = static_cast<uint8_t *>(Data);
        MaxLen = DataSize;
        CurPos = 0;
    }

    void OpenWrite(uint32_t MaxSize)
    {
        DataIsMy = true;
        Data = new uint8_t[MaxSize];
        memset(Data, 0, MaxSize);
        MaxLen = MaxSize;
        CurPos = 0;
    }

    uint32_t Tell() const
    {
        return CurPos;
    }

    uint32_t GetLength() const
    {
        if (DataIsMy)
            return BiggestWritePos;
        return MaxLen;
    }

    void Seek(int NewPos, uint32_t flags)
    {
        switch (flags)
        {
        case VFSEEK_SET:
            CurPos = NewPos;
            break;
        case VFSEEK_CURRENT:
            CurPos += NewPos;
            break;
        case VFSEEK_END:
            CurPos = GetLength() + NewPos;
            break;
        }
    }

    void Close()
    {
        if (DataIsMy)
        {
            delete Data;
            Data = nullptr;
        }
    }

    uint32_t Read(void *Buffer, uint32_t size)
    {
        if (!Data)
            return 0;
        const auto real_size = CurPos + size;
        if (real_size > MaxLen)
            size = size - (real_size - MaxLen);
        if (size <= 0)
            return 0;
        memcpy(Buffer, (Data + CurPos), size);
        CurPos += size;
        return size;
    }

    uint32_t WriteZeroByte()
    {
        uint8_t Zero = 0;
        return Write(&Zero, 1);
    }

    uint32_t Write(const void *Buffer, uint32_t size)
    {
        if (!DataIsMy)
            return 0;
        if (!Data)
            return 0;

        const auto real_size = CurPos + size;
        if (real_size > MaxLen)
            size = size - (real_size - MaxLen);
        if (size <= 0)
            return 0;

        memcpy((Data + CurPos), Buffer, size);
        CurPos += size;
        if (CurPos > BiggestWritePos)
            BiggestWritePos = CurPos;
        return size;
    }

    template <class TYPE> uint32_t WriteType(TYPE &Val)
    {
        return Write(&Val, sizeof(TYPE));
    }

    template <class TYPE> uint32_t ReadType(TYPE &Val)
    {
        return Read(&Val, sizeof(TYPE));
    }

    void *GetBuffer() const
    {
        return Data;
    }
};
