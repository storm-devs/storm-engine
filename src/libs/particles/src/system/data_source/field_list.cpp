#include "field_list.h"

#include "data_bool.h"
#include "data_color.h"
#include "data_float.h"
#include "data_graph.h"
#include "data_position.h"
#include "data_string.h"
#include "data_uv.h"
#include "defines.h"
#include "storm/string_compare.hpp"

FieldList::FieldList()
{
}

FieldList::~FieldList()
{
    // DelAll ();
}

void FieldList::Load(MemFile *File)
{
    uint32_t DataFieldsCount = 0;
    File->ReadType(DataFieldsCount);

    for (uint32_t n = 0; n < DataFieldsCount; n++)
    {
        auto fldType = FIELD_UNKNOWN;
        File->ReadType(fldType);

        switch (fldType)
        {
        case FIELD_BOOL: {
            // core.Trace ("Particles info: BOOL field");
            CreateBoolField(File);
            break;
        }
        case FIELD_FLOAT: {
            // core.Trace ("Particles info: FLOAT field");
            CreateFloatField(File);
            break;
        }
        case FIELD_GRAPH: {
            // core.Trace ("Particles info: GRAPH field");
            CreateGraphField(File);
            break;
        }
        case FIELD_POSITION: {
            // core.Trace ("Particles info: POSITION field");
            CreatePositionField(File);
            break;
        }
        case FIELD_STRING: {
            // core.Trace ("Particles info: STRING field");
            CreateStringField(File);
            break;
        }
        case FIELD_UV: {
            // core.Trace ("Particles info: UV field");
            CreateUVField(File);
            break;
        }
        case FIELD_COLOR: {
            // core.Trace ("Particles info: COLOR field");
            CreateColorField(File);
            break;
        }
        default: {
            throw std::runtime_error("Particles: Unknown field type !!!!");
        }
        } // switch
    }     // for all fileds
}

void FieldList::CreateEmptyBoolField(const char *Name, bool def_value)
{
    auto *Field = new DataBool;
    Field->SetName(Name);
    Field->SetValue(def_value);

    FieldDesc pDesc;
    pDesc.MarkForDelete = false;
    pDesc.Name = Field->GetName();
    pDesc.HashValue = TOREMOVE::HashNoCase(pDesc.Name.c_str());
    pDesc.pPointer = Field;
    pDesc.Type = FIELD_BOOL;
    Fields.push_back(pDesc);
}

void FieldList::CreateEmptyFloatField(const char *Name, float def_value)
{
    auto *Field = new DataFloat;
    Field->SetName(Name);
    Field->SetValue(def_value);

    FieldDesc pDesc;
    pDesc.MarkForDelete = false;
    pDesc.Name = Field->GetName();
    pDesc.HashValue = TOREMOVE::HashNoCase(pDesc.Name.c_str());
    pDesc.pPointer = Field;
    pDesc.Type = FIELD_FLOAT;
    Fields.push_back(pDesc);
}

void FieldList::CreateEmptyGraphField(const char *Name, float def_value_min, float def_value_max)
{
    auto *Field = new DataGraph;
    Field->SetName(Name);
    Field->SetDefaultValue(def_value_max, def_value_min);

    FieldDesc pDesc;
    pDesc.MarkForDelete = false;
    pDesc.Name = Field->GetName();
    pDesc.HashValue = TOREMOVE::HashNoCase(pDesc.Name.c_str());
    pDesc.pPointer = Field;
    pDesc.Type = FIELD_GRAPH;
    Fields.push_back(pDesc);
}

void FieldList::CreateEmptyPositionField(const char *Name, const Vector &def_value)
{
    auto *Field = new DataPosition;
    Field->SetName(Name);
    Field->SetValue(def_value);

    FieldDesc pDesc;
    pDesc.MarkForDelete = false;
    pDesc.Name = Field->GetName();
    pDesc.HashValue = TOREMOVE::HashNoCase(pDesc.Name.c_str());
    pDesc.pPointer = Field;
    pDesc.Type = FIELD_POSITION;
    Fields.push_back(pDesc);
}

