#include "compiler.h"
#include "strings_list.h"
#include "..\..\common_h\vapi.h"
#include "core.h"

#define INVALID_FA	"Invalid function argument"
#define BAD_FA		"Bad function argument"
#define MISSING_PARAMETER	"Missing function parameter(s)"

extern VAPI * _CORE_API;
extern CORE Core;

//#define _TOFF

enum FUNCTION_CODE
{
	FUNC_RAND,
	FUNC_FRAND,
	FUNC_CREATE_CLASS,
	FUNC_CREATE_ENTITY,
	FUNC_DELETE_ENTITY,
	FUNC_SET_EVENT_HANDLER,
	FUNC_EXIT_PROGRAM,
	FUNC_GET_EVENTDATA,
	FUNC_EXECUTE,
	FUNC_STOP,
	FUNC_SEND_MESSAGE,
	FUNC_LOAD_SEGMENT,
	FUNC_UNLOAD_SEGMENT,
	FUNC_TRACE,
	FUNC_MAKE_INT,
	FUNC_MAKE_FLOAT,
	FUNC_LAYER_CREATE,
	FUNC_LAYER_DELETE,
	FUNC_LAYER_DELETE_CONTENT,
	FUNC_LAYER_SET_REALIZE,
	FUNC_LAYER_SET_EXECUTE,
	FUNC_LAYER_SET_MESSAGES,
	FUNC_LAYER_ADDOBJECT,
	FUNC_LAYER_DELOBJECT,
	FUNC_LAYER_FREEZE,
	FUNC_ABS,
	FUNC_SQRT,
	FUNC_SQR,
	FUNC_SIN,
	FUNC_COS,
	FUNC_TAN,
	FUNC_ATAN,
	FUNC_ATAN2,
	FUNC_DELETE_ATTRIBUTE,
	FUNC_SEGMENT_IS_LOADED,
	FUNC_GET_ATTRIBUTES_NUM,
	FUNC_GET_ATTRIBUTE_BYN,
	FUNC_GET_ATTRIBUTE_NAME,
	FUNC_DEL_EVENT_HANDLER,
	FUNC_ENTITY_UPDATE,
	FUNC_IS_ENTITY_LOADED,
	FUNC_DUMP_ATTRIBUTES,
	FUNC_STI,
	FUNC_STF,
	FUNC_CHECK_ATTRIBUTE,
	FUNC_ARGB,
	FUNC_DELETE_ENTITIES,
	FUNC_CLEAR_EVENTS,
	FUNC_SAVEENGINESTATE,
	FUNC_LOADENGINESTATE,
	FUNC_EVENT,
	FUNC_POSTEVENT,
	FUNC_FTS,
	FUNC_CLEAR_POST_EVENTS,
	FUNC_SET_ARRAY_SIZE,
	FUNC_GET_ATTRIBUTE_VALUE,
	FUNC_VARTYPE,
	FUNC_BREAKPOINT,
	FUNC_POW,
	FUNC_COPYATTRIBUTES,
	FUNC_GETENTITY,
	FUNC_GETENTITYNEXT,
	FUNC_GETENTITYNAME,
	FUNC_STRCUT,
	FUNC_FINDSUBSTR,
	FUNC_CLEARREF,
	FUNC_STRLEN,
	//FUNC_SETSAVEDATA,
	//FUNC_GETSAVEDATA,
	FUNC_GETDELTATIME,
	FUNC_EVENTSBREAK,
	FUNC_SHL,
	FUNC_SHR,
	FUNC_AND,
	FUNC_OR,
	FUNC_DELETEENTITIESBYTYPE,
	FUNC_CREATE_CONTROL,
	FUNC_DELETE_CONTROL,
	FUNC_MAP_CONTROL,
	FUNC_SET_CONTROL_FLAGS,
	FUNC_CLEAR_ENTITY_AP,
	FUNC_GET_ARRAY_SIZE,
	FUNC_GETTARGETPLATFORM,
	FUNC_FINDCLASS,
	FUNC_GETSYMBOL,
	FUNC_ISDIGIT,
	FUNC_SAVEVARIABLE,
	FUNC_LOADVARIABLE,
	FUNC_FINDCLASSNEXT,
	FUNC_SET_CONTROL_TRESHOLD,
	FUNC_LOCK_CONTROL,
	FUNC_TEST_REF,
	FUNC_SETTIMESCALE,
	FUNC_CHECKFUNCTION,
	FUNC_GETENGINEVERSION

};


INTFUNCDESC IntFuncTable[]=
{
	1,"Rand",VAR_INTEGER,
	0,"frnd",VAR_FLOAT,
	1,"CreateClass",VAR_OBJECT,
	2,"CreateEntity",VAR_INTEGER,
	1,"DeleteClass",TVOID,
	3,"SetEventHandler",TVOID,
	0,"ExitProgram",TVOID,
	0,"GetEventData",UNKNOWN,
	1,"Execute",TVOID,
	0,"Stop",TVOID,
	0,"SendMessage",VAR_INTEGER,
	1,"LoadSegment",VAR_INTEGER,
	1,"UnloadSegment",TVOID,
	1,"Trace",TVOID,
	1,"MakeInt",VAR_INTEGER,
	1,"MakeFloat",VAR_FLOAT,
	2,"LayerCreate",TVOID,
	1,"LayerDelete",TVOID,
	1,"LayerDeleteContent",TVOID,
	2,"LayerSetRealize",TVOID,
	2,"LayerSetExecute",TVOID,
	2,"LayerSetMessages",TVOID,
	3,"LayerAddObject",TVOID,
	2,"LayerDelObject",TVOID,
	2,"LayerFreeze",TVOID,
	1,"abs",UNKNOWN,
	1,"sqrt",VAR_FLOAT,
	1,"sqr",VAR_FLOAT,
	1,"sin",VAR_FLOAT,
	1,"cos",VAR_FLOAT,
	1,"tan",VAR_FLOAT,
	1,"atan",VAR_FLOAT,
	2,"atan2",VAR_FLOAT,
	2,"DeleteAttribute",TVOID,
	1,"SegmentIsLoaded",VAR_INTEGER,
	1,"GetAttributesNum",VAR_INTEGER,
	2,"GetAttributeN",VAR_AREFERENCE,
	1,"GetAttributeName",VAR_STRING,
	2,"DelEventHandler",TVOID,
	1,"EntityUpdate",TVOID,
	1,"IsEntity",VAR_INTEGER,
	1,"DumpAttributes",TVOID,
	1,"sti",VAR_INTEGER,
	1,"stf",VAR_FLOAT,
	2, "CheckAttribute",VAR_INTEGER,
	4,"argb",VAR_INTEGER,
	0,"DeleteEntities",TVOID,
	0,"ClearEvents",TVOID,
	1,"SaveEngineState",TVOID,
	1,"LoadEngineState",TVOID,
	0,"Event",TVOID,
	0,"PostEvent",TVOID,
	2,"fts",VAR_STRING,
	0,"ClearPostEvents",TVOID,
	2,"SetArraySize",TVOID,
	1,"GetAttributeValue",VAR_STRING,
	1,"Vartype",VAR_STRING,
	0,"Breakpoint",TVOID,
	2,"Pow",VAR_FLOAT,
	2,"CopyAttributes",TVOID,
	2,"GetEntity",VAR_INTEGER,
	1,"GetEntityNext",VAR_INTEGER,
	1,"GetEntityName",VAR_STRING,
	3,"strcut",VAR_STRING,
	3,"findSubStr",VAR_STRING,
	1,"ClearRef",TVOID,
	1,"strlen",VAR_INTEGER,
	0,"GetDeltaTime",VAR_INTEGER,
	0,"EventsBreak",TVOID,
	2,"shl",VAR_INTEGER,
	2,"shr",VAR_INTEGER,
	2,"and",VAR_INTEGER,
	2,"or",VAR_INTEGER,
	1,"DeleteEntitiesByType",TVOID,
	1,"CreateControl",VAR_INTEGER,
	1,"DeleteControl",TVOID,
	2,"MapControl",TVOID,
	2,"SetControlFlags",TVOID,
	1,"ClearEntityAP",TVOID,
	1,"GetArraySize",VAR_INTEGER,
	0,"GetTargetPlatform",VAR_STRING,
	2,"FindClass",VAR_INTEGER,
	2,"GetSymbol",VAR_STRING,
	2,"IsDigit",VAR_INTEGER,
	2,"SaveVariable",VAR_INTEGER,
	2,"LoadVariable",VAR_INTEGER,
	1,"FindClassNext",VAR_INTEGER,
	2,"SetControlTreshold",TVOID,
	2,"LockControl",TVOID,
	1,"TestRef",VAR_INTEGER,
	1,"SetTimeScale",TVOID,
	1,"CheckFunction",VAR_INTEGER,
	0,"GetEngineVersion",VAR_INTEGER
};
/*
char * FuncNameTable[]=
{
	"Rand",
	"frnd",
	"CreateClass",
	"CreateEntity",
	"DeleteClass",
	"SetEventHandler",
	"ExitProgram",
	"GetEventData",
	"Execute",
	"Stop",
	"SendMessage",
	"LoadSegment",
	"UnloadSegment",
	"Trace",
	"MakeInt",
	"MakeFloat",
	"LayerCreate",
	"LayerDelete",
	"LayerDeleteContent",
	"LayerSetRealize",
	"LayerSetExecute",
	"LayerSetMessages",
	"LayerAddObject",
	"LayerDelObject",
	"LayerFreeze",
	"abs",
	"sqrt",
	"sqr",
	"sin",
	"cos",
	"tan",
	"atan",
	"atan2",
	"DeleteAttribute",
	"SegmentIsLoaded",
	"GetAttributesNum",
	"GetAttributeN",
	"GetAttributeName",
	"DelEventHandler",
	"EntityUpdate",
	"IsEntity",
	"DumpAttributes",
	"sti",
	"stf",
	"CheckAttribute",
	"argb",
	"DeleteEntities",
	"ClearEvents",
	"SaveEngineState",
	"LoadEngineState",
	"Event",
	"PostEvent",
	"fts",
	"ClearPostEvents",
	"SetArraySize",
	"GetAttributeValue",
	"Vartype",
	"Breakpoint",
	"Pow",
	"CopyAttributes",
	"GetEntity",
	"GetEntityNext",
	"GetEntityName",
	"strcut",
	"findSubStr",
	"ClearRef",
	"strlen",
	"GetDeltaTime",
	"EventsBreak",
	"shl",
	"shr",
	"and",
	"or",
	"DeleteEntitiesByType",
	"CreateControl",
	"DeleteControl",
	"MapControl",
	"SetControlFlags",
	"ClearEntityAP",
	"GetArraySize",
	"GetTargetPlatform",
	"FindClass",
	"GetSymbol",
	"IsDigit",
	"SaveVariable",
	"LoadVariable",
	"FindClassNext",
	"SetControlTreshold",
	"LockControl",
	"TestRef",
	"SetTimeScale",
};
*/

