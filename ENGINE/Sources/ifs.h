#include "..\..\common_h\vfile_service.h"

#define KNF_KEY				0x1
#define KNF_COMMENTARY		0x2

class FILE_SERVICE;

class KEY_NODE
{
	char * key_name;
	char * key_val;

	long name_size;
	long val_size;
	
	KEY_NODE * l_PTR;
	KEY_NODE * r_PTR;

	DWORD flags;

public:
	 KEY_NODE();
	~KEY_NODE();
	void SetName(char * name);
	void SetValue(char * value);
	char * GetName();
	char * GetValue();
	KEY_NODE * GetLeftNode();
	KEY_NODE * GetRightNode();
	void SetLeftNode(KEY_NODE * node);
	void SetRightNode(KEY_NODE * node);
	void Add(KEY_NODE * * Root,KEY_NODE * * Top);
	void AttachTo(KEY_NODE * node, KEY_NODE * * Top); // left - root, order - from left to right
	void Deattach(KEY_NODE * * Root,KEY_NODE * * Top);
	DWORD SetFlags(DWORD _flags);

};

class SECTION
{
	KEY_NODE * Root;
	KEY_NODE * Top;

	SECTION * l_PTR;
	SECTION * r_PTR;

	char * Name;

public:
	 SECTION();
	~SECTION();
	
	KEY_NODE * AddNode();
	void DelNode(KEY_NODE *);

	

	KEY_NODE * FindKey(KEY_NODE * from, char * key_name, char * key_value);
	KEY_NODE * FindKey(char * key_name, char * key_value);
	KEY_NODE * FindKey(char * key_name);
	KEY_NODE * GetRoot();


	void SetName(char * name);
	char * GetName();

	SECTION * GetLeftNode();
	SECTION * GetRightNode();
	void SetLeftNode(SECTION * node);
	void SetRightNode(SECTION * node);
	void Add(SECTION * * SRoot,SECTION * * STop);
	void Deattach(SECTION * * SRoot,SECTION * * STop);

};


typedef struct 
{
	SECTION *  Section;
	KEY_NODE * Key;
} SEARCH_DATA;


class IFS : public INIFILE
{

friend FILE_SERVICE;

protected:

	SECTION * SectionSNode;

	SECTION * SectionRoot;
	SECTION * SectionTop;

	//KEY_NODE * FindKey(KEY_NODE * from, char * section_name, char * key_name, char * key_value);
	KEY_NODE * FindKey(char * section_name, char * key_name, char * key_value);
	KEY_NODE * FindKey(char * section_name, char * key_name);
	SECTION * FindSection(char * section_name);
	SECTION * FindSection(char * section_name, SECTION * snode);
	
	void Format(char * file_data, long file_size);
	bool VoidSym(char symbol);
	
	
	VFILE_SERVICE * fs;
	char * FileName;
	bool bDataChanged;
	dword Reference;
	dword CompareStrings(char * s1, char * s2);
	bool FlushFile();

public:

	

	 IFS(VFILE_SERVICE * _fs);
	~IFS();

	bool LoadFile(const char * file_name);
	char * GetFileName() {return FileName;};
	void IncReference();
	void DecReference();
	long GetReference();

	SECTION * CreateSection(char * section_name);
	void DeleteSection(char * section_name);

	bool TestKey(char * section_name, char * key_name, char * key_value);	
	void DeleteKey(char * section_name, char * key_name);
	void DeleteKey(char * section_name, char * key_name, char * key_value);

	bool ReadString(SEARCH_DATA * sd, char * section_name, char * key_name, char * buffer, dword buffer_size, char * def_string);
	void ReadString(SEARCH_DATA * sd, char * section_name, char * key_name, char * buffer, dword buffer_size); // rise exception on fault
	bool ReadStringNext(SEARCH_DATA * sd, char * section_name, char * key_name, char * buffer, dword buffer_size);

	long GetLong(SEARCH_DATA * sd, char * section_name, char * key_name);
	long GetLong(SEARCH_DATA * sd, char * section_name, char * key_name, long def_val);
	bool GetLongNext(SEARCH_DATA * sd, char * section_name, char * key_name, long * val);

	double GetDouble(SEARCH_DATA * sd, char * section_name, char * key_name);
	double GetDouble(SEARCH_DATA * sd, char * section_name, char * key_name, double def_val);
	bool   GetDoubleNext(SEARCH_DATA * sd, char * section_name, char * key_name, double * val);

	float GetFloat(SEARCH_DATA * sd, char * section_name, char * key_name);
	float GetFloat(SEARCH_DATA * sd, char * section_name, char * key_name, float def_val);
	bool  GetFloatNext(SEARCH_DATA * sd, char * section_name, char * key_name, float * val);

	void AddString(char * section_name, char * key_name, char * string);
	void WriteString(char * section_name, char * key_name, char * string);
	void WriteLong(char * section_name, char * key_name, long value);
	void WriteDouble(char * section_name, char * key_name,double value);
	void WriteFloat(char * section_name, char * key_name,float value);

	bool GetSectionName(char * section_name_buffer, long buffer_size);
	bool GetSectionNameNext(char * section_name_buffer, long buffer_size);
	void Flush();
	bool Reload();
	bool TestSection(char * section_name);

};

