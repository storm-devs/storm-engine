#ifndef BI_STRING_H
#define BI_STRING_H

#include "defines.h"

class BIImageRender;

class BIString : public IBIString
{
public:
	BIString( BIImageRender* pImgRender, VDX8RENDER* rs );
	~BIString();

	long GetPrioritet() {return m_nPrioritet;}
	void SetPrioritet(long nPrior) {m_nPrioritet=nPrior;}

	void Render();
	void RenderPrioritetRange(long nBegPrior,long nEndPrior) {if(m_nPrioritet>=nBegPrior && m_nPrioritet<=nEndPrior) Render();}

	virtual void SetColor( dword color ) {m_dwColor=color;}
	virtual void SetScale( float fScale ) {m_fScale=fScale;}
	virtual void SetFont( const char* pcFontName );
	virtual void SetAlign( long nHorzAlign, long nVertAlign );
	virtual void SetPosition( long nLeft, long nTop, long nRight, long nBottom );
	virtual void SetString( const char* pcStr );

protected:
	void Release();
	void UpdateString();

	BIImageRender* m_pImgRender;
	VDX8RENDER* m_pRS;
	FRECT m_BasePos;
	long m_nPrioritet;
	string m_sText;

	dword m_dwColor;
	float m_fScale;
	long m_nFontID;

	POINT m_pntOutPos;

	bool m_bUpdateString;
};

#endif
