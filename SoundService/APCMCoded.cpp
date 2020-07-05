#include "..\common_h\exs.h"
//#include <dxerr8.h>
#include "APCMCoded.h"
#include "VSoundService.h"
#include "SoundService.h"

THashPCMMap APCMCoded::pcmMap;
long APCMCoded::allCachedBytes = 0;
long APCMCoded::allMaxCachedBytes = 0;

//--------------------------------------------------------------------
bool ReadWAVHeader (HANDLE _file, WAVEFORMATEX *_waveFormat, DWORD *_size)
{
	DWORD bytesRead;
#ifndef _XBOX
	// read wave format data
	api->fio->_SetFilePointer(_file, 0x14, 0, FILE_BEGIN);
	api->fio->_ReadFile(_file, _waveFormat, sizeof(WAVEFORMATEX), &bytesRead);
	// find data begin
	byte pcv[64];
	api->fio->_SetFilePointer(_file, 0x24, 0, FILE_BEGIN);
	api->fio->_ReadFile(_file, pcv, sizeof(pcv), &bytesRead);
	for( long n=0; n<(long)bytesRead-8; n++ ) {
		if( pcv[n] == 'd' &&
			pcv[n+1] == 'a' &&
			pcv[n+2] == 't' &&
			pcv[n+3] == 'a' )
			break;
	}
	// if finded data, then get data size (buffer size)
	if( n < (long)bytesRead-8 ) {
		*_size = *(dword*)&pcv[n+4];
	} else { // else no buffer - warning!
		api->Trace("Warning! Wrong Sound File Format.");
		*_size = 0;
		return false;
	}
#else
	api->fio->_SetFilePointer(_file, 0x14, 0, FILE_BEGIN);
	api->fio->_ReadFile(_file, _waveFormat, sizeof(WAVEFORMATEX), &bytesRead);
	if (_waveFormat->wFormatTag == WAVE_FORMAT_ADPCM)
	{
		api->fio->_SetFilePointer(_file, 0x38, 0, FILE_BEGIN);
		api->fio->_ReadFile(_file, _size, sizeof(DWORD), &bytesRead);
		if ((_waveFormat->nChannels != 1) && (_waveFormat->nChannels != 2))
			return false;

		if (_waveFormat->nBlockAlign != 36)
			return false;

		if (_waveFormat->wBitsPerSample != 4)
			return false;
	}
	else
	{
		return false;
		api->fio->_SetFilePointer(_file, 0x28, 0, FILE_BEGIN);
		api->fio->_ReadFile(_file, _size, sizeof(DWORD), &bytesRead);
	}

#endif

	return true;
}

//--------------------------------------------------------------------
bool ReadWAVData (HANDLE _file, void *_data, DWORD _dataSize, DWORD _waveFormat = 0)
{
	SOUND_TRACE("SoundService: APCMCoded::ReadWAVData {");
	DWORD bytesRead = 0;
#ifndef _XBOX
	// find data begin
	byte pcv[64];
	api->fio->_SetFilePointer(_file, 0x24, 0, FILE_BEGIN);
	api->fio->_ReadFile(_file, pcv, sizeof(pcv), &bytesRead);
	for( long n=0; n<(long)bytesRead-8; n++ ) {
		if( pcv[n] == 'd' &&
			pcv[n+1] == 'a' &&
			pcv[n+2] == 't' &&
			pcv[n+3] == 'a' )
			break;
	}
	// if finded data, then get data size (buffer size)
	if( n<(long)bytesRead-8 ) {
		api->fio->_SetFilePointer(_file, 0x2C+n, 0, FILE_BEGIN);
		api->fio->_ReadFile(_file, _data, _dataSize, &bytesRead);
	}
#else
	if (_waveFormat == WAVE_FORMAT_XBOX_ADPCM)
		api->fio->_SetFilePointer(_file, 0x3C, 0, FILE_BEGIN); //XBOX ADPCM
	else
	{
		return false;
		api->fio->_SetFilePointer(_file, 0x2C, 0, FILE_BEGIN);
	}
	api->fio->_ReadFile(_file, _data, _dataSize, &bytesRead); //PCM
#endif

	SOUND_TRACE("SoundService: APCMCoded::ReadWAVData }");
	return (bytesRead == _dataSize);
}

