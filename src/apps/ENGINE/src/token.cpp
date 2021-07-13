#include "token.h"
#include <cstdio>

#include "defines.h"

#define DISCARD_DATABUFFER                                                                                             \
    {                                                                                                                  \
        if (pTokenData)                                                                                                \
            pTokenData[0] = 0;                                                                                         \
    }
#define INVALID_ARG_DCHARS 32
const char *TokenTypeName[] = {
    "END_OF_PROGRAMM",
    "INVALID_TOKEN",
    "UNKNOWN",
    "WHITESPACE",
    "COMMENT",
    "INCLIDE_FILE",
    "integer",
    "pointer",
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
    const char *name;
};

S_KEYWORD Keywords[] = {
    HOLD_COMPILATION,
    "#hold",
    INCLIDE_FILE,
    "#include",
    EVENT_HANDLER,
    "#event_handler",
    INCLUDE_LIBRIARY,
    "#libriary",
    VAR_INTEGER,
    "int",
    VAR_PTR,
    "ptr",
    VAR_FLOAT,
    "float",
    VAR_STRING,
    "string",
    VAR_OBJECT,
    "object",
    VAR_REFERENCE,
    "ref",
    VAR_AREFERENCE,
    "aref",
    BLOCK_IN,
    "{",
    BLOCK_OUT,
    "}",
    SEPARATOR,
    ";",
    FUNCTION_RETURN,
    "return",
    //    FUNCTION_RETURN_VOID,"return",
    FOR_BLOCK,
    "for",
    IF_BLOCK,
    "if",
    WHILE_BLOCK,
    "while",
    CONTINUE_COMMAND,
    "continue",
    BREAK_COMMAND,
    "break",
    GOTO_COMMAND,
    "goto",
    LABEL,
    ":",
    TVOID,
    "void",
    SWITCH_COMMAND,
    "switch",
    CASE_COMMAND,
    "case",
    DEFINE_COMMAND,
    "#define",
    MAKEREF_COMMAND,
    "makeref",
    MAKEAREF_COMMAND,
    "makearef",
    OPEN_BRACKET,
    "(",
    CLOSE_BRACKET,
    ")",
    SQUARE_OPEN_BRACKET,
    "[",
    SQUARE_CLOSE_BRACKET,
    "]",
    OP_EQUAL,
    "=",
    OP_BOOL_EQUAL,
    "==",
    OP_GREATER,
    ">",
    OP_GREATER_OR_EQUAL,
    ">=",
    OP_LESSER,
    "<",
    OP_LESSER_OR_EQUAL,
    "<=",
    OP_NOT_EQUAL,
    "!=",
    OP_INCADD,
    "+=",
    OP_DECADD,
    "-=",
    OP_MULTIPLYEQ,
    "*=",
    OP_DIVIDEEQ,
    "/=",
    OP_MINUS,
    "-",
    OP_PLUS,
    "+",
    OP_MULTIPLY,
    "*",
    OP_DIVIDE,
    "/",
    OP_POWER,
    "^",
    OP_MODUL,
    "%",
    OP_INC,
    "++",
    OP_DEC,
    "--",
    LINE_COMMENT,
    "//",
    LINE_COMMENT,
    "#ifndef",
    LINE_COMMENT,
    "#endif",
    COMMA,
    ",",
    DOT,
    ".",
    OP_BOOL_AND,
    "&&",
    OP_BOOL_OR,
    "||",
    AND,
    "&",
    DEBUG_FILE_NAME,
    "#file",
    EXTERN,
    "extern",
    ELSE_BLOCK,
    "else",
    VAR_INTEGER,
    "bool",
    TRUE_CONST,
    "true",
    FALSE_CONST,
    "false",
    OP_BOOL_NEG,
    "!",
    CLASS_DECL,
    "class",
    IMPORT,
    "native",
    CALL,
    "call",

};

