#include <io.h>
#include <fcntl.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include "..\common_h\exs.h"
#include "..\common_h\defines.h"
#include "..\common_h\rands.h"
//#include <dxerr8.h>
#include "AMP3Coded.h"

size_t readFunction(void *ptr, size_t size, size_t nmemb, void *datasource)
{
	DWORD bytesRead;
#ifndef _XBOX
	api->fio->_ReadFile((HANDLE) datasource, ptr, size*nmemb, &bytesRead);
#else
	api->fio->_ReadFile((HANDLE) datasource, ptr, size*nmemb, &bytesRead);
#endif
	return bytesRead;
}

int seekFunction(void *datasource, ogg_int64_t offset, int whence)
{
	switch (whence)
	{
	case SEEK_CUR:
		return api->fio->_SetFilePointer((HANDLE) datasource, (long) offset, 0, FILE_CURRENT);
	case SEEK_END:
		return api->fio->_SetFilePointer((HANDLE) datasource, (long) offset, 0, FILE_END);
	case SEEK_SET:
		return api->fio->_SetFilePointer((HANDLE) datasource, (long) offset, 0, FILE_BEGIN);
	}
	return -1;
}

int closeFunction(void *datasource)
{
	api->fio->_CloseHandle((HANDLE) datasource);
	return 0;
}

long tellFunction(void *datasource)
{
	return api->fio->_SetFilePointer((HANDLE) datasource, 0, 0, FILE_CURRENT);
}

ov_callbacks callbacks = {readFunction, seekFunction, closeFunction, tellFunction};

//--------------------------------------------------------------------
AMP3Coded::AMP3Coded()
	:soundNotify(0)
	,inFile(INVALID_HANDLE_VALUE)
	,validMP3(false)
	,decodedData(0)
	,codedData(0)
	,notifyEvent(0)
	,notifyEvent2(0)
	,flushFirst(true)
	,decodedStream(0)
	,firstEventSignaled(0)
	,newDecodeStage(true)
	,bytesDoneOnStage(0)
	,finishedDecoding(false)
	,bytesPlayed(0)
	,bytesAll(1)
	,leftFlushes(-1)
{
	GUARD(AMP3Coded::AMP3Coded)

	eof = 0;
	//InitMP3(&mp);

	UNGUARD
}

//--------------------------------------------------------------------
AMP3Coded::~AMP3Coded()
{
	GUARD(AMP3Coded::~AMP3Coded)

	AMP3Coded::Unload();

	UNGUARD
}

//--------------------------------------------------------------------
bool AMP3Coded::Unload()
{
	GUARD(AMP3Coded::Unload)

	if (validMP3)
		ov_clear(&vf);

	RELEASE(soundNotify)
	DELETE_ARRAY(decodedData)
	DELETE_ARRAY(codedData)
	DELETE(decodedStream)

	if (notifyEvent)
	{
		CloseHandle(notifyEvent);
		notifyEvent = 0;
	}
	if (notifyEvent2)
	{
		CloseHandle(notifyEvent2);
		notifyEvent2 = 0;
	}
	/*
	if (endEvent)
	{
		CloseHandle(endEvent);
		endEvent = 0;
	}
	*/
	if (inFile != INVALID_HANDLE_VALUE)
	{
		if (SetFilePointer(inFile, 0, 0, FILE_END) != 0xFFFFFFFF)
			FIO(CloseHandle(inFile));
		inFile = INVALID_HANDLE_VALUE;
	}

	validMP3 = false;
	newDecodeStage = true;
	bytesDoneOnStage = 0;
	firstEventSignaled = 0;
	finishedDecoding = false;

	return true;
	UNGUARD
}

