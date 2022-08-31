#include "data.h"

#include <charconv>

#include "core_impl.h"
#include "string_compare.hpp"

#include <fast_float/fast_float.h>

const char *UNINIT_REF = {"Using reference variable without initializing"};
const char *INVALID_REF_OP = {"Invalid operation on reference object"};
const char *NO_INDEX = {"Missed array index"};
const char *INVALID_CONVERSATION = {"Invalid conversation"};
const char *INDEX_ON_SINGLE = {"Incorrect usage of index on single variable"};
const char *INCORRECT_TYPE_ASSIGMENT = {"Incorrect type assigment"};
const char *INCORRECT_INDEX = {"Incorrect index"};
const char *INVALID_TYPE = {"Invalid type"};
const char *INVALID_STRING = {"Invalid string"};

extern void DumpError(const char *data_PTR, ...);

DATA::DATA()
    : pValue(0), object_id(0)
{
    Data_type = UNKNOWN;
    Number_of_elements = 0;
    lValue = 0;
    fValue = 0;
    bArray = false;
    bEntity = false;
    pVCompiler = nullptr;
    Segment_id = 0;
    //    bRef = false;
    pReference = nullptr;
    AttributesClass = nullptr;
    nGlobalVarTableIndex = 0xffffffff;
}

DATA::DATA(const DATA &data)
{
    *this = data;
}

DATA::DATA(DATA &&data) noexcept(false)
    : pValue(0)
{
    Data_type = data.Data_type;
    Number_of_elements = data.Number_of_elements;
    lValue = data.lValue;
    fValue = data.fValue;
    sValue = std::move(data.sValue);
    bArray = data.bArray;
    bEntity = data.bEntity;
    pVCompiler = data.pVCompiler;
    Segment_id = data.Segment_id;
    pReference = data.pReference;
    AttributesClass = data.AttributesClass;
    nGlobalVarTableIndex = data.nGlobalVarTableIndex;
    object_id = data.object_id;
    ArrayPTR = std::move(data.ArrayPTR);

    data.AttributesClass = nullptr;
}

DATA & DATA::operator=(const DATA &data)
{
    if (this == &data)
    {
        return *this;
    }

    Data_type = data.Data_type;
    Number_of_elements = data.Number_of_elements;
    lValue = data.lValue;
    fValue = data.fValue;
    sValue = data.sValue;
    bArray = data.bArray;
    bEntity = data.bEntity;
    pVCompiler = data.pVCompiler;
    Segment_id = data.Segment_id;
    pReference = data.pReference;
    if (data.AttributesClass)
    {
        Assert(&data.AttributesClass->GetStringCodec() == pVCompiler->GetVSC());
        AttributesClass = new ATTRIBUTES(data.AttributesClass->Copy());
    }
    else
    {
        AttributesClass = nullptr;
    }

    nGlobalVarTableIndex = std::numeric_limits<decltype(nGlobalVarTableIndex)>::max();
    object_id = data.object_id;
    ArrayPTR = data.ArrayPTR;

    return *this;
}

DATA::DATA(S_TOKEN_TYPE _element_type)
    : pValue(0), object_id(0)
{
    Data_type = _element_type;
    Number_of_elements = 0;
    lValue = 0;
    fValue = 0;
    bArray = false;
    bEntity = false;
    pVCompiler = nullptr;
    Segment_id = 0;
    //    bRef = false;
    pReference = nullptr;
    AttributesClass = nullptr;
    nGlobalVarTableIndex = 0xffffffff;
}

DATA::DATA(uint32_t _num_of_elements, S_TOKEN_TYPE _element_type)
    : pValue(0), object_id(0)
{
    nGlobalVarTableIndex = 0xffffffff;
    Number_of_elements = _num_of_elements;
    lValue = 0;
    fValue = 0;
    bArray = true;
    bEntity = false;
    Data_type = _element_type;
    pVCompiler = nullptr;
    Segment_id = 0;
    //    bRef = false;
    pReference = nullptr;
    AttributesClass = nullptr;

    ArrayPTR.reserve(Number_of_elements);
    for (uint32_t n = 0; n < Number_of_elements; n++)
    {
        ArrayPTR.emplace_back(_element_type);
        // ArrayPTR[n] =
        // new(&ArrayPTR[n])DATA(_element_type);
        // ArrayPTR[n]->SetVCompiler((VIRTUAL_COMPILER *)0);    // ???! compiler pointer isnt set for this constructor
    }
    /*    switch(Data_type)
      {
        case VAR_INTEGER:
        case VAR_FLOAT:
        case VAR_STRING:
        case VAR_OBJECT:
        break;
        default:
          Error(INVALID_TYPE);
          throw std::runtime_error("INVALID_TYPE");
        break;
      }*/
}

bool DATA::IsReference()
{
    if (Data_type == VAR_REFERENCE)
        return true;
    return false;
}

bool DATA::IsAReference()
{
    if (Data_type == VAR_AREFERENCE)
        return true;
    return false;
}

void DATA::Release()
{
    if (bArray)
    {
        /*for(n=0;n<Number_of_elements;n++)
        {
          ArrayPTR[n].~DATA();
          //delete ArrayPTR[n];
        }*/
        ArrayPTR.clear();
    }
    if (!(Data_type == VAR_REFERENCE || Data_type == VAR_AREFERENCE))
    {
        if (AttributesClass)
        {
            delete AttributesClass;
            AttributesClass = nullptr;
        }
    }
}

DATA::~DATA()
{
    Release();
    /*
    DWORD n;
    if(sValue) delete sValue; sValue = 0;
    if(bArray)
    {
      for(n=0;n<Number_of_elements;n++)
      {
        ArrayPTR[n].~DATA();
        //delete ArrayPTR[n];
      }
      delete (char *)ArrayPTR;
      ArrayPTR = 0;

    }
    if(!(Data_type == VAR_REFERENCE || Data_type == VAR_AREFERENCE))
    {
      if(AttributesClass)
      {
        delete AttributesClass; AttributesClass = 0;
      }
    }*/
}

void DATA::Error(const char *text)
{
    // DumpError(text);
    pVCompiler->SetError(text);
}

void DATA::SetReference(DATA *pRef)
{
    // if(!bRef)
    if (Data_type != VAR_REFERENCE)
    {
        Error("Isnt reference object");
        return;
    }
    pReference = pRef;
    // if(pRef) Data_type = pRef->GetType();
}

void DATA::SetAReference(ATTRIBUTES *pARef)
{
    if (Data_type != VAR_AREFERENCE)
    {
        Error("Isnt A reference object");
        return;
    }
    AttributesClass = pARef;
}

/*void  DATA::SetReference(DATA * pRef, int32_t reference_index)
{
    if(!bRef)
    {
        Error("Isnt reference object");
        return;
    }
    pReference = pRef;
    if(pRef) Data_type = pRef->GetType();
    ReferenceIndex = reference_index;
}*/

void DATA::SetPtr(uintptr_t value)
{
    // if(bRef)
    if (Data_type == VAR_REFERENCE)
    {
        if (pReference)
        {
            pReference->SetPtr(value);
            return;
        }
        Error(UNINIT_REF);
        return;
    }

    if (bArray)
    {
        Error(NO_INDEX);
        return;
    }
    Data_type = VAR_PTR;
    pValue = value;
}

void DATA::Set(int32_t value)
{
    // if(bRef)
    if (Data_type == VAR_REFERENCE)
    {
        if (pReference)
        {
            pReference->Set(value);
            return;
        }
        Error(UNINIT_REF);
        return;
    }

    if (bArray)
    {
        Error(NO_INDEX);
        return;
    }
    Data_type = VAR_INTEGER;
    lValue = value;
}

void DATA::Set(float value)
{
    // if(bRef)
    if (Data_type == VAR_REFERENCE)
    {
        if (pReference)
        {
            pReference->Set(value);
            return;
        }
        Error(UNINIT_REF);
        return;
    }
    if (bArray)
    {
        Error(NO_INDEX);
        return;
    }
    Data_type = VAR_FLOAT;
    fValue = value;
    if (isnan(fValue))
        Error("NAN ERROR");
}

