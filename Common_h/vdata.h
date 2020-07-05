#ifndef _VDATA_H_
#define _VDATA_H_

//#include "token.h"
#include "entity_id.h"
#include "attributes.h"

class DATA;


class VDATA
{
public:

	VDATA(){};
	virtual ~VDATA(){};

	virtual DATA * GetReference()=0;
	virtual void  SetReference(DATA * pRef)=0;
	virtual void  SetAReference(ATTRIBUTES * pARef)=0;
	virtual void Set(long value)=0;
	virtual void Set(float value)=0;
	virtual void Set(char * value)=0;
	virtual void Set(char * attribute_name, char * attribute_value)=0;
	virtual bool Get(long & value)=0;
	virtual bool Get(float & value)=0;
	virtual bool Get(char * & value)=0;
	virtual bool Get(char * attribute_name, char * & value)=0;
	virtual bool Set(long value, DWORD index)=0;
	virtual bool Set(float value, DWORD index)=0;
	virtual bool Set(char * value, DWORD index)=0;
	virtual bool Get(long & value, DWORD index)=0;
	virtual bool Get(float & value, DWORD index)=0;
	virtual bool Get(char * & value, DWORD index)=0;
	virtual bool Set(char * attribute_name, char * attribute_value, DWORD index)=0;
	virtual bool Get(char * attribute_name, char * & value, DWORD index)=0;
	virtual void Set(ENTITY_ID eid)=0;
	virtual void Get(ENTITY_ID & eid)=0;
	virtual bool Set(ENTITY_ID eid, DWORD index)=0;
	virtual bool Get(ENTITY_ID & eid, DWORD index)=0;

	virtual ATTRIBUTES * GetAClass()=0;
	virtual ATTRIBUTES * GetAClass(DWORD index)=0;
	virtual DATA * GetArrayElement(DWORD index)=0;
	//virtual S_TOKEN_TYPE GetType()=0;
	virtual DWORD GetElementsNum()=0;
	virtual bool IsArray()=0;
	virtual bool IsReference()=0;
	virtual bool IsAReference()=0;
	virtual DATA * GetVarPointer()=0;
	virtual ENTITY_ID * GetObjectIDPTR()=0;

	// operations
	virtual bool Inc()=0;
	virtual bool Dec()=0;
	virtual bool Copy(DATA * pV)=0;
	virtual bool Copy(DATA * pV, DWORD index)=0;
	virtual bool CopyOnElement(DATA * pV, DWORD index)=0;
	virtual bool Inverse()=0;
	virtual bool Power(long Deg)=0;
	virtual bool Power(DATA * pV)=0;
	virtual bool Multiply(DATA * pV)=0;
	virtual bool Divide(DATA * pV)=0;
	virtual bool Modul(DATA * pV)=0;
	virtual bool Plus(DATA * pV)=0;
	virtual bool Minus(DATA * pV)=0;
	virtual bool Compare(DATA * pV, char opA, char opB)=0;
	virtual bool Neg()=0;
//	virtual bool CompareAndSetResult(DATA * pV,S_TOKEN_TYPE op)=0;

	virtual void  SetElementsNum(DWORD _asize)= 0;

	virtual long  GetLong()=0;
	virtual float GetFloat()=0;
	virtual char * GetString()=0;
	virtual ENTITY_ID GetEntityID()=0;
};

#endif