//--------------------------------------------------------------------
bool AMP3Coded::OpenMP3File(const char *_filename, WAVEFORMATEX *_waveFormat, unsigned long *_bufferSize)
{
	GUARD(AMP3Coded::OpenWAVFile)
	Assert(_filename && _waveFormat && _bufferSize)

	AMP3Coded::Unload();

	try
	{
		#ifndef _XBOX
			inFile = api->fio->_CreateFile(_filename);
		#else
			inFile = api->fio->_CreateFile(_filename/*,GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0*/);
		#endif
		//inFile = _CORE_API->fio->_CreateFile(_filename);
		if (inFile == INVALID_HANDLE_VALUE)
			THROW_STRING("%s: Can`t open file", _filename)

		validMP3 = true;
		if(ov_open_callbacks((void *) inFile, &vf, 0, 0, callbacks) < 0)
		{
			throw "Can`t recognize OGG file";
		}

	    vorbis_info *vi=ov_info(&vf,-1);
		bytesAll = (DWORD) (ov_pcm_total(&vf, -1) * 4.5); // empiric coeff
		
		ZeroMemory(_waveFormat, sizeof(WAVEFORMATEX));
		_waveFormat->wFormatTag = WAVE_FORMAT_PCM; 
		_waveFormat->nChannels = vi->channels; 
		_waveFormat->nSamplesPerSec = vi->rate; 
		_waveFormat->wBitsPerSample = 16; 
		_waveFormat->nBlockAlign = _waveFormat->wBitsPerSample / 8 * _waveFormat->nChannels;
		_waveFormat->nAvgBytesPerSec = _waveFormat->nSamplesPerSec * _waveFormat->nBlockAlign;

		*_bufferSize = 2*MP3_BUFFER_SIZE;
		codedData = NEW char[MP3_FRAME_DECODE_SIZE];
		decodedData = NEW char[MP3_FRAME_DECODE_SIZE];
		decodedStream = NEW TLimitedStream(MP3_BUFFER_SIZE);

		bytesPlayed = 0;
		//_CORE_API->fio->_SetFilePointer(inFile, 0, 0, FILE_BEGIN);
		//ZeroMemory(decodedData, MP3_MAX_CHUNK_SIZE);
	}
	catch (char *_message)
	{
		api->Trace(_message);
		if (inFile)
		{
			api->fio->_CloseHandle(inFile);
			inFile = INVALID_HANDLE_VALUE;
		}
		return false;
	}

	return true;
	UNGUARD
}

//--------------------------------------------------------------------
bool AMP3Coded::LoadDataToBuffer(LPDIRECTSOUNDBUFFER _buffer, MMCKINFO &_info)
{
	GUARD(AMP3Coded::LoadDataToBuffer)

	if (!validMP3) 
		return false;

	HRESULT outCode;

	//<notify>
	notifyEvent = CreateEvent(0, TRUE, FALSE, 0);
	notifyEvent2 = CreateEvent(0, TRUE, FALSE, 0);
	DSBPOSITIONNOTIFY notifyPositions[2]= { {0, notifyEvent}, 
											{MP3_BUFFER_SIZE, notifyEvent2}
	};//{, endEvent}};

	#ifndef _XBOX
		_buffer->QueryInterface(IID_IDirectSoundNotify, (void **) &soundNotify);
		Assert(soundNotify)

		outCode = soundNotify->SetNotificationPositions(2, notifyPositions);
		Assert(outCode == DS_OK)
	#else
		outCode = _buffer->SetNotificationPositions(2, notifyPositions);
		Assert(outCode == DS_OK)
	#endif

	leftFlushes = -1;
	//<first_fill>
//#ifndef _XBOX
	int flushes = 0;
	while (flushes < 2 && !finishedDecoding) //fill all 2 parts of buffer
	{
		DoSmallDecode(_buffer, true, 0);
		if (decodedStream->Full())
		{
			FlushDecodedToSoundBuffer(_buffer, flushFirst, core->looped);
			++flushes;
		}
	}
//#endif

	return true;
	UNGUARD
}

