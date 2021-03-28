#include "ifs.h"
#include "core.h"
#include "vmodule_api.h"

#define COMMENT ';'
#define SECTION_A '['
#define SECTION_B ']'
#define INI_EQUAL '='
#define VOIDSYMS_NUM 2
const char INI_LINEFEED[3] = {0xd, 0xa, 0};
const char INI_VOIDSYMS[VOIDSYMS_NUM] = {0x20, 0x9};

KEY_NODE::KEY_NODE()
{
    key_name = nullptr;
    key_val = nullptr;
    name_size = 0;
    val_size = 0;
    l_PTR = nullptr;
    r_PTR = nullptr;
    flags = 0;
}

KEY_NODE::~KEY_NODE()
{
    delete key_name;
    delete key_val;
    // if(l_PTR) delete l_PTR;
    // if(r_PTR) delete r_PTR;
}

void KEY_NODE::SetName(const char *name)
{
    if (name == nullptr)
        return;
    delete key_name;
    key_name = nullptr;
    name_size = strlen(name) + 1;

    key_name = new char[name_size];
    if (key_name == nullptr)
        throw std::exception();
    strcpy_s(key_name, name_size, name);
}

void KEY_NODE::SetValue(const char *value)
{
    if (value == nullptr)
        return;
    delete key_val;
    key_val = nullptr;
    val_size = strlen(value) + 1;

    key_val = new char[val_size];
    if (key_val == nullptr)
        throw std::exception();
    strcpy_s(key_val, val_size, value);
}

char *KEY_NODE::GetName()
{
    return key_name;
}

char *KEY_NODE::GetValue()
{
    return key_val;
}

KEY_NODE *KEY_NODE::GetLeftNode()
{
    return l_PTR;
}

KEY_NODE *KEY_NODE::GetRightNode()
{
    return r_PTR;
}

void KEY_NODE::SetLeftNode(KEY_NODE *node)
{
    l_PTR = node;
}

void KEY_NODE::SetRightNode(KEY_NODE *node)
{
    r_PTR = node;
}

void KEY_NODE::Add(KEY_NODE **Root, KEY_NODE **Top)
{
    if (*Root == nullptr)
    {
        // first node, root and top pointed to this node
        *Root = this;
        *Top = this;
        return;
    }
    // attach node to the topmost node
    (*Top)->SetRightNode(this);
    // set node left link to previous top node
    SetLeftNode(*Top);
    // set new top node
    *Top = this;
}

void KEY_NODE::AttachTo(KEY_NODE *node, KEY_NODE **Top)
{
    if (node == nullptr)
        throw "invalid node";
    // save right node for object
    auto *RNode = node->GetRightNode();
    // attach this node
    node->SetRightNode(this);
    // set left link for this node
    SetLeftNode(node);
    // set right link from previous node
    SetRightNode(RNode);
    // if there is node from the right - set left link for it
    if (RNode != nullptr)
        RNode->SetLeftNode(this);
    // else this is new top node
    else
        *Top = this;
}

void KEY_NODE::Deattach(KEY_NODE **Root, KEY_NODE **Top)
{
    if (l_PTR)
        l_PTR->SetRightNode(r_PTR);
    if (r_PTR)
        r_PTR->SetLeftNode(l_PTR);
    if (*Root == this)
        *Root = r_PTR;
    if (*Top == this)
        *Top = l_PTR;
}

uint32_t KEY_NODE::SetFlags(uint32_t _flags)
{
    flags = flags | _flags;
    return flags;
}

//=============================================================================================================

SECTION::SECTION()
{
    Root = nullptr;
    Top = nullptr;
    Name = nullptr;
    l_PTR = nullptr;
    r_PTR = nullptr;
}

SECTION::~SECTION()
{
    while (Root)
    {
        auto *const old_root = Root;
        Root->Deattach(&Root, &Top);
        delete old_root;
    }
    delete Name;
}

void SECTION::SetName(const char *name)
{
    delete Name;
    if (name == nullptr)
    {
        Name = nullptr;
    }
    else
    {
        const auto len = strlen(name) + 1;
        Name = new char[len];
        if (Name == nullptr)
            throw std::exception();
        strcpy_s(Name, len, name);
    }
}

