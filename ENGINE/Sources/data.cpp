#include "data.h"
#include <math.h>
#include "core.h"
#include <float.h>

char * UNINIT_REF = {"Using reference variable without initializing"};
char * INVALID_REF_OP = {"Invalid operation on reference object"};
char * NO_INDEX = {"Missed array index"};
char * INVALID_CONVERSATION = {"Invalid conversation"};
char * INDEX_ON_SINGLE = {"Incorrect usage of index on single variable"};
char * INCORRECT_TYPE_ASSIGMENT = {"Incorrect type assigment"};
char * INCORRECT_INDEX = {"Incorrect index"};
char * INVALID_TYPE = {"Invalid type"};
char * INVALID_STRING = {"Invalid string"};

extern VAPI * api;


extern void DumpError(const char * data_PTR, ...);

DATA::DATA()
{
	Data_type = UNKNOWN;
	Number_of_elements = 0;
	ArrayPTR = 0;
	lValue = 0;
	fValue = 0;
	sValue = 0;
	bArray = false;
	bEntity = false;
	pVCompiler = 0;
	Segment_id = 0;
//	bRef = false;
	pReference = 0;
	AttributesClass = 0;
	ArrayPTR = 0;
	nGlobalVarTableIndex = 0xffffffff;
	
}

DATA::DATA(S_TOKEN_TYPE _element_type)
{
	Data_type = _element_type;
	Number_of_elements = 0;
	ArrayPTR = 0;
	lValue = 0;
	fValue = 0;
	sValue = 0;
	bArray = false;
	bEntity = false;
	pVCompiler = 0;
	Segment_id = 0;
//	bRef = false;
	pReference = 0;
	AttributesClass = 0;
	nGlobalVarTableIndex = 0xffffffff;
	
}

DATA::DATA(DWORD _num_of_elements, S_TOKEN_TYPE _element_type)
{
	nGlobalVarTableIndex = 0xffffffff;
	Number_of_elements = _num_of_elements;
	ArrayPTR = 0;
	lValue = 0;
	fValue = 0;
	sValue = 0;
	bArray = true;
	bEntity = false;
	Data_type = _element_type;
	pVCompiler = 0;
	Segment_id = 0;
//	bRef = false;
	pReference = 0;
	AttributesClass = 0;
	
	DWORD n;

	ArrayPTR = (DATA *) NEW char[Number_of_elements * sizeof(DATA)];
	for(n=0;n<Number_of_elements;n++)
	{
		//ArrayPTR[n] = 
		new(&ArrayPTR[n])DATA(_element_type);
		//ArrayPTR[n]->SetVCompiler((VIRTUAL_COMPILER *)0);	// ???! compiler pointer isnt set for this constructor
	}
/*	switch(Data_type)
	{
		case VAR_INTEGER:
		case VAR_FLOAT:
		case VAR_STRING:
		case VAR_OBJECT:
		break;
		default: 
			Error(INVALID_TYPE); 
			throw "INVALID_TYPE";
		break;
	}*/
}

bool DATA::IsReference() 
{
	if(Data_type == VAR_REFERENCE) return true;
	return false;
}

bool DATA::IsAReference() 
{
	if(Data_type == VAR_AREFERENCE) return true;
	return false;
}

void DATA::Release()
{
	DWORD n;
	if(sValue) delete sValue; sValue = 0;
	if(bArray)
	{
		for(n=0;n<Number_of_elements;n++)
		{
			ArrayPTR[n].~DATA();
			//delete ArrayPTR[n];
		}
		delete (char *)ArrayPTR;
		ArrayPTR = 0;
	}
	if(!(Data_type == VAR_REFERENCE || Data_type == VAR_AREFERENCE)) 
	{
		if(AttributesClass) 
		{
			delete AttributesClass; AttributesClass = 0;
		}
	}
}

DATA::~DATA()
{
	Release();
	/*
	DWORD n;
	if(sValue) delete sValue; sValue = 0;
	if(bArray)
	{
		for(n=0;n<Number_of_elements;n++)
		{
			ArrayPTR[n].~DATA();
			//delete ArrayPTR[n];
		}
		delete (char *)ArrayPTR;
		ArrayPTR = 0;

	}
	if(!(Data_type == VAR_REFERENCE || Data_type == VAR_AREFERENCE)) 
	{
		if(AttributesClass) 
		{
			delete AttributesClass; AttributesClass = 0;
		}
	}*/
}

void DATA::Error(char * text)
{
	//DumpError(text);
	pVCompiler->SetError(text);
}
void  DATA::SetReference(DATA * pRef)
{
	//if(!bRef)
	if(Data_type != VAR_REFERENCE)
	{
		Error("Isnt reference object");
		return;
	}
	pReference = pRef;
	//if(pRef) Data_type = pRef->GetType();
	
}

void DATA::SetAReference(ATTRIBUTES * pARef)
{
	if(Data_type != VAR_AREFERENCE)
	{
		Error("Isnt A reference object");
		return;
	}
	AttributesClass = pARef;
}

/*void  DATA::SetReference(DATA * pRef, long reference_index)
{
	if(!bRef)
	{
		Error("Isnt reference object");
		return;
	}
	pReference = pRef;
	if(pRef) Data_type = pRef->GetType();
	ReferenceIndex = reference_index;
}*/

void DATA::Set(long value)
{
	//if(bRef) 
	if(Data_type == VAR_REFERENCE)
	{
		if(pReference) 
		{ 
			pReference->Set(value); 
			return;
		}
		Error(UNINIT_REF);
		return;
	}

	if(bArray) {Error(NO_INDEX); return;}
	Data_type = VAR_INTEGER;
	lValue = value;
}

void DATA::Set(float value)
{
	//if(bRef) 
	if(Data_type == VAR_REFERENCE)
	{
		if(pReference) 
		{ 
			pReference->Set(value); 
			return;
		}
		Error(UNINIT_REF);
		return;
	}
	if(bArray) {Error(NO_INDEX); return;}
	Data_type = VAR_FLOAT;
	fValue = value;
	if(_isnan(fValue)) Error("NAN ERROR");
}

void DATA::Set(char * value)
{
	//if(bRef)
	if(Data_type == VAR_REFERENCE)
	{
		if(pReference) 
		{
			pReference->Set(value); 
			return;
		}
		Error(UNINIT_REF);
		return;
	}
	if(bArray) {Error(NO_INDEX); return;}
	Data_type = VAR_STRING;
	if(sValue) delete sValue;
	sValue = 0;
	if(value == 0) return;

	sValue = NEW char[strlen(value)+1];
	strcpy(sValue,value);
}

void DATA::Set(char * attribute_name, char * attribute_value)
{
	//if(bRef)
	if(Data_type == VAR_REFERENCE)
	{
		if(pReference) 
		{
			pReference->Set(attribute_name,attribute_value); 
			return;
		}
		Error(UNINIT_REF);
		return;
	}

	if(bArray) {Error(NO_INDEX); return;}
	if(AttributesClass == 0) 
	{
		if(Data_type == VAR_AREFERENCE)
		{
			Error("uninitialized AReference");
			return;
		}

		AttributesClass = NEW ATTRIBUTES(pVCompiler->GetVSC());
	}
	AttributesClass->SetAttribute(attribute_name,attribute_value);
	//Attributes.SetAttribute(attribute_name,attribute_value);
}