void FieldList::CreateEmptyStringField(const char *Name, const char *def_value)
{
    auto *Field = new DataString;
    Field->SetName(Name);
    Field->SetValue(def_value);

    FieldDesc pDesc;
    pDesc.MarkForDelete = false;
    pDesc.Name = Field->GetName();
    pDesc.HashValue = TOREMOVE::HashNoCase(pDesc.Name.c_str());
    pDesc.pPointer = Field;
    pDesc.Type = FIELD_STRING;
    Fields.push_back(pDesc);
}

void FieldList::CreateEmptyUVField(const char *Name)
{
    auto *Field = new DataUV;
    Field->SetName(Name);
    const auto vec = Vector4(0.0f, 0.0f, 1.0f, 1.0f);
    Field->SetValues(&vec, 1);

    FieldDesc pDesc;
    pDesc.MarkForDelete = false;
    pDesc.Name = Field->GetName();
    pDesc.HashValue = TOREMOVE::HashNoCase(pDesc.Name.c_str());
    pDesc.pPointer = Field;
    pDesc.Type = FIELD_UV;
    Fields.push_back(pDesc);
}

void FieldList::CreateEmptyColorField(const char *Name, uint32_t def_value)
{
    auto *Field = new DataColor;
    Field->SetName(Name);
    ColorVertex defVal[2];
    defVal[0].MinValue = def_value;
    defVal[0].MaxValue = def_value;
    defVal[1].MinValue = def_value;
    defVal[1].MaxValue = def_value;
    Field->SetValues(defVal, 2);

    FieldDesc pDesc;
    pDesc.MarkForDelete = false;
    pDesc.Name = Field->GetName();
    pDesc.HashValue = TOREMOVE::HashNoCase(pDesc.Name.c_str());
    pDesc.pPointer = Field;
    pDesc.Type = FIELD_COLOR;
    Fields.push_back(pDesc);
}

void FieldList::CreateBoolField(MemFile *pMemFile)
{
    auto *Field = new DataBool;
    Field->Load(pMemFile);

    FieldDesc pDesc;
    pDesc.MarkForDelete = false;
    pDesc.Name = Field->GetName();
    pDesc.HashValue = TOREMOVE::HashNoCase(pDesc.Name.c_str());
    pDesc.pPointer = Field;
    pDesc.Type = FIELD_BOOL;
    Fields.push_back(pDesc);
}

void FieldList::CreateFloatField(MemFile *pMemFile)
{
    auto *Field = new DataFloat;
    Field->Load(pMemFile);

    FieldDesc pDesc;
    pDesc.MarkForDelete = false;
    pDesc.Name = Field->GetName();
    pDesc.HashValue = TOREMOVE::HashNoCase(pDesc.Name.c_str());
    pDesc.pPointer = Field;
    pDesc.Type = FIELD_FLOAT;
    Fields.push_back(pDesc);
}

void FieldList::CreateGraphField(MemFile *pMemFile)
{
    auto *Field = new DataGraph;
    Field->Load(pMemFile);

    FieldDesc pDesc;
    pDesc.MarkForDelete = false;
    pDesc.Name = Field->GetName();
    pDesc.HashValue = TOREMOVE::HashNoCase(pDesc.Name.c_str());
    pDesc.pPointer = Field;
    pDesc.Type = FIELD_GRAPH;
    Fields.push_back(pDesc);
}

void FieldList::CreatePositionField(MemFile *pMemFile)
{
    auto *Field = new DataPosition;
    Field->Load(pMemFile);

    FieldDesc pDesc;
    pDesc.MarkForDelete = false;
    pDesc.Name = Field->GetName();
    pDesc.HashValue = TOREMOVE::HashNoCase(pDesc.Name.c_str());
    pDesc.pPointer = Field;
    pDesc.Type = FIELD_POSITION;
    Fields.push_back(pDesc);
}

void FieldList::CreateStringField(MemFile *pMemFile)
{
    auto *Field = new DataString;
    Field->Load(pMemFile);

    FieldDesc pDesc;
    pDesc.MarkForDelete = false;
    pDesc.Name = Field->GetName();
    pDesc.HashValue = TOREMOVE::HashNoCase(pDesc.Name.c_str());
    pDesc.pPointer = Field;
    pDesc.Type = FIELD_STRING;
    Fields.push_back(pDesc);
}

