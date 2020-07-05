#include "ifs.h"

#include "..\..\common_h\exs.h"
#include <stdio.h>
#include "..\..\common_h\memop.h"
#include "..\..\common_h\vmodule_api.h"

#define		COMMENT			';'
#define		SECTION_A		'['
#define		SECTION_B		']'
#define		INI_EQUAL		'='
#define		VOIDSYMS_NUM	2
const char	INI_LINEFEED[3] = {0xd,0xa,0};
const char	INI_VOIDSYMS[VOIDSYMS_NUM] = {0x20,0x9};

KEY_NODE::KEY_NODE()
{
	key_name = 0;
	key_val = 0;
	name_size = 0;
	val_size = 0;
	l_PTR = 0;
	r_PTR = 0;
	flags = 0;
}

KEY_NODE::~KEY_NODE()
{
	if(key_name) delete key_name;
	if(key_val) delete key_val;
	//if(l_PTR) delete l_PTR;
	//if(r_PTR) delete r_PTR;
}

void KEY_NODE::SetName(char * name)
{
	if(name == 0) return;
	if(key_name) delete key_name; key_name = 0;
	name_size = strlen(name) + 1;

	key_name = NEW char[name_size];
	if(key_name == 0) THROW;
	strcpy(key_name,name);
}

void KEY_NODE::SetValue(char * value)
{
	if(value == 0) return;
	if(key_val) delete key_val; key_val = 0;
	val_size = strlen(value) + 1;

	key_val = NEW char[val_size];
	if(key_val == 0) THROW;
	strcpy(key_val,value);
}

char * KEY_NODE::GetName()
{
	return key_name;
}

char * KEY_NODE::GetValue()
{
	return key_val;
}

KEY_NODE * KEY_NODE::GetLeftNode()
{
	return l_PTR;
}

KEY_NODE * KEY_NODE::GetRightNode()
{
	return r_PTR;
}

void KEY_NODE::SetLeftNode(KEY_NODE * node)
{
	l_PTR = node;
}

void KEY_NODE::SetRightNode(KEY_NODE * node)
{
	r_PTR = node;
}