void DATA::Set(std::string value)
{
    // if(bRef)
    if (Data_type == VAR_REFERENCE)
    {
        if (pReference)
        {
            pReference->Set(value);
            return;
        }
        Error(UNINIT_REF);
        return;
    }
    if (bArray)
    {
        Error(NO_INDEX);
        return;
    }
    Data_type = VAR_STRING;

    sValue = std::move(value);
}

void DATA::Set(const char *value)
{
    // if(bRef)
    if (Data_type == VAR_REFERENCE)
    {
        if (pReference)
        {
            pReference->Set(value);
            return;
        }
        Error(UNINIT_REF);
        return;
    }
    if (bArray)
    {
        Error(NO_INDEX);
        return;
    }
    Data_type = VAR_STRING;

    if (value != nullptr)
    {
        sValue = value;
    }
    else
    {
        sValue.clear();
    }
}

void DATA::Set(const char *attribute_name, const char *attribute_value)
{
    // if(bRef)
    if (Data_type == VAR_REFERENCE)
    {
        if (pReference)
        {
            pReference->Set(attribute_name, attribute_value);
            return;
        }
        Error(UNINIT_REF);
        return;
    }

    if (bArray)
    {
        Error(NO_INDEX);
        return;
    }
    if (AttributesClass == nullptr)
    {
        if (Data_type == VAR_AREFERENCE)
        {
            Error("uninitialized AReference");
            return;
        }

        AttributesClass = new ATTRIBUTES(pVCompiler->GetVSC());
    }
    AttributesClass->SetAttribute(attribute_name, attribute_value);
    // Attributes.SetAttribute(attribute_name,attribute_value);
}

void DATA::Set(entid_t eid)
{
    // if(bRef)
    if (Data_type == VAR_REFERENCE)
    {
        if (pReference)
        {
            pReference->Set(eid);
            return;
        }
        Error(UNINIT_REF);
        return;
    }
    if (bArray)
    {
        Error(NO_INDEX);
        return;
    }
    if (Data_type == VAR_AREFERENCE)
    {
        memcpy(&object_id, &eid, sizeof(entid_t));
        return;
    }
    Data_type = VAR_OBJECT;
    memcpy(&object_id, &eid, sizeof(entid_t));
    bEntity = true;
}

void DATA::Get(entid_t &eid)
{
    // if(bRef)
    if (Data_type == VAR_REFERENCE)
    {
        if (pReference)
        {
            pReference->Get(eid);
            return;
        }
        Error(UNINIT_REF);
        return;
    }
    if (bArray)
    {
        Error(NO_INDEX);
        return;
    }
    if (!(Data_type == VAR_OBJECT || Data_type == VAR_AREFERENCE))
    {
        Error("Not object");
        return;
    }
    eid = object_id;
}

bool DATA::GetPtr(uintptr_t &value)
{
    // if(bRef)
    if (Data_type == VAR_REFERENCE)
    {
        if (pReference)
        {
            return pReference->GetPtr(value);
        }
        Error(UNINIT_REF);
        return false;
    }
    if (bArray)
    {
        Error(NO_INDEX);
        return false;
    }
    if (Data_type == VAR_PTR)
    {
        value = pValue;
        return true;
    }
    return false;
}

bool DATA::Get(int32_t &value)
{
    // if(bRef)
    if (Data_type == VAR_REFERENCE)
    {
        if (pReference)
        {
            return pReference->Get(value);
        }
        Error(UNINIT_REF);
        return false;
    }
    if (bArray)
    {
        Error(NO_INDEX);
        return false;
    }
    if (Data_type == VAR_INTEGER)
    {
        value = lValue;
        return true;
    }
    return false;
}

bool DATA::Get(float &value)
{
    // if(bRef)
    if (Data_type == VAR_REFERENCE)
    {
        if (pReference)
        {
            return pReference->Get(value);
        }
        Error(UNINIT_REF);
        return false;
    }
    if (bArray)
    {
        Error(NO_INDEX);
        return false;
    }
    if (Data_type == VAR_FLOAT)
    {
        value = fValue;
        return true;
    }
    return false;
}

bool DATA::Get(const char *&value)
{
    // if(bRef)
    if (Data_type == VAR_REFERENCE)
    {
        if (pReference)
        {
            return pReference->Get(value);
        }
        Error(UNINIT_REF);
        return false;
    }
    value = nullptr;
    if (bArray)
    {
        Error(NO_INDEX);
        return false;
    }
    if (Data_type == VAR_STRING)
    {
        value = sValue.c_str();
        return true;
    }
    return false;
}

bool DATA::Get(const char *attribute_name, const char *&value)
{
    // if(bRef)
    if (Data_type == VAR_REFERENCE)
    {
        if (pReference)
        {
            return pReference->Get(attribute_name, value);
        }
        Error(UNINIT_REF);
        return false;
    }
    if (bArray)
    {
        Error(NO_INDEX);
        return false;
    }
    if (AttributesClass == nullptr)
        return false;
    // pAValue = Attributes.GetAttribute(attribute_name);
    const char *pAValue = AttributesClass->GetAttribute(attribute_name);
    if (pAValue == nullptr)
        return false;
    value = pAValue;
    return true;
}

bool DATA::Get(int32_t &value, uint32_t index)
{
    // if(bRef)
    if (Data_type == VAR_REFERENCE)
    {
        if (pReference)
        {
            return pReference->Get(value, index);
        }
        Error(UNINIT_REF);
        return false;
    }
    if (!bArray)
    {
        Error(INDEX_ON_SINGLE);
        return false;
    }
    if (Data_type != VAR_INTEGER)
    {
        Error(INCORRECT_TYPE_ASSIGMENT);
        return false;
    }
    if (index >= Number_of_elements)
    {
        BadIndex(index, Number_of_elements);
        // Error(INCORRECT_INDEX);
        return false;
    }
    return ArrayPTR[index].Get(value);
    /*
      int32_t * pL;
      pL = (int32_t *)ArrayPointer;
      value = pL[index];
      return true;    */
}

bool DATA::Get(float &value, uint32_t index)
{
    // if(bRef)
    if (Data_type == VAR_REFERENCE)
    {
        if (pReference)
        {
            return pReference->Get(value, index);
        }
        Error(UNINIT_REF);
        return false;
    }
    if (!bArray)
    {
        Error(INDEX_ON_SINGLE);
        return false;
    }
    if (Data_type != VAR_FLOAT)
    {
        Error(INCORRECT_TYPE_ASSIGMENT);
        return false;
    }
    if (index >= Number_of_elements)
    {
        BadIndex(index, Number_of_elements);
        // Error(INCORRECT_INDEX    );
        return false;
    }
    return ArrayPTR[index].Get(value);
    /*float * pF;
    pF = (float *)ArrayPointer;
    value = pF[index];
    return true;
    */
}

bool DATA::Get(const char *&value, uint32_t index)
{
    // if(bRef)
    if (Data_type == VAR_REFERENCE)
    {
        if (pReference)
        {
            return pReference->Get(value, index);
        }
        Error(UNINIT_REF);
        return false;
    }
    if (!bArray)
    {
        Error(INDEX_ON_SINGLE);
        return false;
    }
    if (Data_type != VAR_STRING)
    {
        Error(INCORRECT_TYPE_ASSIGMENT);
        return false;
    }
    if (index >= Number_of_elements)
    {
        // Error(INCORRECT_INDEX    );
        BadIndex(index, Number_of_elements);
        return false;
    }
    // if(value == null){ Error(INVALID_STRING); return false;}
    return ArrayPTR[index].Get(value);

    /*    char * * ppChar;
      ppChar = (char **)ArrayPointer;
      if(ppChar[index] == null) value[0] = 0;
      else strcpy_s(value,ppChar[index]);
      return true;    */
}

