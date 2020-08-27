#ifndef _APCMCODED_H_
#define _APCMCODED_H_

#include "..\common_h\vmodule_api.h"
#include "SoundDefines.h"
#include <dsound.h>
//#include "XBOXPCMSound.h"
#include "THashDataMap.h"
#include "TSoundCore.h"

typedef THashDataMapTemplate<tCachedData> THashPCMMap;
///////////////////////////////////////////////////////////////////
// CLASS DEFINITION
///////////////////////////////////////////////////////////////////
class APCMCoded
{
  public:
    APCMCoded();
    virtual ~APCMCoded();

    bool OpenWAVFile(const char *_filename, HANDLE *_inFile, WAVEFORMATEX *_waveFormat, DWORD *_dataSize,
                     tCachedData **_cachedData, bool *_ifStartCaching, bool *_ifLogError);
    bool LoadDataToBuffer(LPDIRECTSOUNDBUFFER _buffer, HANDLE _inFile, DWORD _size, tCachedData *cachedData);
    static void GetStatistics(long *_cachedBytes, long *_maxCachedBytes)
    {
        *_cachedBytes = allCachedBytes;
        *_maxCachedBytes = allMaxCachedBytes;
    }
    static void UncacheLeastUsed();
    bool UnloadIfThisCached(tCachedData *_cachedData);
    static void DeleteCachedData(int _i);
    static void UnloadAllPCMData();

  protected:
    void DeleteCachedData();

    TSoundCore *core;
    long bufferSize;

  private:
    long cachedI;
    static THashPCMMap pcmMap;
    static long allCachedBytes;
    static long allMaxCachedBytes;
};

#endif // !defined
