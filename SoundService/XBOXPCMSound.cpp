#ifdef _XBOX

#include <xtl.h>
#include <stdio.h>
#include "XBOXPCMSound.h"

//-----------------------------------------------------------------------------
// FourCC definitions
//-----------------------------------------------------------------------------
const DWORD FOURCC_RIFF   = 'FFIR';
const DWORD FOURCC_WAVE   = 'EVAW';
const DWORD FOURCC_FORMAT = ' tmf';
const DWORD FOURCC_DATA   = 'atad';
const DWORD FOURCC_WSMP   = 'pmsw';



//-----------------------------------------------------------------------------
// Name: struct WAVESAMPE
// Desc: RIFF chunk type that contains loop point information
//-----------------------------------------------------------------------------
struct WAVESAMPLE
{
    ULONG   cbSize;
    USHORT  usUnityNote;
    SHORT   sFineTune;
    LONG    lGain;
    ULONG   ulOptions;
    ULONG   cSampleLoops;
};

//-----------------------------------------------------------------------------
// Name: struct WAVESAMPLE_LOOP
// Desc: Loop point (contained in WSMP chunk)
//-----------------------------------------------------------------------------
struct WAVESAMPLE_LOOP
{
    ULONG cbSize;
    ULONG ulLoopType;
    ULONG ulLoopStart;
    ULONG ulLoopLength;
};

//-----------------------------------------------------------------------------
// Name: CRiffChunk()
// Desc: Object constructor.
//-----------------------------------------------------------------------------
CRiffChunk::CRiffChunk()
{
    // Initialize defaults
    m_fccChunkId   = 0;
    m_pParentChunk = NULL;
    m_hFile        = INVALID_HANDLE_VALUE;
    m_dwDataOffset = 0;
    m_dwDataSize   = 0;
    m_dwFlags      = 0;
}




//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: Initializes the object
//-----------------------------------------------------------------------------
VOID CRiffChunk::Initialize( FOURCC fccChunkId, const CRiffChunk* pParentChunk, 
                             HANDLE hFile )
{
    m_fccChunkId   = fccChunkId;
    m_pParentChunk = pParentChunk;
    m_hFile        = hFile;
}




//-----------------------------------------------------------------------------
// Name: Open()
// Desc: Opens an existing chunk.
//-----------------------------------------------------------------------------
HRESULT CRiffChunk::Open()
{
    RIFFHEADER rhRiffHeader;
    LONG       lOffset = 0;

    // Seek to the first byte of the parent chunk's data section
    if( m_pParentChunk )
    {
        lOffset = m_pParentChunk->m_dwDataOffset;

        // Special case the RIFF chunk
        if( FOURCC_RIFF == m_pParentChunk->m_fccChunkId )
            lOffset += sizeof(FOURCC);
    }
    
    // Read each child chunk header until we find the one we're looking for
    for( ;; )
    {
        if( INVALID_SET_FILE_POINTER == SetFilePointer( m_hFile, lOffset, NULL, FILE_BEGIN ) )
            return HRESULT_FROM_WIN32( GetLastError() );

        DWORD dwRead;
        if( 0 == ReadFile( m_hFile, &rhRiffHeader, sizeof(rhRiffHeader), &dwRead, NULL ) )
            return HRESULT_FROM_WIN32( GetLastError() );

        // Hit EOF without finding it
        if( 0 == dwRead )
            return E_FAIL;

        // Check if we found the one we're looking for
        if( m_fccChunkId == rhRiffHeader.fccChunkId )
        {
            // Save the chunk size and data offset
            m_dwDataOffset = lOffset + sizeof(rhRiffHeader);
            m_dwDataSize   = rhRiffHeader.dwDataSize;

            // Success
            m_dwFlags |= RIFFCHUNK_FLAGS_VALID;

            return S_OK;
        }

        lOffset += sizeof(rhRiffHeader) + rhRiffHeader.dwDataSize;
    }
}