/*
DWORD FuncArguments[]=
{
	1,//"Rand",
	0,//"frnd",
	1,//"CreateClass",
	2,//"CreateEntity",
	1,//"DeleteClass",
	3,//"SetEventHandler",
	0,//"ExitProgram",
	0,//"GetEventData",
	1,//"Execute",
	0,//"Stop",
	0,//"SendMessage",
	1,//"LoadSegment",
	1,//"UnloadSegment",
	1,//"Trace",
	1,//"MakeInt",
	1,//"MakeFloat",
	2,//"LayerCreate",
	1,//"LayerDelete",
	1,//"LayerDeleteContent",
	2,//"LayerSetRealize",
	2,//"LayerSetExecute",
	2,//"LayerSetMessages",
	3,//"LayerAddObject",
	2,//"LayerDelObject",
	2,//"LayerFreeze",
	1,//"abs",
	1,//"sqrt",
	1,//"sqr",
	1,//"sin",
	1,//"cos",
	1,//"tan",
	1,//"atan",
	2,//"atan2",
	2,//"DeleteAttribute",
	1,//"SegmentIsLoaded",
	1,//"GetAttributesNum",
	2,//"GetAttributeN",
	1,//"GetAttributeName",
	2,//"DelEventHandler",
	1,//"EntityUpdate",
	1,//"IsEntity",
	1,//"DumpAttributes",
	1,//"sti",
	1,//"stf",
	2,// "CheckAttribute",
	4,//"argb",
	0,//"DeleteEntity"
	0,//"ClearEvents"
	1,//"SaveEngineState",
	1,//"LoadEngineState",
	0,//"Event",
	0,//"PostEvent",
	2,//"fts",
	0,//"ClearPostEvents",
	2,//"SetArraySize",
	1,//"GetAttributeValue",
	1,//"Vartype"
	0,//"Breakpoint",
	2,//"Pow",
	2,//"CopyAttributes",
	2,//"GetEntity",
	1,//"GetEntityNext",
	1,//"GetEntityName",
	3,//"strcut",
	3,//"findSubStr",
	1,//"ClearRef",
	1,//"strlen"
	//2,//"SetSaveData"
	//1,//"GetSaveData"
	0,//"GetDeltaTime"
	0,//"EventsBreak"
	2,//"shl",
	2,//"shr",
	2,//"and",
	2,//"or",
	1,//"DeleteEntitiesByType",
	1,//"CreateControl",
	1,//"DeleteControl",
	2,//"MapControl",
	2,//"SetControlFlags",
	1,//"ClearEntityAP"
	1,//"GetArraySize"
	0,//"GetTargetPlatform"
	2,//"FindClass"
	2,//"GetSymbol"
	2,//"IsDigit"
	2,//"SaveVariable",
	2,//"LoadVariable",
	1,//"FindClassNext",
	2,//"SetControlTreshold",
	2,//"LockControl",
	1,//"TestRef"
	1,//"SetTimeScale"
};
*/
DWORD COMPILER::GetInternalFunctionArgumentsNum(DWORD code)
{
	if(GetIntFunctionsNum() <= code) 
	{
		SetError("invalid internal function code");
		return 0;
	}
	return IntFuncTable[code].dwArgsNum;
	//return FuncArguments[code];
}

DWORD COMPILER::GetIntFunctionsNum()
{
	return sizeof(IntFuncTable)/sizeof(INTFUNCDESC);
	//return sizeof(FuncNameTable)/sizeof(char *);
}

bool COMPILER::IsIntFuncVarArgsNum(DWORD code)
{
	//if(code == FUNC_SEND_MESSAGE) return true;
	switch(code)
	{
		case FUNC_SEND_MESSAGE:
		case FUNC_EVENT:
		case FUNC_POSTEVENT:

		return true;
	}
	return false;
}

DWORD COMPILER::GetIntFunctionCode(char * func_name)
{
	DWORD functions_num;
	DWORD n;
	//functions_num = sizeof(FuncNameTable)/sizeof(char *);
	functions_num = sizeof(IntFuncTable)/sizeof(INTFUNCDESC);
	
	for(n=0;n<functions_num;n++)
	{
		//if(strcmp(func_name,FuncNameTable[n])==0) return n;
		if(strcmp(func_name,IntFuncTable[n].pName)==0) return n;
		
	}
	return INVALID_ORDINAL_NUMBER;
}