void FieldList::CreateUVField(MemFile *pMemFile)
{
    auto *Field = new DataUV;
    Field->Load(pMemFile);

    FieldDesc pDesc;
    pDesc.MarkForDelete = false;
    pDesc.Name = Field->GetName();
    pDesc.HashValue = TOREMOVE::HashNoCase(pDesc.Name.c_str());
    pDesc.pPointer = Field;
    pDesc.Type = FIELD_UV;
    Fields.push_back(pDesc);
}

void FieldList::CreateColorField(MemFile *pMemFile)
{
    auto *Field = new DataColor;
    Field->Load(pMemFile);

    FieldDesc pDesc;
    pDesc.MarkForDelete = false;
    pDesc.Name = Field->GetName();
    pDesc.HashValue = TOREMOVE::HashNoCase(pDesc.Name.c_str());
    pDesc.pPointer = Field;
    pDesc.Type = FIELD_COLOR;
    Fields.push_back(pDesc);
}

void FieldList::DelAll()
{
    for (uint32_t n = 0; n < Fields.size(); n++)
    {
        DeleteFieldData(Fields[n]);
    } // loop

    Fields.clear();
}

DataColor *FieldList::FindColor(const char *AttrName)
{
    const uint32_t SearchHash = TOREMOVE::HashNoCase(AttrName);
    for (uint32_t n = 0; n < Fields.size(); n++)
    {
        if (Fields[n].Type == FIELD_COLOR)
        {
            if (SearchHash == Fields[n].HashValue)
            {
                if (storm::iEquals(Fields[n].Name, AttrName))
                {
                    return static_cast<DataColor *>(Fields[n].pPointer);
                }
            }
        }
    }

    return nullptr;
}

DataBool *FieldList::FindBool(const char *AttrName)
{
    const uint32_t SearchHash = TOREMOVE::HashNoCase(AttrName);
    for (uint32_t n = 0; n < Fields.size(); n++)
    {
        if (Fields[n].Type == FIELD_BOOL)
        {
            if (SearchHash == Fields[n].HashValue)
            {
                if (storm::iEquals(Fields[n].Name, AttrName))
                {
                    return static_cast<DataBool *>(Fields[n].pPointer);
                }
            }
        }
    }

    return nullptr;
}

DataFloat *FieldList::FindFloat(const char *AttrName)
{
    const uint32_t SearchHash = TOREMOVE::HashNoCase(AttrName);
    for (uint32_t n = 0; n < Fields.size(); n++)
    {
        if (Fields[n].Type == FIELD_FLOAT)
        {
            if (SearchHash == Fields[n].HashValue)
            {
                if (storm::iEquals(Fields[n].Name, AttrName))
                {
                    return static_cast<DataFloat *>(Fields[n].pPointer);
                }
            }
        }
    }

    return nullptr;
}

DataGraph *FieldList::FindGraph(const char *AttrName)
{
    const uint32_t SearchHash = TOREMOVE::HashNoCase(AttrName);

    for (uint32_t n = 0; n < Fields.size(); n++)
    {
        if (Fields[n].Type == FIELD_GRAPH)
        {
            if (SearchHash == Fields[n].HashValue)
            {
                if (storm::iEquals(Fields[n].Name, AttrName))
                {
                    return static_cast<DataGraph *>(Fields[n].pPointer);
                }
            }
        }
    }

    return nullptr;
}

DataString *FieldList::FindString(const char *AttrName)
{
    const uint32_t SearchHash = TOREMOVE::HashNoCase(AttrName);

    for (uint32_t n = 0; n < Fields.size(); n++)
    {
        if (Fields[n].Type == FIELD_STRING)
        {
            if (SearchHash == Fields[n].HashValue)
            {
                if (storm::iEquals(Fields[n].Name, AttrName))
                {
                    return static_cast<DataString *>(Fields[n].pPointer);
                }
            }
        }
    }

    return nullptr;
}

