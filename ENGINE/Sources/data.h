#ifndef _DATA_H_
#define _DATA_H_

#include "token.h"
#include "..\..\common_h\vdata.h"

class VIRTUAL_COMPILER
{
public:
	virtual void SetError(char * data_PTR, ...){};
	virtual void SetWarning(char * data_PTR, ...){};
	virtual VSTRING_CODEC * GetVSC(){return 0;}
};

/*struct OBJECT_DESC
{
	ENTITY_ID    object_id;
	ATTRIBUTES * pAttributes;
};*/

class COMPILER;

class DATA : public VDATA
{
friend COMPILER;

	VIRTUAL_COMPILER * pVCompiler;
	S_TOKEN_TYPE Data_type;
	DWORD Number_of_elements;
	//bool   bRef;
	bool   bArray;
	//char * ArrayPointer;
	long   lValue;
	float  fValue;
	char * sValue;
	
	bool   bEntity;
	ENTITY_ID object_id;
	DWORD Segment_id;

	DATA * pReference;
	//long   ReferenceIndex;

	DATA * ArrayPTR;
	DWORD nGlobalVarTableIndex;
public:

	void * operator new(size_t size)
	{
		return ::operator new(size);
	};
	void * operator new(size_t size, void * p)
	{
		return p;
	};
	void operator delete(void * p) {::operator delete (p);};
	void operator delete(void * p, void * pp) {};

	void * operator new(size_t size, char * pf, DWORD line)
	{
		return ::operator new(size,pf,line);
	};
	void operator delete(void * p, char * pf, DWORD line) 
	{
		::operator delete (p,pf,line);
	};

	ATTRIBUTES * AttributesClass;
	//ATTRIBUTES Attributes;
	DATA * GetReference();
	void  SetReference(DATA * pRef);
	void  SetAReference(ATTRIBUTES * pARef);
	//void  SetReferenceFlag() {bRef = true;};
	void  SetSegmentID(DWORD id){Segment_id = id;};
	DWORD GetSegmentID(){return Segment_id;};
	void  SetVCompiler(VIRTUAL_COMPILER * pVC);
	DATA();
	DATA(S_TOKEN_TYPE _element_type);
	DATA(DWORD _num_of_elements, S_TOKEN_TYPE _element_type);
	~DATA();
	void Set(long value);
	void Set(float value);
	void Set(char * value);
	void Set(char * attribute_name, char * attribute_value);
	bool Get(long & value);
	bool Get(float & value);
	bool Get(char * & value);
	bool Get(char * attribute_name, char * & value);

	bool Set(long value, DWORD index);
	bool Set(float value, DWORD index);
	bool Set(char * value, DWORD index);

	bool Get(long & value, DWORD index);
	bool Get(float & value, DWORD index);
	bool Get(char * & value, DWORD index);
	
	bool Set(char * attribute_name, char * attribute_value, DWORD index);
	bool Get(char * attribute_name, char * & value, DWORD index);

	void Set(ENTITY_ID eid);
	void Get(ENTITY_ID & eid);

	bool Set(ENTITY_ID eid, DWORD index);
	bool Get(ENTITY_ID & eid, DWORD index);

	ATTRIBUTES * GetAClass();
	ATTRIBUTES * GetAClass(DWORD index);

	void SetType(S_TOKEN_TYPE type, DWORD array_size = 1);
	bool Convert(S_TOKEN_TYPE type);
	void Error(char *);
	DATA * GetArrayElement(DWORD index);

	S_TOKEN_TYPE GetType(){return Data_type;};
	DWORD GetElementsNum();
	void  SetElementsNum(DWORD _asize);
	bool IsArray() {return bArray;};
	bool IsReference();
	bool IsAReference();
	DATA * GetVarPointer();
	void ClearType();

	ENTITY_ID * GetObjectIDPTR() {return &object_id;}

	// operations
	bool Inc();
	bool Dec();
	bool Copy(DATA * pV);
	bool Copy(DATA * pV, DWORD index);
	bool CopyOnElement(DATA * pV, DWORD index);
	bool Inverse();
	bool Power(long Deg);
	bool Power(DATA * pV);
	bool Multiply(DATA * pV);
	bool Divide(DATA * pV);
	bool Modul(DATA * pV);
	bool Plus(DATA * pV);
	bool Minus(DATA * pV);
	bool Compare(DATA * pV, char opA, char opB);
	bool Neg();
	bool CompareAndSetResult(DATA * pV,S_TOKEN_TYPE op);


	bool BoolConvert();
	bool RefConvert();

	void BadIndex(DWORD index, DWORD array_size);


	long  GetLong();
	float GetFloat();
	char * GetString();
	ENTITY_ID GetEntityID();
	void SetGlobalVarTableIndex(DWORD index){nGlobalVarTableIndex = index;};

	void Release();
};

#endif