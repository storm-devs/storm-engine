#include "obj_strservice.h"
#include "..\\..\\engine\\program\\interface\\messages.h"

OBJ_STRSERVICE::OBJ_STRSERVICE()
{
	m_pStrService = null;
}

OBJ_STRSERVICE::~OBJ_STRSERVICE()
{
	m_pStrService = null;
}

bool OBJ_STRSERVICE::Init()
{
	m_pStrService = (VSTRSERVICE *)_CORE_API->CreateService("STRSERVICE");
	if(!m_pStrService)	{_THROW("No service: strservice")}

	return true;
}

dword _cdecl OBJ_STRSERVICE::ProcessMessage(MESSAGE & message)
{
	switch(message.Long())
	{
	case MSG_STRSERVICE_OPEN_FILE:
		{
			char param[256];
			message.String(sizeof(param)-1,param);
			if(m_pStrService!=null) return m_pStrService->OpenUsersStringFile(param);
			return -1;
		}
		break;
	case MSG_STRSERVICE_CLOSE_FILE:
		{
			long fileID = message.Long();
			m_pStrService->CloseUsersStringFile(fileID);
		}
		break;
	case MSG_STRSERVICE_TRANSLATE_STRING:
		{
			long nUsrID = message.Long();
			VDATA * pvdat = message.ScriptVariablePointer();
			char * inStr = pvdat==null ? null : pvdat->GetString();
			pvdat = message.ScriptVariablePointer();
			char * outStr = null;
			if(m_pStrService!=null) outStr = m_pStrService->TranslateFromUsers(nUsrID,inStr);
			if(outStr!=null && pvdat!=null)	pvdat->Set(outStr);
			else
				pvdat->Set("");
		}
		break;
	case MSG_STRSERVICE_GET_LANGUAGE:
		{
			VDATA * pvdat = message.ScriptVariablePointer();
			char * outStr = null;
			if(m_pStrService!=null) outStr = m_pStrService->GetLanguage();
			if(outStr!=null && pvdat!=null)	pvdat->Set(outStr);
			else	pvdat->Set("");
		}
		break;
	}

	return 0;
}
