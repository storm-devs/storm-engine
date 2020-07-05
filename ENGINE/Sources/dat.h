#ifndef _C_DAT_H_
#define _C_DAT_H_

template<class CTYPE>
class CDAT
{
	CTYPE * pPointer;
	long	nSize;
public:
	CDAT(){pPointer = 0; nSize = 0;};
	~CDAT(){if(pPointer) delete pPointer;};
	void SetSize(long size)
	{
		CTYPE * pM;
		long nCopy;
		long n;
		pM = NEW CTYPE[size];
		if(size >= nSize) nCopy = nSize;
		else nCopy = size;
		for(n = 0; n < nCopy; n++)
		{
			pM[n] = pPointer[n];
		}
		delete [] pPointer;
		pPointer = pM;
		nSize = size;

	}
	CTYPE& operator [] (long n) 
	{
		if(n >= nSize) throw "invalid index";
		return pPointer[n];
	}
};


#endif