TOKEN::TOKEN()
{
    pTokenData = nullptr;
    eTokenType = UNKNOWN;
    TokenDataBufferSize = 0;
    PZERO(ProgramSteps, sizeof(ProgramSteps));
    ProgramStepsNum = 0;
    Program = nullptr;
    ProgramBase = nullptr;
    Lines_in_token = 0;

    InitializeHashTable();
}

void TOKEN::Release()
{
    for (uint32_t n = 0; n < TOKENHASHTABLE_SIZE; n++)
    {
        if (KeywordsHash[n].pIndex)
            free(KeywordsHash[n].pIndex);
        KeywordsHash[n].pIndex = nullptr;
    }
}

TOKEN::~TOKEN()
{
    Reset();
}

void TOKEN::SetProgram(char *pProgramBase, char *pProgramControl)
{
    Program = pProgramControl;
    ProgramBase = pProgramBase;
    PZERO(ProgramSteps, sizeof(ProgramSteps));
    ProgramStepsNum = 0;
}

void TOKEN::SetProgramControl(char *pProgramControl)
{
    Program = pProgramControl;
}

char *TOKEN::GetProgramControl()
{
    return Program;
}

ptrdiff_t TOKEN::GetProgramOffset()
{
    return Program - ProgramBase;
}

void TOKEN::Reset()
{
    delete pTokenData;
    pTokenData = nullptr;
    eTokenType = UNKNOWN;
    TokenDataBufferSize = 0;
    PZERO(ProgramSteps, sizeof(ProgramSteps));
    ProgramStepsNum = 0;
    Program = nullptr;
    ProgramBase = nullptr;
}

bool TOKEN::Is(S_TOKEN_TYPE ttype)
{
    if (eTokenType == ttype)
        return true;
    return false;
}

void TOKEN::LowCase()
{
    if (pTokenData[0] == 0)
        return;
    _strlwr(pTokenData);
}

const char *TOKEN::GetData()
{
    // if(pTokenData[0] == 0) return 0;
    return pTokenData;
}

S_TOKEN_TYPE TOKEN::GetType()
{
    return eTokenType;
}

const char *TOKEN::GetTypeName()
{
    return TokenTypeName[eTokenType];
}

const char *TOKEN::GetTypeName(S_TOKEN_TYPE code)
{
    return TokenTypeName[code];
}

S_TOKEN_TYPE TOKEN::Get(bool bKeepData)
{
    char *pBase;
    ptrdiff_t counter;

    eTokenType = UNKNOWN;

    StartArgument(Program);
    CacheToken(Program);

    Lines_in_token = 0;
    auto sym = *Program;
    switch (sym)
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
        if (Program[0] == 0xa)
            Program++;
        return eTokenType;
    case 0xa:
        DISCARD_DATABUFFER
        eTokenType = DEBUG_LINEFEED;
        Program++;
        if (Program[0] == 0xd)
            Program++;
        return eTokenType;

        // commented text
    case '/':
        sym = Program[1];
        if (sym != '*')
            break;
        Program++;
        Program++;

        pBase = Program;

        do
        {
            sym = *Program;
            if (sym == '*')
            {
                sym = Program[1];
                if (sym == '/')
                {
                    SetNTokenData(pBase, Program - pBase);
                    eTokenType = COMMENT;
                    Program++;
                    Program++;
                    return eTokenType;
                }
            }
            else
            {
                switch (sym)
                {
                case 0xd:
                    if (Program[1] == 0xa)
                        Program++;
                    Lines_in_token++;
                    break;
                case 0xa:
                    if (Program[1] == 0xd)
                        Program++;
                    Lines_in_token++;
                    break;
                }
            }
            Program++;
        } while (sym != 0);
        counter = Program - pBase;
        if (counter > INVALID_ARG_DCHARS)
            counter = INVALID_ARG_DCHARS;
        SetNTokenData(pBase, counter);
        eTokenType = INVALID_TOKEN;
        return eTokenType;
    case '"':
        Program++;
        pBase = Program;
        do
        {
            sym = *Program;
            if (sym == '"')
            {
                SetNTokenData(pBase, Program - pBase);
                eTokenType = STRING;
                Program++;
                return eTokenType;
            }
            Program++;
        } while (sym != 0);
        counter = Program - pBase;
        if (counter > INVALID_ARG_DCHARS)
            counter = INVALID_ARG_DCHARS;
        SetNTokenData(pBase, counter);
        eTokenType = INVALID_TOKEN;
        return eTokenType;
    }
    const auto stt = ProcessToken(Program, bKeepData);
    if (stt == HOLD_COMPILATION)
    {
        __debugbreak();
        // stt == HOLD_COMPILATION;
    }
    return stt;
}

