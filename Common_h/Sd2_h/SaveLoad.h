#ifndef SAVE_LOAD_HPP
#define SAVE_LOAD_HPP

#include "..\character.h"

class CSaveLoad
{
private:
	char	* pSaveBuffer;
	dword	dwCurSize, dwMaxSize;
	bool	bSave, bLoad;

public:
	CSaveLoad() 
	{
		dwCurSize = 0;
		dwMaxSize = 0;
		bSave = bLoad = false;
		pSaveBuffer = null;
	}

	~CSaveLoad() 
	{
		Close();
	}

	void Close()
	{
		if (bSave)
		{
			char * pFFSave = NEW char[dwCurSize * 2 + 8 + 1];
			sprintf(pFFSave, "%.8x", dwCurSize);
			for (dword i=0; i<dwCurSize; i++)
				sprintf(&pFFSave[8 + i * 2], "%.2x", byte(pSaveBuffer[i]));

			VDATA * pV = api->Event("SeaLoad_GetPointer", "sl", "seasave", -1);
			if (pV)
				pV->GetAClass()->SetAttribute("save", pFFSave);

			DELETE(pFFSave);
		}
		DELETE(pSaveBuffer);
	}

	void CreateWrite()
	{
		pSaveBuffer = null;
		dwCurSize = 0;
		dwMaxSize = 0;
		bSave = true;
		bLoad = false;
	}

	void CreateLoad()
	{
		bSave = false;
		bLoad = true;
		dwCurSize = 0;

		VDATA * pV = api->Event("SeaLoad_GetPointer", "sl", "seasave", -1);
		char * pSave = pV->GetAClass()->GetAttribute("save");
		dword dwSize;
		char str[256];
		strncpy(str, pSave, 8);
		str[8] = 0;
		sscanf(str, "%x", &dwSize);
		pSaveBuffer = NEW char[dwSize];
		for (dword i=0; i<dwSize; i++)
		{
			strncpy(str, &pSave[8 + i * 2], 2); str[2] = 0;
			dword dwValue;
			sscanf(str, "%x", &dwValue);
			pSaveBuffer[i] = char(dwValue);
		}
	}

	void Write(const void * pBuffer, dword dwSize)
	{
		if (dwMaxSize <= dwCurSize + dwSize)
		{
			dwMaxSize = 2048 * ((dwCurSize + dwSize + 2048) / 2048);
			pSaveBuffer = (char *)RESIZE(pSaveBuffer, dwMaxSize);
		}
		memcpy(&pSaveBuffer[dwCurSize], pBuffer, dwSize);
		dwCurSize += dwSize;
	}

	void Read(void * pBuffer, dword dwSize)
	{
		memcpy(pBuffer, &pSaveBuffer[dwCurSize], dwSize);
		dwCurSize += dwSize;
	}

// =======================================================================================
// Save functions
// =======================================================================================

	void SaveDword(dword dwValue)
	{
		Write(&dwValue, sizeof(dwValue));
	}

	void SaveFloat(float fValue)
	{
		Write(&fValue, sizeof(fValue));
	}

	void SaveLong(long iValue)
	{
		Write(&iValue, sizeof(iValue));
	}

	void SaveString(string & str)
	{
		if (str.Len())
		{
			dword dwLen = str.Len() + 1;
			SaveDword(dwLen);
			Write(str.GetBuffer(), dwLen);
		}
		else
		{
			dword dwLen = 0;
			SaveDword(dwLen);
		}
	}

	void SaveVector(const CVECTOR & vVector)
	{
		Write((void*)&vVector, sizeof(vVector));
	}

	void SaveBuffer(const char * pBuffer, dword dwSize)
	{
		SaveDword(dwSize);
		Write((void*)pBuffer, dwSize);
	}

	void SaveAPointer(const char * pStr, ATTRIBUTES * pAttribute)
	{
		long iIndex = -1;
		if (pAttribute)
		{
			iIndex = (long)pAttribute->GetAttributeAsDword("index", -1);
		}
		SaveLong(iIndex);
		SaveString(string(pStr));
	}

// =======================================================================================
// Load functions
// =======================================================================================

	dword LoadDword()
	{
		dword dwValue;
		Read(&dwValue, sizeof(dwValue));
		return dwValue;
	}

	float LoadFloat()
	{
		float fValue;
		Read(&fValue, sizeof(fValue));
		return fValue;
	}

	long LoadLong()
	{
		long iValue;
		Read(&iValue, sizeof(iValue));
		return iValue;
	}

	string LoadString()
	{
		string str;
		dword dwLen;
		Read(&dwLen, sizeof(dwLen));
		if (dwLen == 0) return string();
		char * pBuffer = NEW char[dwLen];
		Read(pBuffer, dwLen);
		str = pBuffer;
		delete pBuffer;
		return str;
	}

	CVECTOR LoadVector()
	{
		CVECTOR v;
		Read(&v, sizeof(v));
		return v;
	}

	void LoadBuffer(char * * pBuffer)
	{
		dword dwSize;
		Read(&dwSize, sizeof(dwSize));
		*pBuffer = NEW char[dwSize];
		Read(*pBuffer, dwSize);
	}

	void Load2Buffer(char * pBuffer)
	{
		dword dwSize;
		Read(&dwSize, sizeof(dwSize));
		Read(pBuffer, dwSize);
	}

	ATTRIBUTES * LoadAPointer(const char * pStr)
	{
		long iIndex = LoadLong();
		string str = LoadString();
		if (str == "character" && iIndex < 0) return null;
		VDATA * pV = api->Event("SeaLoad_GetPointer", "sl", pStr, iIndex);
		return (pV) ? pV->GetAClass() : null;
	}
};

#endif