DataPosition *FieldList::FindPosition(const char *AttrName)
{
    const uint32_t SearchHash = TOREMOVE::HashNoCase(AttrName);

    for (uint32_t n = 0; n < Fields.size(); n++)
    {
        if (Fields[n].Type == FIELD_POSITION)
        {
            if (SearchHash == Fields[n].HashValue)
            {
                if (storm::iEquals(Fields[n].Name, AttrName))
                {
                    return static_cast<DataPosition *>(Fields[n].pPointer);
                }
            }
        }
    }

    return nullptr;
}

DataUV *FieldList::FindUV(const char *AttrName)
{
    const uint32_t SearchHash = TOREMOVE::HashNoCase(AttrName);

    for (uint32_t n = 0; n < Fields.size(); n++)
    {
        if (Fields[n].Type == FIELD_UV)
        {
            if (SearchHash == Fields[n].HashValue)
            {
                if (storm::iEquals(Fields[n].Name, AttrName))
                {
                    return static_cast<DataUV *>(Fields[n].pPointer);
                }
            }
        }
    }
    return nullptr;
}

FieldList::FieldDesc *FieldList::FindField(const char *Name)
{
    const uint32_t SearchHash = TOREMOVE::HashNoCase(Name);

    for (uint32_t n = 0; n < Fields.size(); n++)
    {
        if (SearchHash == Fields[n].HashValue)
        {
            if (storm::iEquals(Fields[n].Name, Name))
            {
                return &Fields[n];
            }
        }
    }
    return nullptr;
}

float FieldList::GetFloat(const char *AttrName, float def_value)
{
    auto *const pFind = FindFloat(AttrName);
    if (!pFind)
        return def_value;
    return pFind->GetValue();
}

int FieldList::GetFloatAsInt(const char *AttrName, int def_value)
{
    const auto val = GetFloat(AttrName, static_cast<float>(def_value));
    return static_cast<int>(val);
}

bool FieldList::GetBool(const char *AttrName, bool def_value)
{
    auto *const pFind = FindBool(AttrName);
    if (!pFind)
        return def_value;
    return pFind->GetValue();
}

const char *FieldList::GetString(const char *AttrName, const char *def_value)
{
    auto *const pFind = FindString(AttrName);
    if (!pFind)
        return def_value;
    return pFind->GetValue();
}

const Vector &FieldList::GetPosition(const char *AttrName, const Vector &def_value)
{
    auto *const pFind = FindPosition(AttrName);
    if (!pFind)
        return def_value;
    return pFind->GetValue();
}

float FieldList::GetGraphVal(const char *AttrName, float Time, float LifeTime, float K_Rand, float def_value)
{
    auto *pFind = FindGraph(AttrName);
    if (!pFind)
        return def_value;
    return pFind->GetValue(Time, LifeTime, K_Rand);
}

float FieldList::GetRandomGraphVal(const char *AttrName, float Time, float LifeTime, float def_value)
{
    auto *pFind = FindGraph(AttrName);
    if (!pFind)
        return def_value;
    return pFind->GetRandomValue(Time, LifeTime);
}

uint32_t FieldList::GetFieldCount() const
{
    return Fields.size();
}

const FieldList::FieldDesc &FieldList::GetFieldByIndex(uint32_t Index)
{
    return Fields[Index];
}