void DATA::Set(ENTITY_ID eid)
{
	//if(bRef) 
	if(Data_type == VAR_REFERENCE)
	{
		if(pReference) 
		{
			pReference->Set(eid); 
			return;
		}
		Error(UNINIT_REF);
		return;
	}
	if(bArray) {Error(NO_INDEX); return;}
	if(Data_type == VAR_AREFERENCE)
	{
		memcpy(&object_id,&eid,sizeof(ENTITY_ID));
		return;
	}
	Data_type = VAR_OBJECT;
	memcpy(&object_id,&eid,sizeof(ENTITY_ID));
	bEntity = true;
}

void DATA::Get(ENTITY_ID & eid)
{
	//if(bRef)
	if(Data_type == VAR_REFERENCE)
	{
		if(pReference) 
		{ 
			pReference->Get(eid); 
			return;
		}
		Error(UNINIT_REF);
		return;
	}
	if(bArray) {Error(NO_INDEX); return;}
	if(!(Data_type == VAR_OBJECT || Data_type == VAR_AREFERENCE)) {Error("Not object"); return;}
	eid = object_id;
}

bool DATA::Get(long & value)
{
	//if(bRef)
	if(Data_type == VAR_REFERENCE)
	{
		if(pReference) 
		{
			return pReference->Get(value);
		}
		Error(UNINIT_REF);
		return false;
	}
	if(bArray) {Error(NO_INDEX); return false;}
	if(Data_type == VAR_INTEGER) { value = lValue; return true;}
	return false;
}

bool DATA::Get(float & value)
{
	//if(bRef)
	if(Data_type == VAR_REFERENCE)
	{
		if(pReference) 
		{
			return pReference->Get(value);
		}
		Error(UNINIT_REF);
		return false;
	}
	if(bArray) {Error(NO_INDEX); return false;}
	if(Data_type == VAR_FLOAT) { value = fValue; return true;}
	return false;
}

bool DATA::Get(char * & value)
{
	//if(bRef)
	if(Data_type == VAR_REFERENCE)
	{
		if(pReference) 
		{
			return pReference->Get(value);
		}
		Error(UNINIT_REF);
		return false;
	}
	value = 0;
	if(bArray) {Error(NO_INDEX); return false;}
	if(Data_type == VAR_STRING) { value = sValue; return true;}
	return false;
}

bool DATA::Get(char * attribute_name, char * & value)
{
	//if(bRef)
	if(Data_type == VAR_REFERENCE)
	{
		if(pReference) 
		{
			return pReference->Get(attribute_name,value);
		}
		Error(UNINIT_REF);
		return false;
	}
	char * pAValue;
	if(bArray) {Error(NO_INDEX); return false;}
	if(AttributesClass == 0) return false;
	//pAValue = Attributes.GetAttribute(attribute_name);
	pAValue = AttributesClass->GetAttribute(attribute_name);
	if(pAValue == 0) return false;
	value = pAValue;
	return true;
}


bool DATA::Get(long & value, DWORD index)
{
	//if(bRef)
	if(Data_type == VAR_REFERENCE)
	{
		if(pReference) { return pReference->Get(value,index);}
		Error(UNINIT_REF);
		return false;
	}
	if(!bArray) {Error(INDEX_ON_SINGLE); return false;}
	if(Data_type != VAR_INTEGER) { Error(INCORRECT_TYPE_ASSIGMENT); return false;}
	if(index >= Number_of_elements)
	{ 
		BadIndex(index,Number_of_elements);
		//Error(INCORRECT_INDEX); 
		return false;}
	return ArrayPTR[index].Get(value);
/*
	long * pL;
	pL = (long *)ArrayPointer;
	value = pL[index];
	return true;	*/
}
bool DATA::Get(float & value, DWORD index)
{
	//if(bRef)
	if(Data_type == VAR_REFERENCE)
	{
		if(pReference) { return pReference->Get(value,index);}
		Error(UNINIT_REF);
		return false;
	}
	if(!bArray) {Error(INDEX_ON_SINGLE); return false;}
	if(Data_type != VAR_FLOAT) { Error(INCORRECT_TYPE_ASSIGMENT); return false;}
	if(index >= Number_of_elements)
	{ 
		BadIndex(index,Number_of_elements);
		//Error(INCORRECT_INDEX	); 
		return false;}
	return ArrayPTR[index].Get(value);
	/*float * pF;
	pF = (float *)ArrayPointer;
	value = pF[index];
	return true;	
	*/
}
bool DATA::Get(char * & value, DWORD index)
{
	//if(bRef)
	if(Data_type == VAR_REFERENCE)
	{
		if(pReference) { return pReference->Get(value,index);}
		Error(UNINIT_REF);
		return false;
	}
	if(!bArray) {Error(INDEX_ON_SINGLE); return false;}
	if(Data_type != VAR_STRING) { Error(INCORRECT_TYPE_ASSIGMENT); return false;}
	if(index >= Number_of_elements)
	{ 
		//Error(INCORRECT_INDEX	); 
		BadIndex(index,Number_of_elements);
		return false;}
	//if(value == null){ Error(INVALID_STRING); return false;}
	return ArrayPTR[index].Get(value);

/*	char * * ppChar;
	ppChar = (char **)ArrayPointer;
	if(ppChar[index] == null) value[0] = 0;
	else strcpy(value,ppChar[index]);
	return true;	*/
}

bool DATA::Set(long value, DWORD index)
{
	//if(bRef) 
	if(Data_type == VAR_REFERENCE)
	{
		if(pReference) { return pReference->Set(value,index);}
		Error(UNINIT_REF);
		return false;
	}
	if(!bArray) {Error(INDEX_ON_SINGLE); return false;}
	if(Data_type != VAR_INTEGER) { Error(INCORRECT_TYPE_ASSIGMENT); return false;}
	if(index >= Number_of_elements)
	{
		BadIndex(index,Number_of_elements);
		//Error(INCORRECT_INDEX	); 
		return false;
	}
	ArrayPTR[index].Set(value);
	return true;
	/*long * pL;
	pL = (long *)ArrayPointer;
	pL[index] = value;
	return true;	*/
}

bool DATA::Set(float value, DWORD index)
{
	//if(bRef)
	if(Data_type == VAR_REFERENCE)
	{
		if(pReference) { return pReference->Set(value,index);}
		Error(UNINIT_REF);
		return false;
	}
	if(!bArray) {Error(INDEX_ON_SINGLE); return false;}
	if(Data_type != VAR_FLOAT) { Error(INCORRECT_TYPE_ASSIGMENT); return false;}
	if(index >= Number_of_elements)
	{ 
		BadIndex(index,Number_of_elements);
		//Error(INCORRECT_INDEX	); 
		return false;
	}
	ArrayPTR[index].Set(value);
	return true;

	/*float * pF;
	pF = (float *)ArrayPointer;
	pF[index] = value;
	return true;	*/
}

bool DATA::Set(char * value, DWORD index)
{
	//if(bRef)
	if(Data_type == VAR_REFERENCE)
	{
		if(pReference) { return pReference->Set(value,index);}
		Error(UNINIT_REF);
		return false;
	}
	if(!bArray) {Error(INDEX_ON_SINGLE); return false;}
	if(Data_type != VAR_STRING) { Error(INCORRECT_TYPE_ASSIGMENT); return false;}
	if(index >= Number_of_elements)
	{ 
		BadIndex(index,Number_of_elements);
		//Error(INCORRECT_INDEX	); 
		return false;}
	if(value == null){ Error(INVALID_STRING); return false;}
	ArrayPTR[index].Set(value);
	return true;

/*	char * * ppChar;
	ppChar = (char **)ArrayPointer;
	if(ppChar[index] != null) delete ppChar[index];
	ppChar[index] = null;
	ppChar[index] = NEW char[strlen(value) + 1];
	strcpy(ppChar[index],value);
	return true;	*/
}

