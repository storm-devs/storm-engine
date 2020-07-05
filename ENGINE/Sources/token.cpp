#include <stdio.h>
#include <string.h>
#include "token.h"
#include "system_log.h"

#include "..\..\common_h\memop.h"
#include "..\..\common_h\vapi.h"

extern VAPI * api;
#define DISCARD_DATABUFFER {if(pTokenData) pTokenData[0] = 0;}
#define INVALID_ARG_DCHARS	32
char * TokenTypeName[] = 
{
	"END_OF_PROGRAMM",
	"INVALID_TOKEN",
	"UNKNOWN",
	"WHITESPACE",
	"COMMENT",
	"INCLIDE_FILE",
	"integer",
	"float",
	"string",
	"object",
	"reference",
	"attribute reference",
	"BLOCK_IN",
	"BLOCK_OUT",
	"NUMBER",
	"STRING",
	"FLOAT_NUMBER",
	"SEPARATOR",
	"FUNCTION_RETURN",
	"FUNCTION_RETURN_VOID",
	"FOR_BLOCK",
	"IF_BLOCK",
	"WHILE_BLOCK",
	"CONTINUE_COMMAND",
	"BREAK_COMMAND",
	"GOTO_COMMAND",
	"LABEL",
	"VOID",
	"SWITCH_COMMAND",
	"CASE_COMMAND",
	"DEFINE_COMMAND",
	"MAKEREF_COMMAND",
	"MAKEAREF_COMMAND",
	"OPEN_BRACKET",
	"CLOSE_BRACKET",
	"SQUARE_OPEN_BRACKET",
	"SQUARE_CLOSE_BRACKET",
	"OP_EQUAL",
	"OP_BOOL_EQUAL",
	"OP_GREATER",
	"OP_GREATER_OR_EQUAL",
	"OP_LESSER",
	"OP_LESSER_OR_EQUAL",
	"OP_NOT_EQUAL",
	"OP_MINUS",
	"OP_PLUS",
	"OP_MULTIPLY",
	"OP_DIVIDE",
	"OP_POWER",
	"OP_MODUL",
	"OP_INC",
	"OP_DEC",
	"OP_NEG",
	"LINE_COMMENT",
	"COMMA",
	"DOT",
	"AND",
	"CALL_FUNCTION",
	"DEBUG_FILE_NAME",
	"DEBUG_LINE_CODE",
	"DEBUG_LINEFEED",
	"VARIABLE",
	"LOCAL_VARIABLE",
	"EXTERN",
	"ACCESS_WORD",
	"ACCESS_VAR",
	"STACK_ALLOC",
	"STACK_PUSH",
	"STACK_POP",
	"STACK_READ",
	"STACK_WRITE",
	"STACK_WRITE_BXINDEX",
	"STACK_COMPARE",
	"AX",
	"BX",
	"EX",
	"AP",
	"STACK_TOP",
	"AP_VALUE",
	"MOVE",
	"MOVEAP",
	"MOVEAP_BXINDEX",
	"ADVANCE_AP",
	"ARRAY_INDEX",
	"EXPRESSION_END",
	"JUMP",
	"JUMP_Z",
	"JUMP_NZ",
	"PROCESS_EXPRESSION",
	"PUSH_EXPRESULT",
	"POP_EXPRESULT",
	"POP_VOID",
	"OP_BOOL_AND",
	"OP_BOOL_OR",
	"ELSE_BLOCK",
	"TRUE_CONST",
	"FALSE_CONST",
	"OP_BOOL_NEG",
	"ACCESS_WORD_CODE",
	"EVENT_HANDLER",
	"OP_INCADD",
	"OP_DECADD",
	"OP_MULTIPLYEQ",
	"OP_DIVIDEEQ",
	"CLASS_DECL",
	"CLASS_OBJECT",
	"IMPORT",
	"INCLUDE_LIBRIARY",
	"CALL",
	"SETREF",
	"SETREF_BXINDEX",
	"OP_SPLUS",
	"OP_SMINUS",
	"OP_BOOL_CONVERT",
	"OP_REF_CONVERT",
	"OP_COMPARE_AND_SET",
	"LEFT_OPERAND",
	"STACK_POP_VOID",
	"DEFINE_VAL",
	"ARGS_NUM",
	"HOLD_COMPILATION",
	"SETAREF",
	"VERIFY_AP",
	"POP_NZ",
	"LEFTOP_INDEX",
	"PUSH_OBJID",
	"PUSH_OBJID_BXINDEX",

};

