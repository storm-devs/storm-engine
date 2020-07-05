#include "classes_table.h"
#include "..\..\common_h\exs.h"
#include "externs.h"
#include "..\..\common_h\memop.h"

dword CLASSES_TABLE::GetClassCode(char * class_name)
{
	dword class_code;
	class_code = GetStringCode(class_name);
	if(class_code == INVALID_CLASS_CODE) if(Exceptions_Mask &_X_NO_CLASS) _THROW(cant find class);
	return class_code;
}

dword CLASSES_TABLE::GetClassModuleID(dword class_code)
{
	CLASS_SEARCH_DATA class_search_data;
	if(!GetStringData(class_code,&class_search_data)) THROW;
	return class_search_data.module_class_id;
}

dword CLASSES_TABLE::GetModuleCode(dword class_code)
{
	CLASS_SEARCH_DATA class_search_data;
	if(!GetStringData(class_code,&class_search_data)) THROW;
	return class_search_data.module_code;
}

CLASS_SEARCH_DATA CLASSES_TABLE::GetClassData(dword class_code)
{
	CLASS_SEARCH_DATA class_search_data;
	if(!GetStringData(class_code,&class_search_data)) THROW;
	return class_search_data;
}