//--------------------------------------------------------------------
bool AMP3Coded::AddAndDecodeMP3Data(bool _looped)
{
	GUARD(AMP3Coded::AddMP3Data)
	Assert(inFile)

	if (!validMP3 || !decodedStream)
		return false;

    int bytesDone = 1;
	int framesCount = MP3_FRAMES_DECODE;

	try
	{
		for (int framesCount = 0; framesCount < MP3_FRAMES_DECODE; framesCount++)
		{
			ZeroMemory(decodedData, MP3_FRAME_DECODE_SIZE);
		    bytesDone = ov_read(&vf,decodedData,MP3_FRAME_DECODE_SIZE,0,2,1,&current_section);
			if (bytesDone <= 0) //finished
			{
				leftFlushes = 2;
				if (_looped)
				{
					ov_pcm_seek(&vf, 0);
					core->loopPauseTime = (dword) randUpper(MAX_LOOP_PAUSE_TIME);
					finishedDecoding = false;
				}
				else
				{
					//core->dsBuffer->Stop();
					//finishedDecoding = true;

					//leftFlushes = 2;
				}
				break;
			}

			bytesDoneOnStage += bytesDone;
			if (!decodedStream->Write(decodedData, bytesDone))
				SOUND_TRACE2("MP3: Decoded stream capacity too low! (done: %d)", bytesDone);

		}
	}
	catch (char *_message)
	{
		SOUND_TRACE(_message);
		return false;
	}

	return true;
	UNGUARD
}

//--------------------------------------------------------------------
bool AMP3Coded::FlushDecodedToSoundBuffer(LPDIRECTSOUNDBUFFER _buffer, bool _first, bool _looped)
{
	GUARD(AMP3Coded::FlushDecodedToSoundBuffer)

	HRESULT outDSCode;
	DWORD   buffer1Size, 
		    buffer2Size;
	LPVOID  buffer1,
		    buffer2;

	if (leftFlushes != -1)
	{
		if (!leftFlushes)
		{
			if (core->looped)
			{
				ov_pcm_seek(&vf, 0);
				core->loopPauseTime = (long) randUpper((float) core->maxLoopPauseTime);
				finishedDecoding = false;
				core->dsBuffer->Stop();
				core->dsBuffer->SetCurrentPosition(0);

				//SOUND_TRACE2("OGG: Waiting for next loop %d msec", core->loopPauseTime);

				// now fill buffer with zeros to clear artefacts on next loop
				outDSCode = _buffer->Lock(0, 
										  2*MP3_BUFFER_SIZE, 
										  &buffer1, 
										  &buffer1Size, 
										  &buffer2, 
										  &buffer2Size, 
										  0);
				if (outDSCode == DS_OK)
				{
					ZeroMemory(buffer1, 2*MP3_BUFFER_SIZE);
					outDSCode = _buffer->Unlock(buffer1, buffer1Size, buffer2, buffer2Size);
				}
			}
			else
			//if (core->loopPauseTime == -1)
			{
				core->dsBuffer->Stop();
				finishedDecoding = true;
			}
			leftFlushes = -1;
			//SOUND_TRACE("OGG: leftFlushes == 0!");
			return false;
		}
		else
			--leftFlushes;
	}
	if (!decodedStream->Full())
	{
		SOUND_TRACE("MP3: 'Buffer isn`t full on flush'");
		decodedStream->MakeFull();
		//return false;
	}
	if (!validMP3 || !decodedStream)
		return false;

	flushFirst = _first;
	if (flushFirst)
		outDSCode = _buffer->Lock(0, 
								  MP3_BUFFER_SIZE, 
								  &buffer1, 
								  &buffer1Size, 
								  &buffer2, 
								  &buffer2Size, 
								  0);
	else
		outDSCode = _buffer->Lock(MP3_BUFFER_SIZE, 
								  MP3_BUFFER_SIZE, 
								  &buffer1, 
								  &buffer1Size, 
								  &buffer2, 
								  &buffer2Size, 
								  0);
	//if (outDSCode == DSERR_BUFFERLOST) /*XBOX*/
	if (outDSCode != DS_OK)
	{
		SOUND_TRACE("Restoring buffer");
		outDSCode = _buffer->Restore();
		if (outDSCode != DS_OK)
		{
			SOUND_TRACE("Unable to restore buffer");
			return false;
		}
		if (flushFirst)
			outDSCode = _buffer->Lock(0, 
									  MP3_BUFFER_SIZE, 
									  &buffer1, 
									  &buffer1Size, 
									  &buffer2, 
									  &buffer2Size, 
									  0);
		else
			outDSCode = _buffer->Lock(MP3_BUFFER_SIZE, 
									  MP3_BUFFER_SIZE, 
									  &buffer1, 
									  &buffer1Size, 
									  &buffer2, 
									  &buffer2Size, 
									  0);

	}

	if (outDSCode != DS_OK)
	{
		SOUND_TRACE("MP3: Unable to lock buffer");
		return false;
	}

	long bytesCopied = 0;

	ZeroMemory(buffer1, MP3_BUFFER_SIZE);
	if (!decodedStream->Read((char *) buffer1, buffer1Size, &bytesCopied))
		SOUND_TRACE("MP3: No data in decoded stream!");

	if (bytesCopied < (long) buffer1Size)
		SOUND_TRACE("MP3: Performance problem! Didn`t manage to decode enough data in time");

	outDSCode = _buffer->Unlock(buffer1, buffer1Size, buffer2, buffer2Size);
	if (outDSCode != DS_OK)
	{
		SOUND_TRACE("MP3: Error on unlock");
		return false;
	}

	flushFirst = !flushFirst;
	//ZeroMemory(decodedData, decodedData);
	//decodedStream->Clear(); //!!!!

	if (finishedDecoding)
	{
		;
	}

	return true;
	UNGUARD
}

