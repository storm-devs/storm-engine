#include "compiler.h"

char ERR_INVALID_EXPRESSION[] = "Invalid Expression";

bool COMPILER::BC_ProcessExpression(DATA *value)
{
    DATA *pV;
    DATA value2;

    value2.SetVCompiler(this);
    value->SetVCompiler(this);
    value->ClearType();

    auto check_equal = false;

    while (BC_TokenGet() == DEBUG_LINE_CODE)
    {
    }
    if (TokenType() == SEPARATOR)
    {
        SetError("Invalid expression");
        return false;
    }

    //--------------------------------------------------------------------------
    if (TokenIs(AND))
    {
        const VarInfo *real_var;

        auto Token_type = BC_TokenGet();
        if (TokenType() != VARIABLE)
        {
            if (TokenType() != LOCAL_VARIABLE)
            {
                SetError("invalid '&'");
                return false;
            }
        }

        const auto var_code = *((uint32_t *)&pRunCodeBase[TLR_DataOffset]);
        if (TokenType() == VARIABLE)
        {
            real_var = VarTab.GetVar(var_code);
            if (real_var == nullptr)
            {
                SetError("Global variable not found");
                return false;
            }
            pV = real_var->value.get();
        }
        else
        {
            // local variable
            if (pRun_fi)
            {
                pV = SStack.Read(pRun_fi->stack_offset, var_code);
                if (pV == nullptr)
                {
                    SetError("Local variable not found");
                    return false;
                }
            }
            else
                return false;
        }

        Token_type = BC_TokenGet();
        if (Token_type == SQUARE_OPEN_BRACKET)
        {
            DATA array_index;
            long index;
            array_index.SetVCompiler(this);

            if (!pV->IsArray())
            {
                SetError("invalid '[' operator");
                return false;
            }
            BC_TokenGet(); // beginning of subexpression
            BC_ProcessExpression_L2(&array_index);
            array_index.Get(index);
            if (!TokenIs(SQUARE_CLOSE_BRACKET))
                SetError("missed ']' in expression");

            value->SetType(VAR_REFERENCE);
            // pV = pV->GetVarPointer();
            pV = pV->GetArrayElement(index);
            value->SetReference(pV); //->GetVarPointer());
            // BC_TokenGet();
            while (BC_TokenGet() == DEBUG_LINE_CODE)
            {
            }
            return true;
        }
        value->SetType(VAR_REFERENCE);
        value->SetReference(pV); //->GetVarPointer());
        return true;
    }

    BC_ProcessExpression_L0(value);
    //--------------------------------------------------------------------------
    /*    BC_ProcessExpression_L2(value);
      S_TOKEN_TYPE op;
      op = TokenType();
      switch(op)
      {
        case SQUARE_CLOSE_BRACKET: break;
        case CLOSE_BRACKET: break;
        case OP_EQUAL: break;

        case OP_BOOL_AND:
        case OP_BOOL_OR:
        case OP_NOT_EQUAL:
        case OP_LESSER_OR_EQUAL:
        case OP_LESSER:
        case OP_GREATER_OR_EQUAL:
        case OP_GREATER:
        case OP_BOOL_EQUAL:
          //BC_TokenGet();
          while(BC_TokenGet() == DEBUG_LINE_CODE){};
          BC_ProcessExpression_L2(&value2);
          value->CompareAndSetResult(&value2,op);
        break;

        case COMMA: break;
        case DOT: break;
        case ACCESS_WORD: break;
        case ACCESS_VAR: break;
        case DEBUG_LINE_CODE: break;

        case SEPARATOR: break;//return value;
        default:
          SetError("Invalid expression break");
        return false;
      }//*/
    return true;
}

// logical
void COMPILER::BC_ProcessExpression_L0(DATA *value)
{
    long lRes;
    DATA value2;
    value2.SetVCompiler(this);

    BC_ProcessExpression_L1(value);
    while (TokenIs(OP_BOOL_AND) || TokenIs(OP_BOOL_OR))
    {
        const auto op = TokenType();
        switch (op)
        {
        case SQUARE_CLOSE_BRACKET:
            break;
        case CLOSE_BRACKET:
            break;
        case OP_EQUAL:
            break;
        case COMMA:
            break;
        case DOT:
            break;
        case ACCESS_WORD:
            break;
        case ACCESS_WORD_CODE:
            break;
        case ACCESS_VAR:
            break;
        case DEBUG_LINE_CODE:
            break;
        case SEPARATOR:
            break;

        case OP_BOOL_AND:
            value->BoolConvert();
            while (BC_TokenGet() == DEBUG_LINE_CODE)
            {
            }
            value->Get(lRes);
            if (lRes == 0)
                BC_ProcessExpression_L1(&value2, true); // SkipExpression_L1();
            else
            {
                BC_ProcessExpression_L1(&value2);
                value->RefConvert();
                value->CompareAndSetResult(&value2, op);
            }
            break;
        case OP_BOOL_OR:
            value->BoolConvert();
            while (BC_TokenGet() == DEBUG_LINE_CODE)
            {
            }
            value->Get(lRes);
            if (lRes)
                BC_ProcessExpression_L1(&value2, true); // SkipExpression_L1();
            else
            {
                BC_ProcessExpression_L1(&value2);
                value->RefConvert();
                value->CompareAndSetResult(&value2, op);
            }
            break;

        default:
            SetError("Invalid expression break");
            return;
        }
    }
}