struct S_KEYWORD
{
	S_TOKEN_TYPE type;
	char * name;
};


S_KEYWORD Keywords[] = 
{
	HOLD_COMPILATION,"#hold",
	INCLIDE_FILE, "#include",
	EVENT_HANDLER, "#event_handler",
	INCLUDE_LIBRIARY,"#libriary",
	VAR_INTEGER, "int",
	VAR_FLOAT,"float",
	VAR_STRING,"string",
	VAR_OBJECT,"object",
	VAR_REFERENCE,"ref",
	VAR_AREFERENCE,"aref",
	BLOCK_IN,"{",
	BLOCK_OUT,"}",
	SEPARATOR,";",
	FUNCTION_RETURN,"return",
//	FUNCTION_RETURN_VOID,"return",
	FOR_BLOCK,"for",
	IF_BLOCK,"if",
	WHILE_BLOCK,"while",
	CONTINUE_COMMAND,"continue",
	BREAK_COMMAND,"break",
	GOTO_COMMAND,"goto",
	LABEL,":",
	TVOID,"void",
	SWITCH_COMMAND,"switch",
	CASE_COMMAND,"case",
	DEFINE_COMMAND,"#define",
	MAKEREF_COMMAND,"makeref",
	MAKEAREF_COMMAND,"makearef",
	OPEN_BRACKET,"(",
	CLOSE_BRACKET,")",
	SQUARE_OPEN_BRACKET,"[",
	SQUARE_CLOSE_BRACKET,"]",
	OP_EQUAL,"=",
	OP_BOOL_EQUAL,"==",
	OP_GREATER,">",
	OP_GREATER_OR_EQUAL,">=",
	OP_LESSER,"<",
	OP_LESSER_OR_EQUAL,"<=",
	OP_NOT_EQUAL,"!=",
	OP_INCADD,"+=",
	OP_DECADD,"-=",
	OP_MULTIPLYEQ,"*=",
	OP_DIVIDEEQ,"/=",
	OP_MINUS,"-",
	OP_PLUS,"+",
	OP_MULTIPLY,"*",
	OP_DIVIDE,"/",
	OP_POWER,"^",
	OP_MODUL,"%",
	OP_INC,"++",
	OP_DEC,"--",
	LINE_COMMENT,"//",
	LINE_COMMENT,"#ifndef",
	LINE_COMMENT,"#endif",
	COMMA,",",
	DOT,".",
	OP_BOOL_AND,"&&",
	OP_BOOL_OR,"||",
	AND,"&",
	DEBUG_FILE_NAME,"#file",
	EXTERN,"extern",
	ELSE_BLOCK,"else",
	VAR_INTEGER,"bool",
	TRUE_CONST,"true",
	FALSE_CONST,"false",
	OP_BOOL_NEG,"!",
	CLASS_DECL,"class",
	IMPORT,"native",
	CALL,"call",
	
};

TOKEN::TOKEN()
{
	pTokenData = 0;
	eTokenType = UNKNOWN;
	TokenDataBufferSize = 0;
	ZeroMemory(ProgramSteps,sizeof(ProgramSteps));
	ProgramStepsNum = 0;	
	Program = 0;
	ProgramBase = 0;
	Lines_in_token = 0;
	
	InitializeHashTable();
}

void TOKEN::Release()
{
	DWORD n;
	for(n=0;n<TOKENHASHTABLE_SIZE;n++)
	{
		if(KeywordsHash[n].pIndex) delete KeywordsHash[n].pIndex;
		KeywordsHash[n].pIndex = 0;
	}
}