//--------------------------------------------------------------------
APCMCoded::APCMCoded()
	:cachedI(INVALID_I)
{

}

//--------------------------------------------------------------------
APCMCoded::~APCMCoded()
{
	if (cachedI != INVALID_I)
	{
		tCachedData *cachedData;
		cachedData = (tCachedData *) pcmMap[cachedI];
		if (cachedData)
		{
			cachedData->usageCounter--;
		}
	}
}

//--------------------------------------------------------------------
void APCMCoded::DeleteCachedData()
{
	SOUND_TRACE("SoundService: APCMCoded::DeleteCachedData() {");
	if (cachedI != INVALID_I)
	{
		tCachedData *cachedData;
		cachedData = (tCachedData *) pcmMap[cachedI];

		if (cachedData)
		{
			if (cachedData->pcmData)
			{
				delete cachedData->pcmData;
				cachedData->pcmData = 0;
				allCachedBytes -= bufferSize;
				pcmMap.Remove(cachedI);
			}
		}
	}
	SOUND_TRACE("SoundService: APCMCoded::DeleteCachedData() }");
}

//--------------------------------------------------------------------
void APCMCoded::DeleteCachedData(int _i)
{
	SOUND_TRACE("SoundService: APCMCoded::DeleteCachedData(int) {");
	if (_i != INVALID_I)
	{
		tCachedData *cachedData;
		cachedData = (tCachedData *) pcmMap[_i];

		if (cachedData)
		{
			if (cachedData->pcmData)
			{
				delete cachedData->pcmData;
				cachedData->pcmData = 0;
				allCachedBytes -= cachedData->size;
				pcmMap.Remove(_i);
			}
		}
	}
	SOUND_TRACE("SoundService: APCMCoded::DeleteCachedData(int) {");
}

//--------------------------------------------------------------------
bool APCMCoded::LoadDataToBuffer(LPDIRECTSOUNDBUFFER _buffer, HANDLE _inFile, DWORD _size, tCachedData *_cachedData)
{
	GUARD(APCMCoded::LoadDataToBuffer)

// *** READ WAV DATA : PC ***
#ifndef _XBOX
	DWORD   buffer1Size, 
			buffer2Size;
	LPVOID  buffer1,
			buffer2;
	HRESULT outDSCode;

	if (!ReadWAVData(_inFile, (void *) _cachedData->pcmData, _cachedData->size, _cachedData->waveFormat.wFormatTag))
		return false;

	outDSCode = _buffer->Lock(0, _size, &buffer1, &buffer1Size, &buffer2, &buffer2Size, DSBLOCK_ENTIREBUFFER);

	if (outDSCode != DS_OK)
	{
		SOUND_TRACE("!APCMCoded: DS error on buffer lock");
		return false;
	}

	Assert(_cachedData->size == buffer1Size)
	memcpy(buffer1, (void *) _cachedData->pcmData, _cachedData->size);
	/*
	{ //uncached, simply read from file
		if (!ReadWAVData(_inFile, (void *) buffer1, buffer1Size))
		{
			_buffer->Unlock(buffer1, buffer1Size, buffer2, buffer2Size);		
			SOUND_TRACE("!APCMCoded: Can`t read from file to locked buffer");
			return false;
		}
	}*/

	outDSCode = _buffer->Unlock(buffer1, buffer1Size, buffer2, buffer2Size);
	if (outDSCode != DS_OK)
	{
		SOUND_TRACE("!APCMCoded: Can`t unlock buffer");
		return false;
	}
#else
// *** READ WAV DATA : XBOX ***
	if (!ReadWAVData(_inFile, (void *) _cachedData->pcmData, _cachedData->size, _cachedData->waveFormat.wFormatTag))
		return false;
#endif

	return true;
	UNGUARD
}