// logical
void COMPILER::BC_ProcessExpression_L1(DATA *value, bool bSkip)
{
    DATA value2;
    value2.SetVCompiler(this);

    BC_ProcessExpression_L2(value, bSkip);
    while (TokenIs(OP_NOT_EQUAL) || TokenIs(OP_LESSER_OR_EQUAL) || TokenIs(OP_LESSER) || TokenIs(OP_GREATER_OR_EQUAL) ||
           TokenIs(OP_GREATER) || TokenIs(OP_BOOL_EQUAL))
    {
        const auto op = TokenType();
        switch (op)
        {
        case SQUARE_CLOSE_BRACKET:
            break;
        case CLOSE_BRACKET:
            break;
        case OP_EQUAL:
            break;
        case COMMA:
            break;
        case DOT:
            break;
        case ACCESS_WORD:
            break;
        case ACCESS_WORD_CODE:
            break;
        case ACCESS_VAR:
            break;
        case DEBUG_LINE_CODE:
            break;
        case SEPARATOR:
            break;
        case OP_NOT_EQUAL:
        case OP_LESSER_OR_EQUAL:
        case OP_LESSER:
        case OP_GREATER_OR_EQUAL:
        case OP_GREATER:
        case OP_BOOL_EQUAL:
            while (BC_TokenGet() == DEBUG_LINE_CODE)
            {
            }
            BC_ProcessExpression_L2(&value2, bSkip);
            if (!bSkip)
            {
                value->RefConvert();
                value->CompareAndSetResult(&value2, op);
            }
            break;
        default:
            SetError("Invalid expression break");
            return;
        }
    }
}

// '+' and '-' operations
void COMPILER::BC_ProcessExpression_L2(DATA *value, bool bSkip)
{
    DATA TempVal;
    TempVal.SetVCompiler(this);
    BC_ProcessExpression_L3(value, bSkip);
    while (TokenIs(OP_PLUS) || TokenIs(OP_MINUS))
    {
        const auto op = TokenType();
        while (BC_TokenGet() == DEBUG_LINE_CODE)
        {
        }
        TempVal.ClearType();
        BC_ProcessExpression_L3(&TempVal, bSkip);
        if (!bSkip)
        {
            switch (op)
            {
            case OP_PLUS:
                value->Plus(&TempVal);
                break;
            case OP_MINUS:
                value->Minus(&TempVal);
                break;
            }
        }
    }
}

// '*' and '/' operations
void COMPILER::BC_ProcessExpression_L3(DATA *value, bool bSkip)
{
    DATA TempVal;
    TempVal.SetVCompiler(this);

    BC_ProcessExpression_L4(value, bSkip);
    while (TokenIs(OP_MULTIPLY) || TokenIs(OP_DIVIDE) || TokenIs(OP_MODUL))
    {
        const S_TOKEN_TYPE op = TokenType();
        while (BC_TokenGet() == DEBUG_LINE_CODE)
        {
        }
        BC_ProcessExpression_L4(&TempVal, bSkip);
        if (!bSkip)
        {
            switch (op)
            {
            case OP_MULTIPLY:
                value->Multiply(&TempVal);
                break;
            case OP_DIVIDE:
                value->Divide(&TempVal);
                break;
            case OP_MODUL:
                value->Modul(&TempVal);
                break;
            }
        }
    }
}

// '^' operations
void COMPILER::BC_ProcessExpression_L4(DATA *value, bool bSkip)
{
    DATA TempVal;
    TempVal.SetVCompiler(this);
    long Deg;

    BC_ProcessExpression_L5(value, bSkip);
    if (TokenIs(OP_POWER))
    {
        while (BC_TokenGet() == DEBUG_LINE_CODE)
        {
        }
        BC_ProcessExpression_L5(&TempVal, bSkip);
        if (!bSkip)
        {
            if (TempVal.GetType() == VAR_INTEGER)
            {
                TempVal.Get(Deg);
                value->Power(Deg);
            }
        }
    }
}

// sign
void COMPILER::BC_ProcessExpression_L5(DATA *value, bool bSkip)
{
    const S_TOKEN_TYPE op = TokenType();
    if (TokenIs(OP_PLUS) || TokenIs(OP_MINUS) || TokenIs(OP_BOOL_NEG))
    {
        while (BC_TokenGet() == DEBUG_LINE_CODE)
        {
        }
    }
    BC_ProcessExpression_L6(value, bSkip);

    if (!bSkip)
    {
        if (op == OP_MINUS)
            value->Inverse();
        else if (op == OP_BOOL_NEG)
        {
            long iVal;
            const char *sVal;

            switch (value->GetType())
            {
            case VAR_FLOAT:
                value->Convert(VAR_INTEGER);
            case VAR_INTEGER:
                value->Get(iVal);
                if (iVal != 0)
                    iVal = 0;
                else
                    iVal = 1;
                value->Set(iVal);
                break;
            case VAR_STRING:
                value->Get(sVal);
                if (sVal == nullptr)
                    iVal = 1;
                else if (sVal[0] == 0)
                    iVal = 1;
                else
                    iVal = 0;
                value->Convert(VAR_INTEGER);
                value->Set(iVal);
                break;
            default:
                SetError("Invalid argument type for '!' opearator");
                break;
            }
        }
    }
}

// '(' and ')'
void COMPILER::BC_ProcessExpression_L6(DATA *value, bool bSkip)
{
    if (TokenIs(OPEN_BRACKET))
    {
        while (BC_TokenGet() == DEBUG_LINE_CODE)
        {
        }
        BC_ProcessExpression_L1(value, bSkip);
        if (!TokenIs(CLOSE_BRACKET))
            SetError("No matching ')' in expression");
        while (BC_TokenGet() == DEBUG_LINE_CODE)
        {
        }
    }
    else
        BC_ProcessExpression_L7(value, bSkip);
}

