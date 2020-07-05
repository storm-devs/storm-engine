#ifndef DATA_DESCRIPTION_CLASS
#define DATA_DESCRIPTION_CLASS

#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include "../../../common_h/exs.h"
#include "../../../common_h/templates.h"
#include "../../icommon/types.h"


#define MAX_DESC_COUNT 128

class DataDescripion
{

	struct DescItem
	{
		FieldType Type;
		const char* Name;
	};

	DWORD ItemsCount;
	DescItem Fields[MAX_DESC_COUNT];

public:
 
 DataDescripion ();
 ~DataDescripion ();
 
 void AddField (FieldType Type, const char* Name);
 
 const char* GetFieldName (DWORD Index);
 FieldType GetFieldType (DWORD Index);
 int GetFieldCount ();

 void Clear ();

 virtual void CreateFields () = 0;
};





#define BEGIN_DATA_DESC(Name)													\
class DataDescripion__##Name : public DataDescripion  \
{																										  \
public:																								\
	virtual void CreateFields ()												\
	{																										\
		Clear ();																					\



#define END_DATA_DESC(Name)														\
	}																										\
	DataDescripion__##Name()														\
	{																										\
		CreateFields ();																	\
	}																										\
};																										\
DataDescripion__##Name  Name;													\




#define DATA_COLOR(name)			AddField(FIELD_COLOR, name);
#define DATA_BOOL(name)				AddField(FIELD_BOOL, name);
#define DATA_FLOAT(name)			AddField(FIELD_FLOAT, name);
#define DATA_GRAPH(name)			AddField(FIELD_GRAPH, name);
#define DATA_POSITION(name)		AddField(FIELD_POSITION, name);
#define DATA_UV(name)					AddField(FIELD_UV, name);
#define DATA_STRING(name)			AddField(FIELD_STRING, name);


#endif