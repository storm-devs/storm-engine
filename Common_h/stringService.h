#ifndef __VSTRINGSERVICE_H__
#define __VSTRINGSERVICE_H__

#include "..\common_h\vmodule_api.h"

class VSTRSERVICE : public SERVICE
{
public:
	virtual bool	Init() = 0;
	virtual bool	LoadState(ENTITY_STATE *state) = 0;
	virtual bool	CreateState(ENTITY_STATE_GEN *state_gen) = 0;
	virtual void	RunStart() = 0;
	virtual void	RunEnd() = 0;

	virtual void	SetLanguage(const char* sLanguage) = 0;
	virtual char*	GetLanguage() = 0;

	virtual char*	GetString(const char* stringName, char* sBuffer=0, size_t bufferSize=0) = 0;
	virtual long	GetStringNum(const char* stringName) = 0;
	virtual char*	GetString(long strNum) = 0;
	virtual char*	GetStringName(long strNum) = 0;

	virtual long	OpenUsersStringFile(char * fileName) = 0;
	virtual void	CloseUsersStringFile(long id) = 0;
	virtual char*	TranslateFromUsers(long id, char * inStr) = 0;
};

#endif