void KEY_NODE::Add(KEY_NODE * * Root,KEY_NODE * * Top)
{
	if(*Root == 0) 
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

void KEY_NODE::AttachTo(KEY_NODE * node, KEY_NODE * * Top)
{
	KEY_NODE * RNode;
	if(node == 0) throw "invalid node";
	// save right node for object
	RNode = node->GetRightNode();
	// attach this node
	node->SetRightNode(this);
	// set left link for this node
	SetLeftNode(node);
	// set right link from previous node
	SetRightNode(RNode);
	// if there is node from the right - set left link for it
	if(RNode != 0) RNode->SetLeftNode(this);
	// else this is new top node
	else *Top = this;
}

void KEY_NODE::Deattach(KEY_NODE * * Root,KEY_NODE * * Top)
{
	if(l_PTR) l_PTR->SetRightNode(r_PTR);
	if(r_PTR) r_PTR->SetLeftNode(l_PTR);
	if(*Root == this) *Root = r_PTR;
	if(*Top == this) *Top = l_PTR;
}

DWORD KEY_NODE::SetFlags(DWORD _flags)
{
	flags = flags|_flags;
	return flags;
}

//=============================================================================================================

SECTION::SECTION()
{
	Root = 0;
	Top = 0;
	Name = 0;
	l_PTR = 0;
	r_PTR = 0;
}

SECTION::~SECTION()
{
	KEY_NODE * old_root;
	while(Root)
	{
		old_root = Root;
		Root->Deattach(&Root,&Top);
		delete old_root;
	}
	if(Name) delete Name;
}

void SECTION::SetName(char * name)
{
	if(Name) delete Name;
	if(name == 0)
	{
		Name = 0;
	}
	else
	{

		Name = NEW char[strlen(name)+1];
		if(Name == 0) THROW;
		strcpy(Name,name);
	}
}

char * SECTION::GetName()
{
	return Name;
}


KEY_NODE * SECTION::AddNode()
{
	KEY_NODE * node;

	node = NEW KEY_NODE;
	if(node == 0) throw "node creation error";
	node->Add(&Root,&Top);
	return node;
}

void SECTION::DelNode(KEY_NODE * _node)
{
	KEY_NODE * node;
	node = Root;
	while(node)
	{
		if(node == _node)
		{
			node->Deattach(&Root,&Top);
			delete node;
			return;
		}
		node = node->GetRightNode();
	}
}

KEY_NODE * SECTION::FindKey(KEY_NODE * from, char * key_name, char * key_value)
{
	KEY_NODE * node;
	DWORD flags;
	char * char_PTR;

	if(Root == 0) return 0;

	if(key_name == 0) return 0;

	if(from == 0) node = Root;	else node = from;
	
	while(node)
	{
		flags = node->SetFlags(0);
		if(flags & KNF_KEY)
		{
			if(stricmp(key_name,node->GetName()) == 0)
			{
				if(key_value == 0) return node;

				char_PTR = node->GetValue();
				if(char_PTR != 0)
				{
					if(stricmp(key_value,char_PTR) == 0) return node;
				}
			}
		}
		node = node->GetRightNode();
	}
	return 0;
}

KEY_NODE * SECTION::FindKey(char * key_name, char * key_value)
{
	return FindKey(Root,key_name,key_value);
}

KEY_NODE * SECTION::FindKey(char * key_name)
{
	return FindKey(Root,key_name,0);
}

SECTION * SECTION::GetLeftNode()
{
	return l_PTR;
}

SECTION * SECTION::GetRightNode()
{
	return r_PTR;
}

void SECTION::SetLeftNode(SECTION * node)
{
	l_PTR = node;
}

void SECTION::SetRightNode(SECTION * node)
{
	r_PTR = node;
}

void SECTION::Add(SECTION * * SRoot,SECTION * * STop)
{
	if(*SRoot == 0) 
	{
		*SRoot = this;
		*STop = this;
		return;
	}
	(*STop)->SetRightNode(this);
	SetLeftNode(*STop);
	*STop = this;
}

void SECTION::Deattach(SECTION * * SRoot,SECTION * * STop)
{
	if(l_PTR) l_PTR->SetRightNode(r_PTR);
	if(r_PTR) r_PTR->SetLeftNode(l_PTR);
	if(*SRoot == this) *SRoot = r_PTR;
	if(*STop == this) *STop = l_PTR;
}

KEY_NODE * SECTION::GetRoot()
{
	return Root;
}

//=============================================================================================================

IFS::IFS(VFILE_SERVICE * _fs)
{
	fs = _fs;
	FileName = 0;
	bDataChanged = false;
	Reference = 0;
	SectionRoot = 0;
	SectionTop = 0;
	SectionSNode = 0;
}

IFS::~IFS()
{
	SECTION * old_root;
	FlushFile();
	if(FileName) delete FileName;
	while(SectionRoot)
	{
		old_root = SectionRoot;
		SectionRoot->Deattach(&SectionRoot,&SectionTop);
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
	dword n;
	for(n=0;n<VOIDSYMS_NUM;n++)
	{
		if(symbol == INI_VOIDSYMS[n]) return true;
	}
	return false;
}


bool IFS::LoadFile(const char * _file_name)
{
	HANDLE fh;
	dword dwR;
	dword file_size;
	dword name_size;
	char * file_data;
	
	if(_file_name == null) return false;
	fh = fs->_CreateFile(_file_name,GENERIC_READ,FILE_SHARE_READ,OPEN_EXISTING);
	if(fh == INVALID_HANDLE_VALUE) return false;

	file_size = fs->_GetFileSize(fh,0);
	if(file_size == INVALID_FILE_SIZE) {fs->_CloseHandle(fh); return false;}
	

	file_data = NEW char[file_size + 1];	// +1 for zero at the end
	if(file_data == 0) {fs->_CloseHandle(fh); return false;}
	file_data[file_size] = 0;
	
	fs->_ReadFile(fh,file_data,file_size,&dwR);
	if(file_size != dwR) {delete file_data; fs->_CloseHandle(fh); return false;}

	fs->_CloseHandle(fh);

	name_size = strlen(_file_name);

	FileName = NEW char[name_size + 1];

	if(FileName == null) {delete file_data; fs->_CloseHandle(fh); return false;}
	FileName[name_size] = 0;
	strcpy(FileName,_file_name);
	
	Format(file_data,file_size + 1);

	delete file_data;

	return true;
}

void IFS::Format(char * file_data, long file_size)
{
	long n,i,z;
	long lines;
	long backcount;
	long forecount;
	char * data_PTR;
	//bool section_header;
	bool keyval_found;
	bool keyname_set;
	KEY_NODE * node;
	SECTION * Current_Section;
	


	Current_Section = NEW SECTION;
	Current_Section->Add(&SectionRoot,&SectionTop);

	data_PTR = 0;
	lines = 0;

	// terminate each line by zero symbol
	for(n = 0;n < file_size; n++)
	{
		if(file_data[n] == INI_LINEFEED[0] || file_data[n] == INI_LINEFEED[1] || file_data[n] == INI_LINEFEED[2]) 
		{
			file_data[n] = 0;
			if(file_data[n-1] != 0) lines++;
		}
	}

	long offset;

	for(n = 0; n < file_size; n++)
	{
		if(n == 0)
		{
			data_PTR = file_data;
		}
		else
		{
			if(file_data[n] != 0) continue;
			if((n + 1) >= file_size) break;		// end of file
			if(file_data[n + 1] == 0) continue; // separator zero
			data_PTR = file_data + n + 1;
		}
		
		

		// add new key object ------------------------------------------------------
		// (file_data + n + 1) - start of the line
		
		
		
		offset = 0;	for(;data_PTR[offset];offset++){}

		for(i = 0; data_PTR[i]; i++)
		{
			if(VoidSym(data_PTR[i])) continue;	// skip void syms

			if(data_PTR[i] == SECTION_A)
			{ 
				// section header
				for(z = i + 1; data_PTR[z]; z++)
				{
					if(data_PTR[z] == SECTION_B)
					{
						data_PTR[z] = 0;
						//node->SetName(&data_PTR[i+1]);
						//node->SetFlags(KNF_SECTION);

						Current_Section = NEW SECTION;
						Current_Section->Add(&SectionRoot,&SectionTop);
						Current_Section->SetName(&data_PTR[i+1]);
						break; 
					}
				}
				break;
			}

			node = Current_Section->AddNode();
			//node = new KEY_NODE;
			//if(node == 0) throw "node creation error";
			//node->Add(&Root,&Top);

			if(data_PTR[i] == COMMENT) 
			{ 
				// add as commentary
				node->SetName(&data_PTR[i]);
				node->SetFlags(KNF_COMMENTARY);
				break; 
			}

			keyname_set = false;

			// this is real key
			node->SetFlags(KNF_KEY);

			for(z = 0; data_PTR[z]; z++)
			{
				if(data_PTR[z] == INI_EQUAL)
				{
					data_PTR[z] = 0;
					for(backcount = (z - 1);backcount > 0; backcount--)
					{
						if(VoidSym(data_PTR[backcount])) data_PTR[backcount] = 0;
						else break;
					}
					node->SetName(&data_PTR[i]);	// set key name
					keyname_set = true;

					keyval_found = false;
					z++;
					for(;data_PTR[z];z++)
					{
						if(VoidSym(data_PTR[z])) continue;
						else 
						{
							keyval_found = true;
							break;
						}
					}
					for(forecount = z; data_PTR[forecount];forecount++){};
					for(backcount = (forecount - 1);backcount>0;backcount--)
					{
						if(VoidSym(data_PTR[backcount])) data_PTR[backcount] = 0;
						else break;
					}
					if(keyval_found)
					{
						node->SetValue(&data_PTR[z]);

					}
					break;
				}
			}
			if(!keyname_set) node->SetName(&data_PTR[i]);	// key without value
			break;
		}
		n += offset;
	}
}

bool IFS::FlushFile()
{
	GUARD(bool IFS::FlushFile())
	dword dwR;
	dword write_size;
	char  buff[2];
	HANDLE fh;

	if(bDataChanged == false) return true;
	
	fs->_SetFileAttributes(FileName,FILE_ATTRIBUTE_NORMAL);
	fs->_DeleteFile(FileName);
	fh = fs->_CreateFile(FileName,GENERIC_WRITE,FILE_SHARE_READ,CREATE_ALWAYS);
	if(fh == INVALID_HANDLE_VALUE) {/*trace("file: (%s)",FileName);*/ _THROW(cant create file);}

	KEY_NODE * node;
	SECTION * section_node;
	DWORD flags;

	//node = Root;

	section_node = SectionRoot;
	while(section_node)
	{
		if(section_node->GetName() != 0)
		{
			// write section name -----------------------------------------------------------------
			buff[0] = SECTION_A; fs->_WriteFile(fh,buff,1,&dwR);
			if(dwR != 1) { THROW;}

			write_size = strlen(section_node->GetName());
			fs->_WriteFile(fh,section_node->GetName(),write_size,&dwR);
			if(dwR != write_size) { THROW;}

			buff[0] = SECTION_B; fs->_WriteFile(fh,buff,1,&dwR);
			if(dwR != 1) { THROW;}

			buff[0] = INI_LINEFEED[0]; buff[1] = INI_LINEFEED[1]; fs->_WriteFile(fh,buff,2,&dwR);
			if(dwR != 2) { THROW;}

		}
		
		node = section_node->GetRoot();
		while(node)
		{
			flags = node->SetFlags(0);
			if(flags & KNF_COMMENTARY)
			{
				// write commented line ---------------------------------------------------------------
				write_size = strlen(node->GetName());
				fs->_WriteFile(fh,node->GetName(),write_size,&dwR);
				if(dwR != write_size) { THROW;}
				buff[0] = INI_LINEFEED[0]; buff[1] = INI_LINEFEED[1]; fs->_WriteFile(fh,buff,2,&dwR);
				if(dwR != 2) { THROW;}

			}
			else
			if(flags & KNF_KEY)
			{
				// write key -------------------------------------------------------------------------
				write_size = strlen(node->GetName());
				fs->_WriteFile(fh,node->GetName(),write_size,&dwR);
				if(dwR != write_size) { THROW;}
				if(node->GetValue() != 0)
				{
					fs->_WriteFile(fh,&INI_VOIDSYMS[0],1,&dwR);
					if(dwR != 1) { THROW;}
					buff[0] = INI_EQUAL; fs->_WriteFile(fh,buff,1,&dwR);
					if(dwR != 1) { THROW;}
					fs->_WriteFile(fh,&INI_VOIDSYMS[0],1,&dwR);
					if(dwR != 1) { THROW;}
					write_size = strlen(node->GetValue());
					fs->_WriteFile(fh,node->GetValue(),write_size,&dwR);
					if(dwR != write_size) { THROW;}

				}
				buff[0] = INI_LINEFEED[0]; buff[1] = INI_LINEFEED[1]; fs->_WriteFile(fh,buff,2,&dwR);
				if(dwR != 2) { THROW;}

			}
			else throw "invalid key flag";
			node = node->GetRightNode();
		}
		section_node = section_node->GetRightNode();

		buff[0] = INI_LINEFEED[0]; buff[1] = INI_LINEFEED[1]; fs->_WriteFile(fh,buff,2,&dwR);
		if(dwR != 2) { THROW;}
	}


	fs->_CloseHandle(fh);

	UNGUARD
	return false;
}


KEY_NODE * IFS::FindKey(char * section_name, char * key_name)
{
	return FindKey(section_name,key_name,0);
}

KEY_NODE * IFS::FindKey(char * section_name, char * key_name, char * key_value)
{
	SECTION * snode;
	if(SectionRoot == 0) return false;
	snode = FindSection(section_name);
	if(snode == 0) return false;
	return snode->FindKey(key_name,key_value);


	//return FindKey(0,section_name,key_name,key_value);
}

/*KEY_NODE * IFS::FindKey(KEY_NODE * from, char * section_name, char * key_name, char * key_value)
{
	SECTION * snode;
	if(SectionRoot == 0) return false;
	snode = FindSection(section_name);
	if(snode == 0) return false;
	return snode->FindKey(from,key_name,key_value);
}*/

SECTION * IFS::FindSection(char * section_name)
{
	SECTION * node;

	if(SectionRoot == 0) return 0;
	node = SectionRoot;

	while(node)
	{
		if(section_name == 0)
		{
			if(node->GetName() == 0) return node;
		}
		if(node->GetName() != 0)
		if(stricmp(section_name,node->GetName()) == 0) return node;
		node = node->GetRightNode();
	}
	return 0;
}

SECTION * IFS::FindSection(char * section_name, SECTION * snode)
{
	SECTION * node;

	if(SectionRoot == 0) return 0;
	
	// atempt to search by section node pointer
	node = SectionRoot;
	while(node)
	{
		if(node == snode)
		{
			// if node exist and name is correct return ok
			if(section_name != 0)
			{
				if(stricmp(section_name,node->GetName()) == 0) return node;
			} else
			{
				if(node->GetName() == 0) return node;
			}
			break;
		}
		
		node = node->GetRightNode();
	}

	// full search by name
	node = SectionRoot;
	while(node)
	{
		if(section_name == 0)
		{
			if(node->GetName() == 0) return node;
		}
		if(node->GetName() != 0)
		if(stricmp(section_name,node->GetName()) == 0) return node;
		node = node->GetRightNode();
	}
	return 0;
}

SECTION * IFS::CreateSection(char * section_name)
{
	SECTION * node;
	node = FindSection(section_name);
	if(node) return node;

	node = NEW SECTION;
	if(node == 0) throw "section creation error";
	node->Add(&SectionRoot,&SectionTop);
	node->SetName(section_name);
	bDataChanged = true;
	return node;
}

void IFS::DeleteSection(char * section_name)
{
	SECTION * node;
	node = FindSection(section_name);
	if(!node) return;
	if(SectionSNode == node)
	{
		SectionSNode = SectionSNode->GetRightNode(); 
	}
	node->Deattach(&SectionRoot,&SectionTop);
	delete node;
	bDataChanged = true;
}

bool IFS::TestSection(char * section_name)
{
	SECTION * node;
	node = FindSection(section_name);
	if(node) return true;
	return false;
}



bool IFS::TestKey(char * section_name, char * key_name, char * key_value)
{
	SECTION * node;
	node = FindSection(section_name);
	if(node == 0) return false;
	if(node->FindKey(key_name,key_value) != 0) return true;
	return false;
}

void IFS::DeleteKey(char * section_name, char * key_name)
{
	DeleteKey(section_name,key_name,0);
}

void IFS::DeleteKey(char * section_name, char * key_name, char * key_value)
{
	SECTION * node;
	KEY_NODE * knode;
	node = FindSection(section_name);
	if(node)
	{
		knode = node->FindKey(key_name,key_value);
		if(knode) 
		{
			node->DelNode(knode);
			bDataChanged = true;
		}
	}
}

void IFS::ReadString(SEARCH_DATA * sd, char * section_name, char * key_name, char * buffer, dword buffer_size)
{
	ReadString(sd,section_name,key_name,buffer,buffer_size,0);
}

bool IFS::ReadString(SEARCH_DATA * sd, char * section_name, char * key_name, char * buffer, dword buffer_size, char * def_string)
{
	KEY_NODE * node;
	dword write_size;
	char * char_PTR;

	node = FindKey(section_name,key_name);
	if(node == 0)
	{
		sd->Key = 0;
		sd->Section = 0;
		if(def_string == 0)
		{
			_CORE_API->Trace("Warning! IniFile Read String: section=%s, key=%s",section_name,key_name);
			if(buffer) buffer[0] = 0;
			//_THROW(string not found);
		}else
		if(buffer) strcpy(buffer,def_string);
		return false;
	}
	
	sd->Key = node;
	sd->Section = FindSection(section_name);

	if(buffer == 0) _THROW(zero buffer);
	char_PTR = node->GetValue();
	if(char_PTR == 0) 
	{
		if(def_string == 0) _THROW(no key value);
		strcpy(buffer,def_string);
		return false;
	}

	write_size = strlen(char_PTR) + 1;
	//if(write_size > buffer_size) _THROW(buffer size too small); // boal закоменчено по наводке Ёдди, не давало грузить новую ани

	strcpy(buffer,node->GetValue());
	return true;
}

bool IFS::ReadStringNext(SEARCH_DATA * sd, char * section_name, char * key_name, char * buffer, dword buffer_size)
{
	SECTION * snode;
	KEY_NODE * node;
	dword write_size;
	char * char_PTR;
	bool start;


	snode = FindSection(section_name,sd->Section);
	//snode = sd->Section;
	if(snode == 0) return false;

	start = false;
	node = snode->GetRoot();
	//node = sd->Key;
	while(node)
	{
		if(start == true)
		{
			//if(CompareStrings(node->GetName(),key_name) == 0)
			if(stricmp(node->GetName(),key_name) == 0) 
			{
				if(buffer == 0) _THROW(zero buffer);

				char_PTR = node->GetValue();
				if(char_PTR == 0) 
				{
					buffer[0] = 0;
					return true;
					//_THROW(no key value);
				}

				write_size = strlen(char_PTR) + 1;
				if(write_size > buffer_size) _THROW(buffer size too small);

				strcpy(buffer,node->GetValue());
				sd->Key = node;
				sd->Section = snode;
				return true;
			}
		}
		if(sd->Key == node) start = true;
		node = node->GetRightNode();
	}
	sd->Key = 0;
	sd->Section = 0;
	return false;
}


long IFS::GetLong(SEARCH_DATA * sd, char * section_name, char * key_name)
{
	char buffer[256];
	ReadString(sd,section_name,key_name,buffer,sizeof(buffer));
	return atol(buffer);
}

long IFS::GetLong(SEARCH_DATA * sd, char * section_name, char * key_name, long def_val)
{
	char buffer[256];
	if(ReadString(sd,section_name,key_name,buffer,sizeof(buffer),"")) return atol(buffer);
	return def_val;
}

bool IFS::GetLongNext(SEARCH_DATA * sd, char * section_name, char * key_name, long * val)
{
	char buffer[256];
	if(ReadStringNext(sd,section_name,key_name,buffer,sizeof(buffer)))
	{
		*val = atol(buffer);
		return true;
	}
	return false;
}

double IFS::GetDouble(SEARCH_DATA * sd, char * section_name, char * key_name)
{
	char buffer[256];
	ReadString(sd,section_name,key_name,buffer,sizeof(buffer));
	return atof(buffer);
}

double IFS::GetDouble(SEARCH_DATA * sd, char * section_name, char * key_name, double def_val)
{
	char buffer[256];
	if(ReadString(sd,section_name,key_name,buffer,sizeof(buffer),"")) return atof(buffer);
	return def_val;
}

bool IFS::GetDoubleNext(SEARCH_DATA * sd, char * section_name, char * key_name, double * val)
{
	char buffer[256];
	if(ReadStringNext(sd,section_name,key_name,buffer,sizeof(buffer)))
	{
		*val = atof(buffer);
		return true;
	}
	return false;
}

float IFS::GetFloat(SEARCH_DATA * sd, char * section_name, char * key_name)
{
	char buffer[256];
	ReadString(sd,section_name,key_name,buffer,sizeof(buffer));
	return (float)atof(buffer);
}

float IFS::GetFloat(SEARCH_DATA * sd, char * section_name, char * key_name, float def_val)
{
	char buffer[256];
	if(ReadString(sd,section_name,key_name,buffer,sizeof(buffer),"")) return (float)atof(buffer);
	return def_val;
}

bool IFS::GetFloatNext(SEARCH_DATA * sd, char * section_name, char * key_name, float * val)
{
	char buffer[256];
	if(ReadStringNext(sd,section_name,key_name,buffer,sizeof(buffer)))
	{
		*val = (float)atof(buffer);
		return true;
	}
	return false;
}

void IFS::AddString(char * section_name, char * key_name, char * string)
{
	KEY_NODE * node;
	SECTION * snode;
	
	if(key_name == 0) _THROW(zero key);
	snode = FindSection(section_name);
	if(snode == 0) CreateSection(section_name);
	snode = FindSection(section_name);
	if(snode == 0) _THROW(section create error);

	node = snode->AddNode();
	node->SetName(key_name);
	if(string) node->SetValue(string);
	node->SetFlags(KNF_KEY);
	bDataChanged = true;

}

void IFS::WriteString(char * section_name, char * key_name, char * string)
{
	KEY_NODE * node;
	SECTION * snode;
	if(string == 0) _THROW(zero key value);

	snode = CreateSection(section_name);
	if(snode == 0) _THROW(section create error);
	node = snode->FindKey(key_name);
	if(node != 0) 
	{
		node->SetValue(string);
		bDataChanged = true;
		return;
	}
	AddString(section_name,key_name,string);
}

void IFS::WriteLong(char * section_name, char * key_name, long value)
{
	char buffer[256];
	ltoa(value,buffer,10);
	WriteString(section_name,key_name,buffer);
}

void IFS::WriteDouble(char * section_name, char * key_name,double value)
{
	char buffer[256];
	sprintf(buffer,"%g",value);
	WriteString(section_name,key_name,buffer);
}

void IFS::WriteFloat(char * section_name, char * key_name,float value)
{
	char buffer[256];
	sprintf(buffer,"%g",value);
	WriteString(section_name,key_name,buffer);
}

dword IFS::CompareStrings(char * s1, char * s2)
{
	dword n;
	if(s1 == 0 || s2 == 0) return 1;
	n = 0;
	while(s1[n] == s2[n]) { if(s1[n] == 0) return 0; n++;}
	return 1;
}

bool IFS::GetSectionName(char * section_name_buffer, long buffer_size)
{
	SECTION * node;
	long len;
	if(SectionRoot == 0) return false;
	node = SectionRoot;
	node = node->GetRightNode();	// skip zero section (unnamed)
	if(node == 0) return false;

	if(section_name_buffer == 0) throw "zero buffer";
	len = strlen(node->GetName());
	if(len > buffer_size) throw "buffer too small";
	strcpy(section_name_buffer,node->GetName());
	SectionSNode = node;
	return true;
}

bool IFS::GetSectionNameNext(char * section_name_buffer, long buffer_size)
{
	SECTION * node;
	long len;
	if(SectionRoot == 0) return false;
	if(section_name_buffer == 0) throw "zero buffer";
	node = SectionRoot;
	while(node)
	{
		if(node == SectionSNode)
		{
			node = node->GetRightNode();
			if(node == 0) 
			{
				SectionSNode = 0;
				return false;
			}
			len = strlen(node->GetName());
			if(len > buffer_size) throw "buffer too small";
			strcpy(section_name_buffer,node->GetName());
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
	SECTION * old_root;
	while(SectionRoot)
	{
		old_root = SectionRoot;
		SectionRoot->Deattach(&SectionRoot,&SectionTop);
		delete old_root;
	}
	bDataChanged = false;
	//Reference = 0;
	SectionRoot = 0;
	SectionTop = 0;
	SectionSNode = 0;
	return LoadFile(FileName);
}