S_TOKEN_TYPE TOKEN::FormatGet()
{
    char sym;
    char *pBase;
    ptrdiff_t counter;

    eTokenType = UNKNOWN;

    StartArgument(Program, true);
    CacheToken(Program);

    Lines_in_token = 0;
    sym = *Program;
    switch (sym)
    {
        // end of program
    case 0:
        DISCARD_DATABUFFER
        eTokenType = END_OF_PROGRAMM;
        return eTokenType;
    case ';':
        // DISCARD_DATABUFFER
        SetNTokenData(";", 1);

        eTokenType = SEPARATOR;
        Program++;
        return eTokenType;
    case 0xd:
        // DISCARD_DATABUFFER

        eTokenType = DEBUG_LINEFEED;
        Program++;
        if (Program[0] == 0xa)
        {
            SetNTokenData(static_cast<char *>(Program - 1), 2);
            Program++;
        }
        else
            SetNTokenData(&sym, 1);
        return eTokenType;
    case 0xa:
        // DISCARD_DATABUFFER

        eTokenType = DEBUG_LINEFEED;
        Program++;
        if (Program[0] == 0xd)
        {
            SetNTokenData(static_cast<char *>(Program - 1), 2);
            Program++;
        }
        else
            SetNTokenData(&sym, 1);
        return eTokenType;

        // commented text
    case '/':
        pBase = Program;
        sym = Program[1];
        if (sym != '*')
            break;
        Program++;
        Program++;

        // pBase = Program;

        do
        {
            sym = *Program;
            if (sym == '*')
            {
                sym = Program[1];
                if (sym == '/')
                {
                    // SetNTokenData(pBase,(DWORD)Program - (DWORD)pBase);
                    eTokenType = COMMENT;
                    Program++;
                    Program++;
                    SetNTokenData(pBase, Program - pBase);
                    return eTokenType;
                }
            }
            else
            {
                switch (sym)
                {
                case 0xd:
                    if (Program[1] == 0xa)
                        Program++;
                    Lines_in_token++;
                    break;
                case 0xa:
                    if (Program[1] == 0xd)
                        Program++;
                    Lines_in_token++;
                    break;
                }
            }
            Program++;
        } while (sym != 0);
        counter = Program - pBase;
        if (counter > INVALID_ARG_DCHARS)
            counter = INVALID_ARG_DCHARS;
        SetNTokenData(pBase, counter);
        eTokenType = INVALID_TOKEN;
        return eTokenType;
    case '"':
        // Program++;
        pBase = Program;
        Program++;
        do
        {
            sym = *Program;
            if (sym == '"')
            {
                Program++;
                SetNTokenData(pBase, Program - pBase);
                eTokenType = STRING;
                // Program++;
                return eTokenType;
            }
            Program++;
        } while (sym != 0);
        counter = Program - pBase;
        if (counter > INVALID_ARG_DCHARS)
            counter = INVALID_ARG_DCHARS;
        SetNTokenData(pBase, counter);
        eTokenType = INVALID_TOKEN;
        return eTokenType;
    }

    return ProcessToken(Program, true);
}