void COMPILER::BC_ProcessExpression_L7(DATA *value, bool bSkip)
{
    const VarInfo *real_var;
    uint32_t var_code;
    long index;
    DATA array_index;
    DATA TempData;
    TempData.SetVCompiler(this);
    array_index.SetVCompiler(this);
    DATA *pV;
    DATA *pVV;
    DATA Access;
    Access.SetVCompiler(this);
    DATA *pFuncResult;
    ATTRIBUTES *pRoot;
    const char *pString;
    uint32_t func_code, ip;
    S_TOKEN_TYPE vtype;

    switch (TokenType())
    {
    case NUMBER:
        if (bSkip)
        {
            while (BC_TokenGet() == DEBUG_LINE_CODE)
            {
            }
            return;
        }
        value->Set(*((long *)&pRunCodeBase[TLR_DataOffset]));
        while (BC_TokenGet() == DEBUG_LINE_CODE)
        {
        }
        return;
    case FLOAT_NUMBER:
        if (bSkip)
        {
            while (BC_TokenGet() == DEBUG_LINE_CODE)
            {
            }
            return;
        }
        value->Set(*((float *)&pRunCodeBase[TLR_DataOffset]));
        while (BC_TokenGet() == DEBUG_LINE_CODE)
        {
        }
        break;
    case STRING:
        if (bSkip)
        {
            while (BC_TokenGet() == DEBUG_LINE_CODE)
            {
            }
            return;
        }
        value->Set((char *)&pRunCodeBase[TLR_DataOffset + 4]); // 4 - string length
        while (BC_TokenGet() == DEBUG_LINE_CODE)
        {
        }
        break;
    case CALL:
        if (bSkip)
        {
            long local_in;
            BC_TokenGet(); // string var
            if (BC_TokenGet() != OPEN_BRACKET)
            {
                SetError("missing '('");
                return;
            }
            local_in = 1;
            do
            {
                switch (BC_TokenGet())
                {
                case OPEN_BRACKET:
                    local_in++;
                    break;
                case CLOSE_BRACKET:
                    local_in--;
                    break;
                }
            } while (local_in > 0);
            BC_TokenGet();
            return;
        }
        vtype = BC_TokenGet();                               // read variable
        var_code = *((long *)&pRunCodeBase[TLR_DataOffset]); // var code
        if (vtype == VARIABLE)
        {
            real_var = VarTab.GetVar(var_code);
            if (real_var == nullptr)
            {
                SetError("Global variable not found");
                return;
            }

            pVV = real_var->value.get();
        }
        else
        {
            pVV = SStack.Read(pRun_fi->stack_offset, var_code);
            if (pVV == nullptr)
            {
                SetError("Local variable not found");
                return;
            }
        }
        pVV = pVV->GetVarPointer();
        if (pVV->GetType() != VAR_STRING)
        {
            SetError("string call argument isnt string var");
            return;
        }
        pVV->Get(pString); // func name
        func_code = FuncTab.FindFunc(pString);
        if (func_code == INVALID_FUNC_CODE)
        {
            SetError("function '%s' not found", pString);
            return;
        }

        pFuncResult = nullptr;
        if (!BC_CallFunction(func_code, ip, pFuncResult))
            return;
        if (pFuncResult)
        {
            SStack.Pop();
            value->Copy(pFuncResult);
        }
        else
            SetError("void function result in expression");
        while (BC_TokenGet() == DEBUG_LINE_CODE)
        {
        }

        break;
    case CALL_FUNCTION:
        if (bSkip)
        {
            long local_in;
            if (BC_TokenGet() != OPEN_BRACKET)
            {
                SetError("missing '('");
                return;
            }
            local_in = 1;
            do
            {
                switch (BC_TokenGet())
                {
                case OPEN_BRACKET:
                    local_in++;
                    break;
                case CLOSE_BRACKET:
                    local_in--;
                    break;
                }
            } while (local_in > 0);
            BC_TokenGet();
            // while(BC_TokenGet() == DEBUG_LINE_CODE){};
            return;
        }
        // DWORD func_code,ip;
        memcpy(&func_code, &pRunCodeBase[TLR_DataOffset], sizeof(uint32_t));
        pFuncResult = nullptr;
        if (!BC_CallFunction(func_code, ip, pFuncResult))
            return;
        if (pFuncResult)
        {
            SStack.Pop();
            value->Copy(pFuncResult);
        }
        else
            SetError("void function result in expression");
        while (BC_TokenGet() == DEBUG_LINE_CODE)
        {
        }
        break;
    case LOCAL_VARIABLE:
    case VARIABLE:
        var_code = *((long *)&pRunCodeBase[TLR_DataOffset]);
        if (TokenType() == VARIABLE)
        {
            real_var = VarTab.GetVar(var_code);
            if (real_var == nullptr)
            {
                SetError("Global variable not found");
                break;
            }

            pV = real_var->value.get();
        }
        else
        {
            // local variable
            if (pRun_fi)
            {
                pV = SStack.Read(pRun_fi->stack_offset, var_code);
                if (pV == nullptr)
                {
                    SetError("Local variable not found");
                    break;
                }
            }
            else
                return;
        }

        // *****

        bool bref2a;
        bref2a = false;
        if (pV->IsReference())
        {
            pVV = pV->GetVarPointer();
            if (pVV == nullptr)
            {
                SetError("Bad Reference");
                break;
            }
            if (pVV->IsArray())
                bref2a = true;
        }

        if (BC_TokenGet() == SQUARE_OPEN_BRACKET)
        {
            if (!(pV->IsArray() || bref2a))
            {
                SetError("wrong '[' in expression");
                return;
            }
            // beginning of subexpression
            while (BC_TokenGet() == DEBUG_LINE_CODE)
            {
            }
            BC_ProcessExpression_L2(&array_index, bSkip);
            array_index.Get(index);
            if (!TokenIs(SQUARE_CLOSE_BRACKET))
                SetError("missed ']' in expression");
            if (!bSkip)
            {
                if (bref2a)
                    pV = pVV->GetArrayElement(index);
                else
                    pV = pV->GetArrayElement(index);
                if (pV == nullptr)
                {
                    SetError("invalid array index");
                    while (BC_TokenGet() == DEBUG_LINE_CODE)
                    {
                    }
                    break;
                }
            }
            BC_TokenGet();
        }

        //-----

        /*
        if(pV->IsArray() || bref2a)
        {
          // compute array index
          if(BC_TokenGet() != SQUARE_OPEN_BRACKET) { SetError("missed '[' in expression"); return; }
          // beginning of subexpression
          while(BC_TokenGet() == DEBUG_LINE_CODE){};
          BC_ProcessExpression_L2(&array_index,bSkip);
          array_index.Get(index);
          if(!TokenIs(SQUARE_CLOSE_BRACKET)) SetError("missed ']' in expression");
          if(!bSkip)
          {
            if(bref2a) pV = pVV->GetArrayElement(index);
            else pV = pV->GetArrayElement(index);
            if(pV == 0)
            {
              SetError("invalid array index");
              while(BC_TokenGet() == DEBUG_LINE_CODE){};
              break;
            }
          }
        }//*/

        while (TokenType() == DEBUG_LINE_CODE)
        {
            BC_TokenGet();
        }
        // while(BC_TokenGet() == DEBUG_LINE_CODE){};
        if (TokenType() != ACCESS_WORD_CODE && TokenType() != ACCESS_WORD && TokenType() != ACCESS_VAR)
        {
            // pV = pV->GetVarPointer();
            // if(!pV) {SetError("invalid ref in process expression"); return;}
            if (!bSkip)
            {
                if (!pV->IsArray())
                {
                    value->Copy(pV);
                    return;
                }                       // copy single value
                value->Copy(pV, index); // copy array element value
            }
            return;
        }

        if (bSkip)
        {
            while (TokenType() == ACCESS_WORD_CODE || TokenType() == ACCESS_WORD || TokenType() == ACCESS_VAR)
            {
                BC_TokenGet();
                while (TokenType() == DEBUG_LINE_CODE)
                {
                    BC_TokenGet();
                }
            }
            return;
        }
        // accessing to atribute (string) value
        pRoot = pV->GetAClass(); // root attribute class
        if (pRoot == nullptr)
        {
            SetError("zero AClass root");
            value->Set("ERROR: zero AClass root");
            while (TokenType() == ACCESS_WORD_CODE || TokenType() == ACCESS_WORD || TokenType() == ACCESS_VAR)
            {
                BC_TokenGet();
            }
            return;
            // throw std::runtime_error("zero AClass root");
        }
        while (TokenType() == ACCESS_WORD_CODE || TokenType() == ACCESS_WORD || TokenType() == ACCESS_VAR)
        {
            if (TokenType() == ACCESS_WORD_CODE)
            {
                if (pRoot == nullptr)
                {
                    value->Set("");
                    SetError("missed attribute: %s", SCodec.Convert(*((long *)&pRunCodeBase[TLR_DataOffset])));
                    while (TokenType() == ACCESS_WORD_CODE || TokenType() == ACCESS_WORD || TokenType() == ACCESS_VAR)
                    {
                        BC_TokenGet();
                    }
                    return;
                }
                pRoot = pRoot->GetAttributeClassByCode(*((long *)&pRunCodeBase[TLR_DataOffset]));
                if (pRoot == nullptr)
                {
                    value->Set("");
                    SetError("missed attribute: %s", SCodec.Convert(*((long *)&pRunCodeBase[TLR_DataOffset])));
                    while (TokenType() == ACCESS_WORD_CODE || TokenType() == ACCESS_WORD || TokenType() == ACCESS_VAR)
                    {
                        BC_TokenGet();
                    }
                    return;
                }
            }
            else if (TokenType() == ACCESS_WORD)
            {
                if (pRoot == nullptr)
                {
                    value->Set("");
                    SetError("missed attribute: %s", (char *)&pRunCodeBase[TLR_DataOffset]);
                    while (TokenType() == ACCESS_WORD_CODE || TokenType() == ACCESS_WORD || TokenType() == ACCESS_VAR)
                    {
                        BC_TokenGet();
                    }
                    return;
                }
                pRoot = pRoot->GetAttributeClass((char *)&pRunCodeBase[TLR_DataOffset]);
                if (pRoot == nullptr)
                {
                    value->Set("");
                    SetError("missed attribute: %s", (char *)&pRunCodeBase[TLR_DataOffset]);
                    while (TokenType() == ACCESS_WORD_CODE || TokenType() == ACCESS_WORD || TokenType() == ACCESS_VAR)
                    {
                        BC_TokenGet();
                    }
                    return;
                }
            }
            else
            {
                // BC_TokenGet();
                while (BC_TokenGet() == DEBUG_LINE_CODE)
                {
                }
                var_code = *((long *)&pRunCodeBase[TLR_DataOffset]);
                if (TokenType() == VARIABLE)
                {
                    real_var = VarTab.GetVar(var_code);
                    if (real_var == nullptr)
                    {
                        SetError("Global variable not found");
                        break;
                    }

                    pV = real_var->value.get();
                }
                else
                {
                    // local variable
                    if (pRun_fi)
                    {
                        pV = SStack.Read(pRun_fi->stack_offset, var_code);
                        if (pV == nullptr)
                        {
                            SetError("Local variable not found");
                            break;
                        }
                    }
                    else
                        return;
                }

                TempData.Copy(pV);
                TempData.Convert(VAR_STRING);
                TempData.Get(pString);

                pRoot = pRoot->FindAClass(pRoot, pString);
                if (pRoot == nullptr)
                {
                    value->Set("");
                    SetError("missed attribute %s", pString);
                    while (BC_TokenGet() == DEBUG_LINE_CODE)
                    {
                    }
                    return;
                }
                // pRoot = pRoot->GetAttributeClass((char *)&pRunCodeBase[TLR_DataOffset]);
            }
            // BC_TokenGet();
            while (BC_TokenGet() == DEBUG_LINE_CODE)
            {
            }
        }
        pString = pRoot->GetThisAttr();
        value->Set(pString);
        break;
    }
}