DATA * COMPILER::BC_CallIntFunction(DWORD func_code,DATA * & pVResult,DWORD arguments)
{
//	char Format_string[MAX_PATH];
	char Message_string[2*MAX_PATH];
	ENTITY_ID entity_id;
	VAPI * api;
	DWORD functions_num;
	
	//functions_num = sizeof(FuncNameTable)/sizeof(char *);
	functions_num = sizeof(IntFuncTable)/sizeof(INTFUNCDESC);

	if(func_code >= functions_num) return null;

	DATA * pResult;
	DATA * pV;
	DATA * pV2;
	DATA * pV3;
	DATA * pV4;

	DATA Access;
	Access.SetVCompiler(this);
	float TempFloat1;
	float TempFloat2;
	long  TempLong1;
	long  TempLong2;
	long  TempLong;
	char * pChar;
	char * pChar2;
	ENTITY_ID TempEid;
	ENTITY_ID * pEid;
	DWORD n;
	ATTRIBUTES * pA;
	ATTRIBUTES * pRoot;
	ENTITY * pE;
	MESSAGE_SCRIPT ms;
	DWORD s_off;

	pResult = 0;

	pEid = 0;
	TempFloat1 = 0;
	TempLong1 = 0;

	api = _CORE_API;

	pVResult = 0;	// default - no return value

	long slen,slen2;
	char sVarName[64];
	char sBuff2[2];
	switch(func_code)
	{
		case FUNC_GETENGINEVERSION:
			pV = SStack.Push();
			pV->Set((long)ENGINE_SCRIPT_VERSION);
			pVResult = pV;
			return pV;
		break;
		case FUNC_CHECKFUNCTION:
			pV = SStack.Pop(); if(!pV){SetError(INVALID_FA); break;}
			if(pV->GetType() == VAR_STRING)
			{
				pV->Get(pChar);
				if( FuncTab.FindFunc(pChar) == INVALID_FUNC_CODE ) {
					pV = SStack.Push();
					pV->Set((long)0);
				} else {
					pV = SStack.Push();
					pV->Set((long)1);
				}
				pVResult = pV;
			}
			else SetError("incorrect argument type");
		break;
		case FUNC_SETTIMESCALE:
			pV = SStack.Pop(); if(!pV){SetError(INVALID_FA); break;}
			if(pV->GetType() == VAR_FLOAT)
			{
				pV->Get(TempFloat1);
				api->SetTimeScale(TempFloat1);
			} 
			else
			if(pV->GetType() == VAR_INTEGER)
			{
				pV->Get(TempLong1);
				api->SetTimeScale((float)TempLong1);
			}
			else SetError("incorrect argument type");
		break;
		case FUNC_TEST_REF:
			pV = SStack.Pop(); if(!pV){SetError(INVALID_FA); break;}
			pV = pV->GetVarPointer();
			if(pV == 0)
			{
				TempLong1 = 0;
			}
			else
			{
				switch(pV->GetType())
				{
					case VAR_REFERENCE:
						if(pV->pReference) TempLong1 = 1;
						else TempLong1 = 0;
					break;
					case VAR_AREFERENCE:
						if(pV->AttributesClass) TempLong1 = 1;
						else TempLong1 = 0;
					break;
					default: TempLong1 = 1;	break;
				}
			}
			pV = SStack.Push();
			pV->Set(TempLong1);
			pVResult = pV;
		return pV;

		case FUNC_LOCK_CONTROL:
			pV2 = SStack.Pop(); if(!pV2){SetError(INVALID_FA); break;}
			pV = SStack.Pop(); if(!pV){SetError(INVALID_FA); break;}
			pV->Get(pChar);
			pV2->Get(TempLong1);
			if(Core.Controls != 0) Core.Controls->LockControl(pChar,TempLong1!=0);
		break;
		/*case FUNC_SAVEVARIABLE:
			pV = SStack.Pop(); if(!pV){SetError(INVALID_FA); break;};	// var ref
			pV2 = SStack.Pop(); if(!pV2){SetError(INVALID_FA); break;};	// file name

			pV2->Get(pChar);
			
			
			fh = fio->_CreateFile(pChar,GENERIC_WRITE,FILE_SHARE_READ,CREATE_ALWAYS);
			if(fh == INVALID_HANDLE_VALUE) 
			{
				pV = SStack.Push();
				pV->Set((long)0);
				pVResult = pV;
				return pV;
			}

			hSaveFileFileHandle = fh;
			nIOFullSize = 0;
			nIOBufferSize = IOBUFFER_SIZE;
			pIOBuffer = (char *)RESIZE(pIOBuffer,IOBUFFER_SIZE);
			if(!pIOBuffer) 
			{
				fio->_CloseHandle(fh);
				pV = SStack.Push();
				pV->Set((long)0);
				pVResult = pV;
				return pV;
			}

			SaveVariable(pV->GetVarPointer());
			
			if(nIOFullSize > 0)
			{
				DWORD bytes;
				fio->_WriteFile(hSaveFileFileHandle,pIOBuffer,nIOFullSize,&bytes);
			}
			fio->_CloseHandle(fh);
			delete pIOBuffer; pIOBuffer = 0;
			nIOBufferSize = 0;
			nIOFullSize = 0;

			pV = SStack.Push();
			pV->Set((long)1);
			pVResult = pV;
		return pV;
	
		case FUNC_LOADVARIABLE:
			pV = SStack.Pop(); if(!pV){SetError(INVALID_FA); break;};	// var name
			pV2 = SStack.Pop(); if(!pV2){SetError(INVALID_FA); break;};	// file name

			pV2->Get(pChar);

			pV->Get(pChar2);
			
			
			fh = fio->_CreateFile(pChar,GENERIC_READ,FILE_SHARE_READ,OPEN_EXISTING);
			if(fh == INVALID_HANDLE_VALUE) 
			{
				pV = SStack.Push();
				pV->Set((long)0);
				pVResult = pV;
				return pV;
			}

			hSaveFileFileHandle = fh;
			
			ReadVariable(pChar2);
			
			fio->_CloseHandle(fh);
			pV = SStack.Push();
			pV->Set((long)1);
			pVResult = pV;
		return pV;*/
		
		case FUNC_ISDIGIT:
			pV = SStack.Pop(); if(!pV){SetError(INVALID_FA);break;};	// symbol index
			pV2 = SStack.Pop(); if(!pV2){SetError(INVALID_FA);break;};
			pV2 = pV2->GetVarPointer();	// string
			pV->Get(TempLong1);
			pV2->Get(pChar);
			if((DWORD)TempLong1 >= strlen(pChar))
			{
				pV = SStack.Push();
				pV->Set((long)0);
				pVResult = pV;
				return pV;
			}
			pV = SStack.Push();
			if(pChar[TempLong1] >= 0x30 && pChar[TempLong1] <= 0x39) pV->Set((long)1);
			else pV->Set((long)0);
			pVResult = pV;
		return pV;

		break;
		case FUNC_GETSYMBOL:
			pV = SStack.Pop(); if(!pV){SetError(INVALID_FA);break;};	// symbol index
			pV2 = SStack.Pop(); if(!pV2){SetError(INVALID_FA);break;};
			pV2 = pV2->GetVarPointer();	// string
			pV->Get(TempLong1);
			pV2->Get(pChar);
			if((DWORD)TempLong1 >= strlen(pChar))
			{
				pV = SStack.Push();
				pV->Set("");
				pVResult = pV;
				return pV;
			}
			sBuff2[0] = pChar[TempLong1];
			sBuff2[1] = 0;
			pV = SStack.Push();
			pV->Set(sBuff2);
			pVResult = pV;
		return pV;


		case FUNC_FINDCLASS:
			pV = SStack.Pop(); if(!pV){SetError(INVALID_FA);break;};
			pV2 = SStack.Pop(); if(!pV2){SetError(INVALID_FA);break;};
			
			
			pV->Convert(VAR_STRING);
			pV->Get(pChar);
			
			//_CORE_API->SetEntityScanLayer(0)//pChar);
			_CORE_API->FindClass(&entity_id,pChar,0);

			pV2 = pV2->GetVarPointer();
			pV2->Set(entity_id);
			pV2->SetType(VAR_AREFERENCE);
			pV2->SetAReference(_CORE_API->Entity_GetAttributePointer(&entity_id));

			if(_CORE_API->ValidateEntity(&entity_id)) TempLong1 = 1;
			else TempLong1 = 0;
			pV = SStack.Push();
			pV->Set(TempLong1);
			pVResult = pV;
		return pV;

		case FUNC_FINDCLASSNEXT:
			pV2 = SStack.Pop(); if(!pV2){SetError(INVALID_FA);break;};
			_CORE_API->FindClassNext(&entity_id);
			pV2 = pV2->GetVarPointer();
			pV2->Set(entity_id);
			pV2->SetType(VAR_AREFERENCE);
			pV2->SetAReference(_CORE_API->Entity_GetAttributePointer(&entity_id));

			if(_CORE_API->ValidateEntity(&entity_id)) TempLong1 = 1;
			else TempLong1 = 0;
			pV = SStack.Push();
			pV->Set(TempLong1);
			pVResult = pV;
		return pV;


		case FUNC_GETTARGETPLATFORM:
			pV = SStack.Push();	
#ifdef _XBOX			
			pV->Set("xbox");	
#else
			pV->Set("pc");	
#endif
			pVResult = pV;
		return pV;
	
		case FUNC_CLEAR_ENTITY_AP:
			pV = SStack.Pop(); if(!pV){SetError(INVALID_FA); break;}
			pV->Get(TempEid);
			Core.Entity_SetAttributePointer(&TempEid,0);
		break;
		case FUNC_CREATE_CONTROL:
			pV = SStack.Pop(); if(!pV){SetError(INVALID_FA); break;}
			pV->Get(pChar);
			if(Core.Controls != 0) TempLong1 = Core.Controls->CreateControl(pChar);
			else TempLong1 = -1;
			pV = SStack.Push();	
			pV->Set(TempLong1);	
			pVResult = pV;
		return pV;

		case FUNC_DELETE_CONTROL:
			pV = SStack.Pop(); if(!pV){SetError(INVALID_FA);break;}
			pV->Get(pChar);
			
			
		break;

		case FUNC_MAP_CONTROL:
			pV2 = SStack.Pop(); if(!pV2){SetError(INVALID_FA);break;}
			pV2->Get(TempLong1);

			pV = SStack.Pop(); if(!pV){SetError(INVALID_FA);break;}
			pV->Get(TempLong2);

			api->Controls->MapControl(TempLong2,TempLong1);
		break;

		case FUNC_SET_CONTROL_TRESHOLD:
			pV2 = SStack.Pop(); if(!pV2){SetError(INVALID_FA);break;}
			pV2->Get(TempFloat1);

			pV = SStack.Pop(); if(!pV){SetError(INVALID_FA);break;}
			pV->Get(TempLong2);

			api->Controls->SetControlTreshold(TempLong2,TempFloat1);

		break;

		case FUNC_SET_CONTROL_FLAGS:
			pV2 = SStack.Pop(); if(!pV2){SetError(INVALID_FA);break;}
			pV2->Get(TempLong1);

			pV = SStack.Pop(); if(!pV){SetError(INVALID_FA);break;}
			pV->Get(TempLong2);

			api->Controls->SetControlFlags(TempLong2,TempLong1);
		break;
		
		case FUNC_DELETEENTITIESBYTYPE:
			pV = SStack.Pop(); if(!pV){SetError(INVALID_FA);break;}
			pV->Get(pChar);


			if(Core.FindClass(&entity_id,pChar,0))
			{
				Core.DeleteEntity(entity_id);
				while(Core.FindClassNext(&entity_id)) {Core.DeleteEntity(entity_id);}
			}
		break;
		case FUNC_SHL:
			pV = SStack.Pop(); if(!pV){SetError(INVALID_FA);break;}
			pV->Get(TempLong2);

			pV = SStack.Pop(); if(!pV){SetError(INVALID_FA);break;}
			pV->Get(TempLong1);
			TempLong1 = TempLong1<<TempLong2;

			pV = SStack.Push();	
			pV->Set(TempLong1);	
			pVResult = pV;
		return pV;

	
		case FUNC_SHR:
			pV = SStack.Pop(); if(!pV){SetError(INVALID_FA);break;}
			pV->Get(TempLong2);

			pV = SStack.Pop(); if(!pV){SetError(INVALID_FA);break;}
			pV->Get(TempLong1);
			TempLong1 = TempLong1>>TempLong2;

			pV = SStack.Push();	
			pV->Set(TempLong1);	
			pVResult = pV;
		return pV;

		case FUNC_AND:
			pV = SStack.Pop(); if(!pV){SetError(INVALID_FA);break;}
			pV->Get(TempLong2);

			pV = SStack.Pop(); if(!pV){SetError(INVALID_FA);break;}
			pV->Get(TempLong1);
			TempLong1 = TempLong1&TempLong2;

			pV = SStack.Push();	
			pV->Set(TempLong1);	
			pVResult = pV;
		return pV;

		case FUNC_OR:
			pV = SStack.Pop(); if(!pV){SetError(INVALID_FA);break;}
			pV->Get(TempLong2);

			pV = SStack.Pop(); if(!pV){SetError(INVALID_FA);break;}
			pV->Get(TempLong1);
			TempLong1 = TempLong1|TempLong2;

			pV = SStack.Push();	
			pV->Set(TempLong1);	
			pVResult = pV;
		return pV;

		case FUNC_EVENTSBREAK:
			bEventsBreak = true;
		break;
		case FUNC_GETDELTATIME:
			pV = SStack.Push();	
			pV->Set((long)Core.GetDeltaTime());	
			pVResult = pV;
		return pV;

		/*case FUNC_SETSAVEDATA:
			pV = SStack.Pop(); if(!pV){SetError(INVALID_FA);break;}
			pV2 = SStack.Pop(); if(!pV2){SetError(INVALID_FA);break;}
			
			pV->Get(pChar);
			pV2->Get(pChar2);
			SetSaveData(pChar2,pChar);
			
			pV = SStack.Push();	pV->Set((long)1);	pVResult = pV;
		return pV;

		case FUNC_GETSAVEDATA:
			pV = SStack.Pop(); if(!pV){SetError(INVALID_FA);break;}
			pV->Get(pChar);
			GetSaveData(pChar,&Access);
			pV = SStack.Push();	
			Access.Get(pChar);

			pV->Set(pChar);	pVResult = pV;
		return pV;*/
		
		case FUNC_STRLEN:
			pV = SStack.Pop(); if(!pV){SetError(INVALID_FA);break;}// string or ref
			pV = pV->GetVarPointer();
			if(!pV) {SetError(INVALID_FA);break;}
			if(pV->GetType()!=VAR_STRING) {SetError("invalid argument type");break;}
			pV->Get(pChar);
			if(pChar == 0)
			{
				TempLong1 = 0;
			}
			else
			{
				TempLong1 = strlen(pChar);
			}
			pV = SStack.Push();
			pV->Set(TempLong1);
			pVResult = pV;
		return pV;
		case FUNC_CLEARREF:
			pV = SStack.Pop(); if(!pV){SetError(INVALID_FA);break;};// ref or aref
			switch(pV->GetType())
			{
				case VAR_REFERENCE:
					pV->SetReference(0);
				break;
				case VAR_AREFERENCE:
					pV->SetAReference(0);
				break;
			}
		break;

		case FUNC_STRCUT:
			
			pV3 = SStack.Pop(); if(!pV3){SetError(INVALID_FA);break;};// lastn
			pV2 = SStack.Pop(); if(!pV2){SetError(INVALID_FA);break;};// firstn
			pV = SStack.Pop(); if(!pV){SetError(INVALID_FA);break;};// string
			pV->Get(pChar);
			if(pChar == 0) 
			{
				SetError("Invalid string argument");
				pV = SStack.Push();
				pV->Set("");
				pVResult = pV;
				return pV;
			}
			else
			{
				slen = strlen(pChar);
				pV2->Get(TempLong1);
				pV3->Get(TempLong2);
				if(TempLong1 > TempLong2 || TempLong1 >= slen || TempLong2 >= slen)
				{
					SetError("Invalid range");
					pV = SStack.Push();
					pV->Set("");
					pVResult = pV;
					return pV;
				}

				if(TempLong1 == TempLong2)
				{
					Message_string[0] = pChar[TempLong1];
					Message_string[1] = 0;
					pV = SStack.Push();
					pV->Set(Message_string);
					pVResult = pV;
					return pV;
				}
				if(TempLong2 - TempLong1 >= sizeof(Message_string))
				{
					SetError("internal: buffer too small");
					pV = SStack.Push();
					pV->Set("");
					pVResult = pV;
					return pV;
				}
				memcpy(Message_string,pChar + TempLong1,TempLong2 - TempLong1 + 1);
				Message_string[TempLong2 - TempLong1 + 1] = 0;
				pV = SStack.Push();
				pV->Set(Message_string);
				pVResult = pV;
				return pV;

			}
		break;
			
		case FUNC_FINDSUBSTR:
			pV3 = SStack.Pop(); if(!pV3){SetError(INVALID_FA);break;};// fromn
			pV2 = SStack.Pop(); if(!pV2){SetError(INVALID_FA);break;};// substring
			pV = SStack.Pop(); if(!pV){SetError(INVALID_FA);break;};// string
			pV->Get(pChar);
			pV2->Get(pChar2);
			pV3->Get(TempLong1);
			if(pChar == 0 || pChar2 == 0) 
			{
				SetError("Invalid string argument");
				pV = SStack.Push();
				pV->Set("");
				pVResult = pV;
				return pV;
			}
			slen = strlen(pChar);
			slen2 = strlen(pChar2);
			if(slen < slen2)
			{
				pV = SStack.Push();
				pV->Set((long)-1);
				pVResult = pV;
				return pV;
			}
			
			
			n = TempLong1;
			while(n + (DWORD)slen2 <= (DWORD)slen)
			{
				if(strnicmp(pChar + n,pChar2,slen2) == 0)
				{
					pV = SStack.Push();
					pV->Set((long)n);
					pVResult = pV;
					return pV;
				}
				n++;
			}
			pV = SStack.Push();
			pV->Set((long)-1);
			pVResult = pV;
		return pV;
		
		case FUNC_GETENTITYNAME:
			pV = SStack.Pop(); if(!pV){SetError(INVALID_FA);break;};
			pV->Get(entity_id);
			pV = SStack.Push();
			VMA * pClass;
			pClass = Core.FindVMA(entity_id.class_code);
			if(pClass) pV->Set(pClass->GetName());
			else pV->Set("unknown class");
			pVResult = pV;
		return pV;
		case FUNC_GETENTITY:
			pV2 = SStack.Pop(); if(!pV2){SetError(INVALID_FA);break;};
			
			pV = SStack.Pop(); if(!pV){SetError(INVALID_FA);break;};
			pV->Convert(VAR_STRING);
			pV->Get(pChar);
			
			_CORE_API->SetEntityScanLayer(pChar);
			_CORE_API->GetEntity(&entity_id);

			pV2 = pV2->GetVarPointer();
			pV2->Set(entity_id);
			pV2->SetType(VAR_AREFERENCE);
			pV2->SetAReference(_CORE_API->Entity_GetAttributePointer(&entity_id));

			if(_CORE_API->ValidateEntity(&entity_id)) TempLong1 = 1;
			else TempLong1 = 0;
			pV = SStack.Push();
			pV->Set(TempLong1);
			pVResult = pV;
		return pV;
		case FUNC_GETENTITYNEXT:
			_CORE_API->GetEntityNext(&entity_id);
			pV = SStack.Pop(); if(!pV){SetError(INVALID_FA);break;};
			pV->GetVarPointer();
			pV->Set(entity_id);
			pV->SetType(VAR_AREFERENCE);
			pV->SetAReference(_CORE_API->Entity_GetAttributePointer(&entity_id));
			if(_CORE_API->ValidateEntity(&entity_id)) TempLong1 = 1;
			else TempLong1 = 0;
			pV = SStack.Push();
			pV->Set(TempLong1);
			pVResult = pV;
		return pV;
		case FUNC_POW:
			pV = SStack.Pop(); if(!pV){SetError(INVALID_FA);break;};
			pV->Convert(VAR_FLOAT);
			pV->Get(TempFloat1);

			pV2 = SStack.Pop(); if(!pV2){SetError(INVALID_FA);break;};
			pV2->Convert(VAR_FLOAT);
			pV2->Get(TempFloat2);

			TempFloat1 = (float)pow(TempFloat2,TempFloat1);
			pV = SStack.Push();
			pV->Set(TempFloat1);
			pVResult = pV;
		return pV;
	
		case FUNC_BREAKPOINT:
#ifdef _DEBUG
			_asm int 3;
#endif
		break;
		case FUNC_VARTYPE:
			pV = SStack.Pop(); if(!pV){SetError(INVALID_FA);break;};
			pV = pV->GetReference();
			if(!pV){SetError(INVALID_FA);break;};
			if(pV->IsReference()) strcpy(sVarName,"ref:"); else sVarName[0] = 0;
			pV = pV->GetVarPointer();
			switch(pV->GetType())
			{
			case VAR_INTEGER: strcat(sVarName,"int"); break;
			case VAR_FLOAT: strcat(sVarName,"float"); break;
			case VAR_STRING: strcat(sVarName,"string"); break;
			case VAR_OBJECT: strcat(sVarName,"object"); break;
			case VAR_REFERENCE: strcat(sVarName,"ref"); break;
			case VAR_AREFERENCE: strcat(sVarName,"aref"); break;
			}
			pV = SStack.Push();
			pV->Set(sVarName);
			pVResult = pV;
		return pV;
	
		case FUNC_SET_ARRAY_SIZE:
			pV2 = SStack.Pop(); if(!pV2){SetError(INVALID_FA);break;};
			pV = SStack.Pop(); if(!pV){SetError(INVALID_FA);break;};

			if(pV->GetType() != VAR_REFERENCE) {SetError(INVALID_FA); break;};
			pV = pV->GetVarPointer();
			if(pV == 0) {SetError(INVALID_FA); break;};
			if(!pV->IsArray()) {SetError("Not array"); break;};
			pV2->Get(TempLong1);
			if(TempLong1 <= 0) {SetError(INVALID_FA); break;};
			pV->SetElementsNum(TempLong1);
			
			if(pV->nGlobalVarTableIndex != 0xffffffff) VarTab.ArraySizeChanged(pV->nGlobalVarTableIndex,TempLong1);

		break;

		case FUNC_GET_ARRAY_SIZE:
			pV = SStack.Pop(); if(!pV){SetError(INVALID_FA);break;};
			if(pV->GetType() != VAR_REFERENCE) {SetError(INVALID_FA); break;};
			pV = pV->GetVarPointer();
			if(pV == 0) {SetError(INVALID_FA); break;};
			if(!pV->IsArray()) {SetError("Not array"); break;};
			TempLong1 = pV->GetElementsNum();
			pV = SStack.Push();
			pV->Set(TempLong1);
			pVResult = pV;
		return pV;
		

		case FUNC_LAYER_CREATE:

			pV2 = SStack.Pop(); if(!pV2){SetError(INVALID_FA);break;};
			pV = SStack.Pop(); if(!pV){SetError(INVALID_FA);break;};
			pV->Get(pChar);
			pV2->Get(TempLong1);
			if(TempLong1 == 0) _CORE_API->LayerCreate(pChar,false,false);
			else _CORE_API->LayerCreate(pChar,true,false);
		break;
		case FUNC_LAYER_DELETE:
			pV = SStack.Pop(); if(!pV) {SetError(INVALID_FA); break;};
			pV->Get(pChar);
			_CORE_API->LayerDelete(pChar);
		break;
		case FUNC_LAYER_DELETE_CONTENT:
			pV = SStack.Pop(); if(!pV) {SetError(INVALID_FA); break;};
			pV->Get(pChar);
			_CORE_API->LayerDeleteContent(pChar);
		break;
		case FUNC_LAYER_SET_REALIZE:
			pV2 = SStack.Pop(); if(!pV2){SetError(INVALID_FA);break;};
			pV = SStack.Pop(); if(!pV){SetError(INVALID_FA);break;};
			pV->Get(pChar);
			pV2->Get(TempLong1);
			if(TempLong1 == 0) _CORE_API->LayerSetRealize(pChar,false);
			else _CORE_API->LayerSetRealize(pChar,true);
		break;
		case FUNC_LAYER_SET_EXECUTE:
			pV2 = SStack.Pop(); if(!pV2){SetError(INVALID_FA);break;};
			pV = SStack.Pop(); if(!pV){SetError(INVALID_FA);break;};
			pV->Get(pChar);
			pV2->Get(TempLong1);
			if(TempLong1 == 0) _CORE_API->LayerSetExecute(pChar,false);
			else _CORE_API->LayerSetExecute(pChar,true);
		break;
		case FUNC_LAYER_SET_MESSAGES:
			pV2 = SStack.Pop(); if(!pV2){SetError(INVALID_FA);break;};
			pV = SStack.Pop(); if(!pV){SetError(INVALID_FA);break;};
			pV->Get(pChar);
			pV2->Get(TempLong1);
			if(TempLong1 == 0) _CORE_API->LayerClrFlags(pChar,LRFLAG_SYS_MESSAGES);
			else _CORE_API->LayerSetFlags(pChar,LRFLAG_SYS_MESSAGES);
		break;
		case FUNC_LAYER_ADDOBJECT:
			pV3 = SStack.Pop(); if(!pV3){SetError(INVALID_FA);break;};
			pV2 = SStack.Pop(); if(!pV2){SetError(INVALID_FA);break;};
			pV = SStack.Pop(); if(!pV){SetError(INVALID_FA);break;};
			pV->Get(pChar);
			pV2->Get(TempEid);
			pV3->Get(TempLong1);
			_CORE_API->LayerAdd(pChar,TempEid,TempLong1);
		break;
		case FUNC_LAYER_DELOBJECT:
			pV2 = SStack.Pop(); if(!pV2){SetError(INVALID_FA);break;};
			pV = SStack.Pop(); if(!pV){SetError(INVALID_FA);break;};
			pV->Get(pChar);
			pV2->Get(TempEid);
			_CORE_API->LayerDel(pChar,TempEid);
		break;
		case FUNC_LAYER_FREEZE:
			pV2 = SStack.Pop(); if(!pV2){SetError(INVALID_FA);break;};
			pV = SStack.Pop(); if(!pV){SetError(INVALID_FA);break;};
			pV->Get(pChar);
			pV2->Get(TempLong1);
			if(TempLong1 == 0) _CORE_API->LayerClrFlags(pChar,LRFLAG_FROZEN);
			else _CORE_API->LayerSetFlags(pChar,LRFLAG_FROZEN);
		break;

		case FUNC_IS_ENTITY_LOADED:
			pV = SStack.Pop(); if(!pV){SetError(INVALID_FA);break;};
			pV = pV->GetVarPointer();
			pV->Get(TempEid);
			pV = SStack.Push();
			if(Core.GetEntityPointer(&TempEid) != 0) TempLong1 = 1;
			else TempLong1 = 0;
			pV->Set(TempLong1);
			pVResult = pV;
		return pV;

		case FUNC_ENTITY_UPDATE:
			pV = SStack.Pop(); if(!pV){SetError(INVALID_FA);break;};
			pV->Get(TempLong1);
			if(TempLong1) bEntityUpdate = true;
			else bEntityUpdate = false;
		break;

		case FUNC_FRAND:
			TempFloat1 = (float)rand()/RAND_MAX;
			pV = SStack.Push();
			//TempFloat1 = 1.0f;	// ***
			pV->Set(TempFloat1);
			pVResult = pV;
		return pV;

		case FUNC_RAND:
			pV = SStack.Pop(); if(!pV){SetError(INVALID_FA);break;};
			pV->Get(TempLong1);
			TempLong2 = ((TempLong1 + 1) * rand())/RAND_MAX;
			if(TempLong2 > TempLong1) TempLong2 = TempLong1;
			pV = SStack.Push();
			//TempLong2 = TempLong1; // ***
			pV->Set(TempLong2);
			pVResult = pV;
		return pV;
	
		// create entity
		case FUNC_CREATE_ENTITY:
		
			pV2 = SStack.Pop();	// class name
			if(!pV2) { SetError(MISSING_PARAMETER); break; }
			//pV = SStack.Pop();	// object reference
			pV = SStack.Read();	// object reference
			if(!pV) { SetError(MISSING_PARAMETER); break; }

			pV2->Get(pChar);
			if(Core.CreateEntity(&entity_id,pChar,pV->GetAClass()))
			{
				//api->Entity_SetAttributePointer(&entity_id,pV->GetAClass());
				pV->Set(entity_id);
				SStack.Pop();
				pV = SStack.Push();
				pV->Set((long)1);
				pVResult = pV;
				return pV;
			} 
			else 
			{
				SStack.Pop();
				pV = SStack.Push();
				pV->Set((long)0);
				pVResult = pV;
				SetError("Cant create class: %s",pChar); return pV;
			}
		break;
		
		case FUNC_CREATE_CLASS:
			pV = SStack.Pop(); if(!pV){SetError(INVALID_FA);break;};
			pV->Get(pChar);
			if(api->CreateEntity(&entity_id,pChar))
			{
				pV = SStack.Push();
				pV->Set(entity_id);
				pVResult = pV;
				return pV;

			}
			SetError("Cant create class: %s",pChar);
		break;
		//
		case FUNC_DELETE_ENTITY:
			pV = SStack.Pop(); if(!pV){SetError(INVALID_FA);break;};
			pV->Get(entity_id);
			_CORE_API->DeleteEntity(entity_id);
		break;
		//
		case FUNC_DEL_EVENT_HANDLER:
			pV2 = SStack.Pop();	if(!pV2){SetError(INVALID_FA);break;};// func name
			pV = SStack.Pop();	if(!pV){SetError(INVALID_FA);break;};// event name
			pV->Get(pChar);
			pV2->Get(pChar2);
			DelEventHandler(pChar,pChar2);
		break;
		case FUNC_SET_EVENT_HANDLER:
			pV3 = SStack.Pop();	if(!pV3){SetError(INVALID_FA);break;};// flag
			pV2 = SStack.Pop();	if(!pV2){SetError(INVALID_FA);break;};// func name
			pV = SStack.Pop();	if(!pV){SetError(INVALID_FA);break;};// event name
			pV->Get(pChar);
			pV2->Get(pChar2);
			pV3->Get(TempLong1);
			SetEventHandler(pChar,pChar2,TempLong1);
		break;
		//
		case FUNC_EXIT_PROGRAM:
			ExitProgram();
			//_CORE_API->Exit();
		break;
		//
		case FUNC_GET_EVENTDATA:
			if(pEventMessage == 0)
			{
				SetError("No data on this event");
				return null;
			}
			char format_sym;
			format_sym = pEventMessage->GetCurrentFormatType();
			if(format_sym == 0)
			{
				SetError("No (more) data on this event");
				return null;
			}
			switch(format_sym)
			{
				case 'a':
					pResult = SStack.Push();
					pResult->SetType(VAR_AREFERENCE);
					pResult->SetAReference(pEventMessage->AttributePointer());
					pVResult = pResult;
				return pResult;
				case 'l':
					pResult = SStack.Push();
					pResult->Set(pEventMessage->Long());
					pVResult = pResult;
				return pResult;
				case 'f':
					pResult = SStack.Push();
					pResult->Set(pEventMessage->Float());
					pVResult = pResult;
				return pResult;
				case 's':
					pResult = SStack.Push();
					pEventMessage->String(sizeof(Message_string),Message_string);
					pResult->Set(Message_string);
					pVResult = pResult;
				return pResult;
				case 'i':
					pResult = SStack.Push();
					pResult->SetType(VAR_AREFERENCE);
					entity_id = pEventMessage->EntityID();
					pResult->Set(entity_id);
					pResult->SetAReference(_CORE_API->Entity_GetAttributePointer(&entity_id));

					pVResult = pResult;
				return pResult;
				case 'e':
					pResult = SStack.Push();
					DATA * pE;
					pE = (DATA *)pEventMessage->ScriptVariablePointer();
					pResult->SetReference(pE);
					pVResult = pResult;
				return pResult;
				default:
					SetError("Invalid data type in event message: '%c'",format_sym);
				return null;
			}
		break;
		case FUNC_EXECUTE:
			pV = SStack.Pop(); if(!pV){SetError(INVALID_FA);break;};
			pV->Get(pChar);
			_CORE_API->Execute(pChar);
		break;
		case FUNC_LOAD_SEGMENT:
			pV = SStack.Pop(); if(!pV){SetError(INVALID_FA);break;};
			Access.Copy(pV);
			//pV->Get(pChar);
			Access.Get(pChar);
			pV = SStack.Push();
			if(BC_LoadSegment(pChar)) pV->Set((long)1);
			else pV->Set((long)0);
			pVResult = pV;
		return pV;
		// 
		case FUNC_UNLOAD_SEGMENT:
			pV = SStack.Pop(); if(!pV){SetError(INVALID_FA);break;};
			pV->Get(pChar);
			UnloadSegment(pChar);
		break;
		case FUNC_SEGMENT_IS_LOADED:
			pV = SStack.Pop(); if(!pV){SetError(INVALID_FA);break;};
			pV->Get(pChar);
			pV = SStack.Push();
			if(BC_SegmentIsLoaded(pChar)) pV->Set((long)1);
			else pV->Set((long)0);
			pVResult = pV;
		break;
		// 
		case FUNC_STOP:
			bCompleted = true;
		break;
		//
		
		case FUNC_EVENT:
			s_off = SStack.GetDataNum() - arguments;	// set stack offset
			pV = SStack.Read(s_off,0); if(!pV){SetError(INVALID_FA);break;};
			pV->Get(pChar);
			if(arguments > 1)
			{
				CreateMessage(&ms,s_off,1);
				ms.ResetIndex();
				ProcessEvent(pChar,&ms);
			} else ProcessEvent(pChar);
			for(n=0;n<arguments;n++){SStack.Pop();}	// set stack pointer to correct position (vars in stack remain valid)
		break;
		case FUNC_POSTEVENT:
			MESSAGE_SCRIPT * pMS;
			S_EVENTMSG * pEM;
			s_off = SStack.GetDataNum() - arguments;	// set stack offset
			pV = SStack.Read(s_off,0); if(!pV){SetError(INVALID_FA);break;};
			pV->Get(pChar);
			pV = SStack.Read(s_off,1); if(!pV){SetError(INVALID_FA);break;};
			pV->Get(TempLong1);
			if(arguments >= 4)	// event w/o message
			{

				pMS = NEW MESSAGE_SCRIPT;
				CreateMessage(pMS,s_off,2);
				pMS->ResetIndex();
			} else pMS = 0;

			pEM = NEW S_EVENTMSG(pChar,pMS,TempLong1);
			EventMsg.Add(pEM);
			for(n=0;n<arguments;n++){SStack.Pop();}	
		break;
		case FUNC_SEND_MESSAGE:
			
			s_off = SStack.GetDataNum() - arguments;	// set stack offset

			
			pV = SStack.Read(s_off,0); if(!pV){SetError(INVALID_FA);break;};
			pV->Get(entity_id);
			
			CreateMessage(&ms,s_off,1);
			
			dword mresult;
			mresult = 0;
			pE = _CORE_API->GetEntityPointer(&entity_id);
			if(pE)
			{
				ms.ResetIndex();
				mresult = pE->ProcessMessage(ms);
			}
			for(n=0;n<arguments;n++){SStack.Pop();}	// set stack pointer to correct position (vars in stack remain valid)

			pV = SStack.Push();
			pV->Set((long)mresult);
			pVResult = pV;

		return pV;
		//break;
		case FUNC_TRACE:
			pV = SStack.Pop(); if(!pV){SetError(INVALID_FA);break;};
			pV->RefConvert();
			pV->Convert(VAR_STRING);
			pV->Get(pChar);
#ifndef _TOFF
			DTrace(pChar);
#endif
		break;
		case FUNC_STI:
		case FUNC_MAKE_INT:
			pV = SStack.Pop(); if(!pV){SetError(INVALID_FA);break;};
			pV->Convert(VAR_INTEGER);
			pV->Get(TempLong1);
			pV = SStack.Push();
			pV->Set(TempLong1);
			pVResult = pV;
		return pV;
		case FUNC_STF:
		case FUNC_MAKE_FLOAT:
			pV = SStack.Pop(); if(!pV){SetError(INVALID_FA);break;};
			pV->Convert(VAR_FLOAT);
			pV->Get(TempFloat1);
			pV = SStack.Push();
			pV->Set(TempFloat1);
			pVResult = pV;
		return pV;
		case FUNC_FTS:
			pV2 = SStack.Pop(); if(!pV2){SetError(INVALID_FA);break;};
			pV = SStack.Pop(); if(!pV){SetError(INVALID_FA);break;};
			if(pV->GetType() != VAR_FLOAT) {SetError(INVALID_FA); break;}
			if(pV2->GetType() != VAR_INTEGER) {SetError(INVALID_FA); break;}
			pV->Get(TempFloat1);
			pV2->Get(TempLong1);
			pV = SStack.Push();
			gcvt(TempFloat1,TempLong1,gs);
			pV->Set(gs);
			pVResult = pV;
		return pV;
		case FUNC_ABS:
			pV = SStack.Pop(); if(!pV){SetError(INVALID_FA);break;};

			switch(pV->GetType())
			{
				case VAR_INTEGER:
					pV->Get(TempLong1);
					TempLong1 = abs(TempLong1);
					pV = SStack.Push();
					pV->Set(TempLong1);
					pVResult = pV;
				return pV;
				case VAR_FLOAT:
					pV->Get(TempFloat1);
					TempFloat1 = (float)fabs(TempFloat1);
					pV = SStack.Push();
					pV->Set(TempFloat1);
					pVResult = pV;
				return pV;
				default:
					SetError("Invalid func 'abs' argument");
				return null;
			}
		break;

		case FUNC_SQRT:
			pV = SStack.Pop(); if(!pV){SetError(INVALID_FA);break;};
			switch(pV->GetType())
			{
				case VAR_INTEGER:
					pV->Get(TempLong1);
					if(TempLong1 < 0)
					{
						SetError("Negative func 'sqrt' argument");
						return null;
					}
					TempLong1 = (long)sqrtf((float)TempLong1);
					pV = SStack.Push();
					pV->Set(TempLong1);
					pVResult = pV;
				return pV;
				case VAR_FLOAT:
					pV->Get(TempFloat1);
					if(TempFloat1 < 0)
					{
						SetError("Negative func 'sqrt' argument");
						return null;
					}
					TempFloat1 = (float)sqrt(TempFloat1);
					pV = SStack.Push();
					pV->Set(TempFloat1);
					pVResult = pV;
				return pV;
				default:
					SetError("Invalid func 'sqrt' argument");
				return null;
			}
		break;
		case FUNC_SQR:
			pV = SStack.Pop(); if(!pV){SetError(INVALID_FA);break;};
			switch(pV->GetType())
			{
				case VAR_INTEGER:
					pV->Get(TempLong1);
					TempLong1 = TempLong1*TempLong1;
					pV = SStack.Push();
					pV->Set(TempLong1);
					pVResult = pV;
				return pV;
				case VAR_FLOAT:
					pV->Get(TempFloat1);
					TempFloat1 = TempFloat1*TempFloat1;
					pV = SStack.Push();
					pV->Set(TempFloat1);
					pVResult = pV;
				return pV;
				default:
					SetError("Invalid func 'sqr' argument");
				return null;
			}
		break;
		case FUNC_SIN:
			pV = SStack.Pop();
			if(pV == 0)
			{
				SetError("Missing func 'sin' argument(s)");
				return null;
			}
			switch(pV->GetType())
			{
				case VAR_INTEGER:
					pV->Get(TempLong1);
					TempFloat1 = (float)sinf((float)TempLong1);
				break;
				case VAR_FLOAT:
					pV->Get(TempFloat1);
					TempFloat1 = (float)sin(TempFloat1);
				break;
				default:
					SetError("Invalid func 'sin' argument");
				return null;
			}
			pV = SStack.Push();
			pV->Set(TempFloat1);
			pVResult = pV;
		return pV;
		
		case FUNC_COS:
			pV = SStack.Pop(); if(!pV){SetError(INVALID_FA);break;};
			switch(pV->GetType())
			{
				case VAR_INTEGER:
					pV->Get(TempLong1);
					TempFloat1 = (float)cosf((float)TempLong1);
				break;
				case VAR_FLOAT:
					pV->Get(TempFloat1);
					TempFloat1 = (float)cos(TempFloat1);
				break;
				default:
					SetError("Invalid func 'cos' argument");
				return null;
			}
			pV = SStack.Push();
			pV->Set(TempFloat1);
			pVResult = pV;
		return pV;
		case FUNC_TAN:
			pV = SStack.Pop(); if(!pV){SetError(INVALID_FA);break;};
			switch(pV->GetType())
			{
				case VAR_INTEGER:
					pV->Get(TempLong1);
					TempFloat1 = (float)tanf((float)TempLong1);
				break;
				case VAR_FLOAT:
					pV->Get(TempFloat1);
					TempFloat1 = (float)tan(TempFloat1);
				break;
				default:
					SetError("Invalid func 'tan' argument");
				return null;
			}
			pV = SStack.Push();
			pV->Set(TempFloat1);
			pVResult = pV;
		return pV;
		case FUNC_ATAN:
			pV = SStack.Pop(); if(!pV){SetError(INVALID_FA);break;};
			switch(pV->GetType())
			{
				case VAR_INTEGER:
					pV->Get(TempLong1);
					TempFloat1 = (float)atanf((float)TempLong1);
				break;
				case VAR_FLOAT:
					pV->Get(TempFloat1);
					TempFloat1 = (float)atan(TempFloat1);
				break;
				default:
					SetError("Invalid func 'atan' argument");
				return null;
			}
			pV = SStack.Push();
			pV->Set(TempFloat1);
			pVResult = pV;
		return pV;
		case FUNC_ATAN2:
		
			pV2 = SStack.Pop(); if(!pV2){SetError(INVALID_FA);break;};
			pV = SStack.Pop(); if(!pV){SetError(INVALID_FA);break;};
			switch(pV->GetType())
			{
				case VAR_FLOAT:
				case VAR_INTEGER:
					pV->Convert(VAR_FLOAT);
					switch(pV->GetType())
					{
						case VAR_FLOAT:
						case VAR_INTEGER:
							pV2->Convert(VAR_FLOAT);
						break;
						default:
							SetError("Invalid func 'atan2' argument");
						return null;
					}
					pV->Get(TempFloat1);
					pV2->Get(TempFloat2);
					TempFloat1 = (float)atan2(TempFloat1,TempFloat2);
					pV = SStack.Push();
					pV->Set(TempFloat1);
					pVResult = pV;
				return pV;
				default:
					SetError("Invalid func 'atan2' argument");
				return null;
			}
		break;
		case FUNC_COPYATTRIBUTES:
			// source
			pV2 = SStack.Pop(); if(!pV2){SetError(INVALID_FA);break;};
			// destination
			pV = SStack.Pop(); if(!pV){SetError(INVALID_FA);break;};
			
			pRoot = pV->GetAClass();
			pA = pV2->GetAClass();

			if(pA == 0 || pRoot == 0) {SetError("AClass ERROR n1"); break;}
			pRoot->Copy(pA);
		break;
		case FUNC_DELETE_ATTRIBUTE:
			pV2 = SStack.Pop(); if(!pV2){SetError(INVALID_FA);break;};
			pV2->Get(pChar);

			pV = SStack.Pop(); if(!pV){SetError(INVALID_FA);break;};
			//pV->Get(TempEid);
			pRoot = pV->GetAClass();
			if(pRoot == 0) {SetError("AClass ERROR n1"); break;}
			pA = pRoot->FindAClass(pRoot,pChar);
			pRoot->DeleteAttributeClassX(pA);
		break;
		case FUNC_CHECK_ATTRIBUTE:
			pV2 = SStack.Pop(); if(!pV2){SetError(INVALID_FA);break;};
			pV2->Get(pChar);
			pV = SStack.Pop(); if(!pV){SetError(INVALID_FA);break;};

			pV = pV->GetVarPointer();
			if(!pV) TempLong1 = 0;
			else
			{
				switch(pV->GetType())
				{
					case VAR_AREFERENCE:
						if(!pV->AttributesClass) 
						{
							TempLong1 = 0;
							break;
						}

					default:
						pRoot = pV->GetAClass();
						if(pRoot)
						{
							pA = pRoot->FindAClass(pRoot,pChar);
							if(pA) TempLong1 = 1;
							else TempLong1 = 0;
						} else TempLong1 = 0;
					break;
				}
			}
			pV = SStack.Push();
			pV->Set(TempLong1);
			pVResult = pV;
		return pV;
		case FUNC_GET_ATTRIBUTES_NUM:
			pV = SStack.Pop(); if(!pV){SetError(INVALID_FA);break;};
			pV = pV->GetVarPointer();
			if(!(pV->GetType() == VAR_AREFERENCE || pV->GetType() == VAR_OBJECT)) {SetError(BAD_FA);break;};
			pA = pV->GetAClass();
			if(pA) TempLong1 = pA->GetAttributesNum();
			else TempLong1 = 0;
			pV = SStack.Push();
			pV->Set(TempLong1);
			pVResult = pV;
		return pV;
		case FUNC_GET_ATTRIBUTE_BYN:
			pV = SStack.Pop(); if(!pV){SetError(INVALID_FA);break;};
			pV->Get(TempLong1);

			pV = SStack.Pop(); if(!pV){SetError(INVALID_FA);break;};
			pV = pV->GetVarPointer();
			if(!(pV->GetType() == VAR_AREFERENCE || pV->GetType() == VAR_OBJECT)) {SetError(BAD_FA);break;};
			pA = pV->GetAClass();
						
			if(pA) pA = pA->GetAttributeClass(TempLong1);
			if(pA == 0) {SetError("incorrect argument index");break;};
			pV = SStack.Push();
			pV->SetType(VAR_AREFERENCE);
			pV->SetAReference(pA);
			pVResult = pV;
		return pV;
		case FUNC_GET_ATTRIBUTE_VALUE:
			pV = SStack.Pop(); if(!pV){SetError(INVALID_FA);break;};
			pV = pV->GetVarPointer();
			if(!(pV->GetType() == VAR_AREFERENCE || pV->GetType() == VAR_OBJECT)) {SetError(BAD_FA);break;};
			pA = pV->GetAClass();
			if(pA) pChar = pA->GetThisAttr();
			else pChar = "AClass ERROR n1";
			pV = SStack.Push();
			pV->Set(pChar);
			pVResult = pV;
		return pV;
		case FUNC_GET_ATTRIBUTE_NAME:
			pV = SStack.Pop(); if(!pV){SetError(INVALID_FA);break;};
			pV = pV->GetVarPointer();
			if(!(pV->GetType() == VAR_AREFERENCE || pV->GetType() == VAR_OBJECT)) {SetError(BAD_FA);break;};
			pA = pV->GetAClass();
			if(pA) pChar = pA->GetThisName();
			else pChar = "AClass ERROR n1";
			pV = SStack.Push();
			pV->Set(pChar);
			pVResult = pV;
		return pV;
		case FUNC_DUMP_ATTRIBUTES:
			pV = SStack.Pop(); if(!pV){SetError(INVALID_FA);break;};
			pV = pV->GetVarPointer();
			if(!(pV->GetType() == VAR_AREFERENCE || pV->GetType() == VAR_OBJECT)) {SetError(BAD_FA);break;};
			pA = pV->GetAClass();
			if(pA == 0) {SetError("AClass ERROR n1"); break;}
#ifndef _TOFF
			DumpAttributes(pA,0);
#endif
		break;
		case FUNC_ARGB:
			pV4 = SStack.Pop(); if(!pV4){SetError(INVALID_FA);break;};
			pV3 = SStack.Pop(); if(!pV3){SetError(INVALID_FA);break;};
			pV2 = SStack.Pop(); if(!pV2){SetError(INVALID_FA);break;};
			pV = SStack.Pop(); if(!pV){SetError(INVALID_FA);break;};

			pV->Get(TempLong);
			TempLong = TempLong<<24;
			pV2->Get(TempLong2);
			TempLong2 = TempLong2<<16;
			TempLong = TempLong | TempLong2;
			pV3->Get(TempLong2);
			TempLong2 = TempLong2<<8;
			TempLong = TempLong | TempLong2;
			pV4->Get(TempLong2);
			TempLong = TempLong | TempLong2;
			
			pV = SStack.Push();
			pV->Set(TempLong);
			pVResult = pV;
		return pVResult;
		case FUNC_DELETE_ENTITIES:
			Core.DeleteEntities();
		break;
		case FUNC_CLEAR_EVENTS:
			Core.ClearEvents();
		break;
		case FUNC_CLEAR_POST_EVENTS:
			//EventMsg.Release();
			EventMsg.InvalidateAll();
			break;
		case FUNC_SAVEENGINESTATE:
			pV = SStack.Pop(); 
			if(!pV){SetError(INVALID_FA);break;};
			pV->Convert(VAR_STRING);
			pV->Get(pChar);
			_CORE_API->SaveState(pChar);
		break;
		case FUNC_LOADENGINESTATE:
			pV = SStack.Pop(); 
			if(!pV){SetError(INVALID_FA);break;};
			pV->Convert(VAR_STRING);
			pV->Get(pChar);
			_CORE_API->InitiateStateLoading(pChar);
		break;
	}
	return null;
}

