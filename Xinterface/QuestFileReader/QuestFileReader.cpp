#include <assert.h>
#include "QuestFileReader.h"


#define INVALID_LONG 0

#define TOKEN_INVALID	-1
#define TOKEN_VOID		0
#define TOKEN_QUEST		1
#define TOKEN_QTEXT		2
#define TOKEN_UNKNOWN	1000

#define TOKENTABLE_SIZE	2
static struct _TOKENTABLE {const char * token; int cod;} TokenTable[] =
{
	{"#QUEST",TOKEN_QUEST},
	{"#TEXT",TOKEN_QTEXT}
};

bool IS_DIGIT(char ch);
long GET_DIGIT(char ch);
long GetToken(const char* &ps);
static long GetLongFromString(char* &pInStr);
static void GetSubStringFromString(const char* &pInStr,char* pOutBuf,int bufSize);
static const char* GetNextString(const char* &pInStr);
static char * GetTitleString(char* buf, char* &ptr, int &slen);




#define DELETE_PTR(p) {if(p!=NULL) delete p; p=NULL;}








long GetToken(const char* &ps)
{
	if(ps==NULL) return TOKEN_INVALID;

	// удаляем предшествующие пробелы и символы табуляции
	while(*ps==32 || *ps==9) ps++;

	// получаем размер лексемы
	const char *ptoken = ps;
	while(*ps!=0 && (unsigned)*ps>0x20 && *ps!=0x0D && *ps!=0x0A)
		ps++;
	size_t tokensize = (long)ps-(long)ptoken;
	if(tokensize==0) return TOKEN_VOID;

	for(int i=0; i<TOKENTABLE_SIZE; i++)
		if( strncmp(TokenTable[i].token, ptoken, tokensize) == 0 )
			return TokenTable[i].cod;

	// проверим лексему на соответсвие определенным значениям
	return TOKEN_UNKNOWN;
}







// Функции работы с символами
bool IS_SPACE(char ch)
{
	if((unsigned)ch<=' ') return true;
	if(ch==0x0D || ch==0x0A) return true;
	return false;
}
bool IS_DIGIT(char ch)
{
	if(ch>='0' && ch<='9') return true;
	return false;
}
long GET_DIGIT(char ch)
{
	return long(ch-'0');
}








// Функции работы со строками
static long GetLongFromString(char* &pInStr)
{
	if(pInStr==NULL) return INVALID_LONG;

	// удалим лишние пробелы
	while(*pInStr<=' ') pInStr++;
	if( !IS_DIGIT(*pInStr) )
		return INVALID_LONG;

	int retVal=0;
	while(IS_DIGIT(*pInStr))
	{
		retVal=retVal*10+GET_DIGIT(*pInStr);
		pInStr++;
	}
	return retVal;
}
void GetSubStringFromString(const char* &pInStr,char* pOutBuf,int bufSize)
{
	if(bufSize<=0) return;
	// удаляем предшествующие пробелы
	while(IS_SPACE(*pInStr)) pInStr++;
	// перепишем строку в буфер
	for(int i=0;i<bufSize-1;i++,pInStr++)
	{
		if(IS_SPACE(*pInStr)) break;
		pOutBuf[i] = *pInStr;
	}
	pOutBuf[i]=0;
}

static const char* GetNextString(const char* &pInStr)
{
	if(pInStr==NULL || pInStr[0]==0) return NULL;

	while(true)
	{
		bool bYesCurierReturn = false;
		bool bEmptyString = true;
		for(const char* pstart=pInStr; *pInStr!=0; pInStr++)
		{
			if(IS_SPACE(*pInStr))	bEmptyString=false;
			if(*pInStr==0x0D) bYesCurierReturn = true;
			else if(*pInStr==0x0A) bYesCurierReturn = true;
			else if(bYesCurierReturn) break;
		}
		if(*pInStr!=0 && bEmptyString) continue;
		return pstart;
	}

	return NULL;
}

static char * GetTitleString(char* buf, char* &ptr, int &slen)
{
	if(buf==NULL) {slen=0;return NULL;}
	buf[0]=0;
	char* startp = ptr;
	while(ptr!=NULL)
	{
		// Возмем очередную строку
		const char* cstr = GetNextString(ptr);
		if(ptr!=cstr && cstr!=NULL)
		{
			// если полученная строка является заголовком квеста
			const char *tmpstr = cstr;
			int tokType = GetToken(tmpstr);
			if(tokType==TOKEN_QUEST)
			{
				// получим id этого квеста
				GetSubStringFromString(tmpstr,buf,256);
				char * retVal = ptr;
				// найдем конец заголовка квеста
				while(ptr!=NULL)
				{
					tmpstr = ptr;
					tokType = GetToken(tmpstr);
					if(tokType==TOKEN_QTEXT || tokType==TOKEN_QUEST) break;
					GetNextString(ptr);
					if(tmpstr==ptr) break;
				}
				if(ptr!=tmpstr)
					slen = (long)ptr-(long)retVal;
				else
					if(retVal!=NULL)
						slen = strlen(retVal);
					else
						slen = 0;
				if(retVal!=NULL)
				return retVal;
			}
		}
		else break;
	}
	buf[0]=0; slen=0;
	return NULL;
}











