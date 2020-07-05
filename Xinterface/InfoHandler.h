#ifndef _INFO_HANDLER_H_
#define _INFO_HANDLER_H_

#include "..\\common_h\\dx8render.h"

class InfoHandler : public ENTITY
{
	VDX8RENDER * m_rs;

public:
	InfoHandler();
	~InfoHandler();
	bool	Init();
	void Execute(dword delta_time);
	void Realize(dword delta_time);
    dword _cdecl ProcessMessage(MESSAGE & message);

protected:
	void	StringToBufer(char * outStr, int sizeBuf, char * inStr, int copySize);
	char *	GetCutString( char * pstr, int nOutWidth, float fScale );
	bool	DoPreOut();

	IDirect3DSurface8 * m_pSurface;
	IDirect3DSurface8 * m_pRenderTarget;

	/*long	m_idVBuf;

	int		m_dwOutputWidth;
	int		m_nOutCenterX, m_nOutCenterY;

	DWORD	m_dwBackFillColor;
	DWORD	m_dwFrontFillColor;

	long	m_idFont;

	struct	STRING_LIST
	{
		int x,y;
		char * strData;
		STRING_LIST	* next;
	} * m_strList;*/
};

#endif
