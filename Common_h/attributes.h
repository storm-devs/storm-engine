#ifndef _ATTRIBUTES_H_
#define _ATTRIBUTES_H_

#include <stdio.h>
#include "memop.h"
#include "vstring_codec.h"

#define INVALID_ATTRIBUTE_HANDLE	0xffffffff
//--------------------------------------------------------------------------------------------------------------
// Each attribute is a class with Name and Attribute string values. Class can contain list of ... later
//
// GetAttributeClass(attribute_name) - return class ATTRIBUTES pointer for specified attribute
// GetAttribute(attribute_name) - return string value of specified attribute
// SetAttribute(attribute_name,attribute) - create or modify attribute in attributes set
// GetAttributeAsDword(attribute_name, DWORD def = 0) - return attribute value, converted from string to dword
// GetAttributeAsFloat(attribute_name, DWORD def = 0) - return attribute value, converted from string to float
// SetAttributeUseDword(attribute_name, DWORD val) - create or modify attribute, converted from dword to string
// SetAttributeUseFloat(attribute_name, FLOAT val) - create or modify attribute, converted from float to string

#define RDTSC_B(x)	{ _asm rdtsc _asm mov x,eax }
#define RDTSC_E(x)	{ _asm rdtsc _asm sub eax,x _asm mov x,eax }