bool DATA::Set(int32_t value, uint32_t index)
{
    // if(bRef)
    if (Data_type == VAR_REFERENCE)
    {
        if (pReference)
        {
            return pReference->Set(value, index);
        }
        Error(UNINIT_REF);
        return false;
    }
    if (!bArray)
    {
        Error(INDEX_ON_SINGLE);
        return false;
    }
    if (Data_type != VAR_INTEGER)
    {
        Error(INCORRECT_TYPE_ASSIGMENT);
        return false;
    }
    if (index >= Number_of_elements)
    {
        BadIndex(index, Number_of_elements);
        // Error(INCORRECT_INDEX    );
        return false;
    }
    ArrayPTR[index].Set(value);
    return true;
    /*int32_t * pL;
    pL = (int32_t *)ArrayPointer;
    pL[index] = value;
    return true;    */
}

bool DATA::Set(float value, uint32_t index)
{
    // if(bRef)
    if (Data_type == VAR_REFERENCE)
    {
        if (pReference)
        {
            return pReference->Set(value, index);
        }
        Error(UNINIT_REF);
        return false;
    }
    if (!bArray)
    {
        Error(INDEX_ON_SINGLE);
        return false;
    }
    if (Data_type != VAR_FLOAT)
    {
        Error(INCORRECT_TYPE_ASSIGMENT);
        return false;
    }
    if (index >= Number_of_elements)
    {
        BadIndex(index, Number_of_elements);
        // Error(INCORRECT_INDEX    );
        return false;
    }
    ArrayPTR[index].Set(value);
    return true;

    /*float * pF;
    pF = (float *)ArrayPointer;
    pF[index] = value;
    return true;    */
}

bool DATA::Set(const char *value, uint32_t index)
{
    // if(bRef)
    if (Data_type == VAR_REFERENCE)
    {
        if (pReference)
        {
            return pReference->Set(value, index);
        }
        Error(UNINIT_REF);
        return false;
    }
    if (!bArray)
    {
        Error(INDEX_ON_SINGLE);
        return false;
    }
    if (Data_type != VAR_STRING)
    {
        Error(INCORRECT_TYPE_ASSIGMENT);
        return false;
    }
    if (index >= Number_of_elements)
    {
        BadIndex(index, Number_of_elements);
        // Error(INCORRECT_INDEX    );
        return false;
    }
    if (value == nullptr)
    {
        Error(INVALID_STRING);
        return false;
    }
    ArrayPTR[index].Set(value);
    return true;

    /*    char * * ppChar;
      ppChar = (char **)ArrayPointer;
      if(ppChar[index] != null) delete ppChar[index];
      ppChar[index] = null;
      ppChar[index] = new char[strlen(value) + 1];
      strcpy_s(ppChar[index],value);
      return true;    */
}

bool DATA::Set(const char *attribute_name, const char *attribute_value, uint32_t index)
{
    // if(bRef)
    if (Data_type == VAR_REFERENCE)
    {
        if (pReference)
        {
            return pReference->Set(attribute_name, attribute_value, index);
        }
        Error(UNINIT_REF);
        return false;
    }
    if (!bArray)
    {
        Error(INDEX_ON_SINGLE);
        return false;
    }
    if (Data_type != VAR_OBJECT)
    {
        Error("Access to attributes of non object array");
        return false;
    }
    if (index >= Number_of_elements)
    {
        BadIndex(index, Number_of_elements);
        // Error(INCORRECT_INDEX    );
        return false;
    }
    ArrayPTR[index].Set(attribute_name, attribute_value);
    return true;
    /*
      OBJECT_DESC * pOD;
      pOD = (OBJECT_DESC *)ArrayPointer;
      if(pOD[index].pAttributes == null ) pOD[index].pAttributes = new ATTRIBUTES;
      pOD[index].pAttributes->SetAttribute(attribute_name,attribute_value);
      return true;*/
}

bool DATA::Get(const char *attribute_name, const char *&value, uint32_t index)
{
    // if(bRef)
    if (Data_type == VAR_REFERENCE)
    {
        if (pReference)
        {
            return pReference->Get(attribute_name, value, index);
        }
        Error(UNINIT_REF);
        return false;
    }
    if (!bArray)
    {
        Error(INDEX_ON_SINGLE);
        return false;
    }
    if (Data_type != VAR_OBJECT)
    {
        Error("Access to attributes of non object array");
        return false;
    }
    if (index >= Number_of_elements)
    {
        BadIndex(index, Number_of_elements);
        // Error(INCORRECT_INDEX    );
        return false;
    }
    return ArrayPTR[index].Get(attribute_name, value);
    /*
      OBJECT_DESC * pOD;
      pOD = (OBJECT_DESC *)ArrayPointer;
      if(pOD[index].pAttributes == null ) {value = null; return false;}
      value = pOD[index].pAttributes->GetAttribute(attribute_name);
      if(value == null) return false;
      return true;*/
}

bool DATA::Set(entid_t eid, uint32_t index)
{
    // if(bRef)
    if (Data_type == VAR_REFERENCE)
    {
        if (pReference)
        {
            return pReference->Set(eid, index);
        }
        Error(UNINIT_REF);
        return false;
    }
    if (!bArray)
    {
        Error(INDEX_ON_SINGLE);
        return false;
    }
    if (Data_type != VAR_OBJECT)
    {
        Error("Access to attributes of non object array");
        return false;
    }
    if (index >= Number_of_elements)
    {
        BadIndex(index, Number_of_elements);
        // Error(INCORRECT_INDEX    );
        return false;
    }
    ArrayPTR[index].Set(eid);
    return true;
    /*
      OBJECT_DESC * pOD;
      pOD = (OBJECT_DESC *)ArrayPointer;
      pOD[index].object_id = eid;
      //memcpy(&pOD[index].object_id,&eid,sizeof(entid_t));
      return true;*/
}

bool DATA::Get(entid_t &eid, uint32_t index)
{
    // if(bRef)
    if (Data_type == VAR_REFERENCE)
    {
        if (pReference)
        {
            return pReference->Get(eid, index);
        }
        Error(UNINIT_REF);
        return false;
    }
    if (!bArray)
    {
        Error(INDEX_ON_SINGLE);
        return false;
    }
    if (!(Data_type == VAR_OBJECT || Data_type == VAR_AREFERENCE))
    {
        Error("Access to attributes of non object array");
        return false;
    }
    if (index >= Number_of_elements)
    {
        BadIndex(index, Number_of_elements);
        // Error(INCORRECT_INDEX);
        return false;
    }
    ArrayPTR[index].Get(eid);
    return true;
    /*
      OBJECT_DESC * pOD;
      pOD = (OBJECT_DESC *)ArrayPointer;
      eid = pOD[index].object_id;
      //memcpy(&eid,&pOD[index].object_id,sizeof(entid_t));
      return true;*/
}

void DATA::ClearType()
{
    if (Data_type != VAR_AREFERENCE)
    {
        delete AttributesClass;
    }
    AttributesClass = nullptr;
    Data_type = UNKNOWN;
    pReference = nullptr;
    sValue.clear();
}

void DATA::SetType(S_TOKEN_TYPE _element_type, uint32_t array_size)
{
    ClearType();
    Data_type = _element_type;
    bArray = false;

    if (array_size > 1)
    {
        bArray = true;
        /*if(ArrayPTR)
        {
          for(n=0;n<Number_of_elements;n++)
          {

            ArrayPTR[n].~DATA();
            //delete ArrayPTR[n];
          }
        }*/
        ArrayPTR.clear();
        Number_of_elements = array_size;
        ArrayPTR.reserve(Number_of_elements);
        for (uint32_t n = 0; n < Number_of_elements; n++)
        {
            // ArrayPTR[n] = new DATA(_element_type);
            // ArrayPTR[n]->SetVCompiler(pVCompiler);
            // new (&ArrayPTR[n]) DATA(_element_type);
            ArrayPTR.emplace_back(_element_type);
            ArrayPTR.back().SetVCompiler(pVCompiler);
        }
    }
    else
    {
        ArrayPTR.clear();
        Number_of_elements = 1;
    }
}

