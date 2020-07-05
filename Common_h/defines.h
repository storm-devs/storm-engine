#ifndef DEFINES_HPP
#define DEFINES_HPP

// includes
#include "math3d.h"

// Constants

#define PI		3.14159265358979323846f
#define PIm2	(PI * 2.0f)
#define PId2	(PI / 2.0f)
#define PId4	(PI / 4.0f)

#define RDTSC_B(x)	{ _asm rdtsc _asm mov x,eax }
#define RDTSC_E(x)	{ _asm rdtsc _asm sub eax,x _asm mov x,eax }

// Defines
#ifdef RGB
	#undef RGB
#endif

#ifdef DELETE
	#undef DELETE
#endif

#ifdef SQR
	#undef SQR
#endif

inline DWORD F2DW( FLOAT f ) { return *((DWORD*)&f); }

#ifdef _XBOX
#define IS_XBOX(a,b)		a
#else
#define IS_XBOX(a,b)		b
#endif

#define FTOL(l,f)			{ __asm fld dword ptr [f] __asm fistp dword ptr l }
#define GET_DATA(x,p)		{ memcpy(&(x),p,sizeof(x));p+=sizeof(x); }
#define FREE(x)				{ if (x) free(x); x=0; }
#define DELETE(x)			{ if (x) delete x; x=0; }
#define DELETE_ARRAY(x)		{ if (x) delete []x; x=0; }
#define DELETE_ENTITY(x)	{ if (_CORE_API->ValidateEntity(&x)) _CORE_API->DeleteEntity(x); }
#define RELEASE(x)			{ if (x) x->Release(); x = 0; }
#define SQR(a)				( (a)*(a) )
#define FRAND(x)			( (x) * (float)rand()/(float)RAND_MAX  )
#define SIGN(x)				( (x)<0 ? -1.0f : 1.0f )
#define SIGNZ(x)			( (x)==0.0f ? 0.0f : ((x)<0.0f ? -1.0f : 1.0f) )
#define MAX(x,y)			( ((x) > (y)) ? (x) : (y) )
#define MIN(x,y)			( ((x) < (y)) ? (x) : (y) )
#define MAX3(x,y,z)			( ((x) > (y)) ? ((x) > (z) ? (x) : (z)) : ((y) > (z) ? (y) : (z)) )
#define MIN3(x,y,z)			( ((x) < (y)) ? ((x) < (z) ? (x) : (z)) : ((y) < (z) ? (y) : (z)) )
#define CLAMP(x)			( ((x) > 1.0f) ? 1.0f : (((x) < 0.0f) ? 0.0f : (x)) )
#define RGB(r,g,b)			( DWORD(b)|(DWORD(g)<<8L)|(DWORD(r)<<16L) )
#define RGB565(r,g,b)		WORD( ((DWORD(b) >> 3L)) | ((DWORD(g) >> 2L) << 5L) | ((DWORD(r) >> 3L) << 11L) )
#define RGB1555(r,g,b)		WORD( ((DWORD(b) >> 3L)) | ((DWORD(g) >> 3L) << 5L) | ((DWORD(r) >> 3L) << 10L) )
#define ARGB1555(a,r,g,b)	WORD( (DWORD(a&1L) << 15L) | ((DWORD(b) >> 3L)) | ((DWORD(g) >> 3L) << 5L) | ((DWORD(r) >> 3L) << 10L) )
#define ARGB(a,r,g,b)		( DWORD(b)|(DWORD(g)<<8L)|(DWORD(r)<<16L)|(DWORD(a)<<24L) )
#define ZERO(x)				{ ZeroMemory(&x,sizeof(x)); }
#define ZERO2(x,y)			{ ZERO(x) ZERO(y) }
#define ZERO3(x,y,z)		{ ZERO2(x,y) ZERO(z) }
#define ZERO4(x,y,z,a)		{ ZERO2(x,y) ZERO2(z,a) }
#define PZERO(x,size)		{ ZeroMemory(x,size); }
#define COPY_STRING(a,b)	{ a = NEW char[strlen(b)+1]; if (a) strcpy(a,b); }
#define COLOR2VECTOR(a)		CVECTOR(float((a&0xFF0000)>>0x10),float((a&0xFF00)>>0x8),float(a&0xFF));
#define COLOR2VECTOR4(a)	CVECTOR4(float((a&0xFF0000)>>0x10) / 255.0f, float((a&0xFF00)>>0x8) / 255.0f, float(a&0xFF) / 255.0f, float((a&0xFF000000)>>0x18) / 255.0f);

// game defines
#define METERS2KNOTS(x)		((x) * 3600.0f / 1853.0f)
#define KNOTS2METERS(x)		((x) * 1853.0f / 3600.0f)

template<class T> void Swap(T & t1, T & t2) { T tmp; tmp = t1; t1 = t2; t2 = tmp; };
template<class T> T Clamp(T t) { if (t < (T)0) return (T)0; if (t > (T)1) return (T)1; return t; };
template<class T> T Clamp(T t, T Min, T Max) { if (t < Min) return Min; if (t > Max) return Max; return t; };
template<class T> T Bring2Range(T Min1, T Max1, T Min2, T Max2, T Value) { if (Value < Min2) Value = Min2; if (Value > Max2) Value = Max2; float Delta = float(Value - Min2) / float(Max2 - Min2); return Min1 + Delta * (Max1 - Min1); };
template<class T> T Bring2RangeNoCheck(T Min1, T Max1, T Min2, T Max2, T Value) { float Delta = float(Value - Min2) / float(Max2 - Min2); return Min1 + Delta * (Max1 - Min1); };
template<class T> T Min(T t1, T t2) { return ((t1 < t2) ? t1 : t2); };
template<class T> T Max(T t1, T t2) { return ((t1 > t2) ? t1 : t2); };
template<class T> T Sqr(T t1) { return (t1 * t1); };

#include "inlines.h"

#endif