TOKEN::~TOKEN()
{
	Reset();
}

void TOKEN::SetProgram(char * pProgramBase, char * pProgramControl)
{
	Program = pProgramControl;
	ProgramBase = pProgramBase;
	ZeroMemory(ProgramSteps,sizeof(ProgramSteps));
	ProgramStepsNum = 0;
}

void TOKEN::SetProgramControl(char * pProgramControl)
{
	Program = pProgramControl;
}


char * TOKEN::GetProgramControl()
{
	return Program;
}

DWORD TOKEN::GetProgramOffset()
{
	return (DWORD)Program - (DWORD)ProgramBase;
}

void TOKEN::Reset()
{
	if(pTokenData) delete pTokenData;
	pTokenData = 0;
	eTokenType = UNKNOWN;
	TokenDataBufferSize = 0;
	ZeroMemory(ProgramSteps,sizeof(ProgramSteps));
	ProgramStepsNum = 0;	
	Program = 0;
	ProgramBase = 0;
}

bool TOKEN::Is(S_TOKEN_TYPE ttype)
{
	if(eTokenType == ttype) return true;
	return false;
}

void TOKEN::LowCase()
{
	if(pTokenData[0] == 0) return;
	strlwr(pTokenData);
}

char * TOKEN::GetData()
{
	//if(pTokenData[0] == 0) return 0;
	return pTokenData;
}

S_TOKEN_TYPE TOKEN::GetType()
{
	return eTokenType;
}

char * TOKEN::GetTypeName()
{
	return TokenTypeName[eTokenType];
}

char * TOKEN::GetTypeName(S_TOKEN_TYPE code)
{
	return TokenTypeName[code];
}

S_TOKEN_TYPE TOKEN::Get(bool bKeepData)
{
	char sym;
	char * pBase;
	DWORD counter;

	eTokenType = UNKNOWN;
	
	StartArgument(Program);
	CacheToken(Program);

	Lines_in_token = 0;
	sym = *Program;
	switch(sym)
	{
		// end of program
		case 0:
			DISCARD_DATABUFFER
			eTokenType = END_OF_PROGRAMM; 
		return eTokenType;
		case ';':
			DISCARD_DATABUFFER
			eTokenType = SEPARATOR; 
			Program++;
		return eTokenType;
		case 0xd:
			DISCARD_DATABUFFER
			eTokenType = DEBUG_LINEFEED; 
			Program++;
			if(Program[0] == 0xa) Program++;
		return eTokenType;
		case 0xa:
			DISCARD_DATABUFFER
			eTokenType = DEBUG_LINEFEED; 
			Program++;
			if(Program[0] == 0xd) Program++;
		return eTokenType;
		
		// commented text
		case '/':
			sym = Program[1]; 
			if(sym != '*') break;
			Program++;
			Program++;
			
			pBase = Program;
			
			do
			{
				sym = *Program;
				if(sym == '*')
				{
					sym = Program[1];
					if(sym == '/')
					{
						SetNTokenData(pBase,(DWORD)Program - (DWORD)pBase);
						eTokenType = COMMENT;
						Program++;
						Program++;
						return eTokenType;
					}
				} 
				else
				{
					switch(sym)
					{
					case 0xd:
						if(Program[1] == 0xa) Program++;
						Lines_in_token++;
					break;
					case 0xa:
						if(Program[1] == 0xd) Program++;
						Lines_in_token++;
					break;
					}
				}
				Program++;
				
			} while(sym != 0);
			counter = (DWORD)Program - (DWORD)pBase;
			if(counter > INVALID_ARG_DCHARS) counter = INVALID_ARG_DCHARS;
			SetNTokenData(pBase,counter);
			eTokenType = INVALID_TOKEN;
		return eTokenType;
		case '"':
			Program++;
			pBase = Program;
			do
			{
				sym = *Program;
				if(sym == '"')
				{
					
					SetNTokenData(pBase,(DWORD)Program - (DWORD)pBase);
					eTokenType = STRING;
					Program++;
					return eTokenType;
				}
				Program++;
			} while(sym != 0);
			counter = (DWORD)Program - (DWORD)pBase;
			if(counter > INVALID_ARG_DCHARS) counter = INVALID_ARG_DCHARS;
			SetNTokenData(pBase,counter);
			eTokenType = INVALID_TOKEN;
		return eTokenType;
	}
	S_TOKEN_TYPE stt;
	stt = ProcessToken(Program,bKeepData); 
	if(stt == HOLD_COMPILATION)
	{
		 _asm int 3;
		//stt == HOLD_COMPILATION;
	}
	return stt; 
}