char *SECTION::GetName()
{
    return Name;
}

KEY_NODE *SECTION::AddNode()
{
    auto *node = new KEY_NODE;
    if (node == nullptr)
        throw "node creation error";
    node->Add(&Root, &Top);
    return node;
}

void SECTION::DelNode(KEY_NODE *_node)
{
    auto *node = Root;
    while (node)
    {
        if (node == _node)
        {
            node->Deattach(&Root, &Top);
            delete node;
            return;
        }
        node = node->GetRightNode();
    }
}

KEY_NODE *SECTION::FindKey(KEY_NODE *from, const char *key_name, const char *key_value)
{
    KEY_NODE *node;

    if (Root == nullptr)
        return nullptr;

    if (key_name == nullptr)
        return nullptr;

    if (from == nullptr)
        node = Root;
    else
        node = from;

    while (node)
    {
        const auto flags = node->SetFlags(0);
        if (flags & KNF_KEY)
        {
            if (_stricmp(key_name, node->GetName()) == 0)
            {
                if (key_value == nullptr)
                    return node;

                auto *const char_PTR = node->GetValue();
                if (char_PTR != nullptr)
                {
                    if (_stricmp(key_value, char_PTR) == 0)
                        return node;
                }
            }
        }
        node = node->GetRightNode();
    }
    return nullptr;
}

KEY_NODE *SECTION::FindKey(const char *key_name, const char *key_value)
{
    return FindKey(Root, key_name, key_value);
}

KEY_NODE *SECTION::FindKey(const char *key_name)
{
    return FindKey(Root, key_name, nullptr);
}

SECTION *SECTION::GetLeftNode()
{
    return l_PTR;
}

SECTION *SECTION::GetRightNode()
{
    return r_PTR;
}

void SECTION::SetLeftNode(SECTION *node)
{
    l_PTR = node;
}

void SECTION::SetRightNode(SECTION *node)
{
    r_PTR = node;
}

void SECTION::Add(SECTION **SRoot, SECTION **STop)
{
    if (*SRoot == nullptr)
    {
        *SRoot = this;
        *STop = this;
        return;
    }
    (*STop)->SetRightNode(this);
    SetLeftNode(*STop);
    *STop = this;
}

void SECTION::Deattach(SECTION **SRoot, SECTION **STop)
{
    if (l_PTR)
        l_PTR->SetRightNode(r_PTR);
    if (r_PTR)
        r_PTR->SetLeftNode(l_PTR);
    if (*SRoot == this)
        *SRoot = r_PTR;
    if (*STop == this)
        *STop = l_PTR;
}

KEY_NODE *SECTION::GetRoot()
{
    return Root;
}

//=============================================================================================================

IFS::IFS(VFILE_SERVICE *_fs)
{
    fs = _fs;
    FileName = nullptr;
    bDataChanged = false;
    Reference = 0;
    SectionRoot = nullptr;
    SectionTop = nullptr;
    SectionSNode = nullptr;
}

IFS::~IFS()
{
    FlushFile();
    delete FileName;
    while (SectionRoot)
    {
        auto *const old_root = SectionRoot;
        SectionRoot->Deattach(&SectionRoot, &SectionTop);
        delete old_root;
    }
}

void IFS::IncReference()
{
    Reference++;
}

void IFS::DecReference()
{
    Reference--;
}

long IFS::GetReference()
{
    return Reference;
}

bool IFS::VoidSym(char symbol)
{
    for (uint32_t n = 0; n < VOIDSYMS_NUM; n++)
    {
        if (symbol == INI_VOIDSYMS[n])
            return true;
    }
    return false;
}