//===============================================================================================================

// test for makeref
bool COMPILER::CompileExpression(SEGMENT_DESC &Segment)
{
    uint32_t dwVarCode;
    const VarInfo *real_var;
    LocalVarInfo lvi;

    const S_TOKEN_TYPE Token_type = CompileAuxiliaryTokens(Segment);
    if (Token_type == SEPARATOR)
    {
        SetError(ERR_INVALID_EXPRESSION);
        return false;
    }

    if (Token_type == AND)
    {
        // make reference operation

        S_TOKEN_TYPE sttVariableType = Token.Get();

        if (sttVariableType != UNKNOWN)
        {
            SetError("invalid '&' usage");
            return false;
        }

        sttVariableType = DetectUnknown(dwVarCode);
        if (!(sttVariableType == VARIABLE || sttVariableType == LOCAL_VARIABLE))
        {
            SetError("variable not found");
            return false;
        }

        const S_TOKEN_TYPE sttTokenType = Token.Get();

        // alloc for result
        CompileToken(Segment, STACK_ALLOC);

        if (sttTokenType == SQUARE_OPEN_BRACKET)
        {
            if (sttVariableType == VARIABLE)
            {
                real_var = VarTab.GetVarX(dwVarCode);
                if (real_var == nullptr)
                {
                    SetError("Invalid var code");
                    return false;
                }

                // check for possibilities of '[' operator
                if (!real_var->IsArray())
                {
                    if (real_var->type != VAR_REFERENCE)
                    {
                        SetError("EN: %d", real_var->elements);
                        SetError(" A Invalid '[' operator, %s - isnt array", real_var->name.c_str());
                        return false;
                    }
                }
            }
            else
            {
                FuncTab.GetVar(lvi, CurrentFuncCode, dwVarCode);
                // check for possibilities of '[' operator
                if (!lvi.IsArray())
                {
                    if (lvi.type != VAR_REFERENCE)
                    {
                        SetError(" B Invalid '[' operator, %s - isnt array", lvi.name.c_str());
                        return false;
                    }
                }
            }

            // alloc for array index
            // CompileToken(Segment,STACK_ALLOC);
            // after top of the stack must be array index
            Token.Get();
            if (!CompileExpression_L2(Segment))
            {
                SetError("invalid syntax");
                return false;
            }
            // test for ']'
            if (Token.GetType() != SQUARE_CLOSE_BRACKET)
            {
                SetError("missing ']'");
                return false;
            }

            // copy array index to BX
            CompileToken(Segment, STACK_POP);
            CompileToken(Segment, BX);

            CompileToken(Segment, SETREF_BXINDEX);
            CompileToken(Segment, STACK_TOP);
            CompileToken(Segment, sttVariableType, 1, &dwVarCode, sizeof(uint32_t));

            // next after ']'
            CompileAuxiliaryTokens(Segment);
        }
        else
        {
            CompileToken(Segment, SETREF);
            CompileToken(Segment, STACK_TOP);
            CompileToken(Segment, sttVariableType, 1, &dwVarCode, sizeof(uint32_t));
        }

        // proceed to next token
        // CompileAuxiliaryTokens(Segment);

        return true;
    }
    return CompileExpression_L0(Segment);
}

