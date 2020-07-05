#ifndef _MESSAGE_SCRIPT_H_
#define _MESSAGE_SCRIPT_H_

#include "..\..\common_h\entity_id.h"
#include "..\..\common_h\cvector.h"
#include "..\..\common_h\message.h"
#include "..\..\common_h\vdata.h"
#include "..\..\common_h\vapi.h"
#include "..\..\common_h\vmodule_api.h"
// Class for implementing function SendMessage from script in same way as 
// api function Send_Message (with variable number of arguments)

class MESSAGE_SCRIPT : public MESSAGE
{
protected:
	char * format;
	char * pData;
	long Data_size;
	char * ReadPointer;

	//long index;
public:
	 MESSAGE_SCRIPT(){format = 0; index = 0; pData = 0; Data_size = 0; ReadPointer = 0;};
	~MESSAGE_SCRIPT(){if(format) delete format; format = 0; if(pData) delete pData; pData = 0;};
	ENTITY_ID Sender_ID;
	void Move2Start() {ResetIndex();};
	//va_list args;
	bool Set(char * data)
	{
		long arg_size;
		switch(GetCurrentFormatType())
		{
			case 'l': 
				arg_size = sizeof(long); 
			break;
			case 'f': 
				arg_size = sizeof(float); 
			break;
			case 's':
				if(data == 0) arg_size = 1;
				else arg_size = strlen(data) + 1; 
			break;
			case 'i':
				arg_size = sizeof(ENTITY_ID); 
			break;
			case 'e':
				arg_size = sizeof(DATA *); 
			break;
			case 'a':
				arg_size = sizeof(ATTRIBUTES *); 
			break;
			default: throw "Invalid MESSAGE_SCRIPT data type";
		}
		pData = (char *)RESIZE(pData,Data_size + arg_size);
		if(GetCurrentFormatType() == 's' && data == 0)
		{
			char bf = 0;
			memcpy((char *)(pData + Data_size),&bf,1);
		}
		else memcpy((char *)(pData + Data_size),data,arg_size);
		Data_size += arg_size;
		index++;
		return true;
	};
	byte Byte()			{ throw "Invalid MESSAGE_SCRIPT data type"; }
	word Word()			{ throw "Invalid MESSAGE_SCRIPT data type"; }
	long Dword()		{ throw "Invalid MESSAGE_SCRIPT data type"; }
	double Double()		{ throw "Invalid MESSAGE_SCRIPT data type"; }
	char * Pointer()	{ throw "Invalid MESSAGE_SCRIPT data type"; }
	CVECTOR CVector()	{ throw "Invalid MESSAGE_SCRIPT data type"; }
	void MemoryBlock(dword memsize, char * buffer){	throw "Invalid MESSAGE_SCRIPT data type";	}
	void Struct(dword sizeofstruct, char * s) { throw "Invalid MESSAGE_SCRIPT data type";}
	ATTRIBUTES * AttributePointer()
	{ 
		ATTRIBUTES * tAP;
		ValidateFormat('a'); 
		memcpy(&tAP,ReadPointer,sizeof(ATTRIBUTES *));
		ReadPointer += sizeof(ATTRIBUTES *);
		return tAP; 
	}
	long Long()			
	{ 
		long tLong;
		ValidateFormat('l'); 
		memcpy(&tLong,ReadPointer,sizeof(long));
		ReadPointer += sizeof(long);
		return tLong; 
	}
	float Float()		
	{ 
		float tFloat;
		ValidateFormat('f'); 
		memcpy(&tFloat,ReadPointer,sizeof(float));
		ReadPointer += sizeof(float);
		return tFloat; 
	}
	ENTITY_ID EntityID()
	{ 
		ENTITY_ID id;
		ValidateFormat('i'); 
		memcpy(&id,ReadPointer,sizeof(ENTITY_ID));
		ReadPointer += sizeof(ENTITY_ID);
		return id; 
	}
	VDATA * ScriptVariablePointer()
	{ 
		VDATA * ptr;
		ValidateFormat('e'); 
		memcpy(&ptr,ReadPointer,sizeof(VDATA *));
		ReadPointer += sizeof(VDATA *);
		return ptr; 
	}
	void String(dword dest_buffer_size, char * buffer)
	{
		dword size;
		ValidateFormat('s');
		size = strlen(ReadPointer) + 1;
		if(size >= dest_buffer_size)
		{
				memcpy(buffer,ReadPointer,dest_buffer_size);
				if(dest_buffer_size > 0) buffer[dest_buffer_size - 1] = 0;
				api->Trace("MESSAGE_SCRIPT::String() data clamped to %s ",buffer);
				return;
			//throw "insufficient string buffer";
		}
		memcpy(buffer,ReadPointer,size);
		ReadPointer += size;

		/*char * mem_PTR;
		dword size;
		if(!buffer) throw "zero string buffer";
		ValidateFormat('s');
		mem_PTR = va_arg(args,char*);
		size = strlen(mem_PTR) + 1;
		if(size >= dest_buffer_size) throw "insufficient string buffer";
		memcpy(buffer,mem_PTR,size);*/
	}
	virtual char* StringPointer()
	{
		ValidateFormat('s');
		char* pStr = ReadPointer;
		ReadPointer += strlen(ReadPointer) + 1;
		return pStr;
	}
	void ValidateFormat(char c)
	{
		if(!format) throw "Read from empty MESSAGE_SCRIPT";
		if(format[index] != c) throw "Incorrect MESSAGE_SCRIPT data";
		index++;
	}
	void ResetIndex()
	{
		index = 0; 
		ReadPointer = pData;
	};
	void Reset(char * _format) 
	{
		if(!_format) throw "MESSAGE_SCRIPT: cant reset with empty format string";
		if(format) delete format;

		format = NEW char[strlen(_format) + 1];
		index = 0; 
		strcpy(format,_format);
		//format =  _format; 
		if(pData) delete pData;
		pData = 0; Data_size = 0; ReadPointer = 0;
		index = 0;
	}
	char GetCurrentFormatType()
	{
		return format[index];
	};
};

#endif