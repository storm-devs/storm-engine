// Create NMSend
dword _cdecl NMCreate(VS_STACK * pS)
{
	NMSend * pSend = NEW NMSend();
	VDATA * pVR = (VDATA *)pS->Push();
	pVR->Set(long(pSend));
	return IFUNCRESULT_OK;
}

// Delete NMSend 
dword _cdecl NMDelete(VS_STACK * pS)
{
	NMSend * pSend = (NMSend *)(((VDATA*)pS->Pop())->GetLong());
	delete pSend;
	return IFUNCRESULT_OK;
}

// return true if NMRecv is not empty
dword _cdecl NMNotEmpty(VS_STACK * pS)
{
	NMRecv * pR = (NMRecv *)(((VDATA*)pS->Pop())->GetLong());
	((VDATA*)pS->Push())->Set(long(!pR->IsEmpty()));
	return IFUNCRESULT_OK;
}

// NMRecv functions
dword _cdecl NMGetByte(VS_STACK * pS)
{
	NMRecv * pR = (NMRecv *)(((VDATA*)pS->Pop())->GetLong());
	((VDATA*)pS->Push())->Set(long(pR->GetByte()));
	return IFUNCRESULT_OK;
}

dword _cdecl NMGetWord(VS_STACK * pS)
{
	NMRecv * pR = (NMRecv *)(((VDATA*)pS->Pop())->GetLong());
	((VDATA*)pS->Push())->Set(long(pR->GetWord()));
	return IFUNCRESULT_OK;
}

dword _cdecl NMGetDword(VS_STACK * pS)
{
	NMRecv * pR = (NMRecv *)(((VDATA*)pS->Pop())->GetLong());
	((VDATA*)pS->Push())->Set(long(pR->GetDword()));
	return IFUNCRESULT_OK;
}

dword _cdecl NMGet24(VS_STACK * pS)
{
	NMRecv * pR = (NMRecv *)(((VDATA*)pS->Pop())->GetLong());
	((VDATA*)pS->Push())->Set(long(pR->Get24()));
	return IFUNCRESULT_OK;
}

dword _cdecl NMGetFloat(VS_STACK * pS)
{
	NMRecv * pR = (NMRecv *)(((VDATA*)pS->Pop())->GetLong());
	((VDATA*)pS->Push())->Set(pR->GetFloat());
	return IFUNCRESULT_OK;
}

dword _cdecl NMGetString(VS_STACK * pS)
{
	NMRecv * pR = (NMRecv *)(((VDATA*)pS->Pop())->GetLong());
	((VDATA*)pS->Push())->Set((char*)(const char *)pR->GetString());
	return IFUNCRESULT_OK;
}

// NMSend functions

dword _cdecl NMAddByte(VS_STACK * pS)
{
	byte b = byte(((VDATA*)pS->Pop())->GetLong());
	NMSend * pSend = (NMSend *)(((VDATA*)pS->Pop())->GetLong());
	pSend->AddByte(b);
	return IFUNCRESULT_OK;
}

dword _cdecl NMAddWord(VS_STACK * pS)
{
	word w = word(((VDATA*)pS->Pop())->GetLong());
	NMSend * pSend = (NMSend *)(((VDATA*)pS->Pop())->GetLong());
	pSend->AddWord(w);
	return IFUNCRESULT_OK;
}

dword _cdecl NMAddDword(VS_STACK * pS)
{
	dword d = dword(((VDATA*)pS->Pop())->GetLong());
	NMSend * pSend = (NMSend *)(((VDATA*)pS->Pop())->GetLong());
	pSend->AddDword(d);
	return IFUNCRESULT_OK;
}

dword _cdecl NMAdd24(VS_STACK * pS)
{
	dword d = dword(((VDATA*)pS->Pop())->GetLong());
	NMSend * pSend = (NMSend *)(((VDATA*)pS->Pop())->GetLong());
	pSend->Add24(d);
	return IFUNCRESULT_OK;
}

dword _cdecl NMAddFloat(VS_STACK * pS)
{
	float f = ((VDATA*)pS->Pop())->GetFloat();
	NMSend * pSend = (NMSend *)(((VDATA*)pS->Pop())->GetLong());
	pSend->AddFloat(f);
	return IFUNCRESULT_OK;
}

dword _cdecl NMAddString(VS_STACK * pS)
{
	int iMaxLen = ((VDATA*)pS->Pop())->GetLong();
	string s = ((VDATA*)pS->Pop())->GetString();
	NMSend * pSend = (NMSend *)(((VDATA*)pS->Pop())->GetLong());
	pSend->AddString(s, iMaxLen);
	return IFUNCRESULT_OK;
}

dword _cdecl NMAddData(VS_STACK * pS)
{
	dword dwSize = (dword)((VDATA*)pS->Pop())->GetLong();
	char * pBuffer = (char*)((VDATA*)pS->Pop())->GetLong();
	NMSend * pSend = (NMSend *)(((VDATA*)pS->Pop())->GetLong());
	pSend->AddWord((word)dwSize);
	pSend->AddData(pBuffer, dwSize);
	return IFUNCRESULT_OK;
}
