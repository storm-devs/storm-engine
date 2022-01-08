#pragma once
#include <vector>

/* TODO: REMOVE THIS.... */
constexpr uint32_t MakeHashValue(const char *string)
{
    uint32_t hval = 0;

    while (*string != 0)
    {
        auto v = *string++;
        if ('A' <= v && v <= 'Z')
            v += 'a' - 'A';

        hval = (hval << 4) + static_cast<uint32_t>(v);
        const uint32_t g = hval & (static_cast<uint32_t>(0xf) << (32 - 4));
        if (g != 0)
        {
            hval ^= g >> (32 - 8);
            hval ^= g;
        }
    }
    return hval;
}

class VMA;
inline std::vector<VMA *> __STORM_CLASSES_REGISTRY;

class VMA
{
  protected:
    VMA *pNext;
    int32_t nHash;
    int32_t nReference;

  public:
    VMA() : pNext(nullptr)
    {
        nReference = 0;
        nHash = 0;
        __STORM_CLASSES_REGISTRY.push_back(this);
    }

    VMA *Next() const
    {
        return pNext;
    }

    virtual ~VMA() = default;

    int32_t Build_Version()
    {
        return -1;
    }

    void SetHash(int32_t _hash)
    {
        nHash = _hash;
    }

    int32_t GetHash() const
    {
        return nHash;
    }

    void Set(VMA *_p)
    {
        pNext = _p;
    }

    virtual bool Service()
    {
        return false;
    }

    virtual const char *GetName()
    {
        return nullptr;
    }

    virtual void *CreateClass()
    {
        return nullptr;
    }

    virtual void RefDec()
    {
        nReference--;
    }

    virtual int32_t GetReference()
    {
        return nReference;
    }

    virtual void Clear()
    {
        nReference = 0;
    }

    virtual bool ScriptLibriary()
    {
        return false;
    }
};

#define CREATE_CLASS(a)                                                                                                \
    class a##vmacd : public VMA                                                                                        \
    {                                                                                                                  \
      public:                                                                                                          \
        const char *GetName()                                                                                          \
        {                                                                                                              \
            return #a;                                                                                                 \
        }                                                                                                              \
        void *CreateClass()                                                                                            \
        {                                                                                                              \
            nReference++;                                                                                              \
            return new a;                                                                                              \
        }                                                                                                              \
    } a##vmaci;
#define CREATE_SERVICE(a)                                                                                              \
    class a##vmacd : public VMA                                                                                        \
    {                                                                                                                  \
      public:                                                                                                          \
        a *pService = 0;                                                                                               \
        const char *GetName()                                                                                          \
        {                                                                                                              \
            return #a;                                                                                                 \
        }                                                                                                              \
        void *CreateClass()                                                                                            \
        {                                                                                                              \
            if (pService == 0)                                                                                         \
                pService = new a;                                                                                      \
            nReference++;                                                                                              \
            return pService;                                                                                           \
        }                                                                                                              \
        bool Service()                                                                                                 \
        {                                                                                                              \
            return true;                                                                                               \
        }                                                                                                              \
        void Clear()                                                                                                   \
        {                                                                                                              \
            nReference = 0;                                                                                            \
            if (pService)                                                                                              \
                delete pService;                                                                                       \
            pService = 0;                                                                                              \
        };                                                                                                             \
    } a##vmaci;
#define CREATE_SCRIPTLIBRIARY(a)                                                                                       \
    class a##vmacd : public VMA                                                                                        \
    {                                                                                                                  \
      public:                                                                                                          \
        const char *GetName()                                                                                          \
        {                                                                                                              \
            return #a;                                                                                                 \
        }                                                                                                              \
        void *CreateClass()                                                                                            \
        {                                                                                                              \
            return new a;                                                                                              \
        }                                                                                                              \
        bool ScriptLibriary()                                                                                          \
        {                                                                                                              \
            return true;                                                                                               \
        }                                                                                                              \
    } a##vmaci;
