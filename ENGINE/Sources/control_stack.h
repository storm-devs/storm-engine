#ifndef _CONTROL_STACK_H_
#define _CONTROL_STACK_H_

#include "common_h.h"
#include "..\..\common_h\entity_id.h"

typedef struct 
{
	void *	pointer;
	dword	class_code;
	dword	ctp;
} CONTROL_BLOCK;

#define DEFAULT_CONTROL_STACK_SIZE	8
#define CONTROL_BLOCK_SIZE			sizeof(CONTROL_BLOCK)

class CONTROL_STACK
{
	void * Data_PTR;
	dword initiate_blocks;
	dword used_blocks;

public:

	CONTROL_STACK();
	~CONTROL_STACK();
	bool Init();
	void Release();
	void Clear();
	dword DataNum();
	bool Push(void * pointer, dword class_code, dword ctp);
	bool Pop(CONTROL_BLOCK * cb_PTR);
	bool ScanPointer(void * pointer);
	bool ScanClass(dword class_code);
	bool ScanClassCTP(dword class_code,dword ctp_code);
	bool ScanClassCTPInv(dword class_code,dword ctp_code);
	bool ScanCTP(dword ctp_code);
	bool Read(CONTROL_BLOCK * cb_PTR);
};


#endif