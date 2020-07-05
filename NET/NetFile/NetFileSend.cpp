#include "NetFile.h"
#include "..\CRC.h"

array<NetFileSend*> NetFileSend::aNFSend(_FL_);

NetFileSend::NetFileSend(const string & sFileName, long iClientID, NetFileRecv * pRecvHandle)
{
	this->pRecvHandle = pRecvHandle;
	this->sFileName = sFileName;
	this->iClientID = iClientID;
	pBuffer = null;
	dwFileSize = null;
	bLoaded = false;
	bCanSend = false;
	fLoadedCheckTime = 0.0f;
	iCurrentIndex = -1;

	aNFSend.Add(this);
}

NetFileSend::~NetFileSend()
{
	aNFSend.Del(this);
	DELETE(pBuffer);
}

void NetFileSend::DataACK(long iIndex)
{
	if (iCurrentIndex == iIndex)
	{
		if (iCurrentIndex == (long)dwFileSize / 256)
		{
			delete this;
			return;
		}
		bCanSend = true;
		// check for last result
		iCurrentIndex++;
	}
	else
		api->Trace("Invalid NetFileSend result received, need %d, received %d", iCurrentIndex, iIndex);
}

void NetFileSend::SendData(long iIndex)
{
	bCanSend = false;

	dword dwSendSize = (dwFileSize / 256 == iIndex) ? dwFileSize - iIndex * 256 : 256;
	api->Event("NetFileSend_SendData", "llllll", iClientID, (long)this, (long)pRecvHandle, iIndex, (long)&pBuffer[iIndex * 256], dwSendSize);
}

void NetFileSend::ReceiverReady()
{
	bCanSend = true;
}

void NetFileSend::Execute(float fDeltaTime)
{
	if (!bLoaded)
	{
		fLoadedCheckTime -= fDeltaTime; if (fLoadedCheckTime > 0.0f) return;
		fLoadedCheckTime = 2.0f;
		if (fio->LoadFile(sFileName, &pBuffer, &dwFileSize))
		{
			api->Event("NetFileSend_TransferReady", "lllll", iClientID, (long)this, (long)pRecvHandle, CRC32::GetCRC32(pBuffer, dwFileSize), dwFileSize);
			bLoaded = true;
			iCurrentIndex = 0;
		}
		else return;
	}

	if (bCanSend)
		SendData(iCurrentIndex);
}