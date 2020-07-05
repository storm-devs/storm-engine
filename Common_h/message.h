#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#include "entity_id.h"
#include "cvector.h"
#include "vdata.h"

//	#define _INTSIZEOF(n)   ( (sizeof(n) + sizeof(int) - 1) & ~(sizeof(int) - 1) )
//	#define va_arg(ap,t)    ( *(t *)((ap += _INTSIZEOF(t)) - _INTSIZEOF(t)) )

class MESSAGE 
{
protected:
	char * format;
	long index;
	va_list args_start;
	
public:
	ENTITY_ID Sender_ID;
	va_list args;
	virtual void Move2Start() {args = args_start;index = 0;};
	virtual byte Byte()			{ ValidateFormat('b'); return va_arg(args,byte); }
	virtual word Word()			{ ValidateFormat('w'); return va_arg(args,word); }
	virtual long Long()			{ ValidateFormat('l'); return va_arg(args,long); }
	virtual long Dword()		{ ValidateFormat('u'); return va_arg(args,dword); }
	virtual float Float()		{ ValidateFormat('f'); return (float)va_arg(args,double); }
	virtual double Double()		{ ValidateFormat('d'); return va_arg(args,double); }
	virtual char * Pointer()	{ ValidateFormat('p'); return va_arg(args,char*); }
	virtual ATTRIBUTES * AttributePointer(){ ValidateFormat('a'); return va_arg(args,ATTRIBUTES *); }
	virtual ENTITY_ID EntityID(){ ValidateFormat('i'); return va_arg(args,ENTITY_ID); }
	virtual VDATA * ScriptVariablePointer(){ ValidateFormat('e'); return va_arg(args,VDATA*); }
	virtual CVECTOR CVector()	{ ValidateFormat('c'); return va_arg(args,CVECTOR); }
	virtual void String(dword dest_buffer_size, char * buffer)
	{
		char * mem_PTR;
		dword size;
		if(!buffer) throw "zero string buffer";
		ValidateFormat('s');
		mem_PTR = va_arg(args,char*);
		if(!mem_PTR) throw "invalid string : MESSAGE.String()";
		size = strlen(mem_PTR) + 1;
		if(size >= dest_buffer_size) throw "insufficient string buffer";
		memcpy(buffer,mem_PTR,size);
	}
	virtual void MemoryBlock(dword memsize, char * buffer)
	{
		char * mem_PTR;
		dword size;
		if(!buffer) throw "zero mem buffer";
		ValidateFormat('m');
		size = va_arg(args,dword);
		if(memsize != size) throw "invalid memory block size";
		mem_PTR = va_arg(args,char*);
		memcpy(buffer,mem_PTR,memsize);
	}
	virtual void Struct(dword sizeofstruct, char * s) 
	{ 
		dword a;
		dword size;
		if(!s) throw "Invalid s buffer";
		ValidateFormat('v');
		size = va_arg(args,dword);
		if(sizeofstruct != size) throw "Invalid message structure size";
		a = ((sizeofstruct + sizeof(int) - 1) & ~(sizeof(int) - 1));
		memcpy(s,(char *)((args += a) - a),sizeofstruct);
	}
	virtual void ValidateFormat(char c)
	{
		if(!format) throw "Read from empty message";
		if(format[index] != c) throw "Incorrect message data";
		index++;
	}
	virtual void Reset(char * _format) {index = 0; format =  _format; args_start = args;}
	virtual char GetCurrentFormatType()
	{
		return format[index];
	};
	virtual char * StringPointer()
	{
		char * mem_PTR;
		ValidateFormat('s');
		mem_PTR = va_arg(args,char*);
		return mem_PTR;
	}
};

#endif