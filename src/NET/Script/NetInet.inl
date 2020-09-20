dword _cdecl __ntohs(VS_STACK * pS)
{
	word wPort = (word)((VDATA*)pS->Pop())->GetLong();
	((VDATA*)pS->Push())->Set(long(ntohs(wPort)));
	return IFUNCRESULT_OK;
}

dword _cdecl __inet_ntoa(VS_STACK * pS)
{
	in_addr in;
	in.S_un.S_addr = (dword)((VDATA*)pS->Pop())->GetLong();
	((VDATA*)pS->Push())->Set(inet_ntoa(in));
	return IFUNCRESULT_OK;
}

dword _cdecl __inet_ntoax(VS_STACK * pS)
{
	in_addr in;
	in.S_un.S_addr = (dword)((VDATA*)pS->Pop())->GetLong();
	string sIP = inet_ntoa(in);
	for (dword i=0; i<sIP.Len(); i++) if (sIP[i] == '.') sIP[i] = 'x';
	((VDATA*)pS->Push())->Set((char *)(const char *)sIP);
	return IFUNCRESULT_OK;
}
