#include "script_func.h"
#include "..\common_h\sail_base.h"
#include "sail.h"
#include "..\common_h\net.h"

extern float g_fSailHoleDepend;
//extern float GetSailSpeed(int holeQ,int holeMax,float maxSpeed,float fSailHoleDepend);

DWORD __cdecl _ShipSailState(VS_STACK * pS)
{
	VDATA * pChrIdx = (VDATA*)pS->Pop(); if(!pChrIdx) return IFUNCRESULT_FAILED;
	long nChrIdx = pChrIdx->GetLong();

	VDATA * pVR = (VDATA*)pS->Push(); if (!pVR) return IFUNCRESULT_FAILED;

	// find sail class
	ENTITY_ID eid;
	if( api->FindClass(&eid,"SAIL",0) || NetFindClass(true,&eid,"NetSail") )
	{
		long n = ((SAIL*)eid.pointer)->GetSailStateForCharacter(nChrIdx);
		pVR->Set( n );
	}
	else pVR->Set( (long)0 );

	return IFUNCRESULT_OK;
}

DWORD __cdecl _GetAssembledString(VS_STACK * pS)
{
	VDATA * pAttrPnt = (VDATA*)pS->Pop();	if (!pAttrPnt) return IFUNCRESULT_FAILED;
	ATTRIBUTES * pAttr = pAttrPnt->GetAClass();

	VDATA * pFormatStr = (VDATA*)pS->Pop();	if (!pFormatStr) return IFUNCRESULT_FAILED;
	char * formatStr = pFormatStr->GetString();

	char retString[1024];	retString[0] = 0;
	if(formatStr!=null && pAttr!=null)
	{
		bool bBuildAccessString = false;
		char accessString[sizeof(retString)];
		accessString[0] = 0;
		int accessStrSize = 0;
		for(int i=0; ; i++)
		{
			if(formatStr[i]=='#' || !formatStr[i]) 
			{
				accessString[accessStrSize] = 0;
				if(bBuildAccessString)
				{
					if(accessStrSize>1)
					{
						int nAttrNameStart = 1;
						if(accessString[0]=='f' && accessString[1]=='.')
						{
							nAttrNameStart = 3;
						}
						ATTRIBUTES * pA = pAttr->FindAClass(pAttr,&accessString[nAttrNameStart]);
						char * writeStr = null;
						if(pA!=null)	writeStr = pA->GetThisAttr();
						if(writeStr) switch(accessString[0])
						{
						case 's':	strcat(retString,writeStr); break;
						case 'f':
							{
								char tmpp[256]; float ftmp=0.f;
								sscanf(writeStr,"%f",&ftmp);
								if(nAttrNameStart==1)	sprintf(tmpp,"%f",ftmp);
								else
								{
									char tmpFmtStr[5];
									tmpFmtStr[0]='%';
									tmpFmtStr[1]='.';
									tmpFmtStr[2]=accessString[2];
									tmpFmtStr[3]='f';
									tmpFmtStr[4]=0;
									sprintf(tmpp,tmpFmtStr,ftmp);
								}
								strcat(retString,tmpp);
							}
						break;
						case 'd':
							{
								char tmpp[256]; int ntmp=0;
								sscanf(writeStr,"%d",&ntmp);
								sprintf(tmpp,"%d",ntmp);
								strcat(retString,tmpp);
							}
						break;
						}
					}
				}
				else	strcat(retString,accessString);
				bBuildAccessString = !bBuildAccessString;
				accessStrSize = 0;
			}
			else
			{
				accessString[accessStrSize++] = formatStr[i];
			}
			if(!formatStr[i]) break;
		}
	}

	VDATA * pVR = (VDATA*)pS->Push(); if (!pVR) return IFUNCRESULT_FAILED;
	pVR->Set(retString);
	return IFUNCRESULT_OK;
}

DWORD __cdecl _funcGetSailSpeed(VS_STACK * pS)
{
	VDATA * pSailPow = (VDATA*)pS->Pop();
	float fSailPow = pSailPow->GetFloat();

	VDATA * pHoleMax = (VDATA*)pS->Pop();
	long nHoleMax = pHoleMax->GetLong();

	VDATA * pHoleQ = (VDATA*)pS->Pop();
	long nHoleQ = pHoleQ->GetLong();

	VDATA * pVR = (VDATA*)pS->Push(); if (!pVR) return IFUNCRESULT_FAILED;
	pVR->Set( fSailPow - GetSailSpeed(nHoleQ, nHoleMax, fSailPow) );

	return IFUNCRESULT_OK;
}