// copy argument data to buffer and close the termination 0
long TOKEN::SetTokenData(const char *pointer, bool bKeepControlSymbols)
{
    // if(!IsOperator(pointer,Data_size))
    const auto Data_size = StopArgument(pointer, bKeepControlSymbols);
    if (Data_size == 0)
    {
        if (pTokenData)
            pTokenData[0] = 0;
        return 0;
    }
    if (Data_size >= TokenDataBufferSize)
    {
        delete pTokenData;

        pTokenData = new char[Data_size + 1];
        TokenDataBufferSize = Data_size + 1;
    }
    memcpy(pTokenData, pointer, Data_size);
    pTokenData[Data_size] = 0;
    return Data_size;
}

// copy exact nymber of argument data to buffer and close the termination 0
ptrdiff_t TOKEN::SetNTokenData(const char *pointer, ptrdiff_t Data_size)
{
    if (Data_size == 0)
    {
        pTokenData[0] = 0;
        return 0;
    }
    if (Data_size >= TokenDataBufferSize)
    {
        delete pTokenData;

        pTokenData = new char[Data_size + 1];
        TokenDataBufferSize = Data_size + 1;
    }
    memcpy(pTokenData, pointer, Data_size);
    pTokenData[Data_size] = 0;
    return Data_size;
}

// search throw the program code until find non significant argument character:
// SPACE,TAB,0,'\0xd','\0xa'
// return number of significant symbols
long TOKEN::StopArgument(const char *pointer, bool bKeepControlSymbols)
{
    long size = 0;
    auto bDot = false;
    auto bOnlyDigit = true;
    do
    {
        auto sym = *pointer;

        if (sym == '.')
            bDot = true;

        if (sym == 'f')
        {
            if (size > 1)
                if (bDot && bOnlyDigit)
                {
                    size++;
                    return size;
                }
        }

        if (sym < 0x30 || sym > 0x39)
        {
            if (sym != '.')
                bOnlyDigit = false;
        }
        if (bKeepControlSymbols)
        {
            if (sym == 0x9 || sym == 0x20)
            {
                if (size == 0)
                    return 1;
                return size;
            }
        }
        if (sym <= 0x20)
            return size;
        if (sym == ';')
            return size;
        /*if(sym == 'f')
        {
          if(bDot && bOnlyDigit)
          {
            size++;
            return size;
          }
        }*/

        if (sym == '{' || sym == '}' || sym == ':' || sym == '(' || sym == ')' || sym == '[' || sym == ']' ||
            /*sym == '*' || */ sym == '^' || sym == '%' || sym == ',')
        {
            if (size == 0)
                return 1;
            return size;
        }

        if (sym == '*')
        {
            pointer++;
            sym = *pointer;
            switch (sym)
            {
            case '=':
                if (size == 0)
                    return 2;
                return size;
            default:
                if (size == 0)
                    return 1;
                return size;
            }
        }

        if (sym == '.')
        {
            if (size == 0)
                return 1;
            if (!bOnlyDigit)
                return size;
            /*pointer--;
            sym = *pointer;
            if(sym < 0x30 || sym > 0x39) return size;
            pointer++;*/
        }

        if (sym == '>' || sym == '<')
        {
            pointer++;
            sym = *pointer;
            if (sym != '=')
            {
                if (size == 0)
                    return 1;
                return size;
            }
            if (size == 0)
                return 2;
            return size;
        }

        if (sym == '=')
        {
            pointer++;
            sym = *pointer;
            if (sym != '=')
            {
                if (size == 0)
                    return 1;
                return size;
            }
            if (size == 0)
                return 2;
            return size;
        }
        if (sym == '!')
        {
            pointer++;
            sym = *pointer;
            if (sym != '=')
            {
                if (size == 0)
                    return 1;
                return size;
            }
            if (size == 0)
                return 2;
            return size;
        }
        if (sym == '&')
        {
            pointer++;
            sym = *pointer;
            if (sym != '&')
            {
                if (size == 0)
                    return 1;
                return size;
            }
            if (size == 0)
                return 2;
            return size;
        }
        if (sym == '+')
        {
            pointer++;
            sym = *pointer;
            switch (sym)
            {
            case '=':
            case '+':
                if (size == 0)
                    return 2;
                return size;
            default:
                if (size == 0)
                    return 1;
                return size;
            }
        }
        if (sym == '-')
        {
            pointer++;
            sym = *pointer;
            switch (sym)
            {
            case '=':
            case '-':
                if (size == 0)
                    return 2;
                return size;
            default:
                if (size == 0)
                    return 1;
                return size;
            }
        }
        if (sym == '/')
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

            switch (sym)
            {
            case '/':
            case '=':
                if (size == 0)
                    return 2;
                return size;
            default:
                if (size == 0)
                    return 1;
                return size;
            }
        }

        // if(IsOperator(pointer)) return size;
        pointer++;
        size++;
    } while (true);
}