bool DATA::Set(char * attribute_name, char * attribute_value, DWORD index)
{
	//if(bRef)
	if(Data_type == VAR_REFERENCE)
	{
		if(pReference) { return pReference->Set(attribute_name,attribute_value,index);}
		Error(UNINIT_REF);
		return false;
	}
	if(!bArray) {Error(INDEX_ON_SINGLE); return false;}
	if(Data_type != VAR_OBJECT) { Error("Access to attributes of non object array"); return false;}
	if(index >= Number_of_elements)
	{ 
		BadIndex(index,Number_of_elements);
		//Error(INCORRECT_INDEX	); 
		return false;}
	ArrayPTR[index].Set(attribute_name,attribute_value);
	return  true;
/*
	OBJECT_DESC * pOD;
	pOD = (OBJECT_DESC *)ArrayPointer;
	if(pOD[index].pAttributes == null ) pOD[index].pAttributes = NEW ATTRIBUTES;
	pOD[index].pAttributes->SetAttribute(attribute_name,attribute_value);
	return true;*/
}

bool DATA::Get(char * attribute_name, char * & value, DWORD index)
{
	//if(bRef)
	if(Data_type == VAR_REFERENCE)
	{
		if(pReference) { return pReference->Get(attribute_name,value,index);}
		Error(UNINIT_REF);
		return false;
	}
	if(!bArray) {Error(INDEX_ON_SINGLE); return false;}
	if(Data_type != VAR_OBJECT) { Error("Access to attributes of non object array"); return false;}
	if(index >= Number_of_elements)
	{ 
		BadIndex(index,Number_of_elements);
		//Error(INCORRECT_INDEX	); 
		return false;}
	return ArrayPTR[index].Get(attribute_name,value);
/*
	OBJECT_DESC * pOD;
	pOD = (OBJECT_DESC *)ArrayPointer;
	if(pOD[index].pAttributes == null ) {value = null; return false;}
	value = pOD[index].pAttributes->GetAttribute(attribute_name);
	if(value == null) return false;
	return true;*/
}

bool DATA::Set(ENTITY_ID eid, DWORD index)
{
	//if(bRef)
	if(Data_type == VAR_REFERENCE)
	{
		if(pReference) { return pReference->Set(eid,index);}
		Error(UNINIT_REF);
		return false;
	}
	if(!bArray) {Error(INDEX_ON_SINGLE); return false;}
	if(Data_type != VAR_OBJECT) { Error("Access to attributes of non object array"); return false;}
	if(index >= Number_of_elements)
	{ 
		BadIndex(index,Number_of_elements);
		//Error(INCORRECT_INDEX	); 
		return false;}
	ArrayPTR[index].Set(eid);
	return true;
/*
	OBJECT_DESC * pOD;
	pOD = (OBJECT_DESC *)ArrayPointer;
	pOD[index].object_id = eid;
	//memcpy(&pOD[index].object_id,&eid,sizeof(ENTITY_ID));
	return true;*/
}

bool DATA::Get(ENTITY_ID & eid, DWORD index)
{
	//if(bRef)
	if(Data_type == VAR_REFERENCE)
	{
		if(pReference) { return pReference->Get(eid,index);}
		Error(UNINIT_REF);
		return false;
	}
	if(!bArray) {Error(INDEX_ON_SINGLE); return false;}
	if(!(Data_type == VAR_OBJECT  || Data_type == VAR_AREFERENCE)) { Error("Access to attributes of non object array"); return false;}
	if(index >= Number_of_elements)
	{ 
		BadIndex(index,Number_of_elements);
		//Error(INCORRECT_INDEX); 
		return false;}
	ArrayPTR[index].Get(eid);
	return true;
/*	
	OBJECT_DESC * pOD;
	pOD = (OBJECT_DESC *)ArrayPointer;
	eid = pOD[index].object_id;
	//memcpy(&eid,&pOD[index].object_id,sizeof(ENTITY_ID));
	return true;*/
}


void DATA::ClearType()
{
	if(Data_type != VAR_AREFERENCE) 
	{
		if(AttributesClass) 
		{
			delete AttributesClass; 
		}
	}
	if(sValue) delete sValue; sValue = 0;
	AttributesClass = 0;
	Data_type = UNKNOWN;
	pReference = 0;
}

void DATA::SetType(S_TOKEN_TYPE _element_type, DWORD array_size)
{
	DWORD n;
	ClearType();
	Data_type = _element_type;
	bArray = false;


	if(array_size > 1)
	{
		bArray = true;
		if(ArrayPTR)
		{
			for(n=0;n<Number_of_elements;n++)
			{

				ArrayPTR[n].~DATA();
				//delete ArrayPTR[n];
			}
		}
		Number_of_elements = array_size;
		ArrayPTR = (DATA *)RESIZE(ArrayPTR, Number_of_elements * sizeof(DATA));
		for(n=0;n<Number_of_elements;n++)
		{
			//ArrayPTR[n] = NEW DATA(_element_type);
			//ArrayPTR[n]->SetVCompiler(pVCompiler);
			new (&ArrayPTR[n]) DATA(_element_type);
			ArrayPTR[n].SetVCompiler(pVCompiler);
		}

	} 
	else 
	{
		if(ArrayPTR)
		{
			for(n=0;n<Number_of_elements;n++)
			{
				//delete ArrayPTR[n];
				ArrayPTR[n].~DATA();

			}
			delete ArrayPTR;
		}
		
		ArrayPTR = 0;
		Number_of_elements = 1;
	}
}

bool DATA::Convert(S_TOKEN_TYPE type)
{
	//if(bRef)
	if(Data_type == VAR_REFERENCE)
	{
		if(pReference) 
		{
			return pReference->Convert(type);
		}
		Error(UNINIT_REF);
		return false;
	}
	
	if(bArray) {Error("Array conversation not supported"); return false;} // yet
	char buffer[128];
	switch(Data_type)
	{
	case VAR_INTEGER:
		switch(type)
		{
			case VAR_INTEGER: return true;
			case VAR_FLOAT: Data_type = type; fValue = (float)lValue; return true;
			case VAR_STRING: 
				Data_type = type; 
				itoa(lValue,buffer,10);
				Set(buffer);
			return true;
			default: Error(INVALID_CONVERSATION); return false;
		}
	break;
	case VAR_FLOAT:
		switch(type)
		{
			case VAR_INTEGER: Data_type = type; lValue = (long)fValue; return true;
			case VAR_FLOAT: return true;
			case VAR_STRING: 
				Data_type = type; 
				gcvt(fValue,10,buffer);
				Set(buffer);
			return true;
			default: Error(INVALID_CONVERSATION); return false;
		}
	break;
	case VAR_STRING:
		if(sValue == 0) 
		{ 

			sValue = NEW char[1];
			sValue[0] = 0;
			//Error(INVALID_CONVERSATION); return false;
		}
		switch(type)
		{
			case NUMBER:
			case VAR_INTEGER:
				Data_type = VAR_INTEGER;
				lValue = atoi(sValue);
			return true;
			case FLOAT_NUMBER:
			case VAR_FLOAT: 
				Data_type = VAR_FLOAT; 
				fValue = (float)atof(sValue);
			return true;
			case STRING:
			case VAR_STRING: return true;
			default: Error(INVALID_CONVERSATION); return false;
		}
	break;
	case VAR_OBJECT:
		switch(type)
		{
			case VAR_OBJECT: return true;
		}

	break;
	case VAR_AREFERENCE:
		switch(type)
		{
			case VAR_AREFERENCE: return true;
			case STRING:
			case VAR_STRING:
				if(!AttributesClass) break;
				if(!AttributesClass->GetThisAttr()) break;
				Set(AttributesClass->GetThisAttr());
				AttributesClass = 0;
			return true;
			case NUMBER:
			case VAR_INTEGER:
				if(!AttributesClass) break;
				if(!AttributesClass->GetThisAttr()) break;
				Set(AttributesClass->GetThisAttr());
				AttributesClass = 0;
				Data_type = VAR_INTEGER;
				lValue = atoi(sValue);
			return true;
			case FLOAT_NUMBER:
			case VAR_FLOAT:
				if(!AttributesClass) break;
				if(!AttributesClass->GetThisAttr()) break;
				Set(AttributesClass->GetThisAttr());
				AttributesClass = 0;
				Data_type = VAR_FLOAT; 
				fValue = (float)atof(sValue);
			return true;
			
		}
	break;
	}
	return false;
}

