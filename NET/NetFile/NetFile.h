#ifndef NETFILE_HPP
#define NETFILE_HPP

#include "..\common.h"

class NetFileRecv;
class NetFileSend;

class NetFileRecv
{
private:
	string sFileName;
	char * pBuffer;
	dword dwFileSize;
	long iClientID, iEventID, iEventSubID;
	NetFileSend * pSendHandle;

public:
	static array<NetFileRecv*> aNFRecv;

	NetFileRecv(const string & sFileName, long iClientID, long iEventID, long iEventSubID);
	~NetFileRecv();

	void SetFileInfo(NetFileSend * pSendHandle, dword dwFileSize);
	bool IsEqualFile(dword dwExternCRC32, dword dwExternSize);
	void ReceiveData(long iIndex, NMRecv * pRecv);
	void SaveFile();
};

class NetFileSend
{
private:
	string sFileName;
	char * pBuffer;
	dword dwFileSize;

	long iClientID;
	NetFileRecv * pRecvHandle;

	long iCurrentIndex;
	float fLoadedCheckTime;
	bool bLoaded, bCanSend;

public:
	static array<NetFileSend*> aNFSend;

	NetFileSend(const string & sFileName, long iClientID, NetFileRecv * pRecvHandle);
	~NetFileSend();

	void DataACK(long iIndex);
	void SendData(long iIndex);
	void Execute(float fDeltaTime);
	void ReceiverReady();
};

#endif