class ATTRIBUTES
{
	void  xtrace(const char * data_PTR,...)
	{
		char xBuffer_4k[4096];
		if(data_PTR == 0) return;

		HANDLE file_h = CreateFile("attributes.log",GENERIC_WRITE,FILE_SHARE_READ,0,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
		SetFilePointer(file_h,0,0,FILE_END);
		va_list args;

		va_start(args,data_PTR);
		_vsnprintf(xBuffer_4k,sizeof(xBuffer_4k) - 4,data_PTR,args);
		strcat(xBuffer_4k,"\x0d\x0a");
		DWORD bytes;
		WriteFile(file_h,xBuffer_4k,strlen(xBuffer_4k),&bytes,0);
		va_end(args);
		CloseHandle(file_h);
		_flushall();

	}

#ifdef _XBOX
	static VSTRING_CODEC * pVStringCodec;
#else
	VSTRING_CODEC * pVStringCodec;
#endif
	DWORD Attributes_num;
	DWORD nNameCode;
	char * Attribute;
	ATTRIBUTES * * pAttributes;
	ATTRIBUTES * pParent;
	bool bBreak;
	
	inline void SetParent(ATTRIBUTES * pP) {pParent = pP;};

	ATTRIBUTES * CreateNewAttribute(dword name_code)
	{
		pAttributes = (ATTRIBUTES **)RESIZE(pAttributes,GetALen(Attributes_num + 1) * sizeof(ATTRIBUTES *));

		pAttributes[Attributes_num] = NEW ATTRIBUTES(pVStringCodec);
		pAttributes[Attributes_num]->SetParent(this);
		pAttributes[Attributes_num]->nNameCode = name_code;
		pAttributes[Attributes_num]->Attribute = 0;
		Attributes_num++;

		return pAttributes[Attributes_num - 1];
	}

public:
	ATTRIBUTES(VSTRING_CODEC * p)
	{
		pVStringCodec = p;
		pAttributes = 0;
		Attributes_num = 0;
		Attribute = 0;
		pParent = 0;
		bBreak = false;
		nNameCode = pVStringCodec->Convert("root");
	};

	~ATTRIBUTES()
	{
		Release();
	};

	__forceinline void SetBreak(bool bBreak)
	{
		this->bBreak = bBreak;
	}

	__forceinline ATTRIBUTES * GetParent()
	{
		return pParent;
	};

	inline bool operator == (const char * str)
	{
		if (!str || !str[0]) return false;
		return stricmp(pVStringCodec->Convert(nNameCode), str) == 0;
	};

	__forceinline char * GetThisName()
	{ 
		return pVStringCodec->Convert(nNameCode);
	}

	__forceinline char * GetThisAttr() { return Attribute; };

	__forceinline void SetName(const char * _name) 
	{
		if (_name) nNameCode = pVStringCodec->Convert(_name);
	};

	void SetValue(const char * _val) 
	{
		if(_val == 0) 
		{
			if(Attribute) delete Attribute; 
			Attribute = 0;
			return;
		}

		Attribute = (char *)RESIZE(Attribute,GetLen(strlen(_val)+1));
		strcpy(Attribute,_val);

		if (bBreak) pVStringCodec->VariableChanged();
	};

	void Release()
	{
		/*DWORD n;
		if(Attribute) delete Attribute; 
		Attribute = 0;
		if(pAttributes)
		{
			for(n=0;n<Attributes_num;n++) delete pAttributes[n];
			delete pAttributes;
		}
		pAttributes = 0;
		Attributes_num = 0;*/
		if (bBreak) pVStringCodec->VariableChanged();
		ReleaseLeafs();
		if(Attribute) delete Attribute; 
		Attribute = 0;
	};

	void ReleaseLeafs()
	{
		DWORD n;
		if(pAttributes)
		{
			for (n=0; n<Attributes_num; n++) delete pAttributes[n];
			delete pAttributes; 
		}
		pAttributes = 0;
		Attributes_num = 0;
	};

	__forceinline DWORD GetAttributesNum() {return Attributes_num;}

	ATTRIBUTES * GetAttributeClass(const char * name)
	{
		for (dword n=0; n<Attributes_num; n++)
			if (stricmp(name, pAttributes[n]->GetThisName()) == 0) return pAttributes[n];
		return null;
	};

	__forceinline ATTRIBUTES * GetAttributeClass(DWORD n)
	{
		return (n >= Attributes_num) ? null : pAttributes[n];
	};

	ATTRIBUTES * VerifyAttributeClass(const char * name)
	{
		ATTRIBUTES * pTemp = GetAttributeClass(name);
		return (pTemp) ? pTemp : CreateAttribute(name, "");
	};

	__forceinline char * GetAttribute(DWORD n)
	{
		return (n >= Attributes_num) ? null : pAttributes[n]->Attribute;
	};

	__forceinline char * GetAttributeName(DWORD n)
	{
		return (n >= Attributes_num) ? null : pAttributes[n]->GetThisName();
	};

	char * GetAttribute(const char * name)
	{
		if (name == 0) return null;
		for(dword n=0; n<Attributes_num; n++)
			if (stricmp(name,pAttributes[n]->GetThisName())== 0) return pAttributes[n]->Attribute;
		return 0;
	};

	DWORD GetAttributeAsDword(const char * name = 0, DWORD def = 0)
	{
		DWORD vDword;
		char * pAttribute;
		vDword = def;
		if (name)
		{
			pAttribute = GetAttribute(name);
			if (pAttribute) vDword = atol(pAttribute);
		} 
		else
		{
			vDword = atol(Attribute);
		}
		return vDword;
	};

	FLOAT GetAttributeAsFloat(const char * name = 0, FLOAT def = 0)
	{
		FLOAT vFloat;
		char * pAttribute;
		vFloat = def;
		if(name)
		{
			pAttribute = GetAttribute(name);
			if(pAttribute) vFloat = (FLOAT)atof(pAttribute);
		}
		else
		{
			vFloat = (FLOAT)atof(Attribute);
		}
		return vFloat;
	};

	BOOL SetAttributeUseDword(const char * name, DWORD val)
	{
		char buffer[128];
		ultoa(val,buffer,10);
		if (name)
			return SetAttribute(name,buffer);
		else
			SetValue(buffer);
		return true;
	};

	BOOL SetAttributeUseFloat(const char * name, FLOAT val)
	{
		char buffer[128];
		sprintf(buffer, "%g", val);
		if (name)
			return SetAttribute(name,buffer);
		else
			SetValue(buffer);

		return true;
	};

	ATTRIBUTES * CreateAttribute(const char * name, const char * attribute)
	{
		DWORD len;
		if(name == 0) return 0;
		pAttributes = (ATTRIBUTES **)RESIZE(pAttributes,GetALen(Attributes_num + 1) * sizeof(ATTRIBUTES *));
		
		pAttributes[Attributes_num] = NEW ATTRIBUTES(pVStringCodec);
		pAttributes[Attributes_num]->SetParent(this);
		pAttributes[Attributes_num]->SetName(name);
		
		if(attribute) 
		{
			len = strlen(attribute);
			pAttributes[Attributes_num]->Attribute = NEW char[GetLen(len + 1)];
			strcpy(pAttributes[Attributes_num]->Attribute,attribute);
		} 
		else pAttributes[Attributes_num]->Attribute = 0;
		Attributes_num++;
		return pAttributes[Attributes_num-1];
	};

	inline DWORD SetAttribute(const char * name, const char * attribute)
	{
		return SetAttribute(pVStringCodec->Convert(name),attribute);
	};

	void Copy(ATTRIBUTES * pASource)
	{
		DWORD n,ah;
		if(pASource == 0) return;
		ReleaseLeafs();
		for(n=0;n<pASource->Attributes_num;n++)
		{
			ah = SetAttribute(pASource->pAttributes[n]->GetThisName(),pASource->pAttributes[n]->Attribute);
			pAttributes[ah]->Copy(pASource->pAttributes[n]);
		}
	};

	BOOL DeleteAttributeClassX(ATTRIBUTES * pA)
	{
		DWORD n,i;
		if(pA == 0) return false;
		if(pA == this) 
		{
			if(pAttributes)
			{
				for(n=0;n<Attributes_num;n++) delete pAttributes[n];
				delete pAttributes; 
				pAttributes = 0;
				Attributes_num = 0;
			}
		}
		else
		{
			for(n=0;n<Attributes_num;n++)
			{
				if(pAttributes[n] == pA)
				{
					delete pA;
					for(i=n;i<(Attributes_num - 1);i++)
					{
						pAttributes[i] = pAttributes[i+1];
					}
					
					Attributes_num--;
					pAttributes = (ATTRIBUTES **)RESIZE(pAttributes,GetALen(Attributes_num) * sizeof(ATTRIBUTES *));
					return true;
				}
				if(pAttributes[n]->DeleteAttributeClassX(pA)) return true;
			}
		}
		return false;
	};

	ATTRIBUTES * CreateSubAClass(ATTRIBUTES * pRoot, const char * access_string)
	{
		dword dwNameCode;
		dword n = 0;
		ATTRIBUTES * pResult = null;
		ATTRIBUTES * pTemp;
		if(pRoot == 0) return 0;
		if(access_string == 0) return 0;

		while(true)
		{
			switch(access_string[n])
			{
				case '.':
					dwNameCode = pVStringCodec->Convert(access_string, n);
					pTemp = pRoot->GetAttributeClassByCode(dwNameCode);
					if (!pTemp)
						pTemp = pRoot->CreateNewAttribute(dwNameCode);
					pResult = CreateSubAClass(pTemp, &access_string[n + 1]);
					return pResult;

				case 0:
					dwNameCode = pVStringCodec->Convert(access_string);
					pResult = pRoot->GetAttributeClassByCode(dwNameCode);
					return (pResult) ? pResult : pRoot->CreateNewAttribute(dwNameCode);
			}
			n++;
		}
		return 0;
	};

	ATTRIBUTES * FindAClass(ATTRIBUTES * pRoot, const char * access_string)
	{
		DWORD n = 0;
		ATTRIBUTES * pResult = null;
		ATTRIBUTES * pTemp = null;

		if (!pRoot || !access_string) return null;
		if (!access_string[0]) return pRoot;

		while(true)
		{
			switch(access_string[n])
			{
				case '.':
					pTemp = pRoot->GetAttributeClassByCode(pVStringCodec->Convert(access_string, n));
					if (!pTemp) return null;
					pResult = FindAClass(pTemp, &access_string[n+1]);
					return pResult;
				case 0:
					pResult = pRoot->GetAttributeClassByCode(pVStringCodec->Convert(access_string));
					return pResult;
			}
			n++;
		}
		return 0;
	};

	__forceinline ATTRIBUTES * GetAttributeClassByCode(DWORD name_code)
	{
		for (dword n=0; n<Attributes_num; n++)
			if(name_code == pAttributes[n]->nNameCode) return pAttributes[n];
		return 0;
	};

	ATTRIBUTES * VerifyAttributeClassByCode(DWORD name_code)
	{
		ATTRIBUTES * pTemp;
		pTemp = GetAttributeClassByCode(name_code);
		if(pTemp) return pTemp;
		return CreateAttribute(name_code,"");
	};

	ATTRIBUTES * CreateAttribute(DWORD name_code, const char * attribute)
	{
		DWORD len;
		pAttributes = (ATTRIBUTES **)RESIZE(pAttributes,GetALen(Attributes_num + 1) * sizeof(ATTRIBUTES *));
		
		pAttributes[Attributes_num] = NEW ATTRIBUTES(pVStringCodec);
		pAttributes[Attributes_num]->SetParent(this);
		pAttributes[Attributes_num]->nNameCode = name_code;
		if(attribute) 
		{
			len = strlen(attribute);
			pAttributes[Attributes_num]->Attribute = NEW char[GetLen(len + 1)];
			strcpy(pAttributes[Attributes_num]->Attribute,attribute);
		} else pAttributes[Attributes_num]->Attribute = 0;
		Attributes_num++;
		return pAttributes[Attributes_num-1];
	};

	__forceinline dword GetALen(dword dwLen)
	{ 
		return (pParent) ? (1 + dwLen / 2) * 2 : (1 + dwLen / 8) * 8;
	}

	__forceinline dword GetLen(dword dwLen, dword dwAlign = 8)
	{ 
		return (1 + dwLen / dwAlign) * dwAlign;
	}

	DWORD SetAttribute(DWORD name_code, const char * attribute)
	{
		DWORD n;
		DWORD len;
		//xtrace("1");
		
//		dword dw1;
//		RDTSC_B(dw1);

		for(n=0;n<Attributes_num;n++)
		{
			if(pAttributes[n]->nNameCode == name_code)
			{
				if(attribute) 
				{
					len = strlen(attribute);
					pAttributes[n]->Attribute = (char *)RESIZE(pAttributes[n]->Attribute, GetLen(len + 1));
					strcpy(pAttributes[n]->Attribute,attribute);
				}
				else
				{
					if(pAttributes[n]->Attribute) 
					{
						delete pAttributes[n]->Attribute;
					}
					pAttributes[n]->Attribute = 0;
				}
				return n;
			}
		}

		pAttributes = (ATTRIBUTES **)RESIZE(pAttributes,GetALen(Attributes_num + 1) * sizeof(ATTRIBUTES *));
		
		pAttributes[Attributes_num] = NEW ATTRIBUTES(pVStringCodec);
		pAttributes[Attributes_num]->SetParent(this);
		pAttributes[Attributes_num]->nNameCode = name_code;
		if(attribute) 
		{
			len = strlen(attribute);
			pAttributes[n]->Attribute = NEW char[GetLen(len + 1)];
			strcpy(pAttributes[n]->Attribute,attribute);
		} 
		else pAttributes[Attributes_num]->Attribute = 0;
		Attributes_num++;

//		RDTSC_E(dw1);
//		xtrace("SA = %d", dw1);

		return (Attributes_num - 1);
	};

	__forceinline DWORD GetThisNameCode()
	{ 
		return nNameCode;
	}
	__forceinline void SetNameCode(DWORD n)
	{ 
		nNameCode = n;
	}

	void Dump(ATTRIBUTES * pA, long level)
	{
		char buffer[128];
		DWORD n;
		if (pA == 0) return;

		if (level >= 128) level = 127;
		if (level != 0) memset(buffer,' ',level); buffer[level] = 0;
		
		for(n=0;n<pA->GetAttributesNum();n++)
		{
			xtrace("%s%s = %s", buffer, pA->GetAttributeName(n), pA->GetAttribute(n));
			Dump(pA->GetAttributeClass(pA->GetAttributeName(n)), level + 2);
		}
	}
};

#endif