bool DATA::Convert(S_TOKEN_TYPE type)
{
    // if(bRef)
    if (Data_type == VAR_REFERENCE)
    {
        if (pReference)
        {
            return pReference->Convert(type);
        }
        Error(UNINIT_REF);
        return false;
    }

    if (bArray)
    {
        Error("Array conversation not supported");
        return false;
    } // yet
    char buffer[128];
    switch (Data_type)
    {
    case VAR_INTEGER:
        switch (type)
        {
        case VAR_INTEGER:
            return true;
        case VAR_PTR:
            Data_type = type;
            pValue = lValue;
            return true;
        case VAR_FLOAT:
            Data_type = type;
            fValue = static_cast<float>(lValue);
            return true;
        case VAR_STRING:
            Data_type = type;
            sprintf(buffer, "%d", lValue);
            Set(buffer);
            return true;
        default:
            Error(INVALID_CONVERSATION);
            return false;
        }
        break;
    case VAR_FLOAT:
        switch (type)
        {
        case VAR_INTEGER:
            Data_type = type;
            lValue = static_cast<int32_t>(fValue);
            return true;
        case VAR_FLOAT:
            return true;
        case VAR_STRING:
            Data_type = type;
            gcvt(fValue, 10, buffer);
            Set(buffer);
            return true;
        default:
            Error(INVALID_CONVERSATION);
            return false;
        }
        break;
    case VAR_STRING:
        switch (type)
        {
        case NUMBER:
        case VAR_INTEGER:
            Data_type = VAR_INTEGER;
            lValue = 0;
            std::from_chars(sValue.data(), sValue.data() + sValue.length(), lValue);
            return true;
        case FLOAT_NUMBER:
        case VAR_FLOAT:
            Data_type = VAR_FLOAT;
            fValue = 0.0f;
            fast_float::from_chars(sValue.data(), sValue.data() + sValue.length(), fValue);
            return true;
        case STRING:
        case VAR_STRING:
            return true;
        default:
            Error(INVALID_CONVERSATION);
            return false;
        }
        break;
    case VAR_OBJECT:
        switch (type)
        {
        case VAR_OBJECT:
            return true;
        }

        break;
    case VAR_AREFERENCE:
        switch (type)
        {
        case VAR_AREFERENCE:
            return true;
        case STRING:
        case VAR_STRING:
            if (!AttributesClass)
                break;
            if (!AttributesClass->GetThisAttr())
                break;
            Set(to_string(AttributesClass->GetThisAttr()));
            AttributesClass = nullptr;
            return true;
        case NUMBER:
        case VAR_INTEGER:
            if (!AttributesClass)
                break;
            if (!AttributesClass->GetThisAttr())
                break;
            Set(to_string(AttributesClass->GetThisAttr()));
            AttributesClass = nullptr;
            Data_type = VAR_INTEGER;
            lValue = 0;
            std::from_chars(sValue.data(), sValue.data() + sValue.length(), lValue);
            return true;
        case FLOAT_NUMBER:
        case VAR_FLOAT:
            if (!AttributesClass)
                break;
            if (!AttributesClass->GetThisAttr())
                break;
            Set(to_string(AttributesClass->GetThisAttr()));
            AttributesClass = nullptr;
            Data_type = VAR_FLOAT;
            fValue = 0.0f;
            fast_float::from_chars(sValue.data(), sValue.data() + sValue.length(), fValue);
            return true;
        }
        break;
    case VAR_PTR:
        switch (type)
        {
        case VAR_PTR:
            return true;
        case VAR_INTEGER:
            Data_type = type;
            lValue = static_cast<int32_t>(pValue);
            return true;
        default:
            Error(INVALID_CONVERSATION);
            return false;
        }
    }
    return false;
}

void DATA::SetElementsNum(uint32_t _asize)
{
    if (Data_type == VAR_REFERENCE)
    {
        if (pReference)
        {
            pReference->SetElementsNum(_asize);
            return;
        }
        Error(UNINIT_REF);
        return;
    }
    if (!bArray)
    {
        Error("Not Array");
        return;
    }

    if (_asize == 0)
        return;
    if (_asize == Number_of_elements)
        return;

    if (_asize < Number_of_elements)
    {
        /*for(n=_asize;n<Number_of_elements;n++)
        {
          //delete ArrayPTR[n];
          ArrayPTR[n].~DATA();

        }*/
        Number_of_elements = _asize;
        ArrayPTR.resize(Number_of_elements);
        return;
    }

    ArrayPTR.reserve(_asize);
    // for(n=(Number_of_elements - 1);n<_asize;n++)
    for (auto n = Number_of_elements; n < _asize; n++)
    {
        // ArrayPTR[n] = new DATA(Data_type);
        // new (&ArrayPTR[n]) DATA(Data_type);
        ArrayPTR.emplace_back(Data_type);
        ArrayPTR.back().SetVCompiler(ArrayPTR[0].pVCompiler);
    }
    Number_of_elements = _asize;
}

uint32_t DATA::GetElementsNum()
{
    // if(bRef)
    if (Data_type == VAR_REFERENCE)
    {
        if (pReference)
        {
            return pReference->GetElementsNum();
        }
        Error(UNINIT_REF);
        return 0;
    }
    if (bArray)
        return Number_of_elements;
    return 1;
}

bool DATA::Inc()
{
    // if(bRef)
    if (Data_type == VAR_REFERENCE)
    {
        if (pReference)
        {
            return pReference->Inc();
        }
        Error(UNINIT_REF);
        return false;
    }
    if (Data_type != VAR_INTEGER)
    {
        Error("Invalid data type for INC operation");
        return false;
    }
    lValue++;
    return true;
}

bool DATA::Dec()
{
    // if(bRef)
    if (Data_type == VAR_REFERENCE)
    {
        if (pReference)
        {
            return pReference->Dec();
        }
        Error(UNINIT_REF);
        return false;
    }
    if (Data_type != VAR_INTEGER)
    {
        Error("Invalid data type for DEC operation");
        return false;
    }
    lValue--;
    return true;
}

bool DATA::Neg()
{
    switch (GetType())
    {
    case VAR_FLOAT:
        Convert(VAR_INTEGER);
    case VAR_PTR:
        Convert(VAR_INTEGER);
    case VAR_INTEGER:
        if (lValue != 0)
            lValue = 0;
        else
            lValue = 1;
        Set(lValue);
        break;
    case VAR_STRING:
        if (sValue.empty())
            lValue = 1;
        else
            lValue = 0;
        Convert(VAR_INTEGER);
        Set(lValue);
        break;
    default:
        Error("Invalid argument type for '!' opearator");
        return false;
    }
    return true;
}

bool DATA::Inverse()
{
    // if(bRef)
    if (Data_type == VAR_REFERENCE)
    {
        if (pReference)
        {
            return pReference->Inverse();
        }
        Error(UNINIT_REF);
        return false;
    }
    if (IsArray())
        return false;
    switch (Data_type)
    {
    case VAR_INTEGER:
        lValue = -lValue;
        break;
    case VAR_FLOAT:
        fValue = -fValue;
        break;
    }
    return true;
}

bool DATA::Power(DATA *pV)
{
    int32_t lV;
    if (pV->GetType() != VAR_INTEGER)
    {
        Error("bad power argument");
        return false;
    }
    pV->Get(lV);
    return Power(lV);
}

