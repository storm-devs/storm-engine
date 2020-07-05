#ifndef _SERVICES_LIST_H_
#define _SERVICES_LIST_H_

#include "..\..\common_h\service.h"
#include "..\..\common_h\memop.h"

typedef struct
{
	dword reference;
	dword module_code;
	dword class_code;
	SERVICE * pointer;
	void * linkL;
	void * linkR;
} SERVICE_NODE;

class SERVICES_LIST
{
	SERVICE_NODE * List;
	dword Objects;
	dword Search_module_code;
public:
	 SERVICES_LIST();
	~SERVICES_LIST();
	
	void  Release(); 
	SERVICE_NODE * FindNode(dword module_code);
	SERVICE_NODE * FindNode(SERVICE * sp);
	SERVICE * Find(dword module_code);
	SERVICE * Find(dword module_code, dword class_code);
	SERVICE * AddRef(dword module_code);
	SERVICE * Add(dword module_code, dword class_code, SERVICE * pointer);
	dword Release(dword module_code, dword class_code);
	SERVICE * GetService();
	SERVICE * GetServiceNext();

	SERVICE * GetService(dword & class_code);
	SERVICE * GetServiceNext(dword & class_code);
	dword GetCount() {return Objects;};
	dword GetRef(SERVICE * sp);
	void  SetRef(SERVICE * sp, dword ref);
};

#endif