//-----------------------------------------------------------------------------
// Name: Read()
// Desc: Reads from the file
//-----------------------------------------------------------------------------
HRESULT CRiffChunk::ReadData( LONG lOffset, VOID* pData, DWORD dwDataSize )
{
    // Seek to the offset
    DWORD dwPosition = SetFilePointer( m_hFile, m_dwDataOffset+lOffset, NULL, FILE_BEGIN );
    if( INVALID_SET_FILE_POINTER == dwPosition )
        return HRESULT_FROM_WIN32( GetLastError() );

    // Read from the file
    DWORD dwRead;
    if( 0 == ReadFile( m_hFile, pData, dwDataSize, &dwRead, NULL ) )
        return HRESULT_FROM_WIN32( GetLastError() );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: XBOXPCMSound()
// Desc: Object constructor.
//-----------------------------------------------------------------------------
XBOXPCMSound::XBOXPCMSound()
{
    m_hFile = INVALID_HANDLE_VALUE;
}




//-----------------------------------------------------------------------------
// Name: ~XBOXPCMSound()
// Desc: Object destructor.
//-----------------------------------------------------------------------------
XBOXPCMSound::~XBOXPCMSound()
{
    Close();
}




//-----------------------------------------------------------------------------
// Name: Open()
// Desc: Initializes the object.
//-----------------------------------------------------------------------------
HRESULT XBOXPCMSound::Open( const CHAR* strFileName )
{
    // If we're already open, close
    Close();
    
    // Open the file
    m_hFile = CreateFile( strFileName, GENERIC_READ, FILE_SHARE_READ, NULL, 
                          OPEN_EXISTING, 0L, NULL );
    if( INVALID_HANDLE_VALUE == m_hFile )
        return HRESULT_FROM_WIN32( GetLastError() );

    // Initialize the chunk objects
    m_RiffChunk.Initialize( FOURCC_RIFF, NULL, m_hFile );
    m_FormatChunk.Initialize( FOURCC_FORMAT, &m_RiffChunk, m_hFile );
    m_DataChunk.Initialize( FOURCC_DATA, &m_RiffChunk, m_hFile );
    m_WaveSampleChunk.Initialize( FOURCC_WSMP, &m_RiffChunk, m_hFile );

    HRESULT hr = m_RiffChunk.Open();
    if( FAILED(hr) )
        return hr;

    hr = m_FormatChunk.Open();
    if( FAILED(hr) )
        return hr;

    hr = m_DataChunk.Open();
    if( FAILED(hr) )
        return hr;

    // Wave Sample chunk is not required
    m_WaveSampleChunk.Open();

    // Validate the file type
    FOURCC fccType;
    hr = m_RiffChunk.ReadData( 0, &fccType, sizeof(fccType) );
    if( FAILED(hr) )
        return hr;

    if( FOURCC_WAVE != fccType )
        return HRESULT_FROM_WIN32( ERROR_FILE_NOT_FOUND );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: GetFormat()
// Desc: Gets the wave file format
//-----------------------------------------------------------------------------
HRESULT XBOXPCMSound::GetFormat( WAVEFORMATEX* pwfxFormat, DWORD dwFormatSize, DWORD * pdwRequiredSize )
{
    HRESULT hr = S_OK;
    DWORD dwValidSize = m_FormatChunk.GetDataSize();

    // We should be reading a wave format and/or
    // telling the caller the size of the format
    if( ( NULL == pwfxFormat ||
          0 == dwFormatSize ) && 
        NULL == pdwRequiredSize )
        return E_INVALIDARG;

    if( pwfxFormat && dwFormatSize )
    {
        // Read the format chunk into the buffer
        hr = m_FormatChunk.ReadData( 0, pwfxFormat, min(dwFormatSize, dwValidSize) );
        if( FAILED(hr) )
            return hr;

        // Zero out remaining bytes, in case enough bytes were not read
        if( dwFormatSize > dwValidSize )
            ZeroMemory( (BYTE*)pwfxFormat + dwValidSize, dwFormatSize - dwValidSize );
    }
    
    // Tell caller how much space they need for the format
    if( pdwRequiredSize )
    {
        *pdwRequiredSize = max( dwValidSize, sizeof( *pwfxFormat ) );
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: ReadSample()
// Desc: Reads data from the audio file.
//-----------------------------------------------------------------------------
HRESULT XBOXPCMSound::ReadSample( DWORD dwPosition, VOID* pBuffer, 
                               DWORD dwBufferSize, DWORD* pdwRead )
{                                   
    // Don't read past the end of the data chunk
    DWORD dwDuration;
    GetDuration( &dwDuration );

    if( dwPosition + dwBufferSize > dwDuration )
        dwBufferSize = dwDuration - dwPosition;

    HRESULT hr = S_OK;
    if( dwBufferSize )
        hr = m_DataChunk.ReadData( (LONG)dwPosition, pBuffer, dwBufferSize );

    if( pdwRead )
        *pdwRead = dwBufferSize;

    return hr;
}




//-----------------------------------------------------------------------------
// Name: GetLoopRegion
// Desc: Gets the loop region, in terms of samples
//-----------------------------------------------------------------------------
HRESULT XBOXPCMSound::GetLoopRegion( DWORD *pdwStart, DWORD *pdwLength )
{
    HRESULT hr = S_OK;
    WAVESAMPLE ws;
    WAVESAMPLE_LOOP wsl;

    // Check arguments
    if( NULL == pdwStart || NULL == pdwLength )
        return E_INVALIDARG;

    // Check to see if there was a wave sample chunk
    if( !m_WaveSampleChunk.IsValid() )
        return E_FAIL;
    
    // Read the WAVESAMPLE struct from the chunk
    hr = m_WaveSampleChunk.ReadData( 0, &ws, sizeof( WAVESAMPLE ) );
    if( FAILED( hr ) )
        return hr;

    // Currently, only 1 loop region is supported
    if( ws.cSampleLoops != 1 )
        return E_FAIL;

    // Read the loop region
    hr = m_WaveSampleChunk.ReadData( ws.cbSize, &wsl, sizeof( WAVESAMPLE_LOOP ) );
    if( FAILED( hr ) )
        return hr;
    
    // Fill output vars with the loop region
    *pdwStart = wsl.ulLoopStart;
    *pdwLength = wsl.ulLoopLength;

    return S_OK;
}
    



//-----------------------------------------------------------------------------
// Name: Close()
// Desc: Closes the object
//-----------------------------------------------------------------------------
VOID XBOXPCMSound::Close()
{
    if( m_hFile != INVALID_HANDLE_VALUE )
    {
        CloseHandle( m_hFile );
        m_hFile = INVALID_HANDLE_VALUE;
    }
}

#endif