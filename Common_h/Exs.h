#ifndef _EXS_H_
#define _EXS_H_

#include "vsystem_api.h"

extern VSYSTEM_API * _VSYSTEM_API;

#define URE_MESSAGE	"User Rised Exception"

#define EX_OFF
//#define ASSERT_OFF

/*#ifdef _XBOX
#ifdef NDEBUG
#undef EX_OFF
#endif
#endif
*/

enum XTYPE { FATAL, NON_FATAL};

class _EXS 
{
public:
	_EXS(XTYPE type,char * _szdata) 
	{
		xtype = type;
		if(_szdata) strcpy(string,_szdata); 
		else strcpy(string,"EXS( unknown )");
	};
	~_EXS(){};
	char  string[290];
	dword xtype;
};

#ifdef _THROW
#undef _THROW
#endif

#define THROW			{ _VSYSTEM_API->SetX();_VSYSTEM_API->Trace(URE_MESSAGE);_VSYSTEM_API->Trace("%s line %d",__FILE__,__LINE__); throw _EXS(FATAL,"URE"); }
#define _THROW(b)		{ _VSYSTEM_API->SetX();_VSYSTEM_API->Trace(URE_MESSAGE);_VSYSTEM_API->Trace("%s line %d",__FILE__,__LINE__);_VSYSTEM_API->Trace(TEXT(#b)); throw _EXS(FATAL,TEXT(#b));}
#define __THROW(a,b)	{ _VSYSTEM_API->SetXNF();_VSYSTEM_API->Trace(URE_MESSAGE);_VSYSTEM_API->Trace("%s line %d",__FILE__,__LINE__);_VSYSTEM_API->Trace(TEXT(#b)); throw _EXS(a,TEXT(#b)); }

#ifdef EX_OFF

#define GUARD(block)	
#define UNGUARD			

#else

#define GUARD(block)	{ static const TCHAR block_name[] = TEXT(#block); try {
#define UNGUARD			} catch(_EXS exs) { _VSYSTEM_API->Trace((char*)block_name); if(exs.xtype == FATAL) { _VSYSTEM_API->SetX(); throw;}} catch(char * s) { _VSYSTEM_API->SetX();_VSYSTEM_API->Trace("User Rised Exception");_VSYSTEM_API->Trace((char*)block_name); throw _EXS(FATAL,s);} catch(...) { _VSYSTEM_API->SetX();_VSYSTEM_API->Trace("System Rised Exception");_VSYSTEM_API->Trace((char*)block_name); throw _EXS(FATAL,"unknown");}}

#endif
// core will automatically generate exceptions in following situation (if flag is set)

#define _X_NO_MEM				0x1
#define _X_NO_FILE				0x2
#define _X_NO_FILE_READ			0x4
#define _X_NO_FILE_WRITE		0x8
#define _X_NO_CREATE_ENTITY		0x10
#define _X_NO_CLASS				0x20
#define _X_NO_SERVICE			0x40
#define _X_NO_ENTITY			0x80
#define _X_NO_LAYER				0x100

#ifdef ASSERT_OFF

#define Assert(exp)
#define AssertE(exp)

#else

inline void __Storm_Assert__(bool expression, const char * file, long line, const char * str)
{
	if(!expression)
	{
		if(str)
		{
			_VSYSTEM_API->Trace("Assert failed in %s line %d, expression string %s", file, line, str);
		}else{
			_VSYSTEM_API->Trace("Assert failed in %s line %d", file, line);
		}
#ifdef EX_OFF
		_asm int 3;
#else
		throw "assert";
#endif
	}
}

#define Assert(exp) __Storm_Assert__((exp) != 0, __FILE__,__LINE__,#exp);
#define AssertE(exp) __Storm_Assert__((exp) != 0, __FILE__,__LINE__,#exp);

#endif

#endif