#ifndef _FIELD_LIST_H
#define _FIELD_LIST_H

#include "../../icommon/memfile.h"
#include "../../icommon/types.h"
#include "../datadesc/data_desc.h"
#include "math3d.h"
#include <string>
#include <vector>

class DataColor;
class DataBool;
class DataFloat;
class DataGraph;
class DataString;
class DataPosition;
class DataUV;

class FieldList
{
  public:
    struct FieldDesc
    {
        //При конвертации используеться этот флажок
        bool MarkForDelete;

        //Хэш имени
        uint32_t HashValue;

        //Тип
        FieldType Type;

        //Имя
        std::string Name;

        //Указатель
        void *pPointer;
    };

  private:
    std::vector<FieldDesc> Fields;

    void DeleteFieldData(const FieldDesc &pData);

    //=================== создание пустых аттрибутов
    void CreateEmptyBoolField(const char *Name, bool def_value);
    void CreateEmptyFloatField(const char *Name, float def_value);
    void CreateEmptyGraphField(const char *Name, float def_value_min, float def_value_max);
    void CreateEmptyPositionField(const char *Name, const Vector &def_value);
    void CreateEmptyStringField(const char *Name, const char *def_value);
    void CreateEmptyUVField(const char *Name);
    void CreateEmptyColorField(const char *Name, uint32_t def_value);

    //=================== Прогрузка аттрибутов из файла
    void CreateBoolField(MemFile *pMemFile);
    void CreateFloatField(MemFile *pMemFile);
    void CreateGraphField(MemFile *pMemFile);
    void CreatePositionField(MemFile *pMemFile);
    void CreateStringField(MemFile *pMemFile);
    void CreateUVField(MemFile *pMemFile);
    void CreateColorField(MemFile *pMemFile);

    FieldDesc *FindField(const char *Name);

  public:
    FieldList();
    ~FieldList();

    void DelAll();

    //Сохранить/загрузить...
    void Load(MemFile *File);
    void Write(MemFile *File);

    DataColor *FindColor(const char *AttrName);
    DataGraph *FindGraph(const char *AttrName);
    DataUV *FindUV(const char *AttrName);

    DataBool *FindBool(const char *AttrName);
    DataFloat *FindFloat(const char *AttrName);
    DataString *FindString(const char *AttrName);
    DataPosition *FindPosition(const char *AttrName);

    float GetFloat(const char *AttrName, float def_value = 0.0f);
    int GetFloatAsInt(const char *AttrName, int def_value = 0);
    bool GetBool(const char *AttrName, bool def_value = false);
    const char *GetString(const char *AttrName, const char *def_value = "");
    const Vector &GetPosition(const char *AttrName, const Vector &def_value = Vector(0.0f));

    float GetGraphVal(const char *AttrName, float Time, float LifeTime, float K_Rand, float def_value = 0.0f);
    float GetRandomGraphVal(const char *AttrName, float Time, float LifeTime, float def_value = 0.0f);

    uint32_t GetFieldCount() const;
    const FieldDesc &GetFieldByIndex(uint32_t Index);

    //Конвертация ГАРАНТИРУЕТ, что в данном DataSource будут нужные поля
    //и не будет лишнего мусора...
    void Convert(DataDescripion *pDataDescriptor);
};

#endif
