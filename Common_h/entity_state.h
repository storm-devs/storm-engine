#ifndef _ENTITY_STATE_H_
#define _ENTITY_STATE_H_

#ifndef _XBOX 
#include <windows.h>
#else
#include <xtl.h>
#endif
#include "d_types.h"

class ENTITY_STATE_GEN 
{
public:
	ENTITY_STATE_GEN(){};
	virtual ~ENTITY_STATE_GEN(){};
	virtual void _cdecl SetState(char * Format,...){};
};

class ENTITY_STATE
{
public:
	ENTITY_STATE(){};
	virtual ~ENTITY_STATE(){};
	virtual byte	Byte()		{return 0;};
	virtual word	Word()		{return 0;};
	virtual long	Long()		{return 0;};
	virtual dword	Dword()		{return 0;};
	virtual float	Float()		{return 0;};
	virtual double	Double()	{return 0;};
	virtual char *	Pointer()	{return null;}; 
	virtual void	String(dword buffer_size, char * buffer){};
	virtual void	MemoryBlock(dword memsize, char * buffer){};
	virtual void	Struct(dword sizeofstruct, char * s){}; 
};

#endif