bool IFS::LoadFile(const char *_file_name)
{
    if (_file_name == nullptr)
    {
        return false;
    }
    auto fileS = fs->_CreateFile(_file_name, std::ios::binary | std::ios::in);
    if (!fileS.is_open())
    {
        core.tracelog->trace("Unable to load file: {}", _file_name);
        return false;
    }

    const auto file_size = fs->_GetFileSize(_file_name);

    auto *const file_data = new char[file_size + 1]; // +1 for zero at the end
    if (file_data == nullptr)
    {
        fs->_CloseFile(fileS);
        return false;
    }
    file_data[file_size] = 0;

    if (!fs->_ReadFile(fileS, file_data, file_size))
    {
        delete[] file_data;
        fs->_CloseFile(fileS);
        return false;
    }

    fs->_CloseFile(fileS);

    const uint32_t name_size = strlen(_file_name) + 1;

    FileName = new char[name_size];

    if (FileName == nullptr)
    {
        delete[] file_data;
        fs->_CloseFile(fileS);
        return false;
    }
    strcpy_s(FileName, name_size, _file_name);

    Format(file_data, file_size + 1);

    delete[] file_data;

    return true;
}

void IFS::Format(char *file_data, long file_size)
{
    long n, z;
    long backcount;
    long forecount;

    auto *Current_Section = new SECTION;
    Current_Section->Add(&SectionRoot, &SectionTop);

    char *data_PTR = nullptr;
    long lines = 0;

    // terminate each line by zero symbol
    for (n = 0; n < file_size; n++)
    {
        if (file_data[n] == INI_LINEFEED[0] || file_data[n] == INI_LINEFEED[1] || file_data[n] == INI_LINEFEED[2])
        {
            file_data[n] = 0;
            if (n > 0 && file_data[n - 1] != 0)
                lines++;
        }
    }

    for (n = 0; n < file_size; n++)
    {
        if (n == 0)
        {
            data_PTR = file_data;
        }
        else
        {
            if (file_data[n] != 0)
                continue;
            if ((n + 1) >= file_size)
                break; // end of file
            if (file_data[n + 1] == 0)
                continue; // separator zero
            data_PTR = file_data + n + 1;
        }

        // add new key object ------------------------------------------------------
        // (file_data + n + 1) - start of the line

        long offset = 0;
        for (; data_PTR[offset]; offset++)
        {
        }

        for (long i = 0; data_PTR[i]; i++)
        {
            if (VoidSym(data_PTR[i]))
                continue; // skip void syms

            if (data_PTR[i] == SECTION_A)
            {
                // section header
                for (z = i + 1; data_PTR[z]; z++)
                {
                    if (data_PTR[z] == SECTION_B)
                    {
                        data_PTR[z] = 0;
                        // node->SetName(&data_PTR[i+1]);
                        // node->SetFlags(KNF_SECTION);

                        Current_Section = new SECTION;
                        Current_Section->Add(&SectionRoot, &SectionTop);
                        Current_Section->SetName(&data_PTR[i + 1]);
                        break;
                    }
                }
                break;
            }

            auto *node = Current_Section->AddNode();
            // node = new KEY_NODE;
            // if(node == 0) throw "node creation error";
            // node->Add(&Root,&Top);

            if (data_PTR[i] == COMMENT)
            {
                // add as commentary
                node->SetName(&data_PTR[i]);
                node->SetFlags(KNF_COMMENTARY);
                break;
            }

            auto keyname_set = false;

            // this is real key
            node->SetFlags(KNF_KEY);

            for (z = 0; data_PTR[z]; z++)
            {
                if (data_PTR[z] == INI_EQUAL)
                {
                    data_PTR[z] = 0;
                    for (backcount = (z - 1); backcount > 0; backcount--)
                    {
                        if (VoidSym(data_PTR[backcount]))
                            data_PTR[backcount] = 0;
                        else
                            break;
                    }
                    node->SetName(&data_PTR[i]); // set key name
                    keyname_set = true;

                    auto keyval_found = false;
                    z++;
                    for (; data_PTR[z]; z++)
                    {
                        if (VoidSym(data_PTR[z]))
                            continue;
                        keyval_found = true;
                        break;
                    }
                    for (forecount = z; data_PTR[forecount]; forecount++)
                    {
                    }
                    for (backcount = (forecount - 1); backcount > 0; backcount--)
                    {
                        if (VoidSym(data_PTR[backcount]))
                            data_PTR[backcount] = 0;
                        else
                            break;
                    }
                    if (keyval_found)
                    {
                        node->SetValue(&data_PTR[z]);
                    }
                    break;
                }
            }
            if (!keyname_set)
                node->SetName(&data_PTR[i]); // key without value
            break;
        }
        n += offset;
    }
}