// logical operators (OR,AND)
bool COMPILER::CompileExpression_L0(SEGMENT_DESC &Segment)
{
    uint32_t dwJumpOffset1;
    uint32_t dwJumpOffset2;

    if (!CompileExpression_L1(Segment))
    {
        SetError(ERR_INVALID_EXPRESSION);
        return false;
    }

    S_TOKEN_TYPE sttOpType = Token.GetType();

    while (sttOpType == OP_BOOL_AND || sttOpType == OP_BOOL_OR)
    {
        sttOpType = Token.GetType();

        CompileAuxiliaryTokens(Segment);

        // copy to EX value of first operand
        /*CompileToken(Segment,MOVE);
        CompileToken(Segment,EX);
        CompileToken(Segment,STACK_TOP);*/

        CompileToken(Segment, STACK_READ);
        CompileToken(Segment, EX);

        // save jump update position
        const uint32_t dwUpdateOffset1 = Segment.BCode_Program_size + 2;
        // point of jump not determined yet
        dwJumpOffset1 = 0;
        // compile jump command
        if (sttOpType == OP_BOOL_AND)
        {
            // AND : if 1st operand is zero - doesnt need to calculate second - jump to jump1
            CompileToken(Segment, JUMP_Z, 1, (char *)&dwJumpOffset1, sizeof(uint32_t));
        }
        else
        {
            // OR : if 1st operand is non zero - doesnt need to calculate second - jump to jump1
            CompileToken(Segment, JUMP_NZ, 1, (char *)&dwJumpOffset1, sizeof(uint32_t));
        }

        if (!CompileExpression_L1(Segment))
        {
            SetError(ERR_INVALID_EXPRESSION);
            return false;
        }

        CompileToken(Segment, STACK_POP);
        CompileToken(Segment, AX);

        CompileToken(Segment, sttOpType);
        CompileToken(Segment, STACK_TOP);
        CompileToken(Segment, AX);

        // skip next section - proceed to jump2
        const uint32_t dwUpdateOffset2 = Segment.BCode_Program_size + 2;
        dwJumpOffset2 = 0;
        CompileToken(Segment, JUMP, 1, (char *)&dwJumpOffset2, sizeof(uint32_t));

        // jump1
        // just convert stack top to bool
        memcpy(&Segment.pCode[dwUpdateOffset1], &Segment.BCode_Program_size, sizeof(uint32_t));
        CompileToken(Segment, OP_BOOL_CONVERT);
        CompileToken(Segment, STACK_TOP);

        // jump2
        // end of operation
        memcpy(&Segment.pCode[dwUpdateOffset2], &Segment.BCode_Program_size, sizeof(uint32_t));

        sttOpType = Token.GetType();
    }
    return true;
}