S_TOKEN_TYPE TOKEN::FormatGet()
{
	char sym;
	char * pBase;
	DWORD counter;

	eTokenType = UNKNOWN;
	
	StartArgument(Program,true);
	CacheToken(Program);

	Lines_in_token = 0;
	sym = *Program;
	switch(sym)
	{
		// end of program
		case 0:
			DISCARD_DATABUFFER
			eTokenType = END_OF_PROGRAMM; 
		return eTokenType;
		case ';':
			//DISCARD_DATABUFFER
			SetNTokenData(";",1);
			
			eTokenType = SEPARATOR; 
			Program++;
		return eTokenType;
		case 0xd:
			//DISCARD_DATABUFFER
			
			eTokenType = DEBUG_LINEFEED; 
			Program++;
			if(Program[0] == 0xa) 
			{
				SetNTokenData((char *)(Program - 1),2);
				Program++;
			} else SetNTokenData(&sym,1);
		return eTokenType;
		case 0xa:
			//DISCARD_DATABUFFER
			
			eTokenType = DEBUG_LINEFEED; 
			Program++;
			if(Program[0] == 0xd) 
			{
				SetNTokenData((char *)(Program - 1),2);
				Program++;
			} else SetNTokenData(&sym,1);
		return eTokenType;
		
		// commented text
		case '/':
			pBase = Program;
			sym = Program[1]; 
			if(sym != '*') break;
			Program++;
			Program++;
			
			//pBase = Program;
			
			do
			{
				sym = *Program;
				if(sym == '*')
				{
					sym = Program[1];
					if(sym == '/')
					{
						//SetNTokenData(pBase,(DWORD)Program - (DWORD)pBase);
						eTokenType = COMMENT;
						Program++;
						Program++;
						SetNTokenData(pBase,(DWORD)Program - (DWORD)pBase);
						return eTokenType;
					}
				} 
				else
				{
					switch(sym)
					{
					case 0xd:
						if(Program[1] == 0xa) Program++;
						Lines_in_token++;
					break;
					case 0xa:
						if(Program[1] == 0xd) Program++;
						Lines_in_token++;
					break;
					}
				}
				Program++;
				
			} while(sym != 0);
			counter = (DWORD)Program - (DWORD)pBase;
			if(counter > INVALID_ARG_DCHARS) counter = INVALID_ARG_DCHARS;
			SetNTokenData(pBase,counter);
			eTokenType = INVALID_TOKEN;
		return eTokenType;
		case '"':
			//Program++;
			pBase = Program;
			Program++;
			do
			{
				sym = *Program;
				if(sym == '"')
				{
					Program++;
					SetNTokenData(pBase,(DWORD)Program - (DWORD)pBase);
					eTokenType = STRING;
					//Program++;
					return eTokenType;
				}
				Program++;
			} while(sym != 0);
			counter = (DWORD)Program - (DWORD)pBase;
			if(counter > INVALID_ARG_DCHARS) counter = INVALID_ARG_DCHARS;
			SetNTokenData(pBase,counter);
			eTokenType = INVALID_TOKEN;
		return eTokenType;
	}

	return ProcessToken(Program,true); 
}

