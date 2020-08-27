//========= Copyright Valve LLC, All rights reserved. ============
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================

// stdafx_ps3.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently on the PS3 platform
//

#include <PSGL/psgl.h>
#include <PSGL/psglu.h>
#include <cell/dbgfont.h>
#include <cell/error.h>
#include <cell/fios/fios_common.h>
#include <cell/fios/fios_configuration.h>
#include <cell/fios/fios_memory.h>
#include <cell/fios/fios_time.h>
#include <cell/pad/libpad.h>
#include <sys/memory.h>
#include <sys/paths.h>
#include <sys/process.h>
#include <sys/prx.h>
#include <sys/spu_initialize.h>
#include <sys/tty.h>
#include <sys/types.h>
#include <sysutil/sysutil_syscache.h>

//#define PS3_MTT_DEBUG

#ifdef PS3_MTT_DEBUG
#include "../external/libmtt/libmtt/libmtt_log.h"
#endif

extern CellDbgFontConsoleId g_DbgFontConsoleID;