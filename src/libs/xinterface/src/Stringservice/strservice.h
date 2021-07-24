#pragma once

#include "vmodule_api.h"

#include "script_libriary.h"
#include "stringService.h"

//-----------SDEVICE-----------
class STRSERVICE : public VSTRSERVICE
{
    struct UsersStringBlock
    {
        long nref;
        char *fileName;
        long blockID;
        long nStringsQuantity;
        char **psStrName;
        char **psString;

        UsersStringBlock *next;
    };

  public:
    STRSERVICE();
    ~STRSERVICE() override;

    bool Init() override;
    bool LoadState(ENTITY_STATE *state) override;
    bool CreateState(ENTITY_STATE_GEN *state_gen) override;
    void RunStart() override;
    void RunEnd() override;

    void SetLanguage(const char *sLanguage) override;
    char *GetLanguage() override;

    char *GetString(const char *stringName, char *sBuffer = nullptr, size_t bufferSize = 0) override;
    long GetStringNum(const char *stringName) override;
    char *GetString(long strNum) override;
    char *GetStringName(long strNum) override;

    long OpenUsersStringFile(const char *fileName) override;
    void CloseUsersStringFile(long id) override;
    char *TranslateFromUsers(long id, const char *inStr) override;

    void SetDialogSourceFile(const char *fileName);

    char *TranslateForDialog(const char *str)
    {
        return TranslateFromUsers(m_nDialogSourceFile, (char *)str);
    }

  protected:
    void LoadIni();
    long GetFreeUsersID() const;
    bool GetNextUsersString(char *src, long &idx, char **strName, char **strData) const;

  protected:
    char *m_sLanguage;
    char *m_sIniFileName;
    char *m_sLanguageDir;

    long m_nStringQuantity;
    char **m_psStrName;
    char **m_psString;

    UsersStringBlock *m_pUsersBlocks;

    long m_nDialogSourceFile;
};

class SCRIPT_INTERFACE_FUNCTIONS : public SCRIPT_LIBRIARY
{
  public:
    SCRIPT_INTERFACE_FUNCTIONS(){};

    ~SCRIPT_INTERFACE_FUNCTIONS() override{};
    bool Init() override;
};
