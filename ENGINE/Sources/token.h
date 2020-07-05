#ifndef _S_TOKEN_H_
#define _S_TOKEN_H_

//#include <windows.h>
#ifndef _XBOX
#include <windows.h>
#else
#include <xtl.h>
#endif

#include "..\..\common_h\d_types.h"
enum S_TOKEN_TYPE 
{
	END_OF_PROGRAMM,
	INVALID_TOKEN,
	UNKNOWN,
	WHITESPACE,
	COMMENT,
	INCLIDE_FILE,
	VAR_INTEGER,
	VAR_FLOAT,
	VAR_STRING,
	VAR_OBJECT,
	VAR_REFERENCE,
	VAR_AREFERENCE,
	BLOCK_IN,
	BLOCK_OUT,
	NUMBER,
	STRING,
	FLOAT_NUMBER,
	SEPARATOR,
	FUNCTION_RETURN,
	FUNCTION_RETURN_VOID,
	FOR_BLOCK,
	IF_BLOCK,
	WHILE_BLOCK,
	CONTINUE_COMMAND,
	BREAK_COMMAND,
	GOTO_COMMAND,
	LABEL,
	TVOID,
	SWITCH_COMMAND,
	CASE_COMMAND,
	DEFINE_COMMAND,
	MAKEREF_COMMAND,
	MAKEAREF_COMMAND,
	OPEN_BRACKET,
	CLOSE_BRACKET,
	SQUARE_OPEN_BRACKET,
	SQUARE_CLOSE_BRACKET,
	OP_EQUAL,
	OP_BOOL_EQUAL,
	OP_GREATER,
	OP_GREATER_OR_EQUAL,
	OP_LESSER,
	OP_LESSER_OR_EQUAL,
	OP_NOT_EQUAL,
	OP_MINUS,
	OP_PLUS,
	OP_MULTIPLY,
	OP_DIVIDE,
	OP_POWER,
	OP_MODUL,
	OP_INC,
	OP_DEC,
	OP_NEG,
	LINE_COMMENT,
	COMMA,
	DOT,
	AND,
	CALL_FUNCTION,
	DEBUG_FILE_NAME,
	DEBUG_LINE_CODE,
	DEBUG_LINEFEED,
	VARIABLE,
	LOCAL_VARIABLE,
	EXTERN,
	ACCESS_WORD,
	ACCESS_VAR,
	STACK_ALLOC,
	STACK_PUSH,
	STACK_POP,
	STACK_READ,
	STACK_WRITE,
	STACK_WRITE_BXINDEX,
	STACK_COMPARE,
	AX,
	BX,
	EX,
	AP,
	STACK_TOP,
	AP_VALUE,
	MOVE,
	MOVEAP,
	MOVEAP_BXINDEX,
	ADVANCE_AP,
	ARRAY_INDEX,
	EXPRESSION_END,
	JUMP,
	JUMP_Z,
	JUMP_NZ,
	PROCESS_EXPRESSION,
	PUSH_EXPRESULT,
	POP_EXPRESULT,
	POP_VOID,
	OP_BOOL_AND,
	OP_BOOL_OR,
	ELSE_BLOCK,
	TRUE_CONST,
	FALSE_CONST,
	OP_BOOL_NEG,
	ACCESS_WORD_CODE,
	EVENT_HANDLER,
	OP_INCADD,
	OP_DECADD,
	OP_MULTIPLYEQ,
	OP_DIVIDEEQ,
	CLASS_DECL,
	CLASS_OBJECT,
	IMPORT,
	INCLUDE_LIBRIARY,
	CALL,
	SETREF,
	SETREF_BXINDEX,
	OP_SPLUS,
	OP_SMINUS,
	OP_BOOL_CONVERT,
	OP_REF_CONVERT,
	OP_COMPARE_AND_SET,
	LEFT_OPERAND,
	STACK_POP_VOID,
	DEFINE_VAL,
	ARGS_NUM,
	HOLD_COMPILATION,
	SETAREF,
	VERIFY_AP,
	POP_NZ,
	LEFTOP_INDEX,
	PUSH_OBJID,
	PUSH_OBJID_BXINDEX,
};

struct INTFUNCDESC
{
	DWORD dwArgsNum;
	char * pName;
	S_TOKEN_TYPE ReturnType;
};


#define PROGRAM_STEPS_CACHE	16

#define TOKENHASHTABLE_SIZE		256

struct THLINE
{
	THLINE(){dwNum = 0; pIndex = 0;};
	DWORD  dwNum;
	BYTE * pIndex;
};

class TOKEN
{
	THLINE KeywordsHash[TOKENHASHTABLE_SIZE];
	S_TOKEN_TYPE eTokenType;
	long   TokenDataBufferSize;
	long   Lines_in_token;
	char * pTokenData;
	DWORD  ProgramSteps[PROGRAM_STEPS_CACHE];
	long   ProgramStepsNum;
	char * Program;
	char * ProgramBase;
	DWORD  dwKeywordsNum;

public:
	 TOKEN();
	~TOKEN();
	void Release();
	void Reset();
	void SetProgram(char * pProgramBase, char * pProgramControl);
	void SetProgramControl(char * pProgramControl);
	char * GetProgramControl();
	char * GetProgramBase(){return ProgramBase;};
	DWORD  GetProgramOffset();

	S_TOKEN_TYPE Get(bool bKeepData = false);
	S_TOKEN_TYPE ProcessToken(char* & pointer, bool bKeepData = false);
	S_TOKEN_TYPE GetType();
	void CacheToken(char * pointer);
	bool StepBack();
	long SetTokenData(char * pointer, bool bKeepControlSymbols = false);
	long SetNTokenData(char * pointer, long Data_size);
	long StopArgument(char * pointer, bool bKeepControlSymbols = false);
	void StartArgument(char* & pointer, bool bKeepControlSymbols = false);
	char * GetTypeName(S_TOKEN_TYPE code);
	char * GetTypeName();
	char * GetData();
	bool Is(S_TOKEN_TYPE ttype);
	bool IsNumber(char * pointer);
	bool IsFloatNumber(char * pointer);
	long TokenLines();
	void LowCase();

	S_TOKEN_TYPE FormatGet();

	S_TOKEN_TYPE Keyword2TokenType(char * pString);
	DWORD MakeHashValue(const char * string, DWORD max_syms = 0);
	bool InitializeHashTable();

};

#endif