void FieldList::Convert(DataDescripion *pDataDescriptor)
{
    for (uint32_t i = 0; i < Fields.size(); i++)
    {
        Fields[i].MarkForDelete = true;
    }

    const uint32_t NeedFieldsCount = pDataDescriptor->GetFieldCount();

    for (uint32_t n = 0; n < NeedFieldsCount; n++)
    {
        const auto *const NeedFieldName = pDataDescriptor->GetFieldName(n);
        const auto NeedFieldType = pDataDescriptor->GetFieldType(n);
        auto *const pDesc = FindField(NeedFieldName);
        auto FieldExist = false;
        if (pDesc)
        {
            if (pDesc->Type == NeedFieldType)
            {
                // If found a field and it is of the required type, remove the deletion mark
                pDesc->MarkForDelete = false;
                FieldExist = true;
            }
        }

        // If the field does not exist, need to create it ...
        if (!FieldExist)
        {
            switch (NeedFieldType)
            {
            case FIELD_COLOR:
                CreateEmptyColorField(NeedFieldName, 0xFFFFA030);
                break;
            case FIELD_BOOL:
                CreateEmptyBoolField(NeedFieldName, false);
                break;
            case FIELD_FLOAT:
                CreateEmptyFloatField(NeedFieldName, 0.0f);
                break;
            case FIELD_GRAPH:
                CreateEmptyGraphField(NeedFieldName, 0.0f, 0.0f);
                break;
            case FIELD_POSITION:
                CreateEmptyPositionField(NeedFieldName, Vector(0.0f));
                break;
            case FIELD_UV:
                CreateEmptyUVField(NeedFieldName);
                break;
            case FIELD_STRING:
                CreateEmptyStringField(NeedFieldName, "###");
                break;
            default:
                throw std::runtime_error("Unknown Field type !!!");
            }
        }
    }

    for (uint32_t i = 0; i < Fields.size(); i++)
    {
        if (Fields[i].MarkForDelete)
        {
            DeleteFieldData(Fields[i]);
            // Fields.ExtractNoShift(i);
            Fields[i] = Fields.back();
            Fields.pop_back();
            i--;
        }
    }
}

void FieldList::DeleteFieldData(const FieldDesc &pData)
{
    const auto fldType = pData.Type;

    switch (fldType)
    {
    case FIELD_BOOL: {
        delete static_cast<DataBool *>(pData.pPointer);
        break;
    }
    case FIELD_FLOAT: {
        delete static_cast<DataFloat *>(pData.pPointer);
        break;
    }
    case FIELD_GRAPH: {
        delete static_cast<DataGraph *>(pData.pPointer);
        break;
    }
    case FIELD_POSITION: {
        delete static_cast<DataPosition *>(pData.pPointer);
        break;
    }
    case FIELD_STRING: {
        delete static_cast<DataString *>(pData.pPointer);
        break;
    }
    case FIELD_UV: {
        delete static_cast<DataUV *>(pData.pPointer);
        break;
    }
    case FIELD_COLOR: {
        delete static_cast<DataColor *>(pData.pPointer);
        break;
    }
    default: {
        throw std::runtime_error("Particles: Try delete unknown field type !!!!");
    }
    } // switch
}

void FieldList::Write(MemFile *File)
{
    uint32_t DataFieldsCount = Fields.size();
    File->WriteType(DataFieldsCount);

    for (uint32_t n = 0; n < DataFieldsCount; n++)
    {
        File->WriteType(Fields[n].Type);

        switch (Fields[n].Type)
        {
        case FIELD_BOOL: {
            auto *pBoolField = static_cast<DataBool *>(Fields[n].pPointer);
            pBoolField->Write(File);
            break;
        }
        case FIELD_FLOAT: {
            auto *pFloatField = static_cast<DataFloat *>(Fields[n].pPointer);
            pFloatField->Write(File);
            break;
        }
        case FIELD_GRAPH: {
            auto *pGraphField = static_cast<DataGraph *>(Fields[n].pPointer);
            pGraphField->Write(File);
            break;
        }
        case FIELD_POSITION: {
            auto *pPositionField = static_cast<DataPosition *>(Fields[n].pPointer);
            pPositionField->Write(File);
            break;
        }
        case FIELD_STRING: {
            auto *pStringField = static_cast<DataString *>(Fields[n].pPointer);
            pStringField->Write(File);
            break;
        }
        case FIELD_UV: {
            auto *pUVField = static_cast<DataUV *>(Fields[n].pPointer);
            pUVField->Write(File);
            break;
        }
        case FIELD_COLOR: {
            auto *pColorField = static_cast<DataColor *>(Fields[n].pPointer);
            pColorField->Write(File);
            break;
        }
        default: {
            throw std::runtime_error("Particles: Unknown field type !!!!");
        }
        } // switch
    }     // loop
}
