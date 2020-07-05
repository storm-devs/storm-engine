#ifndef _D_TYPES_H_
#define _D_TYPES_H_

typedef unsigned long dword;
typedef unsigned short word;
typedef unsigned char byte;

#define null 0

#define _F				__FILE__
#define _L				__LINE__
#define _FL_			__FILE__, __LINE__
#define _FL				__FILE__, __LINE__
#define _FILELINE_		_FL_

struct FPOINT
{
	float	x,y;
};

struct FRECT
{
	union
	{
		struct { float	x1,y1,x2,y2; };
		struct { float	xs,ys,xe,ye; };
		struct { float	x_start,y_start,x_end,y_end; };
		struct { float	left,top,right,bottom; };
	};
};

inline bool IsPointInFrect(float x, float y, FRECT &r)
{
	if (x < r.x1 || x > r.x2 || y < r.y1 || y > r.y2) return false;
	return true;
}

inline bool IsPointInFrect(FPOINT &fp, FRECT &r)
{
	if (fp.x < r.x1 || fp.x > r.x2 || fp.y < r.y1 || fp.y > r.y1) return false;
	return true;
}

inline bool IntersectFRects(FRECT &r1, FRECT &r2)
{
	if (r1.x2 < r2.x1 || r2.x2 < r1.x1 || r1.x1 > r2.x2 || r2.x1 > r1.x2) return false;
	if (r1.y2 < r2.y1 || r2.y2 < r1.y1 || r1.y1 > r2.y2 || r2.y1 > r1.y2) return false;
	return true;
}

struct CODESOURCE
{
	CODESOURCE(){pFileName = 0; line = 0xffffffff;};
	char * pFileName;
	dword  line;
};

struct MSTATE
{
	unsigned long nBlocksNum;
	unsigned long nMemorySize;
};

#endif