bool DATA::Power(int32_t Deg)
{
    // if(bRef)
    if (Data_type == VAR_REFERENCE)
    {
        if (pReference)
        {
            return pReference->Power(Deg);
        }
        Error(UNINIT_REF);
        return false;
    }
    if (IsArray())
        return false;

    int32_t n;
    int32_t lBase;
    float fBase;

    if (IsArray())
        return false;
    switch (Data_type)
    {
    case VAR_INTEGER:
        if (Deg == 0)
        {
            lValue = 1;
            return true;
        }
        lBase = lValue;
        if (Deg > 0)
        {
            for (n = (Deg - 1); n > 0; --n)
                lValue = lValue * lBase;
        }
        else
        {
            Deg = -Deg;
            for (n = (Deg - 1); n > 0; --n)
                lValue = 1 / lBase;
        }
        break;
    case VAR_FLOAT:
        if (Deg == 0)
        {
            fValue = 1;
            return true;
        }
        fBase = fValue;
        if (Deg > 0)
        {
            for (n = (Deg - 1); n > 0; --n)
                fValue = fValue * fBase;
        }
        else
        {
            Deg = -Deg;
            for (n = (Deg - 1); n > 0; --n)
                fValue = 1.0f / fBase;
        }

        break;
    }
    return true;
}

bool DATA::Multiply(DATA *pV)
{
    // if(bRef)
    if (Data_type == VAR_REFERENCE)
    {
        if (pReference)
        {
            return pReference->Multiply(pV);
        }
        Error(UNINIT_REF);
        return false;
    }

    if (IsArray())
        return false;
    if (pV == nullptr)
        return false;
    pV = pV->GetVarPointer();
    if (pV == nullptr)
        return false;
    switch (Data_type)
    {
    case VAR_INTEGER:
        switch (pV->GetType())
        {
        case VAR_INTEGER:
            lValue = lValue * pV->lValue;
            break;
        case VAR_FLOAT:
            Convert(VAR_FLOAT);
            fValue = fValue * pV->fValue;
            break;
        default:
            return false;
        }
        break;
    case VAR_FLOAT:
        switch (pV->GetType())
        {
        case VAR_INTEGER:
            fValue = fValue * pV->lValue;
            break;
        case VAR_FLOAT:
            fValue = fValue * pV->fValue;
            break;
        default:
            return false;
        }
        break;
    default:
        return false;
    }
    return true;
}

bool DATA::Divide(DATA *pV)
{
    // if(bRef)
    if (Data_type == VAR_REFERENCE)
    {
        if (pReference)
        {
            return pReference->Divide(pV);
        }
        Error(UNINIT_REF);
        return false;
    }
    if (IsArray())
        return false;
    if (pV == nullptr)
        return false;
    pV = pV->GetVarPointer();
    if (pV == nullptr)
        return false;
    switch (Data_type)
    {
    case VAR_INTEGER:
        switch (pV->GetType())
        {
        case VAR_INTEGER:
            if (pV->lValue == 0)
            {
                Error("Divide by zero");
                return false;
            }
            lValue = lValue / pV->lValue;
            break;
        case VAR_FLOAT:
            if (pV->fValue == 0)
            {
                Error("Divide by zero");
                return false;
            }
            Convert(VAR_FLOAT);
            fValue = fValue / pV->fValue;
            break;
        default:
            return false;
        }
        break;
    case VAR_FLOAT:
        switch (pV->GetType())
        {
        case VAR_INTEGER:
            if (pV->lValue == 0)
            {
                Error("Divide by zero");
                return false;
            }
            fValue = fValue / pV->lValue;
            break;
        case VAR_FLOAT:
            if (pV->fValue == 0)
            {
                Error("Divide by zero");
                return false;
            }
            fValue = fValue / pV->fValue;
            break;
        default:
            return false;
        }
        break;
    default:
        return false;
    }
    return true;
}

bool DATA::Modul(DATA *pV)
{
    // if(bRef)
    if (Data_type == VAR_REFERENCE)
    {
        if (pReference)
        {
            return pReference->Modul(pV);
        }
        Error(UNINIT_REF);
        return false;
    }
    if (IsArray())
        return false;
    if (pV == nullptr)
        return false;
    pV = pV->GetVarPointer();
    if (pV == nullptr)
        return false;
    switch (Data_type)
    {
    case VAR_INTEGER:
        switch (pV->GetType())
        {
        case VAR_INTEGER:
            if (pV->lValue == 0)
            {
                Error("Divide by zero");
                return false;
            }
            lValue = lValue % pV->lValue;
            break;
        case VAR_FLOAT:
            if (pV->fValue == 0)
            {
                Error("Divide by zero");
                return false;
            }
            lValue = lValue % static_cast<int32_t>(floor(pV->fValue));
            break;
        default:
            return false;
        }
        break;
    case VAR_FLOAT:
        switch (pV->GetType())
        {
        case VAR_INTEGER:
            if (pV->lValue == 0)
            {
                Error("Divide by zero");
                return false;
            }
            fValue = fValue / pV->lValue;
            fValue = static_cast<float>(floor(fValue));
            break;
        case VAR_FLOAT:
            if (pV->fValue == 0)
            {
                Error("Divide by zero");
                return false;
            }
            fValue = fValue / pV->fValue;
            fValue = static_cast<float>(floor(fValue));
            break;
        default:
            return false;
        }
        break;
    default:
        return false;
    }
    return true;
}

bool DATA::Plus(DATA *pV)
{
    // if(bRef)
    if (Data_type == VAR_REFERENCE)
    {
        if (pReference)
        {
            return pReference->Plus(pV);
        }
        Error(UNINIT_REF);
        return false;
    }
    if (IsArray())
        return false;
    if (pV == nullptr)
        return false;
    pV = pV->GetVarPointer();
    if (pV == nullptr)
        return false;

    switch (Data_type)
    {
    case VAR_INTEGER:
        switch (pV->GetType())
        {
        case VAR_INTEGER:
            lValue = lValue + pV->lValue;
            break;
        case VAR_FLOAT:
            Convert(VAR_FLOAT);
            fValue = fValue + pV->fValue;
            break;
        case VAR_STRING:
            Convert(VAR_STRING);
            Set(sValue + pV->sValue);
            break;
        default:
            return false;
        }
        break;
    case VAR_FLOAT:
        switch (pV->GetType())
        {
        case VAR_INTEGER:
            fValue = fValue + pV->lValue;
            break;
        case VAR_FLOAT:
            fValue = fValue + pV->fValue;
            break;
        case VAR_STRING:
            Convert(VAR_STRING);
            Set(sValue + pV->sValue);
            break;
        default:
            return false;
        }
        break;
    case VAR_STRING:
        switch (pV->GetType())
        {
        case VAR_AREFERENCE:
            if (!pV->AttributesClass || !pV->AttributesClass->GetThisAttr())
                break;
            Set(sValue + to_string(pV->AttributesClass->GetThisAttr()));
            break;
        case VAR_INTEGER:
            Set(sValue + std::to_string(pV->lValue));
            break;
        case VAR_FLOAT:
            Set(sValue + fmt::format("{}", pV->fValue));
            break;
        case VAR_STRING:
            Set(sValue + pV->sValue);
            break;
        case VAR_PTR:
            Set(sValue + std::to_string(pV->pValue));
            break;
        default:
            return false;
        }
        break;
    default:
        return false;
    }
    return true;
}

bool DATA::Minus(DATA *pV)
{
    // if(bRef)
    if (Data_type == VAR_REFERENCE)
    {
        if (pReference)
        {
            return pReference->Minus(pV);
        }
        Error(UNINIT_REF);
        return false;
    }
    if (IsArray())
        return false;
    if (pV == nullptr)
        return false;
    pV = pV->GetVarPointer();
    if (pV == nullptr)
        return false;
    switch (Data_type)
    {
    case VAR_INTEGER:
        switch (pV->GetType())
        {
        case VAR_INTEGER:
            lValue = lValue - pV->lValue;
            break;
        case VAR_FLOAT:
            Convert(VAR_FLOAT);
            fValue = fValue - pV->fValue;
            break;
        default:
            return false;
        }
        break;
    case VAR_FLOAT:
        switch (pV->GetType())
        {
        case VAR_INTEGER:
            fValue = fValue - pV->lValue;
            break;
        case VAR_FLOAT:
            fValue = fValue - pV->fValue;
            break;
        default:
            return false;
        }
        break;
    default:
        return false;
    }
    return true;
}