// copy argument data to buffer and close the termination 0
long TOKEN::SetTokenData(char * pointer, bool bKeepControlSymbols)
{
	long Data_size;
	//if(!IsOperator(pointer,Data_size)) 
		Data_size = StopArgument(pointer,bKeepControlSymbols);
	if(Data_size == 0) 
	{ 
		if(pTokenData) pTokenData[0] = 0; 
		return 0; 
	}
	if(Data_size >= TokenDataBufferSize)
	{
		if(pTokenData) delete pTokenData;

		pTokenData = NEW char[Data_size + 1];
		TokenDataBufferSize = Data_size + 1;
	}
	memcpy(pTokenData,pointer,Data_size);
	pTokenData[Data_size] = 0;
	return Data_size;
}

// copy exact nymber of argument data to buffer and close the termination 0
long TOKEN::SetNTokenData(char * pointer, long Data_size)
{
	if(Data_size == 0) { pTokenData[0] = 0; return 0; }
	if(Data_size >= TokenDataBufferSize)
	{
		if(pTokenData) delete pTokenData;

		pTokenData = NEW char[Data_size + 1];
		TokenDataBufferSize = Data_size + 1;
	}
	memcpy(pTokenData,pointer,Data_size);
	pTokenData[Data_size] = 0;
	return Data_size;
}


// search throw the program code until find non significant argument character:
// SPACE,TAB,0,'\0xd','\0xa'
// return number of significant symbols
long TOKEN::StopArgument(char * pointer, bool bKeepControlSymbols)
{
	long size;
	char sym;
	bool bOnlyDigit;
	bool bDot;
	size = 0;
	bDot = false;
	bOnlyDigit = true;
	do
	{
		sym = *pointer;
		
		if(sym == '.') bDot = true;

		if(sym == 'f')
		{
			if(size > 1)
			if(bDot && bOnlyDigit)
			{
				size++;
				return size;
			}
		}

		if(sym < 0x30 || sym > 0x39) 
		{
			if(sym != '.') bOnlyDigit = false;
		}
		if(bKeepControlSymbols)
		{
			if(sym == 0x9 || sym == 0x20)
			{
				if(size == 0) return 1;
				return size;
			}
		}
		if(sym <= 0x20) return size;
		if(sym == ';') return size;
		/*if(sym == 'f')
		{
			if(bDot && bOnlyDigit)
			{
				size++;
				return size;
			}
		}*/
		
		if(sym == '{' || sym == '}' || sym == ':' || sym == '(' || sym == ')' || sym == '[' || sym == ']' || /*sym == '*' || */sym == '^' || sym == '%' ||  sym == ',') 
		{
			if(size == 0) return 1;
			return size;
		}

		if(sym == '*') 
		{
			pointer++;
			sym = *pointer;
			switch(sym)
			{
				case '=':
					if(size == 0) return 2;
				return size;
				default:
					if(size == 0) return 1;
				return size;
			}
		}

		if(sym == '.')
		{
			if(size == 0) return 1;
			if(!bOnlyDigit) return size;
			/*pointer--;
			sym = *pointer;
			if(sym < 0x30 || sym > 0x39) return size;
			pointer++;*/
		}

		if(sym == '>' || sym == '<') 
		{
			pointer++;
			sym = *pointer;
			if(sym != '=')
			{
				if(size == 0) return 1;
				return size;
			} else
			{
				if(size == 0) return 2;
				return size;
			}
		}

		if(sym == '=') 
		{
			pointer++;
			sym = *pointer;
			if(sym != '=')
			{
				if(size == 0) return 1;
				return size;
			} else
			{
				if(size == 0) return 2;
				return size;
			}
		}
		if(sym == '!') 
		{
			pointer++;
			sym = *pointer;
			if(sym != '=')
			{
				if(size == 0) return 1;
				return size;
			} else
			{
				if(size == 0) return 2;
				return size;
			}
		}
		if(sym == '&') 
		{
			pointer++;
			sym = *pointer;
			if(sym != '&')
			{
				if(size == 0) return 1;
				return size;
			} else
			{
				if(size == 0) return 2;
				return size;
			}
		}
		if(sym == '+') 
		{
			pointer++;
			sym = *pointer;
			switch(sym)
			{
				case '=':
				case '+':
					if(size == 0) return 2;
				return size;
				default:
					if(size == 0) return 1;
				return size;
			}
		}
		if(sym == '-') 
		{
			pointer++;
			sym = *pointer;
			switch(sym)
			{
				case '=':
				case '-':
					if(size == 0) return 2;
				return size;
				default:
					if(size == 0) return 1;
				return size;
			}
		}
		if(sym == '/') 
		{
			pointer++;
			sym = *pointer;
			/*if(sym != '/')
			{
				if(size == 0) return 1;
				return size;
			} else
			{
				if(size == 0) return 2;
				return size;
			}*/

			switch(sym)
			{
				case '/':
				case '=':
					if(size == 0) return 2;
				return size;
				default:
					if(size == 0) return 1;
				return size;
			}
		}
		
		//if(IsOperator(pointer)) return size;
		pointer++;  size++;
	}
	while(true);
}

