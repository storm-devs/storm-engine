#pragma once

#include <string.h>

#define MAX_STRING_SIZE 8192

class GeomNameParser
{
    std::vector<char *> Tokens;
    char NamesString[MAX_STRING_SIZE];

  public:
    // GeomNameParser () : Tokens(_FL_) {};
    //~GeomNameParser () {};

    const char *GetRandomName(const char *names)
    {
        Tokenize(names);
        if (Tokens.size() == 0)
            return nullptr;

        return Tokens[(rand() % Tokens.size())];
    }

    uint32_t GetTokensCount() const
    {
        return Tokens.size();
    }

    const char *GetTokenByIndex(uint32_t Index)
    {
        return Tokens[Index];
    }

    void Tokenize(const char *names)
    {
        Tokens.clear();
        strncpy_s(NamesString, names, MAX_STRING_SIZE);
        int StrLen = strlen(NamesString);
        if (StrLen > 0 && NamesString[StrLen - 1] != ';')
            strcat_s(NamesString, ";");
        StrLen = strlen(NamesString);

        auto FromChar = 0;
        for (auto n = 0; n < StrLen; n++)
        {
            if (NamesString[n] == ';')
            {
                NamesString[n] = 0;

                auto *ModelName = NamesString + FromChar;
                if (ModelName[0] != '\0')
                {
                    Tokens.push_back(ModelName);
                }
                FromChar = n + 1;
            }
        }
    }
};