bool IFS::FlushFile()
{
    // GUARD(bool IFS::FlushFile())
    uint32_t write_size;
    char buff[2];

    if (bDataChanged == false)
        return true;

    fs->_SetFileAttributes(FileName, FILE_ATTRIBUTE_NORMAL);
    fs->_DeleteFile(FileName);
    auto fileS = fio->_CreateFile(FileName, std::ios::binary | std::ios::out);
    if (!fileS.is_open())
    {
        /*trace("file: (%s)",FileName);*/
        throw std::exception("cant create file");
    }

    // node = Root;

    auto *section_node = SectionRoot;
    while (section_node)
    {
        if (section_node->GetName() != nullptr)
        {
            // write section name -----------------------------------------------------------------
            buff[0] = SECTION_A;
            if (!fs->_WriteFile(fileS, buff, 1))
            {
                throw std::exception();
            }

            write_size = strlen(section_node->GetName());
            if (!fs->_WriteFile(fileS, section_node->GetName(), write_size))
            {
                throw std::exception();
            }

            buff[0] = SECTION_B;
            if (!fs->_WriteFile(fileS, buff, 1))
            {
                throw std::exception();
            }

            buff[0] = INI_LINEFEED[0];
            buff[1] = INI_LINEFEED[1];
            if (!fs->_WriteFile(fileS, buff, 2))
            {
                throw std::exception();
            }
        }

        auto *node = section_node->GetRoot();
        while (node)
        {
            const auto flags = node->SetFlags(0);
            if (flags & KNF_COMMENTARY)
            {
                // write commented line ---------------------------------------------------------------
                write_size = strlen(node->GetName());
                if (!fs->_WriteFile(fileS, node->GetName(), write_size))
                {
                    throw std::exception();
                }
                buff[0] = INI_LINEFEED[0];
                buff[1] = INI_LINEFEED[1];
                if (fs->_WriteFile(fileS, buff, 2))
                {
                    throw std::exception();
                }
            }
            else if (flags & KNF_KEY)
            {
                // write key -------------------------------------------------------------------------
                write_size = strlen(node->GetName());
                if (!fs->_WriteFile(fileS, node->GetName(), write_size))
                {
                    throw std::exception();
                }
                if (node->GetValue() != nullptr)
                {
                    if (!fs->_WriteFile(fileS, &INI_VOIDSYMS[0], 1))
                    {
                        throw std::exception();
                    }
                    buff[0] = INI_EQUAL;
                    if (!fs->_WriteFile(fileS, buff, 1))
                    {
                        throw std::exception();
                    }
                    if (!fs->_WriteFile(fileS, &INI_VOIDSYMS[0], 1))
                    {
                        throw std::exception();
                    }
                    write_size = strlen(node->GetValue());
                    if (!fs->_WriteFile(fileS, node->GetValue(), write_size))
                    {
                        throw std::exception();
                    }
                }
                buff[0] = INI_LINEFEED[0];
                buff[1] = INI_LINEFEED[1];
                if (!fs->_WriteFile(fileS, buff, 2))
                {
                    throw std::exception();
                }
            }
            else
                throw "invalid key flag";
            node = node->GetRightNode();
        }
        section_node = section_node->GetRightNode();

        buff[0] = INI_LINEFEED[0];
        buff[1] = INI_LINEFEED[1];
        if (!fs->_WriteFile(fileS, buff, 2))
        {
            throw std::exception();
        }
    }

    fs->_CloseFile(fileS);

    // UNGUARD
    return false;
}

KEY_NODE *IFS::FindKey(const char *section_name, const char *key_name)
{
    return FindKey(section_name, key_name, nullptr);
}

KEY_NODE *IFS::FindKey(const char *section_name, const char *key_name, const char *key_value)
{
    if (SectionRoot == nullptr)
        return nullptr;
    auto *snode = FindSection(section_name);
    if (snode == nullptr)
        return nullptr;
    return snode->FindKey(key_name, key_value);

    // return FindKey(0,section_name,key_name,key_value);
}