void  DATA::SetElementsNum(DWORD _asize)
{
	DWORD n;
	if(Data_type == VAR_REFERENCE)
	{
		if(pReference) { pReference->SetElementsNum(_asize); return;}
		Error(UNINIT_REF);
		return;
	}
	if(!bArray) {Error("Not Array"); return;}

	if(_asize == 0) return;
	if(_asize == Number_of_elements) return;

	if(_asize < Number_of_elements)
	{
		for(n=_asize;n<Number_of_elements;n++)
		{
			//delete ArrayPTR[n];
			ArrayPTR[n].~DATA();

		}
		Number_of_elements = _asize;
		ArrayPTR = (DATA *)RESIZE(ArrayPTR,Number_of_elements * sizeof(DATA));
		return;
	}

	ArrayPTR = (DATA *)RESIZE(ArrayPTR,_asize * sizeof(DATA));
	//for(n=(Number_of_elements - 1);n<_asize;n++)
	for(n=Number_of_elements;n<_asize;n++)
	{
		//ArrayPTR[n] = NEW DATA(Data_type);
		new (&ArrayPTR[n]) DATA(Data_type);
		ArrayPTR[n].SetVCompiler( ArrayPTR[0].pVCompiler );

	}
	Number_of_elements = _asize;
}

DWORD DATA::GetElementsNum()
{
	//if(bRef)
	if(Data_type == VAR_REFERENCE)
	{
		if(pReference) { return pReference->GetElementsNum();}
		Error(UNINIT_REF);
		return 0;
	}
	if(bArray) return Number_of_elements;
	return 1;
}

bool DATA::Inc()
{
	//if(bRef)
	if(Data_type == VAR_REFERENCE)
	{
		if(pReference) { return pReference->Inc();}
		Error(UNINIT_REF);
		return false;
	}
	if(Data_type != VAR_INTEGER)
	{
		Error("Invalid data type for INC operation");
		return false;
	}
	lValue++;
	return true;
}

bool DATA::Dec()
{
	//if(bRef)
	if(Data_type == VAR_REFERENCE)
	{
		if(pReference) { return pReference->Dec();}
		Error(UNINIT_REF);
		return false;
	}
	if(Data_type != VAR_INTEGER)
	{
		Error("Invalid data type for DEC operation");
		return false;
	}
	lValue--;
	return true;
}

bool DATA::Neg()
{
	switch(GetType())
	{
		case VAR_FLOAT:
			Convert(VAR_INTEGER);
		case VAR_INTEGER:
			if(lValue != 0) lValue = 0;
			else lValue = 1;
			Set(lValue);
		break;
		case VAR_STRING:
			if(sValue == 0) lValue = 1;
			else 
			if(sValue[0] == 0) lValue = 1;
			else lValue = 0;
			Convert(VAR_INTEGER);
			Set(lValue);
		break;
		default:
			Error("Invalid argument type for '!' opearator");
		return false;
	}
	return true;
}

bool DATA::Inverse()
{
	//if(bRef)
	if(Data_type == VAR_REFERENCE)
	{
		if(pReference) { return pReference->Inverse();}
		Error(UNINIT_REF);
		return false;
	}
	if(IsArray()) return false;
	switch(Data_type)
	{
		case VAR_INTEGER:
			lValue = -lValue;
		break;
		case VAR_FLOAT:
			fValue = -fValue;
		break;
	}
	return true;
}
bool DATA::Power(DATA * pV)
{
	long lV;
	if(pV->GetType() != VAR_INTEGER)
	{
		Error("bad power argument");
		return false;
	}
	pV->Get(lV);
	return Power(lV);
}

bool DATA::Power(long Deg)
{
	//if(bRef)
	if(Data_type == VAR_REFERENCE)
	{
		if(pReference) { return pReference->Power(Deg);}
		Error(UNINIT_REF);
		return false;
	}
	if(IsArray()) return false;

	long n;
	long  lBase;
	float fBase;

	if(IsArray()) return false;
	switch(Data_type)
	{
		case VAR_INTEGER:
			if(Deg == 0) 
			{
				lValue = 1;
				return true;
			}
			lBase = lValue;
			if(Deg > 0)
			{
				for(n=(Deg - 1);n > 0;--n) lValue = lValue * lBase;
			}
			else
			{
				Deg = -Deg;
				for(n=(Deg - 1);n > 0;--n) lValue = 1 / lBase;
			}
		break;
		case VAR_FLOAT:
			if(Deg == 0) 
			{
				fValue = 1;
				return true;
			}
			fBase = fValue;
			if(Deg > 0)
			{
				for(n=(Deg - 1);n > 0;--n) fValue = fValue * fBase;
			}
			else
			{
				Deg = -Deg;
				for(n=(Deg - 1);n > 0;--n) fValue = 1.0f / fBase;
			}

		break;
	}
	return true;
}

bool DATA::Multiply(DATA * pV)
{
	//if(bRef)
	if(Data_type == VAR_REFERENCE)
	{
		if(pReference) { return pReference->Multiply(pV);}
		Error(UNINIT_REF);
		return false;
	}

	if(IsArray()) return false;
	if(pV == null) return false;
	pV = pV->GetVarPointer(); if(pV == null) return false;
	switch(Data_type)
	{
		case VAR_INTEGER:
			switch(pV->GetType())
			{
				case VAR_INTEGER:
					lValue = lValue * pV->lValue;
				break;
				case VAR_FLOAT:
					Convert(VAR_FLOAT);
					fValue = fValue * pV->fValue;
				break;
				default: return false;
			}
		break;
		case VAR_FLOAT:
			switch(pV->GetType())
			{
				case VAR_INTEGER:
					fValue = fValue * pV->lValue;
				break;
				case VAR_FLOAT:
					fValue = fValue * pV->fValue;
				break;
				default: return false;
			}
		break;
		default: return false;
	}
	return true;
}

bool DATA::Divide(DATA * pV)
{
	//if(bRef)
	if(Data_type == VAR_REFERENCE)
	{
		if(pReference) { return pReference->Divide(pV);}
		Error(UNINIT_REF);
		return false;
	}
	if(IsArray()) return false;
	if(pV == null) return false;
	pV = pV->GetVarPointer(); if(pV == null) return false;
	switch(Data_type)
	{
		case VAR_INTEGER:
			switch(pV->GetType())
			{
				case VAR_INTEGER:
					if(pV->lValue == 0)
					{
						Error("Divide by zero");
						return false;
					}
					lValue = lValue / pV->lValue;
				break;
				case VAR_FLOAT:
					if(pV->fValue == 0)
					{
						Error("Divide by zero");
						return false;
					}
					Convert(VAR_FLOAT);
					fValue = fValue / pV->fValue;
				break;
				default: return false;
			}
		break;
		case VAR_FLOAT:
			switch(pV->GetType())
			{
				case VAR_INTEGER:
					if(pV->lValue == 0)
					{
						Error("Divide by zero");
						return false;
					}
					fValue = fValue / pV->lValue;
				break;
				case VAR_FLOAT:
					if(pV->fValue == 0)
					{
						Error("Divide by zero");
						return false;
					}
					fValue = fValue / pV->fValue;
				break;
				default: return false;
			}
		break;
		default: return false;
	}
	return true;

}