// advance program pointer until not found significant argument symbol
void TOKEN::StartArgument(char *&pointer, bool bKeepControlSymbols)
{
    do
    {
        const auto sym = *pointer;
        if (sym == 0)
            return;
        if (sym == 0xa || sym == 0xd)
            return;
        if (bKeepControlSymbols)
        {
            if (sym == 0x9 || sym == 0x20)
                return;
        }
        if (sym <= 0x20)
            pointer++;
        else
            return;
    } while (true);
}

bool TOKEN::IsNumber(const char *pointer)
{
    if (pointer == nullptr)
        return false;
    for (uint32_t n = 0; pointer[n]; n++)
    {
        if (pointer[n] < 0x20 && n > 0)
            return true; // end on white space
        if (pointer[n] < 0x30 || pointer[n] > 0x39)
            return false; // not digit symbol
    }
    return true;
}

// this function can interpreted integer as float, so always check using IsNumber function
bool TOKEN::IsFloatNumber(const char *pointer)
{
    if (pointer == nullptr)
        return false;
    for (uint32_t n = 0; pointer[n]; n++)
    {
        if (pointer[n] == '.')
        {
            if (n > 0)
                continue;
        }
        if (pointer[n] < 0x20 && n > 0)
            return true; // end on white space
        if (pointer[n] < 0x30 || pointer[n] > 0x39)
            return false; // not digit symbol
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
                    return false;    // this is float number
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
                    return false;    // this is float number
                }
            }
            syms = i;
            return true;
        }
    }
    return false;
}
*/
void TOKEN::CacheToken(const char *pointer)
{
    if (ProgramStepsNum < PROGRAM_STEPS_CACHE)
    {
        ProgramSteps[ProgramStepsNum] = pointer - ProgramBase;
        ProgramStepsNum++;
        return;
    }
    for (uint32_t n = 0; n < (PROGRAM_STEPS_CACHE - 1); n++)
    {
        ProgramSteps[n] = ProgramSteps[n + 1];
    }
    ProgramSteps[PROGRAM_STEPS_CACHE - 1] = pointer - ProgramBase;
}

// set pointer to previous (processed) token, return false if no pointers in cache
bool TOKEN::StepBack()
{
    if (ProgramStepsNum == 0)
        return false;
    ProgramStepsNum--;
    Program = ProgramBase + ProgramSteps[ProgramStepsNum];
    return true;
}