//--------------------------------------------------------------------
bool APCMCoded::OpenWAVFile(const char *_filename, HANDLE *_inFile, WAVEFORMATEX *_waveFormat, DWORD *_dataSize, tCachedData **_cachedData, bool *_ifStartCaching, bool *_ifLogError)
{
	GUARD(APCMCoded::OpenWAVFile)
	*_ifLogError = true;

	if (!_filename || !_inFile || !_waveFormat || !_dataSize)
		return false;

	*_ifStartCaching = false;
	*_cachedData = (tCachedData *) pcmMap[_filename];
	if ((*_cachedData) && (*_cachedData)->pcmData)
	{
		if ((*_cachedData)->usageCounter >= MAX_SOUND_DUPLICATES)
		{
			*_ifLogError = false;
			return false;
		}
		core->isOriginalBuffer = false;
		(*_cachedData)->usageCounter++;
		cachedI = (*_cachedData)->cacheIndex;//INVALID_I;
		return true;
	}

	char     tempString[COMMON_STRING_LENGTH];

	strcpy(tempString, _filename);
	*_inFile = api->fio->_CreateFile(tempString/*,GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0*/);
	if (*_inFile == INVALID_HANDLE_VALUE)
		return false;

	if (!ReadWAVHeader(*_inFile, _waveFormat, _dataSize))
	{
		api->fio->_CloseHandle(*_inFile);
		SOUND_TRACE2("!APCMCoded: Can`t find WAV header (%s)",_filename);
		return false;
	}

	cachedI = pcmMap.Add(_filename, _cachedData);
	(*_cachedData)->pcmData = NEW BYTE[*_dataSize];
	(*_cachedData)->size = *_dataSize;
	(*_cachedData)->waveFormat = *_waveFormat;
	(*_cachedData)->lastUsedTime = GetTickCount();
	(*_cachedData)->usageCounter = 1;
	(*_cachedData)->uncacheable = true/*!core->cached*/;
	(*_cachedData)->cacheIndex = cachedI;
	core->attachedToSoundBuffer = (*_cachedData)->pcmData;

	//statistics
	allCachedBytes += *_dataSize;
	if (allCachedBytes > allMaxCachedBytes)
		allMaxCachedBytes = allCachedBytes;
	*_ifStartCaching = true;

	return true;
	UNGUARD
}

//--------------------------------------------------------------------
void APCMCoded::UncacheLeastUsed()
{
	SOUND_TRACE("SoundService: APCMCoded::UncacheLeastUsed {");
	tCachedData *cachedData;
	dword curLastUsedTime = -1;
	dword minLastUsedTime = -1;
	int uncacheI = -1;

	for (int i=0; i<MAX_VALUES; i++)
	{
		cachedData = (tCachedData *) pcmMap[i];
		if (cachedData)
		{
			if (!cachedData->uncacheable || (cachedData->usageCounter > 0))
				continue;

			/*
			cachedData->cachedBuffer->GetStatus(&playFlag);
			if ((playFlag & DSBSTATUS_PLAYING) 
		//#ifdef _XBOX
		//	 || (playFlag & DSBSTATUS_PAUSED)
		//#endif
		//	 || (playFlag & DSBSTATUS_LOOPING)
				)
				 continue;
			*/
			curLastUsedTime = cachedData->lastUsedTime;
			if (curLastUsedTime < minLastUsedTime)
			{
				minLastUsedTime = curLastUsedTime;
				uncacheI = i;
			}
		}
	}

	if (uncacheI != -1)
	{
		cachedData = (tCachedData *) pcmMap[uncacheI];
		soundService->ReleaseSoundsWithCached(uncacheI, cachedData);
	}

	SOUND_TRACE("SoundService: APCMCoded::UncacheLeastUsed }");
}

//--------------------------------------------------------------------
bool APCMCoded::UnloadIfThisCached(tCachedData *_cachedData)
{
	if (!_cachedData)
		return false;

	if ((_cachedData->pcmData) == (core->attachedToSoundBuffer))
		return true;
	else
		return false;
}

//--------------------------------------------------------------------
void APCMCoded::UnloadAllPCMData()
{
	// ON SOUND SERVICE DTOR
	try
	{
		tCachedData *cachedData;

		for (int i=0; i<MAX_VALUES; i++)
		{
			cachedData = (tCachedData *) pcmMap[i];
			if (cachedData)
			{
				if (cachedData->pcmData)
					delete cachedData->pcmData;
			}
		}
	}
	catch (...)
	{
	}

}

//--------------------------------------------------------------------