bool DATA::Modul(DATA * pV)
{
	//if(bRef)
	if(Data_type == VAR_REFERENCE)
	{
		if(pReference) { return pReference->Modul(pV);}
		Error(UNINIT_REF);
		return false;
	}
	if(IsArray()) return false;
	if(pV == null) return false;
	pV = pV->GetVarPointer(); if(pV == null) return false;
	switch(Data_type)
	{
		case VAR_INTEGER:
			switch(pV->GetType())
			{
				case VAR_INTEGER:
					if(pV->lValue == 0)
					{
						Error("Divide by zero");
						return false;
					}
					lValue = lValue % pV->lValue;
				break;
				case VAR_FLOAT:
					if(pV->fValue == 0)
					{
						Error("Divide by zero");
						return false;
					}
					lValue = lValue % (long)floor(pV->fValue);
				break;
				default: return false;
			}
		break;
		case VAR_FLOAT:
			switch(pV->GetType())
			{
				case VAR_INTEGER:
					if(pV->lValue == 0)
					{
						Error("Divide by zero");
						return false;
					}
					fValue = fValue / pV->lValue;
					fValue = (float)floor(fValue);
   				break;
				case VAR_FLOAT:
					if(pV->fValue == 0)
					{
						Error("Divide by zero");
						return false;
					}
					fValue = fValue / pV->fValue;
					fValue = (float)floor(fValue);
				break;
				default: return false;
			}
		break;
		default: return false;
	}
	return true;
}


bool DATA::Plus(DATA * pV)
{
	//if(bRef)
	if(Data_type == VAR_REFERENCE)
	{
		if(pReference) { return pReference->Plus(pV);}
		Error(UNINIT_REF);
		return false;
	}
	if(IsArray()) return false;
	if(pV == null) return false;
	pV = pV->GetVarPointer(); if(pV == null) return false;
	DWORD size;
	char * sTemp;
	char buffer[128];
	switch(Data_type)
	{
		case VAR_INTEGER:
			switch(pV->GetType())
			{
				case VAR_INTEGER:
					lValue = lValue + pV->lValue;
				break;
				case VAR_FLOAT:
					Convert(VAR_FLOAT);
					fValue = fValue + pV->fValue;
				break;
				case VAR_STRING:
					Convert(VAR_STRING);
					size = strlen(sValue) + strlen(pV->sValue) + 1;

					sTemp = NEW char[size];
					strcpy(sTemp,sValue);
					strcat(sTemp,pV->sValue);
					Set(sTemp);
					delete sTemp;
				break;
				default: return false;
			}
		break;
		case VAR_FLOAT:
			switch(pV->GetType())
			{
				case VAR_INTEGER:
					fValue = fValue + pV->lValue;
				break;
				case VAR_FLOAT:
					fValue = fValue + pV->fValue;
				break;
				case VAR_STRING:
					Convert(VAR_STRING);
					size = strlen(sValue) + strlen(pV->sValue) + 1;

					sTemp = NEW char[size];
					strcpy(sTemp,sValue);
					strcat(sTemp,pV->sValue);
					Set(sTemp);
					delete sTemp;
				break;
				default: return false;
			}
		break;
		case VAR_STRING:
			switch(pV->GetType())
			{
				case VAR_AREFERENCE:
					if(!pV->AttributesClass) break;
					if(!pV->AttributesClass->GetThisAttr()) break;
					if(sValue!=0)
					{
						size = strlen(sValue) + strlen(pV->AttributesClass->GetThisAttr()) + 1;

						sTemp = NEW char[size];
						strcpy(sTemp,sValue);
						strcat(sTemp,pV->AttributesClass->GetThisAttr());
					}
					else 
					{
						size = strlen(pV->AttributesClass->GetThisAttr()) + 1;

						sTemp = NEW char[size];
						strcpy(sTemp,pV->AttributesClass->GetThisAttr());
					}
					Set(sTemp);
					delete sTemp;
				break;
				case VAR_INTEGER:
					ltoa(pV->lValue,buffer,10);
					if(sValue!=0)
					{
						size = strlen(sValue) + strlen(buffer) + 1;

						sTemp = NEW char[size];
						strcpy(sTemp,sValue);
						strcat(sTemp,buffer);
					}
					else 
					{
						size = strlen(buffer) + 1;

						sTemp = NEW char[size];
						strcpy(sTemp,buffer);
					}
					Set(sTemp);
					delete sTemp;
				break;
				case VAR_FLOAT:
					gcvt(pV->fValue,5,buffer);
					if(sValue!=0)
					{
						size = strlen(sValue) + strlen(buffer) + 1;

						sTemp = NEW char[size];
						strcpy(sTemp,sValue);
						strcat(sTemp,buffer);
					}
					else 
					{
						size = strlen(buffer) + 1;

						sTemp = NEW char[size];
						strcpy(sTemp,buffer);
					}
					Set(sTemp);
					delete sTemp;
				break;
				case VAR_STRING:
					if(sValue == 0)
					{
						if(pV->sValue == 0) return false;
						size = strlen(pV->sValue) + 1;

						sTemp = NEW char[size];
						strcpy(sTemp,pV->sValue);
					}
					else 
					{
						if(pV->sValue == 0) return false;
						size = strlen(sValue) + strlen(pV->sValue) + 1;

						sTemp = NEW char[size];
						strcpy(sTemp,sValue);
						strcat(sTemp,pV->sValue);
					}
					Set(sTemp);
					delete sTemp;
				break;
				default: return false;
			}
		break;
		default: return false;
	}
	return true;

}

bool DATA::Minus(DATA * pV)
{
	//if(bRef)
	if(Data_type == VAR_REFERENCE)
	{
		if(pReference) { return pReference->Minus(pV);}
		Error(UNINIT_REF);
		return false;
	}
	if(IsArray()) return false;
	if(pV == null) return false;
	pV = pV->GetVarPointer(); if(pV == null) return false;
	switch(Data_type)
	{
		case VAR_INTEGER:
			switch(pV->GetType())
			{
				case VAR_INTEGER:
					lValue = lValue - pV->lValue;
				break;
				case VAR_FLOAT:
					Convert(VAR_FLOAT);
					fValue = fValue - pV->fValue;
				break;
				default: return false;
			}
		break;
		case VAR_FLOAT:
			switch(pV->GetType())
			{
				case VAR_INTEGER:
					fValue = fValue - pV->lValue;
				break;
				case VAR_FLOAT:
					fValue = fValue - pV->fValue;
				break;
				default: return false;
			}
		break;
		default: return false;
	}
	return true;
}