bool DATA::Compare(DATA *pV, char opA, char opB)
{
    // if(bRef)
    if (Data_type == VAR_REFERENCE)
    {
        if (pReference)
        {
            return pReference->Compare(pV, opA, opB);
        }
        Error(UNINIT_REF);
        return false;
    }
    if (IsArray())
        return false;
    if (pV == nullptr)
        return false;
    pV = pV->GetVarPointer();
    if (pV == nullptr)
        return false;
    switch (Data_type)
    {
    case VAR_INTEGER:
        switch (pV->GetType())
        {
        case VAR_INTEGER:
            switch (opA)
            {
            case '=':
                if (lValue == pV->lValue)
                    return true;
                return false;
            case '!':
                if (lValue != pV->lValue)
                    return true;
                return false;
            case '>':
                if (opB == '=')
                {
                    if (lValue >= pV->lValue)
                        return true;
                }
                else
                {
                    if (lValue > pV->lValue)
                        return true;
                }
                return false;
            case '<':
                if (opB == '=')
                {
                    if (lValue <= pV->lValue)
                        return true;
                }
                else
                {
                    if (lValue < pV->lValue)
                        return true;
                }
                return false;
            }
            break;

        case VAR_FLOAT:
            switch (opA)
            {
            case '=':
                if (lValue == pV->fValue)
                    return true;
                return false;
            case '!':
                if (lValue != pV->fValue)
                    return true;
                return false;
            case '>':
                if (opB == '=')
                {
                    if (lValue >= pV->fValue)
                        return true;
                }
                else
                {
                    if (lValue > pV->fValue)
                        return true;
                }
                return false;
            case '<':
                if (opB == '=')
                {
                    if (lValue <= pV->fValue)
                        return true;
                }
                else
                {
                    if (lValue < pV->fValue)
                        return true;
                }
                return false;
            }
            break;
        default:
            return false;
        }
        break;
    case VAR_FLOAT:
        switch (pV->GetType())
        {
        case VAR_INTEGER:
            switch (opA)
            {
            case '=':
                if (fValue == pV->lValue)
                    return true;
                return false;
            case '!':
                if (fValue != pV->lValue)
                    return true;
                return false;
            case '>':
                if (opB == '=')
                {
                    if (fValue >= pV->lValue)
                        return true;
                }
                else
                {
                    if (fValue > pV->lValue)
                        return true;
                }
                return false;
            case '<':
                if (opB == '=')
                {
                    if (fValue <= pV->lValue)
                        return true;
                }
                else
                {
                    if (fValue < pV->lValue)
                        return true;
                }
                return false;
            }
            break;
        case VAR_FLOAT:
            switch (opA)
            {
            case '=':
                if (fValue == pV->fValue)
                    return true;
                return false;
            case '!':
                if (fValue != pV->fValue)
                    return true;
                return false;
            case '>':
                if (opB == '=')
                {
                    if (fValue >= pV->fValue)
                        return true;
                }
                else
                {
                    if (fValue > pV->fValue)
                        return true;
                }
                return false;
            case '<':
                if (opB == '=')
                {
                    if (fValue <= pV->fValue)
                        return true;
                }
                else
                {
                    if (fValue < pV->fValue)
                        return true;
                }
                return false;
            }
            break;
        default:
            return false;
        }
        break;
    case VAR_STRING:
        switch (pV->GetType())
        {
        case VAR_STRING:
            switch (opA)
            {
            case '=':
                return sValue == pV->sValue;

            case '!':
                return sValue!= pV->sValue;

            case '>':
                if (opB == '=')
                {
                    return std::size(sValue) >= std::size(pV->sValue);
                }
                return std::size(sValue) > std::size(pV->sValue);

            case '<':
                if (opB == '=')
                {
                    return std::size(sValue) <= std::size(pV->sValue);
                }

                return std::size(sValue) < std::size(pV->sValue);
            }
            break;
        default:
            return false;
        }
        break;
    default:
        return false;
    }
    return true;
}

// copy data from pV variable to this variable
// arrays must be the same type and size
bool DATA::Copy(DATA *pV)
{
    // if(bRef)
    if (Data_type == VAR_REFERENCE)
    {
        if (pReference)
        {
            return pReference->Copy(pV);
        }
        if (pV != nullptr)
        {
            if (pV->GetType() == VAR_REFERENCE)
            {
                SetReference(pV->GetVarPointer());
                return true;
            }
            if (pV->GetType() == VAR_OBJECT)
            {
                SetReference(pV->GetVarPointer());
                return true;
            }
        }
        Error(UNINIT_REF);
        return false;
    }
    //    DWORD n;
    //    char * * ppChar;
    //    char * * ppCharpV;

    if (pV == nullptr)
    {
        Error("Invalid DATA in copy operation");
        return false;
    }
    if (pV->IsReference())
    {
        if (Data_type == VAR_REFERENCE)
        {
            SetReference(pV->GetReference());
            return true;
        }
        if (Data_type == UNKNOWN) // stack
        {
            SetType(VAR_REFERENCE);
            SetReference(pV->GetReference());
            return true;
        }
        pV = pV->GetReference();
        if (pV == nullptr)
        {
            Error("Invalid reference v in copy operation");
            return false;
        }
    }

    if (IsArray() && !pV->IsArray())
    {
        Error("Incorrect copy from single to array");
        return false;
    }
    if (!IsArray() && pV->IsArray())
    {
        Error("Incorrect copy from array to single");
        return false;
    }

    if (IsArray())
    {
        if (Data_type != pV->Data_type)
        {
            Error("Can't copy two arrays with different data type");
            return false;
        }
        if (Number_of_elements != pV->Number_of_elements)
        {
            Error("Can't copy two arrays with different size");
            return false;
        }

        ArrayPTR = pV->ArrayPTR;
        return true;
    }

    switch (pV->Data_type)
    {
    case VAR_INTEGER:
        Set(pV->lValue);
        break;
    case VAR_PTR:
        SetPtr(pV->pValue);
        break;
    case VAR_FLOAT:
        Set(pV->fValue);
        break;
    case VAR_STRING:
        Set(pV->sValue);
        break;
    case VAR_OBJECT:
        Set(pV->object_id);
        // Attributes.Copy(&pV->Attributes);

        if (Data_type == VAR_REFERENCE)
        {
            auto *pVV = GetVarPointer();
            if (pVV == nullptr)
            {
                Error(UNINIT_REF);
                return false;
            }
            if (pVV->AttributesClass == nullptr)
            {
                if (pV->AttributesClass != nullptr)
                {
                    Assert(&pV->AttributesClass->GetStringCodec() == pVCompiler->GetVSC());
                    pVV->AttributesClass = new ATTRIBUTES(pV->AttributesClass->Copy());
                }
                else
                {
                    pVV->AttributesClass = new ATTRIBUTES(*pVCompiler->GetVSC());
                }
            }
            else if (pV->AttributesClass != nullptr) {
                Assert(&pV->AttributesClass->GetStringCodec() == pVCompiler->GetVSC());
                *pVV->AttributesClass = pV->AttributesClass->Copy();
            }

        }
        else
        {
            if (Data_type == VAR_AREFERENCE)
            {
                AttributesClass = pV->AttributesClass;
            }
            else
            {
                if (AttributesClass == nullptr) {
                    if (pV->AttributesClass != nullptr) {
                        Assert(&pV->AttributesClass->GetStringCodec() == pVCompiler->GetVSC());
                        AttributesClass = new ATTRIBUTES(pV->AttributesClass->Copy());
                    }
                    else {
                        AttributesClass = new ATTRIBUTES(*pVCompiler->GetVSC());
                    }
                }
                else if (pV->AttributesClass != nullptr) {
                    Assert(&pV->AttributesClass->GetStringCodec() == pVCompiler->GetVSC());
                    *AttributesClass = pV->AttributesClass->Copy();
                }
            }
        }
        break;
    case VAR_AREFERENCE:
        if (Data_type == UNKNOWN)
        {
            SetType(VAR_AREFERENCE);
            AttributesClass = pV->AttributesClass;
            object_id = pV->object_id;
            break;
        }
        if (Data_type != VAR_AREFERENCE)
        {
            Error("invalid aref usage");
            return false;
        }
        AttributesClass = pV->AttributesClass;
        object_id = pV->object_id;
        break;

    case VAR_REFERENCE:
        if (Data_type == UNKNOWN)
        {
            SetType(VAR_REFERENCE);
            SetReference(pV->GetVarPointer());
            break;
        }
        if (Data_type != VAR_REFERENCE)
        {
            Error("invalid ref usage");
            return false;
        }
        break;

    default:
        Error("Unknown data type");
        return false;
    }
    return true;
}