/*KEY_NODE * IFS::FindKey(KEY_NODE * from, char * section_name, char * key_name, char * key_value)
{
    SECTION * snode;
    if(SectionRoot == 0) return false;
    snode = FindSection(section_name);
    if(snode == 0) return false;
    return snode->FindKey(from,key_name,key_value);
}*/

SECTION *IFS::FindSection(const char *section_name)
{
    if (SectionRoot == nullptr)
        return nullptr;
    auto *node = SectionRoot;

    while (node)
    {
        if (section_name == nullptr)
        {
            if (node->GetName() == nullptr)
                return node;
            return nullptr;
        }
        if (node->GetName() != nullptr)
            if (_stricmp(section_name, node->GetName()) == 0)
                return node;
        node = node->GetRightNode();
    }
    return nullptr;
}

SECTION *IFS::FindSection(const char *section_name, SECTION *snode)
{
    if (SectionRoot == nullptr)
        return nullptr;

    // atempt to search by section node pointer
    auto *node = SectionRoot;
    while (node)
    {
        if (node == snode)
        {
            // if node exist and name is correct return ok
            if (section_name != nullptr)
            {
                if (_stricmp(section_name, node->GetName()) == 0)
                    return node;
            }
            else
            {
                if (node->GetName() == nullptr)
                    return node;
            }
            break;
        }

        node = node->GetRightNode();
    }

    // full search by name
    node = SectionRoot;
    while (node)
    {
        if (section_name == nullptr)
        {
            if (node->GetName() == nullptr)
                return node;
        }
        if (node->GetName() != nullptr)
            if (_stricmp(section_name, node->GetName()) == 0)
                return node;
        node = node->GetRightNode();
    }
    return nullptr;
}

SECTION *IFS::CreateSection(const char *section_name)
{
    auto *node = FindSection(section_name);
    if (node)
        return node;

    node = new SECTION;
    if (node == nullptr)
        throw "section creation error";
    node->Add(&SectionRoot, &SectionTop);
    node->SetName(section_name);
    bDataChanged = true;
    return node;
}

void IFS::DeleteSection(const char *section_name)
{
    auto *node = FindSection(section_name);
    if (!node)
        return;
    if (SectionSNode == node)
    {
        SectionSNode = SectionSNode->GetRightNode();
    }
    node->Deattach(&SectionRoot, &SectionTop);
    delete node;
    bDataChanged = true;
}

bool IFS::TestSection(const char *section_name)
{
    auto *const node = FindSection(section_name);
    if (node)
        return true;
    return false;
}

bool IFS::TestKey(const char *section_name, const char *key_name, const char *key_value)
{
    auto *node = FindSection(section_name);
    if (node == nullptr)
        return false;
    if (node->FindKey(key_name, key_value) != nullptr)
        return true;
    return false;
}

void IFS::DeleteKey(const char *section_name, const char *key_name)
{
    DeleteKey(section_name, key_name, nullptr);
}

void IFS::DeleteKey(const char *section_name, const char *key_name, const char *key_value)
{
    auto *node = FindSection(section_name);
    if (node)
    {
        auto *const knode = node->FindKey(key_name, key_value);
        if (knode)
        {
            node->DelNode(knode);
            bDataChanged = true;
        }
    }
}

void IFS::ReadString(SEARCH_DATA *sd, const char *section_name, const char *key_name, char *buffer,
                     uint32_t buffer_size)
{
    ReadString(sd, section_name, key_name, buffer, buffer_size, nullptr);
}

