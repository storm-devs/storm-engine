#include <string.h>

#ifndef _XBOX
	#include <windows.h>
#else
	#include <xtl.h>
#endif

#include "..\COMMON_H\memop.h"
#include "..\COMMON_H\defines.h"
#include "TLimitedStream.h"

#define OVERFLOW_K 2.0f
//-------------------------------------------------------------------
TLimitedStream::TLimitedStream(long _limit)
	:streamLimit(_limit)
	,streamLength(0)
	,streamData(0)
{
	streamData = NEW char[(int) (OVERFLOW_K * streamLimit)]; // data loss is OK here
}

//-------------------------------------------------------------------
TLimitedStream::~TLimitedStream()
{
	if (streamData)
	{
		DELETE_ARRAY(streamData);
		streamData = 0;
	}
}

//-------------------------------------------------------------------
bool TLimitedStream::Read(char *_data, long _size, long *_readSize)
{
	if (streamLength > _size)
		*_readSize = _size;
	else
		*_readSize = streamLength;

	if (!*_readSize)
		return false;

	memcpy(_data, streamData, *_readSize);
	memmove(streamData, (void *) (streamData + *_readSize), streamLength - *_readSize);
	streamLength -= _size;
	return true;
}

//-------------------------------------------------------------------
bool TLimitedStream::Write(const char *_data, long _size)
{
	if ((_size+streamLength) >= OVERFLOW_K*streamLimit)
		return false;
	if (_size >= streamLimit)
		return false;

	memcpy((void *) (streamData + streamLength), (void *) _data, _size);
	streamLength += _size;

	return true;
}

//-------------------------------------------------------------------
bool TLimitedStream::Full()
{
	return (streamLength >= streamLimit);
}

//-------------------------------------------------------------------
bool TLimitedStream::MakeFull()
{
	if (Full())
		return false;

	memset((void *) (streamData + streamLength), 0, streamLimit-streamLength);
	streamLength = streamLimit;
	return true;
}

//-------------------------------------------------------------------
//-------------------------------------------------------------------
//-------------------------------------------------------------------
//-------------------------------------------------------------------
