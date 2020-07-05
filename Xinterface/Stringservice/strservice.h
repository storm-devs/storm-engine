#ifndef __STRSERVICE_H__
#define __STRSERVICE_H__


#include "..\..\common_h\vmodule_api.h"
//#include "..\..\common_h\defines.h"

#include "..\..\common_h\stringService.h"

//-----------SDEVICE-----------
class STRSERVICE : public VSTRSERVICE
{
	struct UsersStringBlock
	{
		long	nref;
		char*	fileName;
		long	blockID;
		long	nStringsQuantity;
		char*	*psStrName;
		char*	*psString;

		UsersStringBlock * next;
	};

public:
	 STRSERVICE();
	~STRSERVICE();

	bool	Init();
	bool	LoadState(ENTITY_STATE *state);
	bool	CreateState(ENTITY_STATE_GEN *state_gen);
	void	RunStart();
	void	RunEnd();

	void	SetLanguage(const char* sLanguage);
	char*	GetLanguage();

	char*	GetString(const char* stringName, char* sBuffer=0, size_t bufferSize=0);
	long	GetStringNum(const char* stringName);
	char*	GetString(long strNum);
	char*	GetStringName(long strNum);

	long	OpenUsersStringFile(char * fileName);
	void	CloseUsersStringFile(long id);
	char*	TranslateFromUsers(long id, char * inStr);

	void	SetDialogSourceFile(const char * fileName);
	char*	TranslateForDialog(const char * str) {return TranslateFromUsers(m_nDialogSourceFile,(char*)str);}

protected:
	void	LoadIni();
	long	GetFreeUsersID();
	bool	GetNextUsersString(char *src,long &idx,char* *strName,char* *strData);

protected:
	char*		m_sLanguage;
	char*		m_sIniFileName;
	char*		m_sLanguageDir;

	long		m_nStringQuantity;
	char*		*m_psStrName;
	char*		*m_psString;

	UsersStringBlock * m_pUsersBlocks;

	long		m_nDialogSourceFile;
};

class SCRIPT_INTERFACE_FUNCTIONS : public SCRIPT_LIBRIARY
{
public:
	SCRIPT_INTERFACE_FUNCTIONS(){};
	~SCRIPT_INTERFACE_FUNCTIONS(){};
	virtual bool Init();
};

#endif