S_TOKEN_TYPE TOKEN::ProcessToken(char *&pointer, bool bKeepData)
{
    char sym;
    // long keywords_num;
    // long n;
    char *pBase;

    pointer += SetTokenData(pointer, bKeepData);

    eTokenType = UNKNOWN;
    if (GetData() == nullptr)
    {
        pointer++;
        DISCARD_DATABUFFER
        return eTokenType;
    }

    /*keywords_num = sizeof(Keywords)/sizeof(S_KEYWORD);
    for(n=0;n<keywords_num;n++)
    {
      if(_stricmp(pTokenData,Keywords[n].name) == 0)
      {
        eTokenType = Keywords[n].type;
        break;
      }
    }*/

    eTokenType = Keyword2TokenType(pTokenData);

    // if(IsOperator(GetData())) eTokenType = OPERATOR;
    // else
    if (IsNumber(GetData()))
        eTokenType = NUMBER;
    else if (IsFloatNumber(GetData()))
        eTokenType = FLOAT_NUMBER;

    switch (eTokenType)
    {
    case INCLUDE_LIBRIARY:
        Get(); //    file name (string)
        eTokenType = INCLUDE_LIBRIARY;
        break;
    case INCLIDE_FILE:
        Get(); //    file name (string)
        eTokenType = INCLIDE_FILE;
        break;
    case DEBUG_FILE_NAME:
        Get(); //    file name (string)
        eTokenType = DEBUG_FILE_NAME;
        break;
    case BLOCK_IN:
        if (bKeepData)
            break;
        DISCARD_DATABUFFER
        // pointer++;
        break;
    case BLOCK_OUT:
        if (bKeepData)
            break;
        DISCARD_DATABUFFER
        // pointer++;
        break;
    case LINE_COMMENT:
        if (bKeepData)
            pBase = Program - 2;
        else
            pBase = Program;
        eTokenType = COMMENT;
        do
        {
            sym = *Program;
            Program++;
            if (sym == 0xd || sym == 0xa)
                break;
        } while (sym != 0);
        SetNTokenData(pBase, Program - pBase);
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
        if (bKeepData)
            break;

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

S_TOKEN_TYPE TOKEN::Keyword2TokenType(const char *pString)
{
    /*    DWORD n;
      for(n=0;n<dwKeywordsNum;n++)
      {
        if(_stricmp(pString,Keywords[n].name) == 0)
        {
          return Keywords[n].type;
        }
      }
      return UNKNOWN;//*/

    const auto hash = MakeHashValue(pString, 4) % TOKENHASHTABLE_SIZE;
    for (uint32_t n = 0; n < KeywordsHash[hash].dwNum; n++)
    {
        const uint32_t index = KeywordsHash[hash].pIndex[n];
        if (_stricmp(pString, Keywords[index].name) == 0)
        {
            return Keywords[index].type;
        }
    }
    return UNKNOWN; //*/
}

uint32_t TOKEN::MakeHashValue(const char *string, uint32_t max_syms)
{
    // if ('A' <= string[0] && string[0] <= 'Z') return (DWORD)(string[0] + 'a' - 'A');
    // else return string[0];
    // return (DWORD)string[0];
    uint32_t hval = 0;
    while (*string != 0)
    {
        auto v = *string++;
        if ('A' <= v && v <= 'Z')
            v += 'a' - 'A'; // case independent
        hval = (hval << 4) + static_cast<unsigned long>(v);
        const uint32_t g = hval & (static_cast<unsigned long>(0xf) << (32 - 4));
        if (g != 0)
        {
            hval ^= g >> (32 - 8);
            hval ^= g;
        }
        if (max_syms != 0)
        {
            max_syms--;
            if (max_syms == 0)
                return hval;
        }
    }
    return hval;
}

bool TOKEN::InitializeHashTable()
{
    static_assert(sizeof(Keywords) / sizeof(S_KEYWORD) < 256, "keywords num more then 256 - change index type");
    dwKeywordsNum = sizeof(Keywords) / sizeof(S_KEYWORD);
    // if(dwKeywordsNum > 255)
    //{
    //    trace("keywords num more then 256 - change index type");
    //    return false;
    //}

    for (uint32_t n = 0; n < dwKeywordsNum; n++)
    {
        const auto hash = MakeHashValue(Keywords[n].name, 4) % TOKENHASHTABLE_SIZE;

        KeywordsHash[hash].dwNum++;
        KeywordsHash[hash].pIndex =
            static_cast<uint8_t *>(realloc(KeywordsHash[hash].pIndex, KeywordsHash[hash].dwNum));
        KeywordsHash[hash].pIndex[KeywordsHash[hash].dwNum - 1] = static_cast<uint8_t>(n);
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