QUEST_FILE_READER::QUEST_FILE_READER() :
	m_aQuest(_FL),
	m_aQuestData(_FL),
	m_aQuestFileName(_FL)
{
	m_pFileBuf = null;
}

QUEST_FILE_READER::~QUEST_FILE_READER()
{
	CloseQuestsQuery();
}

bool QUEST_FILE_READER::InitQuestsQuery()
{
	CloseQuestsQuery();

	if( m_aQuest.Size() == 0 )
	{
		for( long n=0; n<m_aQuestFileName; n++ )
		{
			HANDLE hfile = api->fio->_CreateFile( m_aQuestFileName[n], GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING );
			if(hfile==INVALID_HANDLE_VALUE)
			{
				api->Trace( "WARNING! Can`t open quest log file %s", m_aQuestFileName[n].GetBuffer() );
				continue;
			}

			DWORD filesize = api->fio->_GetFileSize( hfile, 0 );
			if( filesize == 0 )
			{
				api->Trace( "Empty quest log file %s", m_aQuestFileName[n].GetBuffer() );
				api->fio->_CloseHandle( hfile );
				continue;
			}

			long foffset = 0;
			if( m_pFileBuf == null )
			{
				m_pFileBuf = NEW char[filesize+1];
			}
			else
			{
				foffset = strlen( m_pFileBuf );
				m_pFileBuf = (char*)RESIZE( m_pFileBuf, foffset+filesize+1 );
			}

			if( m_pFileBuf == null ) {
				_THROW("allocate memory error");
			}

			DWORD readsize;
			if( api->fio->_ReadFile( hfile, &m_pFileBuf[foffset], filesize, &readsize ) == FALSE ||
				readsize != filesize )
			{
				api->Trace( "Can`t read quest log file: %s", m_aQuestFileName[n].GetBuffer() );
			}
			api->fio->_CloseHandle( hfile );
			m_pFileBuf[foffset+readsize] = 0;
		}
	}

	if( m_pFileBuf==null || m_pFileBuf[0]==0 ) return false;
	return true;
}

void QUEST_FILE_READER::CloseQuestsQuery()
{
	DELETE( m_pFileBuf );
	m_aQuestData.DelAll();
	m_sCurQuestTitle.Empty();
}

bool QUEST_FILE_READER::GetQuestTitle(const char* questId, const char* questUniqueID, size_t buffSize,char * buffer)
{
	if( !buffer || buffSize==0 || !questId ) return false;

	ReadUserData( questUniqueID, -1 );

	buffer[0]=0;

	long n = FindQuestByID( questId );
	if( n < 0 )
	{
		api->Trace("WARNING! Can`t find title whith ID = %s",questId);
		return false;
	}

	AssembleStringToBuffer( m_aQuest[n].sTitle,m_aQuest[n].sTitle.Len(), buffer,buffSize,m_aQuestData);
	return true;
}

void QUEST_FILE_READER::GetRecordTextList(array<string> & asStringList, const char* pcQuestID,const char* pcTextID, const char* pcUserData)
{
	long nQuestIndex = FindQuestByID( pcQuestID );
	long nTextIndex = FindTextByID( nQuestIndex, pcTextID );
	if( nQuestIndex<0 || nTextIndex<0 ) return;

	m_sCurQuestTitle = "";
	m_aQuestData.DelAll();
	FillUserDataList( (char*)pcUserData,m_aQuestData );

	char pcTmp[10240]; // boal 4096
	AssembleStringToBuffer( m_aQuest[nQuestIndex].aText[nTextIndex].str,
		m_aQuest[nQuestIndex].aText[nTextIndex].str.Len(), pcTmp, sizeof(pcTmp),m_aQuestData );
	asStringList.Add(pcTmp);
}

