#ifndef _TLIMITEDSTREAM_H_
#define _TLIMITEDSTREAM_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class TLimitedStream  
{
public:
	TLimitedStream(long _limit);
	virtual ~TLimitedStream();

	bool Read(char *_data, long _size, long *_readSize);
	bool Write(const char *_data, long _size);
	bool Full();
	void Clear() {streamLength = 0; }
	bool MakeFull();
private:
	long streamLimit;
	long streamLength;
	char *streamData;
};

#endif // !defined
