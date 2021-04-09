#pragma once

#include "service.h"
#include "vmodule_api.h"

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
    virtual long GetStringNum(const char *stringName) = 0;
    virtual char *GetString(long strNum) = 0;
    virtual char *GetStringName(long strNum) = 0;

    virtual long OpenUsersStringFile(const char *fileName) = 0;
    virtual void CloseUsersStringFile(long id) = 0;
    virtual char *TranslateFromUsers(long id, const char *inStr) = 0;
};
