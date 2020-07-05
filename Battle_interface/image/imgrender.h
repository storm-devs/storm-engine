#ifndef BI_IMAGERENDER_H
#define BI_IMAGERENDER_H

#include "material.h"

class BIImageRender
{
public:
	BIImageRender( VDX8RENDER* pRS );
	~BIImageRender();

	void Render();
    dword _cdecl ProcessMessage(MESSAGE & message);

	IBIImage* CreateImage( BIImageType type, const char* pcTextureName, dword color, FRECT& uv, long nLeft,long nTop, long nRight,long nBottom, long nPrior=ImagePrioritet_DefaultValue, const char* pcTechniqueName=0 );
	IBIImage* CreateImage( BIImageType type, const char* pcTextureName, dword color, FRECT& uv, RECT& pos, long nPrior=ImagePrioritet_DefaultValue, const char* pcTechniqueName=0 );

	BIImageMaterial* FindMaterial( const char* pcTextureName, const char* pcTechniqueName );
	BIImageMaterial* CreateMaterial( const char* pcTextureName, const char* pcTechniqueName=0 );
	void DeleteMaterial( BIImageMaterial* pMat );

	void ReleaseAllImages();
	long GetImageQuantity();

	void MaterialSorting();
	void ChangeMaterialPosByPrioritet(BIImageMaterial* pMat);

	__forceinline void TranslateBasePosToRealPos(float fXBase, float fYBase, float& fXReal, float& fYReal);

	IBIString* CreateString( const char* text, const char* font_name, float font_scale, dword font_color, long valign, long halign, long nLeft, long nTop, long nRight, long nBottom, long nPrior=ImagePrioritet_DefaultValue);
	IBIString* CreateString( const char* text, const char* font_name, float font_scale, dword font_color, long valign, long halign, RECT& pos, long nPrior=ImagePrioritet_DefaultValue);
	void DeleteString( IBIString* str );
	void ReleaseAllStrings();
	void CutPrioritetRangeByStrings();

	void SetBaseScreenSize(long nHSize, long nVSize, long nHOffset, long nVOffset);

protected:
	void Release();
	bool GetFirstPrioritetRange();
	bool GetNextPrioritetRange();

	long m_nBeginOutputPrioritet;
	long m_nEndOutputPrioritet;

	VDX8RENDER * m_pRS;
	array<BIImageMaterial*> m_apMaterial;
	array<IBIString*> m_apStrings;
	//bool m_bDeleteEverythink;

	float m_fHScale, m_fVScale;
	float m_fHOffset, m_fVOffset;
};

__forceinline void BIImageRender::TranslateBasePosToRealPos(float fXBase, float fYBase, float& fXReal, float& fYReal)
{
	fXReal = (m_fHOffset + fXBase) * m_fHScale;
	fYReal = (m_fVOffset + fYBase) * m_fVScale;
}

#endif
