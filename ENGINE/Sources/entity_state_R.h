#ifndef _ENTITY_STATE_R_H_
#define _ENTITY_STATE_R_H_

#ifndef _XBOX
#include <windows.h>
#else
#include <xtl.h>
#endif

#include "..\..\common_h\entity_state.h"
#include "..\..\common_h\vfile_service.h"
#include "..\..\common_h\memop.h"

#define INITIAL_BUFFER_SIZE		1024

class ENTITY_STATE_GEN_R : public ENTITY_STATE_GEN
{
	HANDLE file_handle;
	VFILE_SERVICE * fio;
	char * Buffer;
	dword  Buffer_size;
	dword  Data_size;
	char * Format_string;
	va_list args;

	void CopyData(dword add_data_size);
	void VerifyFreeSpace(dword add_data_size);

public:
	ENTITY_STATE_GEN_R();
	virtual ~ENTITY_STATE_GEN_R();
	void Init(VFILE_SERVICE * _fio,HANDLE _file_handle);
	void CloseState();
	void _cdecl SetState(char * Format,...);
};

class ENTITY_STATE_R : public ENTITY_STATE
{
	HANDLE file_handle;
	VFILE_SERVICE * fio;

	char * Format_string;
	char * Buffer;
	dword  Buffer_size;
	dword  Data_size;
	dword  Format_index;
	dword  Data_index;
	char * Data_PTR;
	void LoadStateBlock();
	void ValidateFormat(char c);

public:
	ENTITY_STATE_R();
	virtual ~ENTITY_STATE_R();
	void Init(VFILE_SERVICE * _fio,HANDLE _file_handle);

	byte	Byte();
	word	Word();
	long	Long();
	dword	Dword();
	float	Float();
	double	Double();
	char *	Pointer(); 
	void	String(dword dest_buffer_size,char * buffer);
	void	MemoryBlock(dword memsize, char * buffer);
	void	Struct(dword sizeofstruct, char * s); 
};

#endif