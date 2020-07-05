#ifndef _AMP3CODED_H_
#define _AMP3CODED_H_

#include <dsound.h>
#include <stdio.h>
#include "SoundDefines.h"
#include "..\common_h\vmodule_api.h"
#include "TLimitedStream.h"
#include "TSoundCore.h"
#include "ogg_vorbis\codec.h"
#include "ogg_vorbis\vorbisfile.h"

// frame -[decode]-> chunk
static const long MP3_MAX_CHUNK_SIZE= 8192;
static const long MP3_TEST_READ_SIZE = 1024;

#ifndef _XBOX //PC
	static const long MP3_BUFFER_SIZE = 30*4096; //*30
	static const long MP3_FRAMES_DECODE = 8; //8
	static const long MP3_FRAME_DECODE_SIZE = 4096; //4096
#else         //XBOX
	static const long MP3_BUFFER_SIZE = 60*4096; //*30
	static const long MP3_FRAMES_DECODE = 12; //8
	static const long MP3_FRAME_DECODE_SIZE = 4096; //4096
#endif

///////////////////////////////////////////////////////////////////
// CLASS DEFINITION
///////////////////////////////////////////////////////////////////
class AMP3Coded  
{
public:
	AMP3Coded();
	virtual ~AMP3Coded();
	bool Unload();

	bool OpenMP3File(const char *_filename, WAVEFORMATEX *_waveFormat, unsigned long *_bufferSize);
	bool LoadDataToBuffer(LPDIRECTSOUNDBUFFER _buffer, MMCKINFO &_info);

	void DoSmallDecode(LPDIRECTSOUNDBUFFER _buffer, bool _looped, dword _dTime);

protected:
	float GetPosition();
	TSoundCore			*core;

//private:
	bool AddAndDecodeMP3Data(bool _looped);
	bool FlushDecodedToSoundBuffer(LPDIRECTSOUNDBUFFER _buffer, bool _first, bool _looped);

	LPDIRECTSOUNDNOTIFY soundNotify;
	//HANDLE              inFile;
	HANDLE				inFile;
	bool				validMP3;
	char	            *decodedData;
	char		        *codedData;
	HANDLE			    notifyEvent;
	HANDLE			    notifyEvent2;
	bool				flushFirst; //first/second part of SoundBuffer to paste decodedData
	TLimitedStream		*decodedStream;
	int 				firstEventSignaled;
	bool				newDecodeStage;
	long				bytesDoneOnStage;
	bool				finishedDecoding;
	DWORD				bytesPlayed;
	DWORD				bytesAll;
	int					leftFlushes;

	int					allFlushes;
	//HANDLE				endEvent;

	OggVorbis_File vf;
	int eof;
	int current_section;
};

#endif // !defined