bool IFS::ReadString(SEARCH_DATA *sd, const char *section_name, const char *key_name, char *buffer,
                     uint32_t buffer_size, const char *def_string)
{
    auto *node = FindKey(section_name, key_name);
    if (node == nullptr)
    {
        sd->Key = nullptr;
        sd->Section = nullptr;
        if (def_string == nullptr)
        {
            core.Trace("Warning! IniFile Read String: section=%s, key=%s", section_name, key_name);
            if (buffer)
                buffer[0] = 0;
            // throw std::exception(string not found);
        }
        else if (buffer)
            strcpy_s(buffer, buffer_size, def_string);
        return false;
    }

    sd->Key = node;
    sd->Section = FindSection(section_name);

    if (buffer == nullptr)
        throw std::exception("zero buffer");
    auto *const char_PTR = node->GetValue();
    if (char_PTR == nullptr)
    {
        if (def_string == nullptr)
            throw std::exception("no key value");
        strcpy_s(buffer, buffer_size, def_string);
        return false;
    }

    uint32_t write_size = strlen(char_PTR) + 1;
    // if(write_size > buffer_size) throw std::exception(buffer size too small); // commented out because it didn't let
    // to load new ani

    strcpy_s(buffer, buffer_size, node->GetValue());
    return true;
}

bool IFS::ReadStringNext(SEARCH_DATA *sd, const char *section_name, const char *key_name, char *buffer,
                         uint32_t buffer_size)
{
    auto *snode = FindSection(section_name, sd->Section);
    // snode = sd->Section;
    if (snode == nullptr)
        return false;

    auto start = false;
    auto *node = snode->GetRoot();
    // node = sd->Key;
    while (node)
    {
        if (start == true)
        {
            // if(CompareStrings(node->GetName(),key_name) == 0)
            if (_stricmp(node->GetName(), key_name) == 0)
            {
                if (buffer == nullptr)
                    throw std::exception("zero buffer");

                auto *const char_PTR = node->GetValue();
                if (char_PTR == nullptr)
                {
                    buffer[0] = 0;
                    return true;
                    // throw std::exception(no key value);
                }

                const uint32_t write_size = strlen(char_PTR) + 1;
                if (write_size > buffer_size)
                    throw std::exception("buffer size too small");

                strcpy_s(buffer, buffer_size, node->GetValue());
                sd->Key = node;
                sd->Section = snode;
                return true;
            }
        }
        if (sd->Key == node)
            start = true;
        node = node->GetRightNode();
    }
    sd->Key = nullptr;
    sd->Section = nullptr;
    return false;
}

long IFS::GetLong(SEARCH_DATA *sd, const char *section_name, const char *key_name)
{
    char buffer[256];
    ReadString(sd, section_name, key_name, buffer, sizeof(buffer));
    return static_cast<long>(atoll(buffer));
}

long IFS::GetLong(SEARCH_DATA *sd, const char *section_name, const char *key_name, long def_val)
{
    char buffer[256];
    if (ReadString(sd, section_name, key_name, buffer, sizeof(buffer), ""))
        return static_cast<long>(atoll(buffer));
    return def_val;
}

bool IFS::GetLongNext(SEARCH_DATA *sd, const char *section_name, const char *key_name, long *val)
{
    char buffer[256];
    if (ReadStringNext(sd, section_name, key_name, buffer, sizeof(buffer)))
    {
        *val = atol(buffer);
        return true;
    }
    return false;
}

double IFS::GetDouble(SEARCH_DATA *sd, const char *section_name, const char *key_name)
{
    char buffer[256];
    ReadString(sd, section_name, key_name, buffer, sizeof(buffer));
    return atof(buffer);
}

double IFS::GetDouble(SEARCH_DATA *sd, const char *section_name, const char *key_name, double def_val)
{
    char buffer[256];
    if (ReadString(sd, section_name, key_name, buffer, sizeof(buffer), ""))
        return atof(buffer);
    return def_val;
}

bool IFS::GetDoubleNext(SEARCH_DATA *sd, const char *section_name, const char *key_name, double *val)
{
    char buffer[256];
    if (ReadStringNext(sd, section_name, key_name, buffer, sizeof(buffer)))
    {
        *val = atof(buffer);
        return true;
    }
    return false;
}

float IFS::GetFloat(SEARCH_DATA *sd, const char *section_name, const char *key_name)
{
    char buffer[256];
    ReadString(sd, section_name, key_name, buffer, sizeof(buffer));
    return static_cast<float>(atof(buffer));
}

float IFS::GetFloat(SEARCH_DATA *sd, const char *section_name, const char *key_name, float def_val)
{
    char buffer[256];
    if (ReadString(sd, section_name, key_name, buffer, sizeof(buffer), ""))
        return static_cast<float>(atof(buffer));
    return def_val;
}