// copy data from pV array on index to this single variable
bool DATA::Copy(DATA *pV, uint32_t index)
{
    // if(bRef)
    if (Data_type == VAR_REFERENCE)
    {
        if (pReference)
        {
            return pReference->Copy(pV, index);
        }
        Error(UNINIT_REF);
        return false;
    }

    //    char * * ppChar;
    //    int32_t *   pLong;
    //    float *  pFloat;
    //    OBJECT_DESC * pOD;

    if (pV == nullptr)
    {
        Error("Invalid DATA in copy operation");
        return false;
    }
    if (pV->IsReference())
    {
        pV = pV->GetReference();
        if (pV == nullptr)
        {
            Error("Invalid reference v in copy operation");
            return false;
        }
    }
    if (IsArray())
    {
        Error("Incorrect copy from single element to array");
        return false;
    }

    if (!pV->IsArray())
    {
        Error("Incorrect copy: using index on single variable");
        return false;
    }

    if (index >= pV->GetElementsNum())
    {
        Error("Incorrect copy: invalid index");
        return false;
    }

    /*    if(Data_type == VAR_STRING)
      {
        if(sValue) delete sValue; sValue = 0;
      }
    */
    return Copy(&pV->ArrayPTR[index]);

    /*switch(pV->GetType())
    {
      case VAR_INTEGER:
        pLong = (int32_t *)pV->ArrayPointer;
        Set(pLong[index]);
      break;
      case VAR_FLOAT:
        pFloat = (float *)pV->ArrayPointer;
        Set(pFloat[index]);
      break;
      case VAR_STRING:
        ppChar = (char **)pV->ArrayPointer;
        Set(ppChar[index]);
      break;
      case VAR_OBJECT:
        pOD = (OBJECT_DESC *)pV->ArrayPointer;
        Set(pOD[index].object_id);
        //Attributes.Copy(pOD[index].pAttributes);
        if(AttributesClass == 0) AttributesClass = new ATTRIBUTES;
        AttributesClass->Copy(pOD[index].pAttributes);
      break;
    }*/

    // return true;
}

bool DATA::CopyOnElement(DATA *pV, uint32_t index)
{
    // if(bRef)
    if (Data_type == VAR_REFERENCE)
    {
        if (pReference)
        {
            return pReference->CopyOnElement(pV, index);
        }
        Error(UNINIT_REF);
        return false;
    }

    if (pV == nullptr)
    {
        Error("Invalid DATA in copy operation");
        return false;
    }

    if (pV->IsReference())
    {
        pV = pV->GetReference();
        if (pV == nullptr)
        {
            Error("Invalid reference v in copy operation");
            return false;
        }
    }

    if (Data_type != pV->GetType())
    {
        Error("Incorrect copy: different types");
        return false;
    }

    if (!IsArray())
    {
        Error("Incorrect copy: using index on single variable");
        return false;
    }

    if (pV->IsArray())
    {
        Error("Incorrect copy: using array as single");
        return false;
    }

    if (index >= GetElementsNum())
    {
        Error("Incorrect copy: invalid index");
        return false;
    }
    return ArrayPTR[index].Copy(pV);
    /*
      switch(Data_type)
      {
        case VAR_INTEGER:
          Set(pV->lValue,index);
        break;
        case VAR_FLOAT:
          Set(pV->fValue,index);
        break;
        case VAR_STRING:
          Set(pV->sValue,index);
        break;
        case VAR_OBJECT:
          OBJECT_DESC * pOD;
          pOD = (OBJECT_DESC *)ArrayPointer;
          Set(pV->object_id,index);
          if(pV->AttributesClass)
          {
            if(pOD[index].pAttributes == null ) pOD[index].pAttributes = new ATTRIBUTES;
            //pOD[index].pAttributes->Copy(&pV->Attributes);
            pOD[index].pAttributes->Copy(pV->AttributesClass);
          }

        break;
      }

      return true;
    */
}

ATTRIBUTES *DATA::GetAClass()
{
    // if(bRef)
    if (Data_type == VAR_REFERENCE)
    {
        if (pReference)
        {
            return pReference->GetAClass();
        }
        Error(UNINIT_REF);
        return nullptr;
    }
    if (AttributesClass == nullptr)
    {
        if (Data_type == VAR_AREFERENCE)
        {
            Error("uninitialized aref");
            // throw std::runtime_error("uninitialized aref");
            return nullptr;
        }

        AttributesClass = new ATTRIBUTES(pVCompiler->GetVSC());
    }
    return AttributesClass;
}

ATTRIBUTES *DATA::GetAClass(uint32_t index)
{
    // if(bRef)
    if (Data_type == VAR_REFERENCE)
    {
        if (pReference)
        {
            return pReference->GetAClass(index);
        }
        Error(UNINIT_REF);
        return nullptr;
    }
    if (!IsArray())
    {
        Error("Incorrect copy: using index on single variable");
        return nullptr;
    }
    if (index >= GetElementsNum())
    {
        Error("Incorrect copy: invalid index");
        return nullptr;
    }

    return ArrayPTR[index].GetAClass();
    /*
      OBJECT_DESC * pOD;
      pOD = (OBJECT_DESC *)ArrayPointer;
      if(pOD[index].pAttributes == null ) pOD[index].pAttributes = new ATTRIBUTES;
      return pOD[index].pAttributes;*/
}

DATA *DATA::GetReference()
{
    return pReference;
}

DATA *DATA::GetArrayElement(uint32_t index)
{
    if (!bArray)
    {
        Error(INDEX_ON_SINGLE);
        return nullptr;
    }
    if (index >= Number_of_elements)
    {
        BadIndex(index, Number_of_elements);
        // Error(INCORRECT_INDEX    );
        return nullptr;
    }
    return &ArrayPTR[index];
}

void DATA::SetVCompiler(VIRTUAL_COMPILER *pVC)
{
    pVCompiler = pVC;
    if (bArray)
    {
        for (uint32_t n = 0; n < Number_of_elements; n++)
        {
            ArrayPTR[n].SetVCompiler(pVC);
        }
    }
}

DATA *DATA::GetVarPointer()
{
    // if(!bRef) return this;
    if (Data_type != VAR_REFERENCE)
        return this;
    if (pReference)
    {
        return pReference->GetVarPointer();
    }
    Error(UNINIT_REF);
    return nullptr;
}

