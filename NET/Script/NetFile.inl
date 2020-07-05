void SaveAttribute(ATTRIBUTES * pA, NMSend & nmWrite)
{
	nmWrite.AddDword_NoDbg(pA->GetAttributesNum());
	nmWrite.AddString_NoDbg(string(pA->GetThisName()), 65536 * 32);
	nmWrite.AddString_NoDbg(string(pA->GetThisAttr()), 65536 * 32);

	for (dword i=0; i<pA->GetAttributesNum(); i++)
		SaveAttribute(pA->GetAttributeClass(i), nmWrite);
}

void LoadAttribute(ATTRIBUTES * pA, NMRecv & nmLoad)
{
	dword dwNum = nmLoad.GetDword_NoDbg();

	string sName = nmLoad.GetString_NoDbg();
	string sAttribute = nmLoad.GetString_NoDbg();
	pA->SetName(sName);
	pA->SetValue((sAttribute.Len()) ? (char *)(const char *)sAttribute : "");

	for (dword i=0; i<dwNum; i++)
		LoadAttribute(pA->CreateSubAClass(pA, "_x_y_z"), nmLoad);
}

bool CreateFolders(const char * pFolders)
{
	string sTmp, sFolders = pFolders; sFolders.CheckPath();
	for (dword i=0; i<sFolders.Len(); i++)
	{
		if (sFolders[i] == '\\')
			if (sTmp.Len()) ::CreateDirectory(sTmp, null);

		sTmp += sFolders[i];
	}

	return true;
}

dword _cdecl Net_SaveFile(VS_STACK * pS)
{
	string sFileName = ((VDATA*)pS->Pop())->GetString();
	VDATA * pRef = (VDATA*)pS->Pop();
	bool bServer = ((VDATA*)pS->Pop())->GetLong() != 0;
	ATTRIBUTES * pA = pRef->GetAClass();
	string sFullFileName = string("SAVE\\NetSaves\\") + string((bServer) ? "Server\\" : "Client\\") + sFileName;
	CreateFolders(sFullFileName);
	HANDLE hWrite = fio->_CreateFile(sFullFileName, GENERIC_WRITE, FILE_SHARE_WRITE, CREATE_ALWAYS);
	if (INVALID_HANDLE_VALUE != hWrite)
	{
		NMSend nmWrite;
		SaveAttribute(pA, nmWrite);
		fio->_WriteFile(hWrite, nmWrite.GetDataBuffer(), nmWrite.GetDataSize(), null);
		fio->_CloseHandle(hWrite);
	}
	return IFUNCRESULT_OK;
}

dword _cdecl Net_LoadFile(VS_STACK * pS)
{
	string sFileName = ((VDATA*)pS->Pop())->GetString();
	VDATA * pRef = (VDATA*)pS->Pop();
	bool bServer = ((VDATA*)pS->Pop())->GetLong() != 0;
	ATTRIBUTES * pA = pRef->GetAClass();
	pA->DeleteAttributeClassX(pA);
	string sFullFileName = string("SAVE\\NetSaves\\") + string((bServer) ? "Server\\" : "Client\\") + sFileName;
	char * pBuffer = null;
	dword dwSize = 0;
	if (fio->LoadFile(sFullFileName, &pBuffer, &dwSize))
	{
		NMRecv nmLoad(pBuffer, dwSize);
		LoadAttribute(pA, nmLoad);
		DELETE(pBuffer);
	}
	return IFUNCRESULT_OK;
}

dword _cdecl NFExist(VS_STACK * pS)
{
	string sFileName = ((VDATA*)pS->Pop())->GetString();
	((VDATA*)pS->Push())->Set((long)fio->FileExist(sFileName));
	return IFUNCRESULT_OK;
}

dword _cdecl NFRCreateFile(VS_STACK * pS)
{
	long iEventSubID = ((VDATA*)pS->Pop())->GetLong();
	long iEventID = ((VDATA*)pS->Pop())->GetLong();
	string sFileName = ((VDATA*)pS->Pop())->GetString();
	long iClientID = ((VDATA*)pS->Pop())->GetLong();
	
	NetFileRecv * pFile = NEW NetFileRecv(sFileName, iClientID, iEventID, iEventSubID);
	((VDATA*)pS->Push())->Set((long)pFile);
	return IFUNCRESULT_OK;
}