bool DATA::Compare(DATA * pV, char opA, char opB)
{
	//if(bRef)
	if(Data_type == VAR_REFERENCE)
	{
		if(pReference) { return pReference->Compare(pV,opA,opB);}
		Error(UNINIT_REF);
		return false;
	}
	if(IsArray()) return false;
	if(pV == null) return false;
	pV = pV->GetVarPointer(); if(pV == null) return false;
	switch(Data_type)
	{
		case VAR_INTEGER:
			switch(pV->GetType())
			{
				case VAR_INTEGER:
					switch(opA)
					{
						case '=':
							if(lValue == pV->lValue) return true;
						return false;
						case '!':
							if(lValue != pV->lValue) return true;
						return false;
						case '>':
							if(opB == '=') 
							{
								if(lValue >= pV->lValue) return true;
							} else
							{
								if(lValue > pV->lValue) return true;
							}
						return false;
						case '<':
							if(opB == '=') 
							{
								if(lValue <= pV->lValue) return true;
							} else
							{
								if(lValue < pV->lValue) return true;
							}
						return false;
					}
				break;

				case VAR_FLOAT:
					switch(opA)
					{
						case '=':
							if(lValue == pV->fValue) return true;
						return false;
						case '!':
							if(lValue != pV->fValue) return true;
						return false;
						case '>':
							if(opB == '=') 
							{
								if(lValue >= pV->fValue) return true;
							} else
							{
								if(lValue > pV->fValue) return true;
							}
						return false;
						case '<':
							if(opB == '=') 
							{
								if(lValue <= pV->fValue) return true;
							} else
							{
								if(lValue < pV->fValue) return true;
							}
						return false;
					}
				break;
				default: return false;
			}
		break;
		case VAR_FLOAT:
			switch(pV->GetType())
			{
				case VAR_INTEGER:
					switch(opA)
					{
						case '=':
							if(fValue == pV->lValue) return true;
						return false;
						case '!':
							if(fValue != pV->lValue) return true;
						return false;
						case '>':
							if(opB == '=') 
							{
								if(fValue >= pV->lValue) return true;
							} else
							{
								if(fValue > pV->lValue) return true;
							}
						return false;
						case '<':
							if(opB == '=') 
							{
								if(fValue <= pV->lValue) return true;
							} else
							{
								if(fValue < pV->lValue) return true;
							}
						return false;
					}
				break;
				case VAR_FLOAT:
					switch(opA)
					{
						case '=':
							if(fValue == pV->fValue) return true;
						return false;
						case '!':
							if(fValue != pV->fValue) return true;
						return false;
						case '>':
							if(opB == '=') 
							{
								if(fValue >= pV->fValue) return true;
							} else
							{
								if(fValue > pV->fValue) return true;
							}
						return false;
						case '<':
							if(opB == '=') 
							{
								if(fValue <= pV->fValue) return true;
							} else
							{
								if(fValue < pV->fValue) return true;
							}
						return false;
					}
				break;
				default: return false;
			}
		break;
		case VAR_STRING:
			switch(pV->GetType())
			{
				case VAR_STRING:
					switch(opA)
					{
						case '=':
							if(sValue == 0 || pV->sValue == 0)
							{
								if(sValue == pV->sValue) return true;
								return false;
							}
							if(stricmp(sValue,pV->sValue)==0) return true;
						return false;
						case '!':
							if(sValue == 0 || pV->sValue == 0)
							{
								if(sValue != pV->sValue) return true;
								return false;
							}
							if(stricmp(sValue,pV->sValue)!=0) return true;
						return false;
						case '>':
							if(sValue == 0 || pV->sValue == 0)
							{
								return false;
							}
							if(opB == '=') 
							{
								if(strlen(sValue) >= strlen(pV->sValue)) return true;
							} else
							{
								if(strlen(sValue) > strlen(pV->sValue)) return true;
							}
						return false;
						case '<':
							if(sValue == 0 || pV->sValue == 0)
							{
								return false;
							}
							if(opB == '=') 
							{
								if(strlen(sValue) <= strlen(pV->sValue)) return true;
							} else
							{
								if(strlen(sValue) < strlen(pV->sValue)) return true;
							}
						return false;
					}
				break;
				default: return false;
			}
		break;
		default: return false;
	}
	return true;
}

// copy data from pV variable to this variable
// arrays must be the same type and size
bool DATA::Copy(DATA * pV)
{
	//if(bRef)
	if(Data_type == VAR_REFERENCE)
	{
		if(pReference) { return pReference->Copy(pV); }
		if(pV != null)
		{
			if(pV->GetType() == VAR_REFERENCE)
			{
				SetReference(pV->GetVarPointer());
				return true;
			}
			if(pV->GetType() == VAR_OBJECT)
			{
				SetReference(pV->GetVarPointer());
				return true;
			}
		}
		Error(UNINIT_REF);
		return false;
	}
	DWORD copy_size;
//	DWORD n;
//	char * * ppChar;
//	char * * ppCharpV;

	if(pV == null)
	{
		Error("Invalid DATA in copy operation");
		return false;
	}
	if(pV->IsReference())
	{
		if(Data_type == VAR_REFERENCE)
		{
			SetReference(pV->GetReference());
			return true;
		}
		if(Data_type == UNKNOWN)	// stack
		{
			SetType(VAR_REFERENCE);
			SetReference(pV->GetReference());
			return true;
		}
		pV = pV->GetReference();
		if(pV == null)
		{
			Error("Invalid reference v in copy operation");
			return false;
		}
	}

	if(IsArray() && !pV->IsArray())
	{
		Error("Incorrect copy from single to array");
		return false;
	}
	if(!IsArray() && pV->IsArray())
	{
		Error("Incorrect copy from array to single");
		return false;
	}
	

	if(IsArray())
	{
		if(Data_type != pV->Data_type)
		{
			Error("Can't copy two arrays with different data type");
			return false;
		}
		if(Number_of_elements != pV->Number_of_elements)
		{
			Error("Can't copy two arrays with different size");
			return false;
		}
		copy_size = Number_of_elements * sizeof(DATA *);
		memcpy(ArrayPTR,pV->ArrayPTR,copy_size);

		return true;
	}

	if(Data_type == VAR_STRING)
	{
		if(sValue) delete sValue; sValue = 0;
	}

	switch(pV->Data_type)
	{
		case VAR_INTEGER:
			Set(pV->lValue);
		break;
		case VAR_FLOAT:
			Set(pV->fValue);
		break;
		case VAR_STRING:
			Set(pV->sValue);
		break;
		case VAR_OBJECT:
			Set(pV->object_id);
			//Attributes.Copy(&pV->Attributes);

			if(Data_type == VAR_REFERENCE)
			{
				DATA * pVV;
				pVV = GetVarPointer();
				if(pVV == 0)
				{
					Error(UNINIT_REF);
					return false;
				}
				if(pVV->AttributesClass == 0) pVV->AttributesClass = NEW ATTRIBUTES(pVCompiler->GetVSC());
				pVV->AttributesClass->Copy(pV->AttributesClass);
			}
			else
			{
				if(Data_type == VAR_AREFERENCE)
				{
					AttributesClass = pV->AttributesClass;
				}
				else
				{
					if(AttributesClass == 0) AttributesClass = NEW ATTRIBUTES(pVCompiler->GetVSC());
					AttributesClass->Copy(pV->AttributesClass);
				}
			}
		break;
		case VAR_AREFERENCE:
			if(Data_type == UNKNOWN)
			{
				SetType(VAR_AREFERENCE);
				AttributesClass = pV->AttributesClass;
				object_id = pV->object_id;
				break;
			}
			if(Data_type != VAR_AREFERENCE) {Error("invalid aref usage");return false;}
			AttributesClass = pV->AttributesClass;
			object_id = pV->object_id;
		break;

		case VAR_REFERENCE:
			if(Data_type == UNKNOWN)
			{
				SetType(VAR_REFERENCE);
				SetReference(pV->GetVarPointer());
				break;
			}
			if(Data_type != VAR_REFERENCE) {Error("invalid ref usage");return false;}
		break;

		default:
			Error("Unknown data type");
		return false;
	}
	return true;
}

