#ifndef _VSTRING_CODEC_H_
#define _VSTRING_CODEC_H_

class VSTRING_CODEC
{
public:
	VSTRING_CODEC(){};
	virtual ~VSTRING_CODEC(){};
	virtual DWORD GetNum(){return 0;};
	virtual DWORD Convert(const char * pString){return 0;};
	virtual DWORD Convert(const char * pString, long iLen) {return 0;};
	virtual char * Convert(DWORD code){return 0;};
	virtual void VariableChanged() {};
};

#endif