dword _cdecl NFSCreateFile(VS_STACK * pS)
{
	string sFileName = ((VDATA*)pS->Pop())->GetString();
	NetFileRecv * pRecvFile = (NetFileRecv*)((VDATA*)pS->Pop())->GetLong();
	long iClientID = ((VDATA*)pS->Pop())->GetLong();

	NetFileSend * pFile = NEW NetFileSend(sFileName, iClientID, pRecvFile);
	((VDATA*)pS->Push())->Set((long)pFile);
	return IFUNCRESULT_OK;
}

dword _cdecl NFSDataACK(VS_STACK * pS)
{
	long iIndex = ((VDATA*)pS->Pop())->GetLong();
	NetFileSend * pFile = (NetFileSend*)((VDATA*)pS->Pop())->GetLong();
	
	pFile->DataACK(iIndex);
	return IFUNCRESULT_OK;
}

dword _cdecl NFRReceiveData(VS_STACK * pS)
{
	NMRecv * pRecv = (NMRecv *)(((VDATA*)pS->Pop())->GetLong());
	long iIndex = ((VDATA*)pS->Pop())->GetLong();
	NetFileRecv * pFile = (NetFileRecv*)((VDATA*)pS->Pop())->GetLong();

	pFile->ReceiveData(iIndex, pRecv);
	return IFUNCRESULT_OK;
}

dword _cdecl NFRSetSendHandle(VS_STACK * pS)
{
	dword dwFileSize = (dword)((VDATA*)pS->Pop())->GetLong();
	NetFileSend * pSendFile = (NetFileSend*)((VDATA*)pS->Pop())->GetLong();
	NetFileRecv * pRecvFile = (NetFileRecv*)((VDATA*)pS->Pop())->GetLong();

	pRecvFile->SetFileInfo(pSendFile, dwFileSize);
	return IFUNCRESULT_OK;
}

dword _cdecl NFSReceiverReady(VS_STACK * pS)
{
	NetFileSend * pFile = (NetFileSend*)((VDATA*)pS->Pop())->GetLong();
	pFile->ReceiverReady();
	return IFUNCRESULT_OK;
}

dword _cdecl NFRIsEqualFile(VS_STACK * pS)
{
	dword dwSize = (dword)((VDATA*)pS->Pop())->GetLong();
	dword dwCRC32 = (dword)((VDATA*)pS->Pop())->GetLong();
	NetFileRecv * pFile = (NetFileRecv*)((VDATA*)pS->Pop())->GetLong();

	((VDATA*)pS->Push())->Set((long)pFile->IsEqualFile(dwCRC32, dwSize));
	return IFUNCRESULT_OK;
}

dword _cdecl NFSDeleteFile(VS_STACK * pS)
{
	NetFileSend * pFile = (NetFileSend*)((VDATA*)pS->Pop())->GetLong();
	DELETE(pFile);
	return IFUNCRESULT_OK;
}

dword _cdecl NFRDeleteFile(VS_STACK * pS)
{
	NetFileRecv * pFile = (NetFileRecv*)((VDATA*)pS->Pop())->GetLong();
	DELETE(pFile);
	return IFUNCRESULT_OK;
}

struct TextureFile
{
	TextureFile() { pTexture = null; iTextureNum = -1; }
	~TextureFile() { if (iTextureNum >= 0) NET_SCRIPT_LIBRIARY::pRS->TextureRelease(iTextureNum); RELEASE(pTexture); }

	long iTextureNum;
	IDirect3DTexture8 * pTexture;
	string sFilename, sGroupID;
};

array<TextureFile*> aTextureFiles(_FL_);

dword _cdecl NFDelGroupTextures(VS_STACK * pS)
{
	string sGroupID = ((VDATA*)pS->Pop())->GetString();
	
	for (long i=0; i<aTextureFiles.Len(); i++) if (aTextureFiles[i]->sGroupID == sGroupID)
	{
		DELETE(aTextureFiles[i]);
		aTextureFiles.ExtractNoShift(dword(i));
		i--;
	}
	return IFUNCRESULT_OK;
}

dword _cdecl NFDelTexture(VS_STACK * pS)
{
	string sFilename = ((VDATA*)pS->Pop())->GetString();
	string sGroupID = ((VDATA*)pS->Pop())->GetString();

	for (long i=0; i<aTextureFiles.Len(); i++) if (aTextureFiles[i]->sFilename == sFilename && aTextureFiles[i]->sGroupID == sGroupID)
	{
		DELETE(aTextureFiles[i]);
		aTextureFiles.ExtractNoShift(dword(i));
		i--;
	}
	return IFUNCRESULT_OK;
}