bool QUEST_FILE_READER::AssembleStringToBuffer(const char* pSrc, long nSrcSize, char* pBuf, long nBufSize,array<UserData>& aUserData)
{
	if(!pSrc || !pBuf || nBufSize<1) return false;

	char insertID[256];
	long nSrc,nDst, nIns;
	bool bMakeID = false;

	for(nSrc=0,nDst=0; nSrc<nSrcSize && nDst<nBufSize-1; nSrc++)
	{
		if( pSrc[nSrc]==0 ) break;
		if( bMakeID ) {
			// создание ID для текстовой вставки
			if( pSrc[nSrc]=='>' ) {
				// завершение ID
				insertID[nIns]=0;
				nDst += AddToBuff(&pBuf[nDst], nBufSize-nDst, GetInsertStringByID(insertID,aUserData) );
				bMakeID = false;
			} else {
				insertID[nIns] = pSrc[nSrc];
				nIns++;
			}
		} else {
			// копирование
			if( pSrc[nSrc]=='@' && pSrc[nSrc+1]=='<' ) {
				nSrc++;
				bMakeID = true;
				nIns = 0;
			} else {
				pBuf[nDst] = pSrc[nSrc];
				nDst++;
			}
		}
	}
	pBuf[nDst] = 0;

	return true;
}

const char* QUEST_FILE_READER::GetInsertStringByID(char* pID,array<UserData>& aUserData)
{
	if(!pID) return 0;
	for(long n=0; n<aUserData; n++)
		if( aUserData[n].id == pID )
			return aUserData[n].str.GetBuffer();
	return 0;
}

long QUEST_FILE_READER::AddToBuff(const char* pDst, long nDstSize, const char* pSrc, long nSrcSize)
{
	if(!pSrc || !pDst) return 0;
	if( nSrcSize<0 ) nSrcSize = strlen(pSrc);
	if( nSrcSize>nDstSize-1 ) nSrcSize = nDstSize-1;
	if( nSrcSize>0 ) strncpy((char*)pDst, pSrc, nSrcSize);
	return nSrcSize;
}

void QUEST_FILE_READER::ReadUserData(const char * sQuestName,long nRecordIndex)
{
	if( m_sCurQuestTitle == sQuestName ) return; // уже установлено для этого

	m_sCurQuestTitle = sQuestName;
	m_aQuestData.DelAll();

	if( !sQuestName ) return;

	VDATA * pVD = api->Event( "evntQuestUserData", "sl", sQuestName,nRecordIndex );
	if( !pVD ) return;
	char * pStr = pVD->GetString();
	if( !pStr || pStr[0]==0 ) return;

	FillUserDataList( pStr,m_aQuestData );
}

void QUEST_FILE_READER::FillUserDataList(char * sStrData,array<UserData>& aUserData)
{
	if(!sStrData) return;

	int n,q, nBegID,nLenID, nBegStr,nLenStr;

	q = strlen(sStrData);

	bool bYesID = false;
	bool bGetID = false;

	for(n=0; n<q; n++)
	{
		if(bGetID)
		{
			if( sStrData[n]=='>' )
			{
				nLenID = n-nBegID;
				bYesID = true;
				bGetID = false;
				nBegStr = n+1;
			}
		}
		else
		{
			if( sStrData[n]=='@' && sStrData[n+1]=='<' )
			{
				if(bYesID)
				{
					nLenStr = n-nBegStr;
					bYesID = false;

					// write info
					long i = aUserData.Add();
					WriteToString( aUserData[i].id, &sStrData[nBegID], &sStrData[nBegID+nLenID] );
					WriteToString( aUserData[i].str, &sStrData[nBegStr], &sStrData[nBegStr+nLenStr] );
				}
				nBegID = n+2;
				bGetID = true;
				n++;
			}
		}
	}

	if( bYesID )
	{
		nLenStr = n-nBegStr;
		// write info
		long i = aUserData.Add();
		WriteToString( aUserData[i].id, &sStrData[nBegID], &sStrData[nBegID+nLenID] );
		WriteToString( aUserData[i].str, &sStrData[nBegStr], &sStrData[nBegStr+nLenStr] );
	}
}

void QUEST_FILE_READER::SetQuestTextFileName(const char * pcFileName)
{
	if( pcFileName == null || pcFileName[0] == 0 ) return;

	// check for already included:
	for( long n=0; n<m_aQuestFileName; n++ )
		if( m_aQuestFileName[n] == pcFileName )
			return;

	m_aQuestFileName.Add( string(pcFileName) );

	// открываем этот файл
	HANDLE hfile = api->fio->_CreateFile( pcFileName, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING );
	if(hfile==INVALID_HANDLE_VALUE)
	{
		api->Trace( "WARNING! Can`t open quest log file %s", pcFileName );
		return;
	}
	// его размер
	DWORD filesize = api->fio->_GetFileSize( hfile, 0 );
	if( filesize == 0 )
	{
		api->Trace( "Empty quest log file %s", pcFileName );
		api->fio->_CloseHandle( hfile );
		return;
	}
	// создаем буфер для него
	char* pBuf = NEW char[filesize+1];
	Assert( pBuf );
	// читаем в этот буфер из файла
	DWORD readsize;
	if( api->fio->_ReadFile( hfile, pBuf, filesize, &readsize ) == FALSE ||
		readsize != filesize )
	{
		api->Trace( "Can`t read quest log file: %s", pcFileName );
	}
	api->fio->_CloseHandle( hfile );
	pBuf[readsize] = 0;

	AddQuestFromBuffer( pBuf );
	delete pBuf;
}