// logical compare (>,<,==,>=,<=,!=)
bool COMPILER::CompileExpression_L1(SEGMENT_DESC &Segment)
{
    if (!CompileExpression_L2(Segment))
    {
        SetError(ERR_INVALID_EXPRESSION);
        return false;
    }

    S_TOKEN_TYPE sttOpType = Token.GetType();

    while (sttOpType == OP_NOT_EQUAL || sttOpType == OP_LESSER_OR_EQUAL || sttOpType == OP_LESSER ||
           sttOpType == OP_GREATER_OR_EQUAL || sttOpType == OP_GREATER || sttOpType == OP_BOOL_EQUAL)
    {
        sttOpType = Token.GetType();

        CompileAuxiliaryTokens(Segment);

        if (!CompileExpression_L2(Segment))
        {
            SetError(ERR_INVALID_EXPRESSION);
            return false;
        }

        CompileToken(Segment, STACK_POP);
        CompileToken(Segment, AX);
        CompileToken(Segment, sttOpType);
        CompileToken(Segment, STACK_TOP);
        CompileToken(Segment, AX);

        sttOpType = Token.GetType();
    }

    return true;
}

// '+' and '-' operators
bool COMPILER::CompileExpression_L2(SEGMENT_DESC &Segment)
{
    if (!CompileExpression_L3(Segment))
    {
        SetError(ERR_INVALID_EXPRESSION);
        return false;
    }

    S_TOKEN_TYPE sttOpType = Token.GetType();

    while (sttOpType == OP_PLUS || sttOpType == OP_MINUS)
    {
        sttOpType = Token.GetType();

        CompileAuxiliaryTokens(Segment);

        if (!CompileExpression_L3(Segment))
        {
            SetError(ERR_INVALID_EXPRESSION);
            return false;
        }

        CompileToken(Segment, STACK_POP);
        CompileToken(Segment, AX);
        CompileToken(Segment, sttOpType);
        CompileToken(Segment, STACK_TOP);
        CompileToken(Segment, AX);

        sttOpType = Token.GetType();
    }
    return true;
}

// '*' and '/' operators
bool COMPILER::CompileExpression_L3(SEGMENT_DESC &Segment)
{
    if (!CompileExpression_L4(Segment))
    {
        SetError(ERR_INVALID_EXPRESSION);
        return false;
    }

    S_TOKEN_TYPE sttOpType = Token.GetType();

    while (sttOpType == OP_MULTIPLY || sttOpType == OP_DIVIDE || sttOpType == OP_MODUL)
    {
        sttOpType = Token.GetType();
        CompileAuxiliaryTokens(Segment);

        if (!CompileExpression_L4(Segment))
        {
            SetError(ERR_INVALID_EXPRESSION);
            return false;
        }

        CompileToken(Segment, STACK_POP);
        CompileToken(Segment, AX);
        CompileToken(Segment, sttOpType);
        CompileToken(Segment, STACK_TOP);
        CompileToken(Segment, AX);

        sttOpType = Token.GetType();
    }
    return true;
}

// '^' operations
bool COMPILER::CompileExpression_L4(SEGMENT_DESC &Segment)
{
    if (!CompileExpression_L5(Segment))
    {
        SetError(ERR_INVALID_EXPRESSION);
        return false;
    }

    if (Token.GetType() == OP_POWER)
    {
        CompileAuxiliaryTokens(Segment);

        if (!CompileExpression_L5(Segment))
        {
            SetError(ERR_INVALID_EXPRESSION);
            return false;
        }

        CompileToken(Segment, STACK_POP);
        CompileToken(Segment, AX);
        CompileToken(Segment, OP_POWER);
        CompileToken(Segment, STACK_TOP);
        CompileToken(Segment, AX);
    }
    return true;
}

// sign
bool COMPILER::CompileExpression_L5(SEGMENT_DESC &Segment)
{
    const S_TOKEN_TYPE sttOpType = Token.GetType();
    if (sttOpType == OP_PLUS || sttOpType == OP_MINUS || sttOpType == OP_BOOL_NEG)
        CompileAuxiliaryTokens(Segment);

    if (!CompileExpression_L6(Segment))
    {
        SetError(ERR_INVALID_EXPRESSION);
        return false;
    }

    switch (sttOpType)
    {
    case OP_MINUS:
        CompileToken(Segment, OP_SMINUS);
        CompileToken(Segment, STACK_TOP);
        break;
    case OP_BOOL_NEG:
        CompileToken(Segment, OP_BOOL_NEG);
        CompileToken(Segment, STACK_TOP);
        break;
    }
    return true;
}

// '(' and ')'
bool COMPILER::CompileExpression_L6(SEGMENT_DESC &Segment)
{
    if (Token.GetType() == OPEN_BRACKET)
    {
        CompileAuxiliaryTokens(Segment);
        if (!CompileExpression_L1(Segment))
        {
            SetError(ERR_INVALID_EXPRESSION);
            return false;
        }
        if (Token.GetType() != CLOSE_BRACKET)
        {
            SetError("No matching ')' in expression");
            return false;
        }
        CompileAuxiliaryTokens(Segment);
        return true;
    }
    return CompileExpression_L7(Segment);
}

