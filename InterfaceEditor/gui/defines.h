#ifndef _IEDITOR_DEFINES_H_
#define _IEDITOR_DEFINES_H_

#include "..\..\common_h\vsystem_api.h"
#include "..\..\common_h\CVector.h"

#define EdVF_ColorUV	(D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1|D3DFVF_TEXTUREFORMAT2)
struct EdVertexColorUV
{
	CVECTOR vPos;
	dword dwColor;
	float tu,tv;
};

struct EdPoint
{
	float x,y;

	EdPoint();
	EdPoint(float x, float y);
	EdPoint(const EdPoint& ep);

	const EdPoint& operator = (EdPoint& ep);
	const EdPoint& operator += (EdPoint& ep);
	const EdPoint& operator -= (EdPoint& ep);
	const EdPoint& operator *= (float fK);
};

inline EdPoint::EdPoint()
{
	x = y = 0.f;
}

inline EdPoint::EdPoint(float x, float y)
{
	this->x = x;
	this->y = y;
}

inline EdPoint::EdPoint(const EdPoint& ep)
{
	x = ep.x;
	y = ep.y;
}

inline const EdPoint& EdPoint::operator = (EdPoint& ep)
{
	x = ep.x;
	y = ep.y;
	return (*this);
}

inline const EdPoint& EdPoint::operator += (EdPoint& ep)
{
	x += ep.x;
	y += ep.y;
	return (*this);
}

inline const EdPoint& EdPoint::operator -= (EdPoint& ep)
{
	x -= ep.x;
	y -= ep.y;
	return (*this);
}

inline const EdPoint& EdPoint::operator *= (float fK)
{
	x *= fK;
	y *= fK;
	return (*this);
}

inline EdPoint operator + (EdPoint& ep1, EdPoint& ep2)
{
	EdPoint ep(ep1);
	ep += ep2;
	return ep;
}

inline EdPoint operator - (EdPoint& ep1, EdPoint& ep2)
{
	EdPoint ep(ep1);
	ep -= ep2;
	return ep;
}




struct EdRect
{
	union
	{
		struct {
			float x, y;
			float w, h;
		};
		struct {
			EdPoint pos;
			EdPoint size;
		};
	};

	EdRect();
	EdRect(const EdRect& er);

	EdRect& operator = (const EdRect& er);

	float Right() const;
	float Bottom() const;
};

inline EdRect::EdRect()
{
	x = y = w = h = 0.f;
}

inline EdRect::EdRect( const EdRect& er )
{
	x = er.x;
	y = er.y;
	w = er.w;
	h = er.h;
}

inline EdRect& EdRect::operator = (const EdRect& er)
{
	x = er.x;
	y = er.y;
	w = er.w;
	h = er.h;
	return (*this);
}

inline float EdRect::Right() const
{
	return x + w;
}

inline float EdRect::Bottom() const
{
	return y + h;
}

#endif