// advance program pointer until not found significant argument symbol
void TOKEN::StartArgument(char* & pointer, bool bKeepControlSymbols)
{
	char sym;
	do
	{
		sym = *pointer;
		if(sym == 0) return;
		if(sym == 0xa || sym == 0xd) return;
		if(bKeepControlSymbols)
		{
			if(sym == 0x9 || sym == 0x20) return;
		}
		if(sym <= 0x20) pointer++;	else return;
	}
	while(true);
}

bool TOKEN::IsNumber(char * pointer)
{
	DWORD n;
	if(pointer == 0) return false;
	for(n=0;pointer[n];n++)
	{
		if(pointer[n] < 0x20 && n > 0) return true;				// end on white space
		if(pointer[n] < 0x30 || pointer[n] > 0x39) return false;// not digit symbol
	}
	return true;
}

// this function can interpreted integer as float, so always check using IsNumber function
bool TOKEN::IsFloatNumber(char * pointer)
{
	DWORD n;
	if(pointer == 0) return false;
	for(n=0;pointer[n];n++)
	{
		if(pointer[n] == '.') 
		{
			if(n > 0) continue;
		}
		if(pointer[n] < 0x20 && n > 0) return true;				// end on white space
		if(pointer[n] < 0x30 || pointer[n] > 0x39) return false;// not digit symbol
	}
	return true;
}
/*
bool TOKEN::IsOperator(char * pointer)
{
	DWORD operators_num,n,i;
	bool found;
	operators_num = sizeof(Operators)/sizeof(char *);
	
	for(n=0;n<operators_num;n++)
	{
		i = 0;
		found = true;
		while(Operators[n][i] && pointer[i])
		{
			if(Operators[n][i] != pointer[i]) { found = false; break; }
			i++;
		}
		if(found) 
		{
			if(Operators[n][0] == '.')
			{
				if(pointer[i] >= 0x30 && pointer[i] <= 0x39)
				{
					return false;	// this is float number
				}
			}
			return true;
		}
	}
	return false;
}
*/
/*
bool TOKEN::IsOperator(char * pointer, long & syms)
{
	DWORD operators_num,n,i;
	bool found;
	operators_num = sizeof(Operators)/sizeof(char *);
	syms = 0;
	for(n=0;n<operators_num;n++)
	{
		i = 0;
		found = true;
		while(Operators[n][i] && pointer[i])
		{
			if(Operators[n][i] != pointer[i]) { found = false; break; }
			i++;
		}
		if(found) 
		{
			syms = i; 
			if(Operators[n][0] == '.')
			{
				if(pointer[i] >= 0x30 && pointer[i] <= 0x39)
				{
					return false;	// this is float number
				}
			}
			syms = i; 
			return true; 
		}
	}
	return false;
}
*/
void TOKEN::CacheToken(char * pointer)
{
	if(ProgramStepsNum < PROGRAM_STEPS_CACHE)
	{
		ProgramSteps[ProgramStepsNum] = (DWORD)pointer - (DWORD)ProgramBase;
		ProgramStepsNum++;
		return;
	}
	DWORD n;
	for(n=0;n<(PROGRAM_STEPS_CACHE - 1);n++)
	{
		ProgramSteps[n] = ProgramSteps[n + 1];
	}
	ProgramSteps[PROGRAM_STEPS_CACHE - 1] = (DWORD)pointer - (DWORD)ProgramBase;
}