dword _cdecl NFGetTexture(VS_STACK * pS)
{
	string sFilename = ((VDATA*)pS->Pop())->GetString();
	string sGroupID = ((VDATA*)pS->Pop())->GetString();

	TextureFile * pTF = NEW TextureFile();
	pTF->sFilename = sFilename;
	pTF->sGroupID = sGroupID;

	pTF->iTextureNum = NET_SCRIPT_LIBRIARY::pRS->TextureCreate(sFilename);
	if (pTF->iTextureNum >= 0)
	{
		// if texture created
		pTF->pTexture = (IDirect3DTexture8*)NET_SCRIPT_LIBRIARY::pRS->GetBaseTexture(pTF->iTextureNum);
		((VDATA*)pS->Push())->Set((long)pTF->pTexture);
	}
	else
	{
		// else try to load tga file
		char * pBuffer = null;
		dword dwSize;
		if (fio->LoadFile(sFilename, &pBuffer, &dwSize))
		{
			pTF->pTexture = (IDirect3DTexture8*)NET_SCRIPT_LIBRIARY::pRS->CreateTextureFromFileInMemory(pBuffer, dwSize);
			((VDATA*)pS->Push())->Set((long)pTF->pTexture);

			/*TGA_H * pTga = (TGA_H*)pBuffer;
			if (pTga->bpp == 32)
			{
				dword dwWidth = pTga->width;
				dword dwHeight = pTga->height;
				NET_SCRIPT_LIBRIARY::pRS->CreateTexture(dwWidth, dwHeight, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &pTF->pTexture);
				D3DLOCKED_RECT lr;
				if (pTF->pTexture->LockRect(0, &lr, null, 0) == D3D_OK)
				{
					char * p32 = (char *)lr.pBits;
					for (dword y=0; y<dwHeight; y++)
						memcpy(&p32[(dwHeight - y - 1) * lr.Pitch], &pBuffer[sizeof(TGA_H) + y * dwWidth * 4], dwWidth * 4);
				}
				((VDATA*)pS->Push())->Set((long)pTF->pTexture);
			}
			if (pTga->bpp == 16)
			{
				dword dwWidth = pTga->width;
				dword dwHeight = pTga->height;
				NET_SCRIPT_LIBRIARY::pRS->CreateTexture(dwWidth, dwHeight, 1, 0, D3DFMT_A1R5G5B5, D3DPOOL_MANAGED, &pTF->pTexture);
				D3DLOCKED_RECT lr;
				if (pTF->pTexture->LockRect(0, &lr, null, 0) == D3D_OK)
				{
					char * p16 = (char *)lr.pBits;
					for (dword y=0; y<dwHeight; y++)
					{
						memcpy(&p16[(dwHeight - y - 1) * lr.Pitch], &pBuffer[sizeof(TGA_H) + y * dwWidth * 2], dwWidth * 2);
					}
				}
				((VDATA*)pS->Push())->Set((long)pTF->pTexture);
			}*/
			DELETE(pBuffer);
		}
		else
		{
			((VDATA*)pS->Push())->Set((long)0);
		}
	}
	return IFUNCRESULT_OK;
}

void NetTmp_FindWindowsFiles(ATTRIBUTES * pA, string sDirectory, const string & sMask, bool bRecursive)
{
	WIN32_FIND_DATA wfd;
	long iNumFiles = 0;
	string sNewDirectory;
	string sFileMask = sDirectory + "\\*.*";

	HANDLE hFind = ::FindFirstFile(sFileMask, &wfd);

	if (INVALID_HANDLE_VALUE == hFind) return;

	do {
		string sFileName = wfd.cFileName;

		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (sFileName == ".") continue;
			sNewDirectory = sDirectory + string("\\") + wfd.cFileName;
			if (sFileName != "..") continue;
		}
		else
			if (!sFileName.IsFileMask(sMask)) continue;

		bool bDot = sFileName == "." || sFileName == "..";
		bool bDir = (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;

		string sFullName = sDirectory + sFileName;
		if (!bDot && !bDir)
		{
			// add new file or directory record 
			ATTRIBUTES * pAF = pA->CreateSubAClass(pA, string().Format("f%d", pA->GetAttributesNum()));
			pAF->SetAttribute("FilePath", sFullName);
			pAF->SetAttribute("FileName", sFileName);
			if (string::EqualPostfix(sFileName, ".tga.tx"))
				pAF->SetAttribute("Name", sFileName - string(".tga.tx"));
			else
				pAF->SetAttribute("Name", string().GetFileTitle(sFileName));
			pAF->SetAttribute("Ext", string().GetFileExt(sFileName));
			//pA->SetAttribute(string().Format("f%d", pA->GetAttributesNum()), sFullName);
		}

		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (bRecursive && sFileName != "..")
				NetTmp_FindWindowsFiles(pA, sNewDirectory, sMask, bRecursive);
		}

	} while(::FindNextFile(hFind, &wfd));

	FindClose(hFind);
}