bool DATA::CompareAndSetResult(DATA *pV, S_TOKEN_TYPE op)
{
    if (Data_type == VAR_REFERENCE)
    {
        if (pReference)
        {
            return pReference->CompareAndSetResult(pV, op);
        }
        Error(UNINIT_REF);
        return false;
    }
    if (IsArray())
        return false;
    if (pV == nullptr)
        return false;
    pV = pV->GetVarPointer();
    if (pV == nullptr)
    {
        Set(0);
        return false;
    }

    if (Data_type == VAR_STRING && pV->GetType() == VAR_INTEGER)
    {
        if (!Convert(VAR_INTEGER))
        {
            Set(0);
            return false;
        }
    }

    if (Data_type == VAR_STRING && pV->GetType() == VAR_FLOAT)
    {
        if (!Convert(VAR_FLOAT))
        {
            Set(0);
            return false;
        }
    }

    switch (Data_type)
    {
    case VAR_PTR:
        Convert(VAR_INTEGER);
    case VAR_INTEGER:
        switch (pV->GetType())
        {
        case VAR_INTEGER:
            switch (op)
            {
            case VAR_PTR:
                pV->Convert(VAR_INTEGER);
            case OP_BOOL_EQUAL:
                if (lValue == pV->lValue)
                    Set(1);
                else
                    Set(0);
                break;
            case OP_GREATER:
                if (lValue > pV->lValue)
                    Set(1);
                else
                    Set(0);
                break;
            case OP_GREATER_OR_EQUAL:
                if (lValue >= pV->lValue)
                    Set(1);
                else
                    Set(0);
                break;
            case OP_LESSER:
                if (lValue < pV->lValue)
                    Set(1);
                else
                    Set(0);
                break;
            case OP_LESSER_OR_EQUAL:
                if (lValue <= pV->lValue)
                    Set(1);
                else
                    Set(0);
                break;
            case OP_NOT_EQUAL:
                if (lValue != pV->lValue)
                    Set(1);
                else
                    Set(0);
                break;
            case OP_BOOL_AND:
                if (lValue != 0 && pV->lValue != 0)
                    Set(1);
                else
                    Set(0);
                break;
            case OP_BOOL_OR:
                if (lValue != 0 || pV->lValue != 0)
                    Set(1);
                else
                    Set(0);
                break;
            }
            break;

        case VAR_FLOAT:
            switch (op)
            {
            case OP_BOOL_EQUAL:
                if (lValue == pV->fValue)
                    Set(1);
                else
                    Set(0);
                break;
            case OP_GREATER:
                if (lValue > pV->fValue)
                    Set(1);
                else
                    Set(0);
                break;
            case OP_GREATER_OR_EQUAL:
                if (lValue >= pV->fValue)
                    Set(1);
                else
                    Set(0);
                break;
            case OP_LESSER:
                if (lValue < pV->fValue)
                    Set(1);
                else
                    Set(0);
                break;
            case OP_LESSER_OR_EQUAL:
                if (lValue <= pV->fValue)
                    Set(1);
                else
                    Set(0);
                break;
            case OP_NOT_EQUAL:
                if (lValue != pV->fValue)
                    Set(1);
                else
                    Set(0);
                break;
            case OP_BOOL_AND:
                if (lValue != 0 && pV->fValue != 0)
                    Set(1);
                else
                    Set(0);
                break;
            case OP_BOOL_OR:
                if (lValue != 0 || pV->fValue != 0)
                    Set(1);
                else
                    Set(0);
                break;
            }
            break;
        default:
            Set(0);
            return false;
        }
        break;
    case VAR_FLOAT:
        switch (pV->GetType())
        {
        case VAR_INTEGER:
            switch (op)
            {
            case OP_BOOL_EQUAL:
                if (fValue == pV->lValue)
                    Set(1);
                else
                    Set(0);
                break;
            case OP_GREATER:
                if (fValue > pV->lValue)
                    Set(1);
                else
                    Set(0);
                break;
            case OP_GREATER_OR_EQUAL:
                if (fValue >= pV->lValue)
                    Set(1);
                else
                    Set(0);
                break;
            case OP_LESSER:
                if (fValue < pV->lValue)
                    Set(1);
                else
                    Set(0);
                break;
            case OP_LESSER_OR_EQUAL:
                if (fValue <= pV->lValue)
                    Set(1);
                else
                    Set(0);
                break;
            case OP_NOT_EQUAL:
                if (fValue != pV->lValue)
                    Set(1);
                else
                    Set(0);
                break;
            case OP_BOOL_AND:
                if (fValue != 0 && pV->lValue != 0)
                    Set(1);
                else
                    Set(0);
                break;
            case OP_BOOL_OR:
                if (fValue != 0 || pV->lValue != 0)
                    Set(1);
                else
                    Set(0);
                break;
            }
            break;
        case VAR_FLOAT:
            switch (op)
            {
            case OP_BOOL_EQUAL:
                if (fValue == pV->fValue)
                    Set(1);
                else
                    Set(0);
                break;
            case OP_GREATER:
                if (fValue > pV->fValue)
                    Set(1);
                else
                    Set(0);
                break;
            case OP_GREATER_OR_EQUAL:
                if (fValue >= pV->fValue)
                    Set(1);
                else
                    Set(0);
                break;
            case OP_LESSER:
                if (fValue < pV->fValue)
                    Set(1);
                else
                    Set(0);
                break;
            case OP_LESSER_OR_EQUAL:
                if (fValue <= pV->fValue)
                    Set(1);
                else
                    Set(0);
                break;
            case OP_NOT_EQUAL:
                if (fValue != pV->fValue)
                    Set(1);
                else
                    Set(0);
                break;
            case OP_BOOL_AND:
                if (fValue != 0 && pV->fValue != 0)
                    Set(1);
                else
                    Set(0);
                break;
            case OP_BOOL_OR:
                if (fValue != 0 || pV->fValue != 0)
                    Set(1);
                else
                    Set(0);
                break;
            }
            break;
        default:
            Set(0);
            return false;
        }
        break;
    case VAR_STRING:
        switch (pV->GetType())
        {
        case VAR_STRING:
            switch (op)
            {
            case OP_BOOL_EQUAL:
                Set(static_cast<int32_t>(storm::iEquals(sValue, pV->sValue)));
                break;
            case OP_GREATER:
                Set(static_cast<int32_t>(storm::iGreater(sValue, pV->sValue)));
                break;
            case OP_GREATER_OR_EQUAL:
                Set(static_cast<int32_t>(storm::iGreaterOrEqual(sValue, pV->sValue)));
                break;
            case OP_LESSER:
                Set(static_cast<int32_t>(storm::iLess(sValue, pV->sValue)));
                break;
            case OP_LESSER_OR_EQUAL:
                Set(static_cast<int32_t>(storm::iLessOrEqual(sValue, pV->sValue)));
                break;
            case OP_NOT_EQUAL:
                Set(static_cast<int32_t>(!storm::iEquals(sValue, pV->sValue)));
                break;
            case OP_BOOL_AND:
            case OP_BOOL_OR:
                Error("bool operation on string");
                return false;
            }

            break;
        default:
            Set(0);
            return false;
        }
        break;
    default:
        return false;
    }
    // return true;
    return false;
}

bool DATA::BoolConvert()
{
    if (Data_type == VAR_REFERENCE)
    {
        if (pReference)
        {
            return pReference->BoolConvert();
        }
        Error(UNINIT_REF);
        return false;
    }
    if (IsArray())
        return false;
    switch (Data_type)
    {
    case VAR_INTEGER:
        if (lValue != 0)
            Set(1);
        else
            Set(0);
        break;
    case VAR_FLOAT:
        if (fValue != 0)
            Set(1);
        else
            Set(0);
        break;
    case VAR_STRING:
        Set(static_cast<int32_t>(!sValue.empty()));
        break;
    case VAR_PTR:
        if (pValue != 0)
            Set(1);
        else
            Set(0);
        break;
    default:

        return false;
    }
    return true;
}

bool DATA::RefConvert()
{
    if (Data_type != VAR_REFERENCE)
        return false;
    if (IsArray())
        return false;
    if (!pReference)
    {
        Error(UNINIT_REF);
        return false;
    }
    auto *const pV = pReference->GetVarPointer();
    if (!pV)
    {
        Error(UNINIT_REF);
        return false;
    }
    ClearType();
    Copy(pV);
    return true;
}

void DATA::BadIndex(uint32_t index, uint32_t array_size)
{
    char buffer[MAX_PATH];
    sprintf_s(buffer, "invalid index %d [size:%d]", index, array_size);
    Error(buffer);
}

int32_t DATA::GetInt()
{
    return lValue;
}

uintptr_t DATA::GetPtr()
{
    return pValue;
}

float DATA::GetFloat()
{
    return fValue;
}

const char *DATA::GetString()
{
    return sValue.c_str();
}

entid_t DATA::GetEntityID()
{
    return object_id;
}