//--------------------------------------------------------------------
void AMP3Coded::DoSmallDecode(LPDIRECTSOUNDBUFFER _buffer, bool _looped, dword _dTime)
{
	GUARD(AMP3Coded::DoSmallDecode)
	if (core->loopPauseTime != LOOP_DISABLED)
	{
		core->loopPauseTime -= _dTime;
		if ((core->loopPauseTime <= 0) && core->playing)
		{
			core->loopPauseTime = -1;
			core->dsBuffer->Play(0, 0, DSBPLAY_LOOPING);
			bytesPlayed = 0;
		}
	}

	if (!notifyEvent || !decodedStream || finishedDecoding)
		return;

	try
	{
		if (!decodedStream->Full() && (leftFlushes == -1))
			AddAndDecodeMP3Data(_looped);  
		else
		{
			if	(WaitForSingleObject(notifyEvent, 0) == WAIT_OBJECT_0) //event happened
			{
				bytesPlayed += MP3_BUFFER_SIZE;
				if (firstEventSignaled < 1)
				{
					ResetEvent(notifyEvent);
					++firstEventSignaled;

					return;
				}
				
				//if (finishedDecoding && !_looped)
				//	core->dsBuffer->Stop();
				ResetEvent(notifyEvent);
				FlushDecodedToSoundBuffer(_buffer, false, _looped);
				//return;
			}

			if	(WaitForSingleObject(notifyEvent2, 0) == WAIT_OBJECT_0) //event happened
			{
				bytesPlayed += MP3_BUFFER_SIZE;
				if (firstEventSignaled < 1)
				{
					ResetEvent(notifyEvent2);
					++firstEventSignaled;

					return;
				}

				//if (finishedDecoding && !_looped)
				//	core->dsBuffer->Stop();
				ResetEvent(notifyEvent2);
				FlushDecodedToSoundBuffer(_buffer, true, _looped);
			}
		}
	}
	catch (...)
	{
		SOUND_TRACE("Exception on MP3 decode");
	}

	UNGUARD
}

//--------------------------------------------------------------------
float AMP3Coded::GetPosition()
{
	GUARD(AMP3Coded::GetPosition)

	DWORD playPosition = 0
		 ,writePosition;
	if (core->dsBuffer)
		core->dsBuffer->GetCurrentPosition(&playPosition, &writePosition);

	if (bytesAll)
		return ((float) bytesPlayed) / bytesAll;
	else
		return ERROR_POSITION;

	UNGUARD
}