void QUEST_FILE_READER::AddQuestFromBuffer( const char* pcSrcBuffer )
{
	if( !pcSrcBuffer ) return;

	const char* pcStr = pcSrcBuffer;
	string sQuestID;
	string sTextID;
	string sQuestText;
	string sTextText;
	const char* pcPrev = pcStr;
	char tmp[1024];
	long nCurToken = TOKEN_UNKNOWN;

	while( pcStr )
	{
		if( !pcStr[0] ) { // проверка на завершение
			if( nCurToken == TOKEN_QTEXT ) {
				WriteToString( sTextText, pcPrev, pcStr );
				AddToQuestList( sQuestID, sTextID, sQuestText, sTextText );
			}
			break;
		}
		const char* pcLine = GetNextString( pcStr );
		const char* pcToken = pcLine;
		long nToken = GetToken( pcToken );
		switch( nToken )
		{
		case TOKEN_QUEST:
			{
				if( nCurToken == TOKEN_QTEXT ) {
					WriteToString( sTextText, pcPrev, pcLine );
					AddToQuestList( sQuestID, sTextID, sQuestText, sTextText );
				}

				GetSubStringFromString( pcToken, tmp, sizeof(tmp) );
				sQuestID = tmp;
				sTextID.Empty();
				sQuestText.Empty();
				sTextText.Empty();
				pcPrev = pcStr;
				nCurToken = TOKEN_QUEST;
			}
			break;

		case TOKEN_QTEXT:
			{
				if( nCurToken == TOKEN_QTEXT ) {
					WriteToString( sTextText, pcPrev, pcLine );
					AddToQuestList( sQuestID, sTextID, sQuestText, sTextText );
				}
				if( nCurToken == TOKEN_QUEST ) {
					WriteToString( sQuestText, pcPrev, pcLine );
				}
				GetSubStringFromString( pcToken, tmp, sizeof(tmp) );
				sTextID = tmp;
				sTextText.Empty();
				pcPrev = pcStr;
				nCurToken = TOKEN_QTEXT;
			}
			break;
		}
	}
}

void QUEST_FILE_READER::WriteToString( string& sDst, const char* pcStart, const char* pcEnd )
{
	if( !pcEnd || !pcStart )
		{sDst = ""; return;}
	/*while( pcStart[0]!=0 && (*(UCHAR*)pcStart<=0x20) )
		pcStart++;
	while( pcEnd>pcStart && (*(UCHAR*)(pcEnd-1)<=0x20) )
		pcEnd--;   */ // boal нефиг удалать пробелы в начале строки!!!

	char chTmp = *pcEnd;
	*(char*)pcEnd = 0;
	sDst = pcStart;
	*(char*)pcEnd = chTmp;
}

void QUEST_FILE_READER::AddToQuestList( string& sQuestID, string& sTextID, string& sQuestText, string& sTextText )
{
	if( sQuestID.IsEmpty() || sTextID.IsEmpty() ) return;
	long q = FindQuestByID( sQuestID );
	if( q < 0 )
	{
		q = m_aQuest.Add();
		m_aQuest[q].sHeader = sQuestID;
		m_aQuest[q].sTitle = sQuestText;
	}

	long t = FindTextByID( q, sTextID );
	if( t < 0 )
	{
		t = m_aQuest[q].aText.Add();
		m_aQuest[q].aText[t].id = sTextID;
		m_aQuest[q].aText[t].str = sTextText;
	}
}

long QUEST_FILE_READER::FindQuestByID( const char* pcQuestID )
{
	for( long n=0; n<m_aQuest; n++ )
		if( m_aQuest[n].sHeader == pcQuestID )
			return n;
	return -1;
}

long QUEST_FILE_READER::FindTextByID( long nQuest, const char* pcTextID )
{
	if( nQuest < 0 || nQuest >= m_aQuest ) return -1;
	for( long n=0; n<m_aQuest[nQuest].aText; n++ )
		if( m_aQuest[nQuest].aText[n].id == pcTextID )
			return n;
	return -1;
}