#pragma once

#include "entity.h"

class DATA;

class VDATA
{
  public:
    VDATA(){};

    virtual ~VDATA(){};

    virtual DATA *GetReference() = 0;
    virtual void SetReference(DATA *pRef) = 0;
    virtual void SetAReference(ATTRIBUTES *pARef) = 0;
    virtual void Set(int32_t value) = 0;
    virtual void SetPtr(uintptr_t value) = 0;
    virtual bool GetPtr(uintptr_t &value) = 0;
    virtual void Set(float value) = 0;
    virtual void Set(std::string value) = 0;
    virtual void Set(const char *value) = 0;
    virtual void Set(const char *attribute_name, const char *attribute_value) = 0;
    virtual bool Get(int32_t &value) = 0;
    virtual bool Get(float &value) = 0;
    virtual bool Get(const char *&value) = 0;
    virtual bool Get(const char *attribute_name, const char *&value) = 0;
    virtual bool Set(int32_t value, uint32_t index) = 0;
    virtual bool Set(float value, uint32_t index) = 0;
    virtual bool Set(const char *value, uint32_t index) = 0;
    virtual bool Get(int32_t &value, uint32_t index) = 0;
    virtual bool Get(float &value, uint32_t index) = 0;
    virtual bool Get(const char *&value, uint32_t index) = 0;
    virtual bool Set(const char *attribute_name, const char *attribute_value, uint32_t index) = 0;
    virtual bool Get(const char *attribute_name, const char *&value, uint32_t index) = 0;
    virtual void Set(entid_t eid) = 0;
    virtual bool Set(entid_t eid, uint32_t index) = 0;

    virtual ATTRIBUTES *GetAClass() = 0;
    virtual ATTRIBUTES *GetAClass(uint32_t index) = 0;
    virtual DATA *GetArrayElement(uint32_t index) = 0;
    // virtual S_TOKEN_TYPE GetType()=0;
    virtual uint32_t GetElementsNum() = 0;
    virtual bool IsArray() = 0;
    virtual bool IsReference() = 0;
    virtual bool IsAReference() = 0;
    virtual DATA *GetVarPointer() = 0;
    virtual entid_t GetObjectIDPTR() = 0;

    // operations
    virtual bool Inc() = 0;
    virtual bool Dec() = 0;
    virtual bool Copy(DATA *pV) = 0;
    virtual bool Copy(DATA *pV, uint32_t index) = 0;
    virtual bool CopyOnElement(DATA *pV, uint32_t index) = 0;
    virtual bool Inverse() = 0;
    virtual bool Power(int32_t Deg) = 0;
    virtual bool Power(DATA *pV) = 0;
    virtual bool Multiply(DATA *pV) = 0;
    virtual bool Divide(DATA *pV) = 0;
    virtual bool Modul(DATA *pV) = 0;
    virtual bool Plus(DATA *pV) = 0;
    virtual bool Minus(DATA *pV) = 0;
    virtual bool Compare(DATA *pV, char opA, char opB) = 0;
    virtual bool Neg() = 0;
    //    virtual bool CompareAndSetResult(DATA * pV,S_TOKEN_TYPE op)=0;

    virtual void SetElementsNum(uint32_t asize) = 0;

    virtual int32_t GetInt() = 0;
    virtual uintptr_t GetPtr() = 0;
    virtual float GetFloat() = 0;
    virtual const char *GetString() = 0;
    virtual entid_t GetEntityID() = 0;
};
