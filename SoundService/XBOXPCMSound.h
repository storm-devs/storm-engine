#ifndef _XBOXPCMSOUND_H_
#define _XBOXPCMSOUND_H_

#ifdef _XBOX
//-----------------------------------------------------------------------------
// Name: RIFFHEADER
// Desc: For parsing WAV files
//-----------------------------------------------------------------------------
struct RIFFHEADER
{
    FOURCC  fccChunkId;
    DWORD   dwDataSize;
};

#define RIFFCHUNK_FLAGS_VALID   0x00000001

//-----------------------------------------------------------------------------
// Name: class CRiffChunk
// Desc: RIFF chunk utility class
//-----------------------------------------------------------------------------
class CRiffChunk
{
    FOURCC            m_fccChunkId;       // Chunk identifier
    const CRiffChunk* m_pParentChunk;     // Parent chunk
    HANDLE            m_hFile;
    DWORD             m_dwDataOffset;     // Chunk data offset
    DWORD             m_dwDataSize;       // Chunk data size
    DWORD             m_dwFlags;          // Chunk flags

public:
    CRiffChunk();

    // Initialization
    VOID    Initialize( FOURCC fccChunkId, const CRiffChunk* pParentChunk,
                        HANDLE hFile );
    HRESULT Open();
    BOOL    IsValid()     { return !!(m_dwFlags & RIFFCHUNK_FLAGS_VALID); }

    // Data
    HRESULT ReadData( LONG lOffset, VOID* pData, DWORD dwDataSize );

    // Chunk information
    FOURCC  GetChunkId()  { return m_fccChunkId; }
    DWORD   GetDataSize() { return m_dwDataSize; }
};

//-----------------------------------------------------------------------------
// Name: class XBOXPCMSound
// Desc: Wave file utility class
//-----------------------------------------------------------------------------
class XBOXPCMSound
{
    HANDLE      m_hFile;            // File handle
    CRiffChunk  m_RiffChunk;        // RIFF chunk
    CRiffChunk  m_FormatChunk;      // Format chunk
    CRiffChunk  m_DataChunk;        // Data chunk
    CRiffChunk  m_WaveSampleChunk;  // Wave Sample chunk
    
public:
    XBOXPCMSound();
    ~XBOXPCMSound();

    // Initialization
    HRESULT Open( const CHAR* strFileName );
    VOID    Close();

    // File format
    HRESULT GetFormat( WAVEFORMATEX* pwfxFormat, DWORD dwFormatSize, DWORD *pdwRequiredSize = NULL );

    // File data
    HRESULT ReadSample( DWORD dwPosition, VOID* pBuffer, DWORD dwBufferSize, 
                        DWORD* pdwRead );

    // Loop region
    HRESULT GetLoopRegion( DWORD* pdwStart, DWORD* pdwLength );

    // File properties
    VOID    GetDuration( DWORD* pdwDuration ) { *pdwDuration = m_DataChunk.GetDataSize(); }
};

#endif
#endif