// read value
bool COMPILER::CompileExpression_L7(SEGMENT_DESC &Segment)
{
    uint32_t dwRCode = 0;
    S_TOKEN_TYPE sttResult;
    S_TOKEN_TYPE sttVariableField;
    S_TOKEN_TYPE sttFuncCallType;
    uint32_t dwVarCode;
    uint32_t dwAWCode;
    const VarInfo *real_var;
    LocalVarInfo lvi;
    FuncInfo fi;

    bool bDynamicCall = false;
    switch (Token.GetType())
    {
    case SEPARATOR:

        return true;
    case NUMBER:
        CompileToken(Segment, STACK_PUSH);
        CompileNumber(Segment);
        CompileAuxiliaryTokens(Segment);
        return true;
    case FLOAT_NUMBER:
        CompileToken(Segment, STACK_PUSH);
        CompileFloatNumber(Segment);
        CompileAuxiliaryTokens(Segment);
        return true;
    case STRING:
        CompileToken(Segment, STACK_PUSH);
        CompileString(Segment);
        CompileAuxiliaryTokens(Segment);
        return true;
    case TRUE_CONST:
        CompileToken(Segment, STACK_PUSH);
        dwRCode = 1;
        CompileToken(Segment, NUMBER, 1, (char *)&dwRCode, sizeof(uint32_t));
        CompileAuxiliaryTokens(Segment);
        return true;
    case FALSE_CONST:
        CompileToken(Segment, STACK_PUSH);
        dwRCode = 0;
        CompileToken(Segment, NUMBER, 1, (char *)&dwRCode, sizeof(uint32_t));
        CompileAuxiliaryTokens(Segment);
        return true;

    case CALL:
        bDynamicCall = true;
    case UNKNOWN:
        if (!bDynamicCall)
        {
            sttResult = DetectUnknown(dwRCode);
            // set func call method to standart
            sttFuncCallType = CALL_FUNCTION;
        }
        else
        {
            sttResult = CALL;
            sttFuncCallType = CALL_FUNCTION;
        }

        switch (sttResult)
        {
        case DEFINE_VAL:
            DEFINFO di;
            DefTab.GetDef(di, dwRCode);
            switch (di.deftype)
            {
            case NUMBER:
                CompileToken(Segment, STACK_PUSH);
                CompileToken(Segment, NUMBER, 1, (char *)&di.data4b, sizeof(uint32_t));
                CompileAuxiliaryTokens(Segment);
                break;
            case FLOAT_NUMBER:
                CompileToken(Segment, STACK_PUSH);
                CompileToken(Segment, FLOAT_NUMBER, 1, (char *)&di.data4b, sizeof(uint32_t));
                CompileAuxiliaryTokens(Segment);
                break;
            case STRING:
                CompileToken(Segment, STACK_PUSH);
                uint32_t string_size;
                string_size = strlen((char *)di.data4b) + 1;
                CompileToken(Segment, STRING, 2, (char *)&string_size, sizeof(uint32_t), (char *)di.data4b,
                             string_size);
                CompileAuxiliaryTokens(Segment);
                break;
            }
            return true;
        case CALL:
            // this is dynamic function call

            // call type - dynamic
            sttFuncCallType = CALL;

            if (Token.Get() != UNKNOWN)
            {
                SetError("invalid dynamic call syntax");
                return false;
            }

            // check for string variable
            sttVariableField = DetectUnknown(dwVarCode);

            if (sttVariableField == VARIABLE)
            {
                real_var = VarTab.GetVarX(dwVarCode);
                if (real_var == nullptr)
                {
                    SetError("Invalid var code");
                    return false;
                }

                if (real_var->type != VAR_STRING)
                {
                    SetError("'%s' must be string variable", Token.GetData());
                    return false;
                }
            }
            else if (sttVariableField == LOCAL_VARIABLE)
            {
                FuncTab.GetVar(lvi, CurrentFuncCode, dwVarCode);
                if (lvi.type != VAR_STRING)
                {
                    SetError("'%s' must be string variable", Token.GetData());
                    return false;
                }
            }
            else
            {
                SetError("invalid dynamic call syntax: '%s' must be string variable", Token.GetData());
                return false;
            }

            // proceed to standart func call routine

        case CALL_FUNCTION:
            // this is function call

            // first prepare stack for calling function

            if (Token.Get() != OPEN_BRACKET)
            {
                SetError("missing '('");
                return false;
            }

            // just get, already checked by DetectUnknown
            FuncTab.GetFuncX(fi, dwRCode);

            // if(fi.return_type == TVOID){ SetError("void function in expression: %s",fi.name); return false; }

            uint32_t func_args;
            func_args = 0;

            if (fi.arguments == 0 && !IsIntFuncVarArgsNum(dwRCode) && sttFuncCallType != CALL)
            {
                // function without parameters
                if (Token.Get() != CLOSE_BRACKET)
                {
                    SetError("missing ')'");
                    return false;
                }
            }
            else
            {
                // proceed with parameters list
                do
                {
                    // compile expression leave in stack top expression result
                    if (!CompileExpression(Segment))
                    {
                        SetError("invalid function argument");
                        return false;
                    }
                    if (Token.GetType() != COMMA)
                    {
                        if (Token.GetType() != CLOSE_BRACKET)
                        {
                            SetError("invalid function argument");
                            return false;
                        }
                    }
                    func_args++;
                } while (Token.GetType() == COMMA);
            }

            /*
            // off for debug needs
            if(sttFuncCallType != CALL)    // skip argument checking for dynamic function call
            {
              if(func_args != fi.arguments)
              {
                if(fi.segment_id != INTERNAL_SEGMENT_ID)
                {
                  // not internal function cant accept different arguments number
                  SetError("function %s(args:%d) doesnt accept %d arguments",fi.name,fi.arguments,func_args);
                  return false;
                }
                else
                {
                  if(!IsIntFuncVarArgsNum(dwRCode))
                  {
                    SetError("function '%s(args:%d)' doesnt accept %d arguments",fi.name,fi.arguments,func_args);
                    return false;
                  }

                }
              }
            }//*/

            // call function
            if (sttFuncCallType == CALL_FUNCTION)
                CompileToken(Segment, CALL_FUNCTION, 1, (char *)&dwRCode, sizeof(uint32_t));
            else
            {
                // code of function must be find out on the fly from variable
                CompileToken(Segment, CALL);
                CompileToken(Segment, sttVariableField, 1, (char *)&dwVarCode, sizeof(uint32_t));
            }
            CompileToken(Segment, ARGS_NUM, 1, (char *)&func_args, sizeof(uint32_t));

            // ...
            // proceed to next token
            CompileAuxiliaryTokens(Segment);

            return true;
        case LOCAL_VARIABLE:
        case VARIABLE:

            // set local or global type
            sttVariableField = sttResult;

            // variable code
            dwVarCode = dwRCode;

            // we going to place into stack variable value or attribute string - alloc stack data
            CompileToken(Segment, STACK_ALLOC);

            sttResult = Token.Get();
            if (sttResult == SQUARE_OPEN_BRACKET)
            {
                // way for array element

                // doesnt need to check, becose already pass DetectUnknown function

                if (sttVariableField == VARIABLE)
                {
                    real_var = VarTab.GetVarX(dwVarCode);
                    if (real_var == nullptr)
                    {
                        SetError("Invalid var code");
                        return false;
                    }

                    // check for possibilities of '[' operator
                    if (!real_var->IsArray())
                    {
                        if (real_var->type != VAR_REFERENCE)
                        {
                            SetError(" C Invalid '[' operator, %s - isnt array", real_var->name.c_str());
                            return false;
                        }
                    }
                }
                else
                {
                    FuncTab.GetVar(lvi, CurrentFuncCode, dwVarCode);
                    // check for possibilities of '[' operator
                    if (!lvi.IsArray())
                    {
                        if (lvi.type != VAR_REFERENCE)
                        {
                            SetError(" D Invalid '[' operator, %s - isnt array", lvi.name.c_str());
                            return false;
                        }
                    }
                }

                // CompileToken(Segment,STACK_ALLOC);
                // after top of the stack must be array index
                Token.Get();
                if (!CompileExpression_L2(Segment))
                    return false;
                // test for ']'
                if (Token.GetType() != SQUARE_CLOSE_BRACKET)
                {
                    SetError("missing ']'");
                    return false;
                }
                // proceed to next token
                sttResult = CompileAuxiliaryTokens(Segment);

                // if not accessing attribute - copy value and return
                if (sttResult != DOT)
                {
                    // optimiztion possible: MOVE [STACK-1],[STACK]

                    // copy array index to BX
                    CompileToken(Segment, STACK_POP);
                    CompileToken(Segment, BX);

                    // write to stack element value (index in X)
                    CompileToken(Segment, STACK_WRITE_BXINDEX);
                    CompileToken(Segment, sttVariableField, 1, (char *)&dwVarCode, sizeof(uint32_t));

                    // proceed to next token
                    // CompileAuxiliaryTokens(Segment);

                    return true;
                }

                // in other case want to access attribute string value

                // copy array index to BX
                CompileToken(Segment, STACK_POP);
                CompileToken(Segment, BX);

                // move ap to AP register
                CompileToken(Segment, MOVEAP_BXINDEX);
                CompileToken(Segment, AP);
                CompileToken(Segment, sttVariableField, 1, (char *)&dwVarCode, sizeof(uint32_t));

                // proceed to ADVANCE AP
            }
            else
            {
                // way for non array var

                if (sttResult != DOT)
                {
                    // copy variavle to stack
                    CompileToken(Segment, STACK_WRITE);
                    CompileToken(Segment, sttVariableField, 1, (char *)&dwVarCode, sizeof(uint32_t));
                    return true;
                }

                // in other case want to access attribute string value

                // move ap to AP register
                CompileToken(Segment, MOVEAP);
                CompileToken(Segment, AP);
                CompileToken(Segment, sttVariableField, 1, (char *)&dwVarCode, sizeof(uint32_t));
            }

            // ADVANCE AP

            sttResult = Token.Get();
            do
            {
                // if(sttResult == OPEN_BRACKET)
                if (Token.GetType() == OPEN_BRACKET)
                {
                    // access var
                    sttResult = Token.Get();
                    if (sttResult != UNKNOWN)
                    {
                        SetError("Invalid access var syntax");
                        return false;
                    }
                    sttResult = DetectUnknown(dwVarCode);
                    if (!(sttResult == VARIABLE || sttResult == LOCAL_VARIABLE))
                    {
                        SetError("not variable: %s", Token.GetData());
                        return false;
                    }
                    CompileToken(Segment, ADVANCE_AP);
                    CompileToken(Segment, sttResult, 1, &dwVarCode, sizeof(uint32_t));
                    if (Token.Get() != CLOSE_BRACKET)
                    {
                        SetError("missing ')'");
                        return false;
                    }
                }
                else
                {
                    // access word
                    if (Token.GetData() == nullptr)
                    {
                        SetError("Invalid access string");
                        return false;
                    }
                    Token.LowCase();
                    dwAWCode = SCodec.Convert(Token.GetData());
                    CompileToken(Segment, ADVANCE_AP);
                    CompileToken(Segment, ACCESS_WORD_CODE, 1, &dwAWCode, sizeof(uint32_t));
                }
                sttResult = Token.Get();
                if (sttResult == DOT)
                    Token.Get(); // get next token (after dot)
            } while (sttResult == DOT);

            // write attribute string to stack top
            CompileToken(Segment, STACK_WRITE);
            CompileToken(Segment, AP_VALUE);

            return true;
        default:
            // ???
            SetError(ERR_INVALID_EXPRESSION);
            return false;
        }
        return true;
    }
    SetError(ERR_INVALID_EXPRESSION);
    return false;
}
