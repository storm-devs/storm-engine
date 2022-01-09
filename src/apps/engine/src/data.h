#pragma once

#include "token.h"
#include "v_data.h"

class VIRTUAL_COMPILER
{
  public:
    virtual void SetError(const char *data_PTR, ...){};

    virtual void SetWarning(const char *data_PTR, ...){};

    virtual VSTRING_CODEC *GetVSC()
    {
        return nullptr;
    }
};

/*struct OBJECT_DESC
{
    entid_t    object_id;
    ATTRIBUTES * pAttributes;
};*/

class COMPILER;

class DATA : public VDATA
{
    friend COMPILER;

    VIRTUAL_COMPILER *pVCompiler;
    S_TOKEN_TYPE Data_type;
    uint32_t Number_of_elements;
    // bool   bRef;
    bool bArray;
    // char * ArrayPointer;
    int32_t lValue;
    uintptr_t pValue;
    float fValue;
    std::string sValue;

    bool bEntity;
    entid_t object_id;
    uint32_t Segment_id;

    DATA *pReference;
    // int32_t   ReferenceIndex;

    std::vector<DATA> ArrayPTR;
    uint32_t nGlobalVarTableIndex;

  public:
    ATTRIBUTES *AttributesClass;
    // ATTRIBUTES Attributes;
    DATA *GetReference() override;
    void SetReference(DATA *pRef) override;
    void SetAReference(ATTRIBUTES *pARef) override;
    // void  SetReferenceFlag() {bRef = true;};
    void SetSegmentID(uint32_t id)
    {
        Segment_id = id;
    };

    uint32_t GetSegmentID()
    {
        return Segment_id;
    };
    void SetVCompiler(VIRTUAL_COMPILER *pVC);
    DATA();
    DATA(const DATA &);
    DATA(DATA &&) noexcept(false);
    DATA &operator=(const DATA &);
    DATA(S_TOKEN_TYPE _element_type);
    DATA(uint32_t num_of_elements, S_TOKEN_TYPE _element_type);
    ~DATA() override;
    void Set(int32_t value) override;
    void SetPtr(uintptr_t value) override;
    bool GetPtr(uintptr_t &value) override;

    void Set(float value) override;
    void Set(std::string value) override;
    void Set(const char* value) override;
    void Set(const char *attribute_name, const char *attribute_value) override;
    bool Get(int32_t &value) override;
    bool Get(float &value) override;
    bool Get(const char *&value) override;
    bool Get(const char *attribute_name, const char *&value) override;

    bool Set(int32_t value, uint32_t index) override;
    bool Set(float value, uint32_t index) override;
    bool Set(const char *value, uint32_t index) override;

    bool Get(int32_t &value, uint32_t index) override;
    bool Get(float &value, uint32_t index) override;
    bool Get(const char *&value, uint32_t index) override;

    bool Set(const char *attribute_name, const char *attribute_value, uint32_t index) override;
    bool Get(const char *attribute_name, const char *&value, uint32_t index) override;

    void Set(entid_t eid) override;
    void Get(entid_t &eid);

    bool Set(entid_t eid, uint32_t index) override;
    bool Get(entid_t &eid, uint32_t index);

    ATTRIBUTES *GetAClass() override;
    ATTRIBUTES *GetAClass(uint32_t index) override;

    void SetType(S_TOKEN_TYPE type, uint32_t array_size = 1);
    bool Convert(S_TOKEN_TYPE type);
    void Error(const char *);
    DATA *GetArrayElement(uint32_t index) override;

    S_TOKEN_TYPE GetType()
    {
        return Data_type;
    };
    uint32_t GetElementsNum() override;
    void SetElementsNum(uint32_t asize) override;

    bool IsArray() override
    {
        return bArray;
    };
    bool IsReference() override;
    bool IsAReference() override;
    DATA *GetVarPointer() override;
    void ClearType();

    entid_t GetObjectIDPTR() override
    {
        return object_id;
    }

    // operations
    bool Inc() override;
    bool Dec() override;
    bool Copy(DATA *pV) override;
    bool Copy(DATA *pV, uint32_t index) override;
    bool CopyOnElement(DATA *pV, uint32_t index) override;
    bool Inverse() override;
    bool Power(int32_t Deg) override;
    bool Power(DATA *pV) override;
    bool Multiply(DATA *pV) override;
    bool Divide(DATA *pV) override;
    bool Modul(DATA *pV) override;
    bool Plus(DATA *pV) override;
    bool Minus(DATA *pV) override;
    bool Compare(DATA *pV, char opA, char opB) override;
    bool Neg() override;
    bool CompareAndSetResult(DATA *pV, S_TOKEN_TYPE op);

    bool BoolConvert();
    bool RefConvert();

    void BadIndex(uint32_t index, uint32_t array_size);

    int32_t GetInt() override;
    uintptr_t GetPtr() override;
    float GetFloat() override;
    const char *GetString() override;
    entid_t GetEntityID() override;

    void SetGlobalVarTableIndex(uint32_t index)
    {
        nGlobalVarTableIndex = index;
    };

    void Release();
};
