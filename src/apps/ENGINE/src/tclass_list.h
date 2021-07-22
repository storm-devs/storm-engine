#pragma once

template <class ClassType> class TCLASS_LIST
{
    ClassType **pTable;
    uint32_t nClassesNum;

  public:
    TCLASS_LIST()
    {
        nClassesNum = 0;
        pTable = 0;
    };

    ~TCLASS_LIST()
    {
        Release();
    };

    void Release()
    {
        if (pTable)
        {
            for (uint32_t n = 0; n < nClassesNum; n++)
                delete pTable[n];
            free(pTable);
            pTable = 0;
        }
        nClassesNum = 0;
    };

    void Add(ClassType *pClass)
    {
        uint32_t n = nClassesNum;
        nClassesNum++;
        pTable = (ClassType **)realloc(pTable, nClassesNum * sizeof(ClassType *));
        pTable[n] = pClass;
    };

    void Del(uint32_t _n)
    {
        if (_n >= nClassesNum)
            return;
        delete pTable[_n];
        for (uint32_t n = _n; n < (nClassesNum - 1); n++)
            pTable[n] = pTable[n + 1];
        nClassesNum--;
    }

    ClassType *Read(uint32_t _n)
    {
        if (_n >= nClassesNum)
            return nullptr;
        return pTable[_n];
    };

    uint32_t GetClassesNum()
    {
        return nClassesNum;
    }
};
