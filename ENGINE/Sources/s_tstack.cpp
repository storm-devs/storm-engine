#include <memory.h> 
#include "s_tstack.h"
#include "..\..\common_h\memop.h"


S_TSTACK::S_TSTACK()
{
	Unit_size = 0;
	pStackData = 0;
	Buffer_size = 0;
	Data_num = 0;
}

void S_TSTACK::SetUnitSize(unsigned long unit_size)
{
	Unit_size = unit_size;
}

S_TSTACK::~S_TSTACK()
{
	Release();
}

void S_TSTACK::Release()
{
	if(pStackData) delete pStackData;
	pStackData = 0;
	Buffer_size = 0;
	Data_num = 0;
}

bool S_TSTACK::Push(char * pUnit)
{
	if(pUnit == 0) return false;
	if(Data_num > TSTACK_BUFFER_LIMIT) throw "stack overflaw";
	if(Data_num >= Buffer_size)
	{
		Buffer_size += TSTACK_BUFFER_BLOCK_SIZE;
		pStackData = (char*)RESIZE(pStackData,Buffer_size*Unit_size);
	}
	memcpy(pStackData + Data_num*Unit_size,pUnit,Unit_size);
	Data_num++;
	return true;
}

bool S_TSTACK::Pop(char * pUnit)
{
	if(pUnit == 0) throw "invalid 'pUnit'";
	if(Data_num == 0) return false;
	Data_num--;
	memcpy(pUnit,pStackData + Data_num*Unit_size,Unit_size);
	return true;
}

bool S_TSTACK::Read(char * pUnit)
{
	if(pUnit == 0) throw "invalid 'pUnit'";
	if(Data_num == 0) return false;
	memcpy(pUnit,pStackData + (Data_num - 1)*Unit_size,Unit_size);
	return true;
}