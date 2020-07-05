#include "s_classtab.h"
#include <string.h>

S_CLASSTAB::S_CLASSTAB()
{
	pTable = 0;
	Buffer_size = 0;
	nClassesNum = 0;
}

S_CLASSTAB::~S_CLASSTAB()
{
	Release();
}

void  S_CLASSTAB::Release()
{
	dword n,m;
	if(pTable) 
	{
		for(n=0;n<nClassesNum;n++)	
		{ 
			if(pTable[n].pComponent) 
			{
				// delete components names memory (allocated by Compiler)
				for(m=0;m<pTable[n].nComponentsNum;m++) { if(pTable[n].pComponent[m].name) delete pTable[n].pComponent[m].name; }
				// delete components info memory
				delete pTable[n].pComponent;
			}
			if(pTable[n].name) delete pTable[n].name;	
		}
		delete pTable; pTable = 0;
	}
	Buffer_size = 0;
	nClassesNum = 0;
}

bool S_CLASSTAB::GetClass(CLASSINFO& ci,dword class_code)
{
	if(class_code >= nClassesNum) return false;
	if(pTable[class_code].segment_id == INVALID_SEGMENT_ID) return false;
	ci = pTable[class_code];
	return true;
}

bool S_CLASSTAB::GetClassX(CLASSINFO& ci,dword class_code)
{
	if(class_code >= nClassesNum) return false;
	ci = pTable[class_code];
	return true;
}

dword S_CLASSTAB::AddClass(CLASSINFO& ci, bool bRegisterOnly)
{
	dword n;
	dword hash;
	dword nClassN;

	if(ci.name == 0) return INVALID_CLASS_CODE;

	hash = MakeHashValue(ci.name);
	
	for(n=0;n<nClassesNum;n++)
	{
		if(pTable[n].hash == hash)
		{
			if(stricmp(pTable[n].name,ci.name) != 0) continue;

			// class with such name already registred,

			// if we dont care about class components just return class ID
			if(bRegisterOnly) return n;

			// otherwise we want to set class components data
			
			if(!pTable[n].bClassRegistredOnly)
			{
				// but information already set
				// this is 'double class name' error 
				pVCompiler->SetError("Class with name '%s' already declared", ci.name);
				return INVALID_CLASS_CODE;
			}

			// class registred only - set components data
			pTable[n].segment_id = ci.segment_id;
			pTable[n].nComponentsNum = ci.nComponentsNum;
			pTable[n].bClassRegistredOnly = false;
			if(ci.nComponentsNum)
			{

				pTable[n].pComponent = (CLASS_COMPONENT *)NEW CLASS_COMPONENT[ci.nComponentsNum];
				memcpy(pTable[n].pComponent,ci.pComponent,ci.nComponentsNum * sizeof(CLASS_COMPONENT));
			}

			return n;
		}
	}
	// class not found, add a new one
	
	// check for class components description
	if(ci.nComponentsNum && ci.pComponent == 0) return INVALID_CLASS_CODE;
	
	
	// adjust buffer size
	if(nClassesNum >= Buffer_size)
	{
		Buffer_size += CLASS_BUFFER_BLOCK_SIZE;
		pTable = (CLASSINFO *)RESIZE(pTable,Buffer_size*sizeof(CLASSINFO));
	}
	
	nClassN = nClassesNum;
	if(bRegisterOnly)
	{
		pTable[nClassN].segment_id = 0xffffffff;
		pTable[nClassN].nComponentsNum = 0;
		pTable[nClassN].pComponent = 0;
	}
	else
	{
		pTable[nClassN].segment_id = ci.segment_id;
		pTable[nClassN].nComponentsNum = ci.nComponentsNum;
		if(ci.nComponentsNum)
		{

			pTable[nClassN].pComponent = (CLASS_COMPONENT *)NEW CLASS_COMPONENT[ci.nComponentsNum];
			memcpy(pTable[nClassN].pComponent,ci.pComponent,ci.nComponentsNum * sizeof(CLASS_COMPONENT));
		}
	}
	pTable[nClassN].bClassRegistredOnly = bRegisterOnly;
	pTable[nClassN].hash = hash;

	pTable[nClassN].name = NEW char[strlen(ci.name) + 1];
	strcpy(pTable[nClassN].name,ci.name);
	
	nClassesNum++;

	return nClassN;
}

dword S_CLASSTAB::MakeHashValue(const char * string)
{
	dword hval = 0;
	dword g;
	char v;
	while(*string != 0)
	{
		v = *string++;
		if ('A' <= v && v <= 'Z') v += 'a' - 'A';	// case independent
		hval = (hval<<4) + (unsigned long int)v;
		g = hval & ((unsigned long int) 0xf << (32 - 4));
		if(g != 0)
		{
			hval ^= g >> (32 - 8);
			hval ^= g;
		}
	}
	return hval;
}

void S_CLASSTAB::InvalidateBySegmentID(dword segment_id)
{
	dword n;
	for(n=0;n<nClassesNum;n++)
	{
		if(pTable[n].segment_id != segment_id) continue;
		if(pTable[n].pComponent) delete pTable[n].pComponent;
		pTable[n].pComponent = 0;
		pTable[n].nComponentsNum = 0;
		pTable[n].bClassRegistredOnly = true;
		pTable[n].segment_id = INVALID_SEGMENT_ID;
	}
}

dword S_CLASSTAB::FindClass(const char * class_name)
{
	dword n;
	dword hash;
	if(class_name == 0) return INVALID_CLASS_CODE;
	hash = MakeHashValue(class_name);
	for(n=0;n<nClassesNum;n++)
	{
		if(pTable[n].hash == hash) 
		{
			if(stricmp(pTable[n].name,class_name)==0) return n;	// case independent
		}
	}
	return INVALID_CLASS_CODE;
}

bool S_CLASSTAB::IsClassDataSet(dword class_id)
{
	if(class_id >= nClassesNum) return false;
	return !pTable[class_id].bClassRegistredOnly;
}

void S_CLASSTAB::InitSystemClasses()
{
	Release();
	CLASSINFO ci;
	CLASS_COMPONENT cc;
	
	ci.segment_id = 0xffffffff;
	cc.nFlags = CCF_SYSTEMTYPEVARIABLE;
	ci.pComponent = &cc;
	ci.nComponentsNum = 1;
	
	ci.name = "int";
	cc.nID = VAR_INTEGER;
	AddClass(ci);

	ci.name = "float";
	cc.nID = VAR_FLOAT;
	AddClass(ci);

	ci.name = "string";
	cc.nID = VAR_STRING;
	AddClass(ci);

	ci.name = "object";
	cc.nID = VAR_OBJECT;
	AddClass(ci);

}
