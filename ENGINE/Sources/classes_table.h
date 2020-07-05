#ifndef _CLASSES_TABLE_H_
#define _CLASSES_TABLE_H_

#include "strings_list.h"

#define INVALID_CLASS_CODE			INVALID_ORDINAL_NUMBER

typedef struct 
{
	long  module_code;
	long  module_class_id;
	dword real_class_id;
} CLASS_SEARCH_DATA;

class CLASSES_TABLE : public STRINGS_LIST
{
public:
	CLASSES_TABLE(){};
	~CLASSES_TABLE(){};
	dword GetClassCode(char * class_name);
	dword GetModuleCode(dword class_code);
	dword GetClassModuleID(dword class_code);
	CLASS_SEARCH_DATA GetClassData(dword class_code);

};
#endif