#pragma once

#include "vfile_service.h"
#include <stdexcept>

#define KNF_KEY 0x1
#define KNF_COMMENTARY 0x2

class FILE_SERVICE;

class KEY_NODE
{
    char *key_name;
    char *key_val;

    long name_size;
    long val_size;

    KEY_NODE *l_PTR;
    KEY_NODE *r_PTR;

    uint32_t flags;

  public:
    KEY_NODE();
    ~KEY_NODE();
    void SetName(const char *name);
    void SetValue(const char *value);
    char *GetName();
    char *GetValue();
    KEY_NODE *GetLeftNode();
    KEY_NODE *GetRightNode();
    void SetLeftNode(KEY_NODE *node);
    void SetRightNode(KEY_NODE *node);
    void Add(KEY_NODE **Root, KEY_NODE **Top);
    void AttachTo(KEY_NODE *node, KEY_NODE **Top); // left - root, order - from left to right
    void Deattach(KEY_NODE **Root, KEY_NODE **Top);
    uint32_t SetFlags(uint32_t flags);
};

class SECTION
{
    KEY_NODE *Root;
    KEY_NODE *Top;

    SECTION *l_PTR;
    SECTION *r_PTR;

    char *Name;

  public:
    SECTION();
    ~SECTION();

    KEY_NODE *AddNode();
    void DelNode(KEY_NODE *);

    KEY_NODE *FindKey(KEY_NODE *from, const char *key_name, const char *key_value);
    KEY_NODE *FindKey(const char *key_name, const char *key_value);
    KEY_NODE *FindKey(const char *key_name);
    KEY_NODE *GetRoot();

    void SetName(const char *name);
    char *GetName();

    SECTION *GetLeftNode();
    SECTION *GetRightNode();
    void SetLeftNode(SECTION *node);
    void SetRightNode(SECTION *node);
    void Add(SECTION **SRoot, SECTION **STop);
    void Deattach(SECTION **SRoot, SECTION **STop);
};

typedef struct
{
    SECTION *Section;
    KEY_NODE *Key;
} SEARCH_DATA;

class IFS : public INIFILE
{

    friend FILE_SERVICE;

  protected:
    SECTION *SectionSNode;

    SECTION *SectionRoot;
    SECTION *SectionTop;

    // KEY_NODE * FindKey(KEY_NODE * from, char * section_name, char * key_name, char * key_value);
    KEY_NODE *FindKey(const char *section_name, const char *key_name, const char *key_value);
    KEY_NODE *FindKey(const char *section_name, const char *key_name);
    SECTION *FindSection(const char *section_name);
    SECTION *FindSection(const char *section_name, SECTION *snode);

    void Format(char *file_data, long file_size);
    bool VoidSym(char symbol);

    VFILE_SERVICE *fs;
    char *FileName;
    bool bDataChanged;
    uint32_t Reference;
    uint32_t CompareStrings(const char *s1, const char *s2);
    bool FlushFile();

  public:
#define IFS_NOT_IMPLEMENTED(x)                                                                                         \
    x override                                                                                                         \
    {                                                                                                                  \
        throw std::logic_error("not implemented");                                                                     \
    }
    IFS_NOT_IMPLEMENTED(void ReadString(const char *section_name, const char *key_name, char *buffer,
                                        size_t buffer_size))
    IFS_NOT_IMPLEMENTED(bool ReadString(const char *section_name, const char *key_name, char *buffer,
                                        size_t buffer_size, const char *def_string))
    IFS_NOT_IMPLEMENTED(bool ReadStringNext(const char *section_name, const char *key_name, char *buffer,
                                            size_t buffer_size))
    IFS_NOT_IMPLEMENTED(long GetLong(const char *section_name, const char *key_name))
    IFS_NOT_IMPLEMENTED(long GetLong(const char *section_name, const char *key_name, long def_val))
    IFS_NOT_IMPLEMENTED(bool GetLongNext(const char *section_name, const char *key_name, long *val))
    IFS_NOT_IMPLEMENTED(double GetDouble(const char *section_name, const char *key_name))
    IFS_NOT_IMPLEMENTED(double GetDouble(const char *section_name, const char *key_name, double def_val))
    IFS_NOT_IMPLEMENTED(bool GetDoubleNext(const char *section_name, const char *key_name, double *val))
    IFS_NOT_IMPLEMENTED(float GetFloat(const char *section_name, const char *key_name))
    IFS_NOT_IMPLEMENTED(float GetFloat(const char *section_name, const char *key_name, float def_val))
    IFS_NOT_IMPLEMENTED(bool GetFloatNext(const char *section_name, const char *key_name, float *val))
    bool CaseSensitive(bool yes)
    {
        return false;
    }
#undef IFS_NOT_IMPLEMENTED

    IFS(VFILE_SERVICE *_fs);
    ~IFS();

    bool LoadFile(const char *file_name);
    char *GetFileName()
    {
        return FileName;
    };
    void IncReference();
    void DecReference();
    long GetReference();

    SECTION *CreateSection(const char *section_name);
    void DeleteSection(const char *section_name) override;

    bool TestKey(const char *section_name, const char *key_name, const char *key_value) override;
    void DeleteKey(const char *section_name, const char *key_name) override;
    void DeleteKey(const char *section_name, const char *key_name, const char *key_value) override;

    bool ReadString(SEARCH_DATA *sd, const char *section_name, const char *key_name, char *buffer, uint32_t buffer_size,
                    const char *def_string);
    void ReadString(SEARCH_DATA *sd, const char *section_name, const char *key_name, char *buffer,
                    uint32_t buffer_size); // rise exception on fault
    bool ReadStringNext(SEARCH_DATA *sd, const char *section_name, const char *key_name, char *buffer,
                        uint32_t buffer_size);

    long GetLong(SEARCH_DATA *sd, const char *section_name, const char *key_name);
    long GetLong(SEARCH_DATA *sd, const char *section_name, const char *key_name, long def_val);
    bool GetLongNext(SEARCH_DATA *sd, const char *section_name, const char *key_name, long *val);

    double GetDouble(SEARCH_DATA *sd, const char *section_name, const char *key_name);
    double GetDouble(SEARCH_DATA *sd, const char *section_name, const char *key_name, double def_val);
    bool GetDoubleNext(SEARCH_DATA *sd, const char *section_name, const char *key_name, double *val);

    float GetFloat(SEARCH_DATA *sd, const char *section_name, const char *key_name);
    float GetFloat(SEARCH_DATA *sd, const char *section_name, const char *key_name, float def_val);
    bool GetFloatNext(SEARCH_DATA *sd, const char *section_name, const char *key_name, float *val);

    void AddString(const char *section_name, const char *key_name, const char *string) override;
    void WriteString(const char *section_name, const char *key_name, const char *string) override;
    void WriteLong(const char *section_name, const char *key_name, long value) override;
    void WriteDouble(const char *section_name, const char *key_name, double value) override;
    void WriteFloat(const char *section_name, const char *key_name, float value);

    bool GetSectionName(char *section_name_buffer, long buffer_size) override;
    bool GetSectionNameNext(char *section_name_buffer, long buffer_size) override;
    void Flush() override;
    bool Reload() override;
    bool TestSection(const char *section_name) override;
};