// set pointer to previous (processed) token, return false if no pointers in cache
bool TOKEN::StepBack()
{
	if(ProgramStepsNum == 0) return false;
	ProgramStepsNum--;
	Program = ProgramBase + ProgramSteps[ProgramStepsNum];
	return true;
}

S_TOKEN_TYPE TOKEN::ProcessToken(char* & pointer, bool bKeepData)
{
	char sym;
	//long keywords_num;
	//long n;
	char * pBase;

	pointer += SetTokenData(pointer,bKeepData);

	eTokenType = UNKNOWN;
	if(GetData() == 0) 
	{
		pointer++;
		DISCARD_DATABUFFER
		return eTokenType;
	}

	/*keywords_num = sizeof(Keywords)/sizeof(S_KEYWORD);
	for(n=0;n<keywords_num;n++)
	{
		if(stricmp(pTokenData,Keywords[n].name) == 0)
		{
			eTokenType = Keywords[n].type;
			break;
		}
	}*/

	eTokenType = Keyword2TokenType(pTokenData);

	//if(IsOperator(GetData())) eTokenType = OPERATOR;
	//else
	if(IsNumber(GetData())) eTokenType = NUMBER;
	else
	if(IsFloatNumber(GetData())) eTokenType = FLOAT_NUMBER;


	switch(eTokenType)
	{
		case INCLUDE_LIBRIARY:
			Get();	//	file name (string)
			eTokenType = INCLUDE_LIBRIARY;
		break;
		case INCLIDE_FILE:
			Get();	//	file name (string)
			eTokenType = INCLIDE_FILE;
		break;
		case DEBUG_FILE_NAME:
			Get();	//	file name (string)
			eTokenType = DEBUG_FILE_NAME;
		break;
		case BLOCK_IN: 
			if(bKeepData) break;
			DISCARD_DATABUFFER 
			//pointer++;
		break;
		case BLOCK_OUT: 
			if(bKeepData) break;
			DISCARD_DATABUFFER 
			//pointer++;
		break;
		case LINE_COMMENT:
			if(bKeepData) pBase = Program - 2;
			else
			pBase = Program;
			eTokenType = COMMENT;
			do
			{
				sym = *Program;
				Program++;
				if(sym == 0xd || sym == 0xa) break;

			} while (sym != 0);
			SetNTokenData(pBase,(DWORD)Program - (DWORD)pBase);
		break;
		case CALL:
		case IMPORT:
		case EXTERN:
		case COMMA:
		case DOT:
		case AND:
		case OP_MODUL:
		case OP_MINUS:
		case OP_PLUS:
		case OP_MULTIPLY:
		case OP_DIVIDE:
		case OP_POWER:
		case OP_NOT_EQUAL:
		case OP_GREATER:
		case OP_GREATER_OR_EQUAL:
		case OP_LESSER:
		case OP_LESSER_OR_EQUAL:
		case OP_EQUAL:
		case OP_BOOL_EQUAL:
		case OP_BOOL_AND:
		case OP_BOOL_OR:
		case OP_INC:
		case OP_DEC:
		case OP_INCADD:
		case OP_DECADD:
		case OP_MULTIPLYEQ:
		case OP_DIVIDEEQ:
		case SQUARE_OPEN_BRACKET:
		case SQUARE_CLOSE_BRACKET:
		case OPEN_BRACKET:
		case CLOSE_BRACKET:
		case MAKEREF_COMMAND:
		case MAKEAREF_COMMAND:
		case LABEL:
		case SWITCH_COMMAND:
		case CASE_COMMAND:
		case WHILE_BLOCK:
		case IF_BLOCK:
		case ELSE_BLOCK:
		case FOR_BLOCK:
		case STRING:
		case NUMBER:
		case FLOAT_NUMBER:
		case UNKNOWN:
		case CLASS_DECL:
		case CONTINUE_COMMAND:
		case BREAK_COMMAND:
		case FUNCTION_RETURN:
		case FUNCTION_RETURN_VOID:
		case TRUE_CONST:
		case FALSE_CONST:
		case OP_BOOL_NEG:
		case GOTO_COMMAND:
		case EVENT_HANDLER:
		case STACK_ALLOC:
		case STACK_PUSH:
		case STACK_POP:
		case STACK_READ:
		case STACK_WRITE:
		case STACK_WRITE_BXINDEX:
		case STACK_COMPARE:
		case STACK_POP_VOID:
		case LEFT_OPERAND:
	
		break;
		default:
			if(bKeepData) break;

			StartArgument(pointer);
			pointer += SetTokenData(pointer);
		break;
	}
	return eTokenType;
}

