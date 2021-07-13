#pragma once
#include <vector>

/* TODO: REMOVE THIS.... */
constexpr uint32_t
MakeHashValue(const char *string)
{
    uint32_t hval = 0;

    while (*string != 0)
    {
        auto v = *string++;
        if ('A' <= v && v <= 'Z')
            v += 'a' - 'A';

        hval = (hval << 4) + static_cast<unsigned long>(v);
        const uint32_t g = hval & (static_cast<unsigned long>(0xf) << (32 - 4));
        if (g != 0)
        {
            hval ^= g >> (32 - 8);
            hval ^= g;
        }
    }
    return hval;
}

class VMA;

// extern VMA* _pModuleClassRoot;
inline std::vector<VMA *> GP_CLASSES_STACK;
// extern VSYSTEM_API* _VSYSTEM_API;

class VMA
{
  protected:
    VMA *pNext;
    long nHash;
    long nReference;

  public:
    VMA() : pNext(nullptr)
    {
        nReference = 0;
        nHash = 0;
        GP_CLASSES_STACK.push_back(this);
        // pNext = _pModuleClassRoot;
        //_pModuleClassRoot = this;
    };
    VMA *Next() const
    {
        return pNext;
    }

    virtual ~VMA(){};
    long Build_Version()
    {
        return -1;
    };
    void SetHash(long _hash)
    {
        nHash = _hash;
    }
    long GetHash() const
    {
        return nHash;
    }
    void Set(VMA *_p)
    {
        pNext = _p;
    };
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
    };
    virtual long GetReference()
    {
        return nReference;
    }
    virtual void Clear()
    {
        nReference = 0;
    };
    virtual bool ScriptLibriary()
    {
        return false;
    }
};

#define STORM_KEEP_SYMBOL(x) __pragma(optimize("", off)) static volatile x; __pragma(optimize("", on))

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
    } STORM_KEEP_SYMBOL(a##vmaci)
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
    } STORM_KEEP_SYMBOL(a##vmaci)
#define CREATE_SCRIPTLIBRIARY(a)                                                                                       \
    class a##vmacd : public VMA                                                                                        \
    {                                                                                                                  \
      public: /*a * pLibraryInitClass;*/                                                                               \
        const char *GetName()                                                                                          \
        {                                                                                                              \
            return #a;                                                                                                 \
        }                                                                                                              \
        void *CreateClass()                                                                                            \
        { /*if(pLibraryInitClass == 0) pLibraryInitClass = new a; nReference++; return pLibraryInitClass;*/            \
            return new a;                                                                                              \
        }                                                                                                              \
        bool ScriptLibriary()                                                                                          \
        {                                                                                                              \
            return true;                                                                                               \
        } /*void Clear(){nReference = 0; if(pLibraryInitClass) delete pLibraryInitClass; pLibraryInitClass = 0;}*/;    \
    } STORM_KEEP_SYMBOL(a##vmaci)