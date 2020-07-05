#include "NetFile.h"
#include "..\CRC.h"

array<NetFileRecv*> NetFileRecv::aNFRecv(_FL_);

NetFileRecv::NetFileRecv(const string & sFileName, long iClientID, long iEventID, long iEventSubID)
{
	this->sFileName = sFileName;
	this->iClientID = iClientID;

	this->pBuffer = null;
	this->dwFileSize = 0;

	this->iEventID = iEventID;
	this->iEventSubID = iEventSubID;

	aNFRecv.Add(this);
}

NetFileRecv::~NetFileRecv()
{
	DELETE(pBuffer);
	aNFRecv.Del(this);
}

void NetFileRecv::SetFileInfo(NetFileSend * pSendHandle, dword dwFileSize)
{
	this->pSendHandle = pSendHandle;
	this->dwFileSize = dwFileSize;

	pBuffer = NEW char[dwFileSize];
}

void NetFileRecv::ReceiveData(long iIndex, NMRecv * pRecv)
{
	dword dwDataSize;
	char * pData = pRecv->GetData(dwDataSize);
	memcpy(&pBuffer[iIndex * 256], pData, dwDataSize);

	if (iIndex == (long)dwFileSize / 256)
	{
		SaveFile();
		delete this;
		return;
	}
}

void NetFileRecv::SaveFile()
{
	bool bSuccess = false;
	HANDLE hFile = fio->_CreateFile(sFileName, GENERIC_WRITE, FILE_SHARE_WRITE, CREATE_ALWAYS);

	if (INVALID_HANDLE_VALUE != hFile)
	{
		if (pBuffer)
		{
			fio->_WriteFile(hFile, pBuffer, dwFileSize, null);
			bSuccess = true;
		}
		else
			api->Trace("Can't create downloaded file: %s, because buffer is null", sFileName.GetBuffer());

		fio->_CloseHandle(hFile);
	}
	else
		api->Trace("Can't create downloaded file: %s", sFileName.GetBuffer());

	if (bSuccess)
	{
		api->Trace("Successfully downloaded file: %s", sFileName.GetBuffer());
		api->Event("NetFileRecv_Done", "lll", iClientID, iEventID, iEventSubID);
	}
}

bool NetFileRecv::IsEqualFile(dword dwExternCRC32, dword dwExternSize)
{
	if (fio->LoadFile(sFileName, &pBuffer, &dwFileSize))
	{
		bool bRes = (dwFileSize == dwExternSize && CRC32::GetCRC32(pBuffer, dwFileSize) == dwExternCRC32);
		dwFileSize = 0;
		DELETE(pBuffer);
		return bRes;
	}

	return false;
}
