#ifndef _GI_EDITOR_LIST_H
#define _GI_EDITOR_LIST_H

#include "..\inode.h"
#include "defines.h"

class GIEditor;
class CXI_IMAGE;
class GIFont;

class GIEditorList : public GIEditorObject
{
public:
	GIEditorList(GIEditor* pEditor);
	~GIEditorList();

	void Release();
	void Create();

	void Render();

	void SetPosition( float fLeft, float fTop, float fRight, float fBottom );

	void AddString( string& sNewStr );
	void RemoveString( string& sStr );
	void RemoveString( long nIndex );
	void RemoveAllStrings();
	long FindString( string& sStr );
	long GetStringQuantity() {return m_aStrings.Size();}
	string& GetString( long nIndex );
	long GetSelectIndex() {return m_nSelectIndex;}
	void SetSelectIndex( long nIndex );
	string& GetSelectString() {return GetString(m_nSelectIndex);}

	bool CheckMouseInside( float fX, float fY );
	void MakeMouseClick( float fX, float fY );
	void DoKeyChecking();

	void IncrementSelectedLine( bool bIncr );

protected:
	void UpdateSelectPosition();

	GIEditor* m_pEditor;

	CXI_IMAGE* m_pBackImage;
	CXI_IMAGE* m_pSelectImage;

	bool m_bShowSelect;
	FXYRECT m_frBackRect;
	FXYRECT m_frSelectRect;

	string m_sEmptyString;
	GIFont* m_pFont;

	FXYRECT m_frStrOffset;
	float m_fStrLineStep;
	array<string> m_aStrings;
	long m_nSelectIndex;
	long m_nTopIndex;
	long m_nLineQuantity;

	float m_fUpPressTime;
	float m_fDownPressTime;
	float m_fKeyRepeatDelay;
	float m_fKeyRepeatInterval;

public:
	dword m_dwStatus;
	GIEditorEventHandler* m_pChangeSelected;
};

#endif