DWORD __cdecl _RandomHole2Sail(VS_STACK * pS)
{
	VDATA * pData;

	if ( !(pData=(VDATA*)pS->Pop()) ) return IFUNCRESULT_FAILED;
	int _addHoleQ = pData->GetLong();
	if ( !(pData=(VDATA*)pS->Pop()) ) return IFUNCRESULT_FAILED;
	DWORD _holeData = pData->GetLong();
	if ( !(pData=(VDATA*)pS->Pop()) ) return IFUNCRESULT_FAILED;
	int _maxHole = pData->GetLong();
	if ( !(pData=(VDATA*)pS->Pop()) ) return IFUNCRESULT_FAILED;
	int _groupNum = pData->GetLong();
	if ( !(pData=(VDATA*)pS->Pop()) ) return IFUNCRESULT_FAILED;
	char * _reyName = pData->GetString();
	if ( !(pData=(VDATA*)pS->Pop()) ) return IFUNCRESULT_FAILED;
	int _chrIdx = pData->GetLong();

	VDATA * pVR = (VDATA*)pS->Push(); if (!pVR) return IFUNCRESULT_FAILED;

	SAILONE_BASE * pSail = null;
	ENTITY_ID	ei;
	if( api->FindClass(&ei,"sail",0) || NetFindClass(true,&ei,"NetSail") )
	{
		pSail = ((SAIL_BASE*)api->GetEntityPointer(&ei))->FindSailForCharacter(_chrIdx, _reyName, _groupNum);
	}

	int holeArraySize = 0;
	int holeIdx[20];
	DWORD holeMask = _holeData;
	for(int i=0; holeMask>0; i++,holeMask>>=1)
		if( !(holeMask&1) )
			holeIdx[holeArraySize++] = i;

	holeMask = _holeData;
	while(holeArraySize>0 && _addHoleQ>0)
	{
		i = rand()%holeArraySize;
		holeMask |= 1<<holeIdx[i];
		holeArraySize--;
		_addHoleQ--;
		holeIdx[i] = holeIdx[holeArraySize];
	}

	if(pSail!=null && holeMask!=_holeData)
	{
		pSail->SetAllHole(holeMask);
		pSail->CalculateMirrorSailIndex();
	}

	pVR->Set((long)holeMask);

	return IFUNCRESULT_OK;
}

DWORD __cdecl _DeleteOneSailHole(VS_STACK * pS)
{
	VDATA * pData;

	if ( !(pData=(VDATA*)pS->Pop()) ) return IFUNCRESULT_FAILED;
	int _delHoleQ = pData->GetLong();
	if ( !(pData=(VDATA*)pS->Pop()) ) return IFUNCRESULT_FAILED;
	DWORD _holeData = pData->GetLong();
	if ( !(pData=(VDATA*)pS->Pop()) ) return IFUNCRESULT_FAILED;
	char * _reyName = pData->GetString();
	if ( !(pData=(VDATA*)pS->Pop()) ) return IFUNCRESULT_FAILED;
	char * _groupName = pData->GetString();
	if ( !(pData=(VDATA*)pS->Pop()) ) return IFUNCRESULT_FAILED;
	int _chrIdx = pData->GetLong();

	VDATA * pVR = (VDATA*)pS->Push(); if (!pVR) return IFUNCRESULT_FAILED;

	int _groupNum;
	sscanf(_groupName,"%d",&_groupNum);

	SAILONE_BASE * pSail = null;
	ENTITY_ID	ei;
	if( api->FindClass(&ei,"sail",0) || NetFindClass(true,&ei,"NetSail") )
	{
		pSail = ((SAIL_BASE*)api->GetEntityPointer(&ei))->FindSailForCharacter(_chrIdx, _reyName, _groupNum);
	}

	int holeArraySize = 0;
	int holeIdx[20];
	DWORD holeMask = _holeData;
	for(int i=0; holeMask>0; i++,holeMask>>=1)
		if(holeMask&1)
			holeIdx[holeArraySize++] = i;

	holeMask = _holeData;
	while(holeArraySize>0 && _delHoleQ>0)
	{
		i = rand()%holeArraySize;
		holeMask &= ~(1<<holeIdx[i]);
		holeArraySize--;
		_delHoleQ--;
		holeIdx[i] = holeIdx[holeArraySize];
	}

	if(pSail!=null && holeMask!=_holeData)
	{
		pSail->SetAllHole(holeMask);
		pSail->CalculateMirrorSailIndex();
	}

	pVR->Set((long)holeMask);

	return IFUNCRESULT_OK;
}

bool SCRIPT_RIGGING_FILES::Init()
{
	IFUNCINFO sIFuncInfo;

	sIFuncInfo.nArguments = 3;
	sIFuncInfo.pFuncName = "funcGetSailSpeed";
	sIFuncInfo.pReturnValueName = "float";
	sIFuncInfo.pFuncAddress = _funcGetSailSpeed;
	api->SetScriptFunction(&sIFuncInfo);

	sIFuncInfo.nArguments = 6;
	sIFuncInfo.pFuncName = "RandomHole2Sail";
	sIFuncInfo.pReturnValueName = "int";
	sIFuncInfo.pFuncAddress = _RandomHole2Sail;
	api->SetScriptFunction(&sIFuncInfo);

	sIFuncInfo.nArguments = 5;
	sIFuncInfo.pFuncName = "DeleteOneSailHole";
	sIFuncInfo.pReturnValueName = "int";
	sIFuncInfo.pFuncAddress = _DeleteOneSailHole;
	api->SetScriptFunction(&sIFuncInfo);

	sIFuncInfo.nArguments = 2;
	sIFuncInfo.pFuncName = "GetAssembledString";
	sIFuncInfo.pReturnValueName = "string";
	sIFuncInfo.pFuncAddress = _GetAssembledString;
	api->SetScriptFunction(&sIFuncInfo);

	sIFuncInfo.nArguments = 1;
	sIFuncInfo.pFuncName = "ShipSailState";
	sIFuncInfo.pReturnValueName = "float";
	sIFuncInfo.pFuncAddress = _ShipSailState;
	api->SetScriptFunction(&sIFuncInfo);

	return true;
}