// copy data from pV array on index to this single variable
bool DATA::Copy(DATA * pV, DWORD index)
{
	//if(bRef)
	if(Data_type == VAR_REFERENCE)
	{
		if(pReference) { return pReference->Copy(pV,index);}
		Error(UNINIT_REF);
		return false;
	}

//	char * * ppChar;
//	long *   pLong;
//	float *  pFloat;
//	OBJECT_DESC * pOD;

	if(pV == null)
	{
		Error("Invalid DATA in copy operation");
		return false;
	}
	if(pV->IsReference())
	{
		pV = pV->GetReference();
		if(pV == null)
		{
			Error("Invalid reference v in copy operation");
			return false;
		}
	}
	if(IsArray())
	{
		Error("Incorrect copy from single element to array");
		return false;
	}

	if(!pV->IsArray())
	{
		Error("Incorrect copy: using index on single variable");
		return false;
	}
	
	if(index >= pV->GetElementsNum())
	{
		Error("Incorrect copy: invalid index");
		return false;
	}

/*	if(Data_type == VAR_STRING)
	{
		if(sValue) delete sValue; sValue = 0;
	}
*/
	return Copy(&pV->ArrayPTR[index]);

	/*switch(pV->GetType())
	{
		case VAR_INTEGER:
			pLong = (long *)pV->ArrayPointer;
			Set(pLong[index]);
		break;
		case VAR_FLOAT:
			pFloat = (float *)pV->ArrayPointer;
			Set(pFloat[index]);
		break;
		case VAR_STRING:
			ppChar = (char **)pV->ArrayPointer;
			Set(ppChar[index]);
		break;
		case VAR_OBJECT:
			pOD = (OBJECT_DESC *)pV->ArrayPointer;
			Set(pOD[index].object_id);
			//Attributes.Copy(pOD[index].pAttributes);
			if(AttributesClass == 0) AttributesClass = new ATTRIBUTES;
			AttributesClass->Copy(pOD[index].pAttributes);
		break;
	}*/

	//return true;
}

bool DATA::CopyOnElement(DATA * pV, DWORD index)
{
	//if(bRef)
	if(Data_type == VAR_REFERENCE)
	{
		if(pReference) { return pReference->CopyOnElement(pV,index);}
		Error(UNINIT_REF);
		return false;
	}

	if(pV == null)
	{
		Error("Invalid DATA in copy operation");
		return false;
	}

	if(pV->IsReference())
	{
		pV = pV->GetReference();
		if(pV == null)
		{
			Error("Invalid reference v in copy operation");
			return false;
		}
	}

	if(Data_type != pV->GetType())
	{
		Error("Incorrect copy: different types");
		return false;
	}

	if(!IsArray())
	{
		Error("Incorrect copy: using index on single variable");
		return false;
	}

	if(pV->IsArray())
	{
		Error("Incorrect copy: using array as single");
		return false;
	}
	
	if(index >= GetElementsNum())
	{
		Error("Incorrect copy: invalid index");
		return false;
	}
	return ArrayPTR[index].Copy(pV);
/*
	switch(Data_type)
	{
		case VAR_INTEGER:
			Set(pV->lValue,index);
		break;
		case VAR_FLOAT:
			Set(pV->fValue,index);
		break;
		case VAR_STRING:
			Set(pV->sValue,index);
		break;
		case VAR_OBJECT:
			OBJECT_DESC * pOD;
			pOD = (OBJECT_DESC *)ArrayPointer;
			Set(pV->object_id,index);
			if(pV->AttributesClass)
			{
				if(pOD[index].pAttributes == null ) pOD[index].pAttributes = new ATTRIBUTES;
				//pOD[index].pAttributes->Copy(&pV->Attributes);
				pOD[index].pAttributes->Copy(pV->AttributesClass);
			}
			
		break;
	}

	return true;
*/
}

ATTRIBUTES * DATA::GetAClass()
{
	//if(bRef)
	if(Data_type == VAR_REFERENCE)
	{
		if(pReference) { return pReference->GetAClass();}
		Error(UNINIT_REF);
		return 0;
	}
	if(AttributesClass == 0) 
	{
		if(Data_type == VAR_AREFERENCE)
		{
			Error("uninitialized aref");
			//throw "uninitialized aref";
			return 0;
		}

		AttributesClass = NEW ATTRIBUTES(pVCompiler->GetVSC());
	}
	return AttributesClass;
}

ATTRIBUTES * DATA::GetAClass(DWORD index)
{
	//if(bRef)
	if(Data_type == VAR_REFERENCE)
	{
		if(pReference) { return pReference->GetAClass(index);}
		Error(UNINIT_REF);
		return 0;
	}
	if(!IsArray())
	{
		Error("Incorrect copy: using index on single variable");
		return null;
	}
	if(index >= GetElementsNum())
	{
		Error("Incorrect copy: invalid index");
		return null;
	}

	return ArrayPTR[index].GetAClass();
/*
	OBJECT_DESC * pOD;
	pOD = (OBJECT_DESC *)ArrayPointer;
	if(pOD[index].pAttributes == null ) pOD[index].pAttributes = new ATTRIBUTES;
	return pOD[index].pAttributes;*/
}

DATA * DATA::GetReference()
{
	return pReference;
}

DATA * DATA::GetArrayElement(DWORD index)
{
	if(!bArray) {Error(INDEX_ON_SINGLE); return 0;}
	if(index >= Number_of_elements)
	{ 
		BadIndex(index,Number_of_elements);
		//Error(INCORRECT_INDEX	); 
		return 0;}
	return &ArrayPTR[index];
}

void  DATA::SetVCompiler(VIRTUAL_COMPILER * pVC) 
{
	DWORD n;
	pVCompiler = pVC;
	if(bArray)
	{
		for(n=0;n<Number_of_elements;n++)
		{
			ArrayPTR[n].SetVCompiler(pVC);
		}
	}
}

DATA * DATA::GetVarPointer()
{
	//if(!bRef) return this;
	if(Data_type != VAR_REFERENCE) return this;
	if(pReference) { return pReference->GetVarPointer();}
	Error(UNINIT_REF);
	return 0;
}

