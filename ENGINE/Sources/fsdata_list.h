#ifndef _FSDATA_LIST_H_
#define _FSDATA_LIST_H_

//-------------------------------------------------------------------------------------------------
// Simple class for creating and keeping in memory list of structures with fixed size
// This class can only accumulate data

#include "common_h.h"


class FSDATA_LIST
{
	void * Data_PTR;
	dword structure_size;
	dword initiate_blocks;
	dword used_blocks;

public:

	FSDATA_LIST() {structure_size = 0; initiate_blocks = 0; Data_PTR = null; used_blocks = 0;};
	~FSDATA_LIST(){if(Data_PTR) delete Data_PTR;};
	void Release() {if(Data_PTR) delete Data_PTR; structure_size = 0; initiate_blocks = 0; Data_PTR = null; used_blocks = 0;};
	void Clear() {used_blocks = 0;};
	dword DataNum() {return used_blocks;};
	bool Init(dword _structure_size, dword _initiate_blocks)
	{
		if(Data_PTR) delete Data_PTR; Data_PTR = null;
		used_blocks = 0;
		structure_size = _structure_size;
		if(structure_size == 0) return false;
		initiate_blocks = _initiate_blocks;
		if(initiate_blocks == 0) initiate_blocks = 1;

		Data_PTR = (void *) NEW char[structure_size*initiate_blocks];
		if(Data_PTR == null) return false;
		return true;
	};
	bool AddData(void * _data_ptr)
	{
		char * mem_PTR;
		if(_data_ptr == null) return false;
		if(Data_PTR == null) return false;
		if(used_blocks >= initiate_blocks)
		{
			initiate_blocks = initiate_blocks*2;
			Data_PTR = RESIZE(Data_PTR,structure_size*initiate_blocks);
			if(Data_PTR == null) return false;
		}
		mem_PTR = (char*)Data_PTR + used_blocks*structure_size;
		memcpy(mem_PTR,_data_ptr,structure_size);
		used_blocks++;
		return true;
	};
	bool GetData(dword _n, void * struct_PTR)
	{
		char * mem_PTR;
		if(_n >= used_blocks) return false;
		if(struct_PTR == null) return false;
		if(Data_PTR == null) return false;
		mem_PTR = (char*)Data_PTR + _n*structure_size;
		memcpy(struct_PTR,mem_PTR,structure_size);
		return true;
	};
	void ShiftLeft()
	{
		if (used_blocks==0) return;
		if (Data_PTR == null) return;
		memmove(Data_PTR,((char*)Data_PTR+structure_size),structure_size*(used_blocks-1));
		used_blocks--;
	}
	
};

#endif