void COMPILER::DumpAttributes(ATTRIBUTES * pA, long level)
{
	char buffer[128];
	DWORD n;
	if(pA == 0) return;

	if(level >= 128) level = 127;
	if(level != 0) memset(buffer,' ',level); buffer[level] = 0;
	
	for(n=0;n<pA->GetAttributesNum();n++)
	{
		DTrace("%s%s = %s",buffer,pA->GetAttributeName(n),pA->GetAttribute(n));
		DumpAttributes(pA->GetAttributeClass(pA->GetAttributeName(n)), level + 2);
	}
}


// assume first param - format string
bool COMPILER::CreateMessage(MESSAGE_SCRIPT * pMs, DWORD s_off, DWORD var_offset, bool s2s)
{
	long  TempLong1;
	float TempFloat1;
	ENTITY_ID TempEid;
	ATTRIBUTES * pA;
	char * Format_string;
	char * pChar;
	DATA * pV;
	DWORD n;
	
	if(pMs == 0) return false;

	// read format string
	pV = SStack.Read(s_off,var_offset); if(!pV){SetError(INVALID_FA); return false;};
	var_offset++;
	// set pointer to format string
	pV->Get(Format_string);
	if(Format_string == 0)
	{
		SetError("format string is null"); return false;
	}
	// reset message class data
	pMs->Reset(Format_string);
	// scan format string	
	n = 0;
	while(Format_string[n])
	{
		// read stack data
		pV = SStack.Read(s_off,var_offset); var_offset++;
		if(!pV)	{ SetError("No data in CreateMessage()"); return false; }

		switch(Format_string[n])
		{
			case 'l':
				pV = pV->GetVarPointer();
				if(pV->GetType() != VAR_INTEGER) 
				{
					SetError("CreateMessage: Invalid Data");
					return false;
				}
				pV->Get(TempLong1);
				pMs->Set((char *)&TempLong1);
			break;
			case 'f':
				pV = pV->GetVarPointer();
				if(pV->GetType() != VAR_FLOAT) 
				{
					if(pV->GetType() == VAR_INTEGER)
					{
						pV->Convert(VAR_FLOAT);
					}
					else
					{
						SetError("CreateMessage: Invalid Data");
						return false;
					}
				}
				pV->Get(TempFloat1);
				pMs->Set((char *)&TempFloat1);
			break;
			case 'i':
				pV = pV->GetVarPointer();
				if(!(pV->GetType() == VAR_OBJECT || pV->GetType() == VAR_AREFERENCE)) 
				{
					SetError("CreateMessage: Invalid Data");
					return false;
				}
				pV->Get(TempEid);
				pMs->Set((char *)&TempEid);
			break;
			case 'e':
				pV = pV->GetVarPointer();
				pMs->Set((char *)&pV);
			break;
			case 's':
				if(pV->GetType() != VAR_STRING) 
				{
					SetError("CreateMessage: Invalid Data");
					return false;
				}
				pV->Get(pChar);
				pMs->Set((char *)pChar);
			break;
			case 'a':
				pV = pV->GetVarPointer();
				if(!(pV->GetType() == VAR_OBJECT || pV->GetType() == VAR_AREFERENCE)) 
				{
					SetError("CreateMessage: Invalid Data");
					return false;
				}
				pA = pV->GetAClass();
				pMs->Set((char *)&pA);
			break;

		}
		n++;
	}
	return true;
}