bool DATA::CompareAndSetResult(DATA * pV,S_TOKEN_TYPE op)
{
	if(Data_type == VAR_REFERENCE)
	{
		if(pReference) { return pReference->CompareAndSetResult(pV,op);}
		Error(UNINIT_REF);
		return false;
	}
	if(IsArray()) return false;
	if(pV == null) return false;
	pV = pV->GetVarPointer(); 
	if(pV == null) 
	{
		Set((long)0);
		return false;
	}


	if(Data_type == VAR_STRING && pV->GetType() == VAR_INTEGER)
	{
		if(!Convert(VAR_INTEGER)) 
		{
			Set((long)0); return false;
		}
	}

	if(Data_type == VAR_STRING && pV->GetType() == VAR_FLOAT)
	{
		if(!Convert(VAR_FLOAT)) 
		{
			Set((long)0); return false;
		}
	}


	switch(Data_type)
	{
		case VAR_INTEGER:
			switch(pV->GetType())
			{
				case VAR_INTEGER:
					switch(op)
					{
						case OP_BOOL_EQUAL:
							if(lValue == pV->lValue) Set((long)1); else Set((long)0);
						break;
						case OP_GREATER:
							if(lValue > pV->lValue) Set((long)1); else Set((long)0);
						break;
						case OP_GREATER_OR_EQUAL:
							if(lValue >= pV->lValue) Set((long)1); else Set((long)0);
						break;
						case OP_LESSER:
							if(lValue < pV->lValue) Set((long)1); else Set((long)0);
						break;
						case OP_LESSER_OR_EQUAL: 
							if(lValue <= pV->lValue) Set((long)1); else Set((long)0);
						break;
						case OP_NOT_EQUAL: 
							if(lValue != pV->lValue) Set((long)1); else Set((long)0);
						break;
						case OP_BOOL_AND:
							if(lValue != 0 && pV->lValue != 0) Set((long)1); else Set((long)0);
						break;
						case OP_BOOL_OR:
							if(lValue != 0 || pV->lValue != 0) Set((long)1); else Set((long)0);
						break;
					}
				break;

				case VAR_FLOAT:
					switch(op)
					{
						case OP_BOOL_EQUAL:
							if(lValue == pV->fValue) Set((long)1); else Set((long)0);
						break;
						case OP_GREATER:
							if(lValue > pV->fValue) Set((long)1); else Set((long)0);
						break;
						case OP_GREATER_OR_EQUAL:
							if(lValue >= pV->fValue) Set((long)1); else Set((long)0);
						break;
						case OP_LESSER:
							if(lValue < pV->fValue) Set((long)1); else Set((long)0);
						break;
						case OP_LESSER_OR_EQUAL: 
							if(lValue <= pV->fValue) Set((long)1); else Set((long)0);
						break;
						case OP_NOT_EQUAL: 
							if(lValue != pV->fValue) Set((long)1); else Set((long)0);
						break;
						case OP_BOOL_AND:
							if(lValue != 0 && pV->fValue != 0) Set((long)1); else Set((long)0);
						break;
						case OP_BOOL_OR:
							if(lValue != 0 || pV->fValue != 0) Set((long)1); else Set((long)0);
						break;
					}
				break;
				default: 
					Set((long)0);
				return false;
			}
		break;
		case VAR_FLOAT:
			switch(pV->GetType())
			{
				case VAR_INTEGER:
					switch(op)
					{
						case OP_BOOL_EQUAL:
							if(fValue == pV->lValue) Set((long)1); else Set((long)0);
						break;
						case OP_GREATER:
							if(fValue > pV->lValue) Set((long)1); else Set((long)0);
						break;
						case OP_GREATER_OR_EQUAL:
							if(fValue >= pV->lValue) Set((long)1); else Set((long)0);
						break;
						case OP_LESSER:
							if(fValue < pV->lValue) Set((long)1); else Set((long)0);
						break;
						case OP_LESSER_OR_EQUAL: 
							if(fValue <= pV->lValue) Set((long)1); else Set((long)0);
						break;
						case OP_NOT_EQUAL: 
							if(fValue != pV->lValue) Set((long)1); else Set((long)0);
						break;
						case OP_BOOL_AND:
							if(fValue != 0 && pV->lValue != 0) Set((long)1); else Set((long)0);
						break;
						case OP_BOOL_OR:
							if(fValue != 0 || pV->lValue != 0) Set((long)1); else Set((long)0);
						break;
					}
				break;
				case VAR_FLOAT:
					switch(op)
					{
						case OP_BOOL_EQUAL:
							if(fValue == pV->fValue) Set((long)1); else Set((long)0);
						break;
						case OP_GREATER:
							if(fValue > pV->fValue) Set((long)1); else Set((long)0);
						break;
						case OP_GREATER_OR_EQUAL:
							if(fValue >= pV->fValue) Set((long)1); else Set((long)0);
						break;
						case OP_LESSER:
							if(fValue < pV->fValue) Set((long)1); else Set((long)0);
						break;
						case OP_LESSER_OR_EQUAL: 
							if(fValue <= pV->fValue) Set((long)1); else Set((long)0);
						break;
						case OP_NOT_EQUAL: 
							if(fValue != pV->fValue) Set((long)1); else Set((long)0);
						break;
						case OP_BOOL_AND:
							if(fValue != 0 && pV->fValue != 0) Set((long)1); else Set((long)0);
						break;
						case OP_BOOL_OR:
							if(fValue != 0 || pV->fValue != 0) Set((long)1); else Set((long)0);
						break;
					}
				break;
				default: 
					Set((long)0);
				return false;
			}
		break;
		case VAR_STRING:
			switch(pV->GetType())
			{
				case VAR_STRING:
					switch(op)
					{
						case OP_BOOL_EQUAL:
							if(sValue == pV->sValue) { Set((long)1); break; }
							if(sValue == 0 || pV->sValue == 0) { Set((long)0);  break; }
							if(stricmp(sValue,pV->sValue)==0) Set((long)1); else Set((long)0);
						break;
						case OP_GREATER:
							if(sValue == 0 || pV->sValue == 0) { Set((long)0); break; }
							if(strlen(sValue) > strlen(pV->sValue)) Set((long)1); else Set((long)0);
						break;
						case OP_GREATER_OR_EQUAL:
							if(sValue == 0 || pV->sValue == 0) { Set((long)0); break; }
							if(strlen(sValue) >= strlen(pV->sValue)) Set((long)1); else Set((long)0);
						break;
						case OP_LESSER:
							if(sValue == 0 || pV->sValue == 0) { Set((long)0); break; }
							if(strlen(sValue) < strlen(pV->sValue)) Set((long)1); else Set((long)0);
						break;
						case OP_LESSER_OR_EQUAL: 
							if(sValue == 0 || pV->sValue == 0) { Set((long)0); break; }
							if(strlen(sValue) <= strlen(pV->sValue)) Set((long)1); else Set((long)0);
						break;
						case OP_NOT_EQUAL: 
							if(sValue == pV->sValue) { Set((long)0); break; }
							if(sValue == 0 || pV->sValue == 0) { Set((long)1);  break; }
							if(stricmp(sValue,pV->sValue)!=0) Set((long)1); else Set((long)0);
	
						break;
						case OP_BOOL_AND:
						case OP_BOOL_OR:
							Error("boll operation on sting");
						return false;		
						
					}

				break;
				default: 
					Set((long)0);
				return false;
			}
		break;
		default: return false;
	}
	//return true;
	return false;
}

bool DATA::BoolConvert()
{
	if(Data_type == VAR_REFERENCE)
	{
		if(pReference) { return pReference->BoolConvert();}
		Error(UNINIT_REF);
		return false;
	}
	if(IsArray()) return false;
	switch(Data_type)
	{
		case VAR_INTEGER:
			if(lValue != 0) Set((long)1); else Set((long)0);
		break;
		case VAR_FLOAT:
			if(fValue != 0) Set((long)1); else Set((long)0);
		break;
		case VAR_STRING:
			if(sValue == 0 ) { Set((long)0);  break; }
			if(sValue[0] == 0 ) { Set((long)0);  break; }
			Set((long)1);
		break;
		default: 

		return false;
	}
	return true;
}

bool DATA::RefConvert()
{
	if(Data_type != VAR_REFERENCE) return false;
	if(IsArray()) return false;
	if(!pReference) { Error(UNINIT_REF); return false;}
	DATA * pV;
	pV = pReference->GetVarPointer();
	if(!pV) { Error(UNINIT_REF); return false;}
	ClearType();
	Copy(pV);
	return true;
}

void DATA::BadIndex(DWORD index, DWORD array_size)
{
	char buffer[MAX_PATH];
	wsprintf(buffer,"invalid index %d [size:%d]",index,array_size);
	Error(buffer);
}

long DATA::GetLong()
{
	return lValue;
}
	
float DATA::GetFloat()
{
	return fValue;
}
char * DATA::GetString()
{
	return sValue;
}

ENTITY_ID DATA::GetEntityID()
{
	return object_id;
}