long TOKEN::TokenLines()
{
	return Lines_in_token;
}

S_TOKEN_TYPE TOKEN::Keyword2TokenType(char * pString)
{
/*	DWORD n;
	for(n=0;n<dwKeywordsNum;n++)
	{
		if(stricmp(pString,Keywords[n].name) == 0)
		{
			return Keywords[n].type;
		}
	}
	return UNKNOWN;//*/

	DWORD n;
	DWORD index;
	DWORD hash = MakeHashValue(pString,4)%TOKENHASHTABLE_SIZE;
	for(n=0;n<KeywordsHash[hash].dwNum;n++)
	{
		index = KeywordsHash[hash].pIndex[n];
		if(stricmp(pString,Keywords[index].name) == 0)
		{
			return Keywords[index].type;
		}

	}
	return UNKNOWN;//*/
}

DWORD TOKEN::MakeHashValue(const char * string, DWORD max_syms)
{
	//if ('A' <= string[0] && string[0] <= 'Z') return (DWORD)(string[0] + 'a' - 'A');
	//else return string[0];
	//return (DWORD)string[0];
	DWORD hval = 0;
	DWORD g;
	char v;
	while(*string != 0)
	{
		v = *string++;
		if ('A' <= v && v <= 'Z') v += 'a' - 'A';	// case independent
		hval = (hval<<4) + (unsigned long int)v;
		g = hval & ((unsigned long int) 0xf << (32 - 4));
		if(g != 0)
		{
			hval ^= g >> (32 - 8);
			hval ^= g;
		}
		if(max_syms != 0)
		{
			max_syms--;
			if(max_syms == 0) return hval;
		}
	}
	return hval;
}

bool TOKEN::InitializeHashTable()
{
	dwKeywordsNum = sizeof(Keywords)/sizeof(S_KEYWORD);
	if(dwKeywordsNum > 255)
	{
		trace("keywords num more then 256 - change index type");
		return false;
	}

	DWORD n;
	DWORD hash;
	for(n=0;n<dwKeywordsNum;n++)
	{
		hash = MakeHashValue(Keywords[n].name,4)%TOKENHASHTABLE_SIZE;
		
		KeywordsHash[hash].dwNum++;
		KeywordsHash[hash].pIndex = (BYTE*)RESIZE(KeywordsHash[hash].pIndex,KeywordsHash[hash].dwNum);
		KeywordsHash[hash].pIndex[KeywordsHash[hash].dwNum-1] = (BYTE)n;
	}

	// debug
	/*DWORD i;
	for(n=0;n<TOKENHASHTABLE_SIZE;n++)
	{
		trace("Hash: %d",n);
		for(i=0;i<KeywordsHash[n].dwNum;i++)
		{
			trace("      String: %s",Keywords[KeywordsHash[n].pIndex[i]].name);
		}
	}//*/

	return true;
}