bool IFS::GetFloatNext(SEARCH_DATA *sd, const char *section_name, const char *key_name, float *val)
{
    char buffer[256];
    if (ReadStringNext(sd, section_name, key_name, buffer, sizeof(buffer)))
    {
        *val = static_cast<float>(atof(buffer));
        return true;
    }
    return false;
}

void IFS::AddString(const char *section_name, const char *key_name, const char *string)
{
    if (key_name == nullptr)
        throw std::exception("zero key");
    auto *snode = FindSection(section_name);
    if (snode == nullptr)
        CreateSection(section_name);
    snode = FindSection(section_name);
    if (snode == nullptr)
        throw std::exception("section create error");

    auto *node = snode->AddNode();
    node->SetName(key_name);
    if (string)
        node->SetValue(string);
    node->SetFlags(KNF_KEY);
    bDataChanged = true;
}

void IFS::WriteString(const char *section_name, const char *key_name, const char *string)
{
    if (string == nullptr)
        throw std::exception("zero key value");

    auto *snode = CreateSection(section_name);
    if (snode == nullptr)
        throw std::exception("section create error");
    auto *node = snode->FindKey(key_name);
    if (node != nullptr)
    {
        node->SetValue(string);
        bDataChanged = true;
        delete snode;
        return;
    }
    AddString(section_name, key_name, string);
}

void IFS::WriteLong(const char *section_name, const char *key_name, long value)
{
    char buffer[256];
    _ltoa(value, buffer, 10);
    WriteString(section_name, key_name, buffer);
}

void IFS::WriteDouble(const char *section_name, const char *key_name, double value)
{
    char buffer[256];
    sprintf_s(buffer, "%g", value);
    WriteString(section_name, key_name, buffer);
}

void IFS::WriteFloat(const char *section_name, const char *key_name, float value)
{
    char buffer[256];
    sprintf_s(buffer, "%g", value);
    WriteString(section_name, key_name, buffer);
}

uint32_t IFS::CompareStrings(const char *s1, const char *s2)
{
    if (s1 == nullptr || s2 == nullptr)
        return 1;
    uint32_t n = 0;
    while (s1[n] == s2[n])
    {
        if (s1[n] == 0)
            return 0;
        n++;
    }
    return 1;
}

bool IFS::GetSectionName(char *section_name_buffer, long buffer_size)
{
    if (SectionRoot == nullptr)
        return false;
    auto *node = SectionRoot;
    node = node->GetRightNode(); // skip zero section (unnamed)
    if (node == nullptr)
        return false;

    if (section_name_buffer == nullptr)
        throw "zero buffer";
    const long len = strlen(node->GetName());
    if (len > buffer_size)
        throw "buffer too small";
    strcpy_s(section_name_buffer, buffer_size, node->GetName());
    SectionSNode = node;
    return true;
}

bool IFS::GetSectionNameNext(char *section_name_buffer, long buffer_size)
{
    if (SectionRoot == nullptr)
        return false;
    if (section_name_buffer == nullptr)
        throw "zero buffer";
    auto *node = SectionRoot;
    while (node)
    {
        if (node == SectionSNode)
        {
            node = node->GetRightNode();
            if (node == nullptr)
            {
                SectionSNode = nullptr;
                return false;
            }
            const long len = strlen(node->GetName());
            if (len > buffer_size)
                throw "buffer too small";
            strcpy_s(section_name_buffer, buffer_size, node->GetName());
            SectionSNode = node;
            return true;
        }
        node = node->GetRightNode();
    }
    return false;
}

void IFS::Flush()
{
    FlushFile();
}

bool IFS::Reload()
{
    while (SectionRoot)
    {
        auto *const old_root = SectionRoot;
        SectionRoot->Deattach(&SectionRoot, &SectionTop);
        delete old_root;
    }
    bDataChanged = false;
    // Reference = 0;
    SectionRoot = nullptr;
    SectionTop = nullptr;
    SectionSNode = nullptr;
    return LoadFile(FileName);
}
