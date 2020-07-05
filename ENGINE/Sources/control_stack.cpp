#include "control_stack.h"
#include "..\..\common_h\memop.h"
#include "..\..\common_h\exs.h"
#include "externs.h"


CONTROL_STACK::CONTROL_STACK() {initiate_blocks = 0; Data_PTR = null; used_blocks = 0;};
CONTROL_STACK::	~CONTROL_STACK()
{
	if(Data_PTR) delete Data_PTR;
};
bool CONTROL_STACK::Init() 
{
	Release();
	initiate_blocks = DEFAULT_CONTROL_STACK_SIZE;

	Data_PTR = (void *) NEW char[CONTROL_BLOCK_SIZE*initiate_blocks];
	if(Data_PTR == null) return false;
	return true;
};
void CONTROL_STACK::Release() 
{
	if(Data_PTR) delete Data_PTR; 
	initiate_blocks = 0; 
	Data_PTR = null; 
	used_blocks = 0;
};
void CONTROL_STACK::Clear() {used_blocks = 0;};
dword CONTROL_STACK::DataNum() {return used_blocks;};
bool CONTROL_STACK::Push(void * pointer, dword class_code, dword ctp)
{
	//trace("PUSH: %d class: %d ctp: %d",pointer,class_code,ctp);
	CONTROL_BLOCK * cb_PTR;
	char * mem_PTR;
	//if(pointer) Memory_Service.Lock(pointer);
	if(Data_PTR == null) return false;
	if(used_blocks >= initiate_blocks)
	{
		initiate_blocks = initiate_blocks*2;
		Data_PTR = RESIZE(Data_PTR,CONTROL_BLOCK_SIZE*initiate_blocks);
		if(Data_PTR == null) return false;
	}
	mem_PTR = (char*)Data_PTR + used_blocks*CONTROL_BLOCK_SIZE;
	cb_PTR = (CONTROL_BLOCK *)mem_PTR;
	cb_PTR->class_code = class_code;
	cb_PTR->pointer = pointer;
	cb_PTR->ctp = ctp;
	used_blocks++;
	return true;
};
bool CONTROL_STACK::Pop(CONTROL_BLOCK * cb_PTR)
{
	GUARD(CONTROL_STACK::Pop)
	char * mem_PTR;
	CONTROL_BLOCK * b_PTR;
	if(used_blocks == 0)
	{
		_THROW(invalid pop);
	}
	if(Data_PTR == null) return false;
	used_blocks--;
	b_PTR = (CONTROL_BLOCK *)((char *)Data_PTR + used_blocks*CONTROL_BLOCK_SIZE);
	//trace(" POP: %d class: %d ctp: %d",b_PTR->pointer,b_PTR->class_code,b_PTR->ctp);
	if(cb_PTR != null)
	{
		mem_PTR = (char*)Data_PTR + used_blocks*CONTROL_BLOCK_SIZE;
		memcpy(cb_PTR,mem_PTR,CONTROL_BLOCK_SIZE);
		
	}	
//	if(b_PTR->pointer) Memory_Service.Unlock(b_PTR->pointer);
	UNGUARD
	return true;
};
bool CONTROL_STACK::ScanPointer(void * pointer)
{
	CONTROL_BLOCK * cb_PTR;
	char * mem_PTR;
	dword n;
	if(used_blocks == 0) return false;
	if(Data_PTR == null) return false;
	for(n=0;n<used_blocks;n++)
	{
		mem_PTR = (char*)Data_PTR + n*CONTROL_BLOCK_SIZE;
		cb_PTR = (CONTROL_BLOCK *)mem_PTR;
		if(pointer == cb_PTR->pointer) 
			return true;
	}
	return false;
};
bool CONTROL_STACK::ScanClass(dword class_code)
{
	CONTROL_BLOCK * cb_PTR;
	char * mem_PTR;
	dword n;
	if(used_blocks == 0) return false;
	if(Data_PTR == null) return false;
	for(n=0;n<used_blocks;n++)
	{
		mem_PTR = (char*)Data_PTR + n*CONTROL_BLOCK_SIZE;
		cb_PTR = (CONTROL_BLOCK *)mem_PTR;
		if(class_code == cb_PTR->class_code) return true;
	}
	return false;
};
bool CONTROL_STACK::ScanClassCTP(dword class_code,dword ctp_code)
{
	CONTROL_BLOCK * cb_PTR;
	char * mem_PTR;
	dword n;
	if(used_blocks == 0) return false;
	if(Data_PTR == null) return false;
	for(n=0;n<used_blocks;n++)
	{
		mem_PTR = (char*)Data_PTR + n*CONTROL_BLOCK_SIZE;
		cb_PTR = (CONTROL_BLOCK *)mem_PTR;
		if(class_code == cb_PTR->class_code) 
		{
			if(ctp_code == cb_PTR->ctp) return true;
		}
	}
	return false;
};
bool CONTROL_STACK::ScanClassCTPInv(dword class_code,dword ctp_code)
{
	CONTROL_BLOCK * cb_PTR;
	char * mem_PTR;
	dword n;
	if(used_blocks == 0) return false;
	if(Data_PTR == null) return false;
	for(n=0;n<used_blocks;n++)
	{
		mem_PTR = (char*)Data_PTR + n*CONTROL_BLOCK_SIZE;
		cb_PTR = (CONTROL_BLOCK *)mem_PTR;
		if(class_code == cb_PTR->class_code) 
		{
			if(ctp_code != cb_PTR->ctp) return true;
		}
	}
	return false;
};

bool CONTROL_STACK::ScanCTP(dword ctp_code)
{
	CONTROL_BLOCK * cb_PTR;
	char * mem_PTR;
	dword n;
	if(used_blocks == 0) return false;
	if(Data_PTR == null) return false;
	for(n=0;n<used_blocks;n++)
	{
		mem_PTR = (char*)Data_PTR + n*CONTROL_BLOCK_SIZE;
		cb_PTR = (CONTROL_BLOCK *)mem_PTR;
		if(ctp_code == cb_PTR->ctp) return true;
	}
	return false;
};
bool CONTROL_STACK::Read(CONTROL_BLOCK * cb_PTR)
{
	char * mem_PTR;
	if(cb_PTR) memset(cb_PTR,0,sizeof(CONTROL_BLOCK));
	if(used_blocks == 0) return false;
	if(Data_PTR == null) return false;
	mem_PTR = (char*)Data_PTR + (used_blocks - 1)*CONTROL_BLOCK_SIZE;
	if(cb_PTR) memcpy(cb_PTR,mem_PTR,sizeof(CONTROL_BLOCK));
	return true;
};
