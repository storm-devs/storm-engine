#pragma once

#include "service.h"
#include "vma.hpp"

class VSTRSERVICE : public SERVICE
{
  public:
    bool Init() override = 0;
    bool LoadState(ENTITY_STATE *state) override = 0;
    bool CreateState(ENTITY_STATE_GEN *state_gen) override = 0;
    void RunStart() override = 0;
    void RunEnd() override = 0;

    virtual void SetLanguage(const char *sLanguage) = 0;
    virtual char *GetLanguage() = 0;

    virtual char *GetString(const char *stringName, char *sBuffer = nullptr, size_t bufferSize = 0) = 0;
    virtual int32_t GetStringNum(const char *stringName) = 0;
    virtual char *GetString(int32_t strNum) = 0;
    virtual char *GetStringName(int32_t strNum) = 0;

    virtual int32_t OpenUsersStringFile(const char *fileName) = 0;
    virtual void CloseUsersStringFile(int32_t id) = 0;
    virtual char *TranslateFromUsers(int32_t id, const char *inStr) = 0;
};
