#ifndef _QUEST_FILE_READER_H_
#define _QUEST_FILE_READER_H_

#include "..\..\common_h\vmodule_api.h"
#include "..\templates\ctext.h"
#include "..\templates\TDynArray.h"
#include "..\..\common_h\templates\array.h"
#include "..\..\common_h\templates\string.h"

class QUEST_FILE_READER
{
public:
	struct UserData
	{
		string	id;
		string	str;
	};

	struct QuestDescribe
	{
		struct TextDescribe
		{
			string id;
			string str;
		};

		string sHeader;
		string sTitle;
		array<TextDescribe> aText;
		QuestDescribe() : aText(_FL) {}
	};

protected:
	array<string> m_aQuestFileName;
	char* m_pFileBuf;

	array<QuestDescribe> m_aQuest;

	string m_sCurQuestTitle;
	array<UserData> m_aQuestData;

	long m_nDefQuestIndex;
	long m_nCurQuestIndex;
	long m_nCurTextIndex;
	long m_nCurLineIndex;

public:
	QUEST_FILE_READER();
	~QUEST_FILE_READER();

	void SetQuestTextFileName(const char * pcFileName);
	bool InitQuestsQuery();
	void CloseQuestsQuery();

	bool GetQuestTitle(const char* questId, const char* questUniqueID, size_t buffSize, char * buffer);

	void GetRecordTextList(array<string> & asStringList, const char* pcQuestID,const char* pcTextID, const char* pcUserData);

	static bool AssembleStringToBuffer(const char* pSrc, long nSrcSize, char* pBuf, long nBufSize,array<UserData>& aUserData);
	static const char* GetInsertStringByID(char* pID,array<UserData>& aUserData);
	static long AddToBuff(const char* pDst, long nDstSize, const char* pSrc, long nSrcSize = -1);
	static void WriteToString( string& sDst, const char* pcStart, const char* pcEnd );
	static void FillUserDataList(char * sStrData,array<UserData>& aUserData);

private:
	void ReadUserData(const char * sQuestName,long nRecordIndex);

	void AddQuestFromBuffer( const char* pcSrcBuffer );
	void AddToQuestList( string& sQuestID, string& sTextID, string& sQuestText, string& sTextText );
	long FindQuestByID( const char* pcQuestID );
	long FindTextByID( long nQuest, const char* pcTextID );
};

#endif