dword _cdecl NFFindFiles(VS_STACK * pS)
{
	bool bRecursive = ((VDATA*)pS->Pop())->GetLong() != 0;
	string sMask = ((VDATA*)pS->Pop())->GetString();
	string sDirectory = ((VDATA*)pS->Pop())->GetString();
	ATTRIBUTES * pA = ((VDATA*)((VDATA*)pS->Pop())->GetReference())->GetAClass();
	// Clear any attributes
	pA->DeleteAttributeClassX(pA);
	// Find Files
	NetTmp_FindWindowsFiles(pA, sDirectory, sMask, bRecursive);
	((VDATA*)pS->Push())->Set((long)pA->GetAttributesNum());
	return IFUNCRESULT_OK;
}


dword _cdecl NetIni_GetLong(VS_STACK * pS)
{
	string sValue = ((VDATA*)pS->Pop())->GetString();
	string sSection = ((VDATA*)pS->Pop())->GetString();
	string sIniFile = ((VDATA*)pS->Pop())->GetString();

	INIFILE * ini = fio->OpenIniFile(sIniFile);
	long iLongValue = (ini) ? ini->GetLong((char*)sSection.c_str(), (char*)sValue.c_str()) : 0;
	((VDATA*)pS->Push())->Set(iLongValue);
	delete ini;

	return IFUNCRESULT_OK;
}

dword _cdecl NetIni_GetString(VS_STACK * pS)
{
	char cBuffer[1024]; cBuffer[0] = 0;

	string sValue = ((VDATA*)pS->Pop())->GetString();
	string sSection = ((VDATA*)pS->Pop())->GetString();
	string sIniFile = ((VDATA*)pS->Pop())->GetString();

	INIFILE * ini = fio->OpenIniFile(sIniFile);
	ini->ReadString((char*)sSection.c_str(), (char*)sValue.c_str(), cBuffer, sizeof(cBuffer));
	delete ini;

	((VDATA*)pS->Push())->Set(cBuffer);
	return IFUNCRESULT_OK;
}

struct sort2i
{
	long iIndex;
	long iValue1, iValue2;

	static bool QSort1(const sort2i & s1, const sort2i & s2) 
	{ 
		if (s1.iValue1 > s2.iValue1) return true;
		if (s1.iValue1 == s2.iValue1 && s1.iValue2 > s2.iValue2) return true;
		return false;
	}
	static bool QSort2(const sort2i & s1, const sort2i & s2) { return !QSort1(s1, s2); }
};

dword _cdecl Net_Sort2i(VS_STACK * pS)
{ 
	string sValue2 = ((VDATA*)pS->Pop())->GetString();
	string sValue1 = ((VDATA*)pS->Pop())->GetString();
	long iMaxOutRecords = ((VDATA*)pS->Pop())->GetLong();
	long iDirection = ((VDATA*)pS->Pop())->GetLong();
	VDATA * pOutMassive = (VDATA*)pS->Pop();
	VDATA * pInMassive = (VDATA*)pS->Pop();

	ATTRIBUTES * pAInMassive = pInMassive->GetAClass();
	long iNumAttributes = pAInMassive->GetAttributesNum();

	long * pSorted = new long[iNumAttributes];
	long * pValues1 = new long[iNumAttributes];
	long * pValues2 = new long[iNumAttributes];

	array<sort2i> aSorted(_FL_, 128);
	aSorted.DelAll();

	for (long i=0; i<iNumAttributes; i++)
	{
		sort2i & s2i = aSorted[aSorted.Add()];
		
		s2i.iIndex = i;
		s2i.iValue1 = pAInMassive->GetAttributeClass(i)->GetAttributeAsDword(sValue1);
		s2i.iValue2 = pAInMassive->GetAttributeClass(i)->GetAttributeAsDword(sValue2);
	}

	aSorted.QSort((iDirection) ? sort2i::QSort1 : sort2i::QSort2);

	for (long i=0; i<iNumAttributes, i<iMaxOutRecords; i++)
	{
		pOutMassive->Set(aSorted[i].iIndex, i);
	}
	aSorted.DelAll();

	return IFUNCRESULT_OK;
}
