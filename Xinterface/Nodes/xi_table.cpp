#include <stdio.h>
#include "xi_table.h"
#include "xi_scroller.h"

#define ALIGN_BOTTOM	16
#define ALIGN_TOP		17
#define NOTUSE_OFFSET	-1000.f

XI_TableLineDescribe::XI_TableLineDescribe(CXI_TABLE* pTable) :
	m_aCell(_FL)
{
	m_pTable = pTable;
	m_nRowIndex = -1;
	m_bUseSpecColor = false;
	if( m_pTable && m_pTable->m_bVariableLineHeight ) m_nHeight = m_pTable->m_nNormalLineHeight;
	else m_nHeight = 0;
}

XI_TableLineDescribe::~XI_TableLineDescribe()
{
	m_aCell.DelAllWithPointers();
	m_pTable = 0;
}

void XI_TableLineDescribe::Draw( float fTop )
{
	float fX = (float)m_pTable->m_rect.left;
	for( long n=0; n<m_aCell; n++ )
	{
		m_aCell[n]->Draw( fX, fTop );
		fX += m_pTable->m_anColsWidth[n];
	}
}

void XI_TableLineDescribe::DrawSpecColor( float fTop )
{
	if( m_bUseSpecColor )
	{
		XI_NOTEX_VERTEX v[4];
		v[0].color = v[1].color = v[2].color = v[3].color = m_dwSpecColor;
		v[0].pos.z = v[1].pos.z = v[2].pos.z = v[3].pos.z = 1.f;
		float fBottom = fTop + (float)m_pTable->m_anRowsHeights[m_nRowIndex];
		float fLeft = (float)m_pTable->m_rect.left;
		float fRight = (float)m_pTable->m_rect.right;
		v[0].pos.x = fLeft;		v[0].pos.y = fTop;
		v[1].pos.x = fLeft;		v[1].pos.y = fBottom;
		v[2].pos.x = fRight;	v[2].pos.y = fTop;
		v[3].pos.x = fRight;	v[3].pos.y = fBottom;
		m_pTable->m_rs->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, XI_NOTEX_FVF, 2, v, sizeof(XI_NOTEX_VERTEX), "iRectangle" );
	}
}

void XI_TableLineDescribe::SetData( long nRowIndex, ATTRIBUTES* pLA, bool bHeader )
{
	m_nRowIndex = nRowIndex;
	char pcAttrName[64];
	m_bUseSpecColor = false;
	dword dwSpecColor = 0;
	if( pLA ) dwSpecColor = pLA->GetAttributeAsDword("speccolor",0);
	if( dwSpecColor != 0 )
	{
		m_bUseSpecColor = true;
		m_dwSpecColor = dwSpecColor;
	}

	if( pLA && m_pTable->m_bVariableLineHeight ) {
		m_nHeight = pLA->GetAttributeAsDword("height",m_pTable->m_nNormalLineHeight);
	} else m_nHeight = m_pTable->m_anRowsHeights[nRowIndex];

	for( long c=0; c<10000; c++ )
	{
		sprintf( pcAttrName, "td%d", c+1 );
		ATTRIBUTES* pA = (pLA ? pLA->GetAttributeClass( pcAttrName ) : 0);
		if( !pA && c>=m_pTable->m_nColQuantity ) break;
		XI_TableCellDescribe* pTD = 0;
		if( c >= m_aCell ) {
			pTD = NEW XI_TableCellDescribe( m_pTable, this );
			m_aCell.Add( pTD );
		} else {
			pTD = m_aCell[c];
		}
		Assert( pTD );
		pTD->SetData( c, pA, bHeader );
	}
	while( c < m_aCell )
	{
		DELETE( m_aCell[c] );
		m_aCell.DelIndex( c );
	}
}

long XI_TableLineDescribe::GetLineHeight()
{
	if( !m_pTable ) return 0;
	if( m_pTable->m_bVariableLineHeight ) return m_nHeight;
	if( m_nRowIndex<0 && m_nRowIndex>=m_pTable->m_anRowsHeights ) return 0;
	return m_pTable->m_anRowsHeights[m_nRowIndex];
}



XI_TableCellDescribe::XI_TableCellDescribe(CXI_TABLE* pTable, XI_TableLineDescribe* pLine) :
	m_aStrings(_FL),
	m_aImage(_FL)
{
	m_pTable = pTable;
	m_pLine = pLine;
	m_nFontIndex = -1;
}

XI_TableCellDescribe::~XI_TableCellDescribe()
{
	m_aImage.DelAll();
	m_aStrings.DelAll();
	m_pLine = 0;
	m_pTable = 0;
}

void XI_TableCellDescribe::Draw( float fLeft, float fTop )
{
	long n;

	fLeft += m_pTable->m_pntSpaceSize.x;
	fTop += m_pTable->m_pntSpaceSize.y;
	if( m_aImage.Size() > 0 ) {
		long nX = (long)fLeft + m_nLeftLineWidth;
		long nY = (long)fTop + m_nTopLineHeight;
		for( n=0; n<m_aImage; n++ )
			if( m_aImage[n].pImage )
				m_aImage[n].pImage->Draw( nX+m_aImage[n].offset.x, nY+m_aImage[n].offset.y, IPType_LeftTop );
	}

	fLeft += m_TextOffset.x;
	fTop += m_TextOffset.y;
	float fY = 0.f;
	for( n=0; n<m_aStrings; n++ )
	{
		if( m_aStrings[n].offset.y != NOTUSE_OFFSET ) fY = m_aStrings[n].offset.y;
		float fNewY = fY + m_pTable->m_rs->CharHeight( m_nFontID ) * m_fScale;
		if( fNewY >= m_pLine->GetLineHeight() ) break; // больше не влазит в таблицу

		CXI_UTILS::PrintTextIntoWindow( m_pTable->m_rs, m_nFontIndex<0?m_nFontID:m_pTable->m_anFontList[m_nFontIndex],
			m_dwColor, ALIGN_LEFT, true, m_fScale, m_pTable->m_screenSize.x, m_pTable->m_screenSize.y,
			(long)(fLeft+m_aStrings[n].offset.x), (long)(fTop+fY), m_aStrings[n].str.GetBuffer(),
			(long)fLeft, (long)fTop, m_pTable->m_anColsWidth[m_nColIndex], 20);

		fY = fNewY;
	}
}

void XI_TableCellDescribe::SetData( long nColIndex, ATTRIBUTES* pAttr, bool bHeader )
{
	if( !pAttr ) {
		// пустая ячейка
		m_aStrings.DelAll();
		m_aImage.DelAll();
		return;
	}
	ATTRIBUTES* pA;
	const char *pcStr, *pcTmpStr;

	m_nColIndex = nColIndex;

	m_nLeftLineWidth = nColIndex==0 ? m_pTable->m_nBorderWidth : m_pTable->m_nVLineWidth;
	m_nTopLineHeight = bHeader ? m_pTable->m_nHeaderLineHeight : m_pTable->m_nHLineHeight;

	// читаем картинку
	long nIconQuantity = 0;
	pA = pAttr->GetAttributeClass( "icon" );
	if( pA )
	{
		if( m_aImage.Size()==0 )
			m_aImage.Add();
		LoadImageParam( &m_aImage[nIconQuantity], pA );
		nIconQuantity++;
	}
	else {
		char tmpaname[16];
		while(true) {
			_snprintf( tmpaname,sizeof(tmpaname),"icon%d", nIconQuantity+1 );
			pA = pAttr->GetAttributeClass( tmpaname );
			if( !pA ) break;
			if( (long)m_aImage <= nIconQuantity )
				m_aImage.Add();
			LoadImageParam( &m_aImage[nIconQuantity], pA );
			nIconQuantity++;
		}
	}
	// удалить лишние картинки
	while( (long)m_aImage > nIconQuantity )
		m_aImage.DelIndex( nIconQuantity );

	// читаем строку
	m_dwColor = pAttr->GetAttributeAsDword( "color", bHeader ? m_pTable->m_dwFontTitleColor : m_pTable->m_dwFontCellColor );
	m_fScale = pAttr->GetAttributeAsFloat( "scale", bHeader ? m_pTable->m_fFontTitleScale : m_pTable->m_fFontCellScale );
	m_nFontID = bHeader ? m_pTable->m_nFontTitleID : m_pTable->m_nFontCellID;
	m_nFontIndex = pAttr->GetAttributeAsDword( "fontidx", -1 );
	if( m_nFontIndex<0 || m_nFontIndex>=m_pTable->m_anFontList ) m_nFontIndex = -1;
	m_nAlignment = bHeader ? m_pTable->m_nFontTitleAlignment : m_pTable->m_nFontCellAlignment;
	m_nVAlignment = bHeader ? m_pTable->m_nFontTitleVAlignment : m_pTable->m_nFontCellVAlignment;
	pcTmpStr = pAttr->GetAttribute( "align" );
	if( pcTmpStr )
	{
		if( stricmp(pcTmpStr,"left") == 0 ) m_nAlignment = ALIGN_LEFT;
		if( stricmp(pcTmpStr,"center") == 0 ) m_nAlignment = ALIGN_CENTER;
		if( stricmp(pcTmpStr,"right") == 0 ) m_nAlignment = ALIGN_RIGHT;
	}
	pcTmpStr = pAttr->GetAttribute( "valign" );
	if( pcTmpStr )
	{
		if( stricmp(pcTmpStr,"top") == 0 ) m_nVAlignment = ALIGN_TOP;
		if( stricmp(pcTmpStr,"center") == 0 ) m_nVAlignment = ALIGN_CENTER;
		if( stricmp(pcTmpStr,"bottom") == 0 ) m_nVAlignment = ALIGN_BOTTOM;
	}

	m_TextOffset.x = m_TextOffset.y = 0;
	if( pAttr->GetAttribute("textoffset") )
		sscanf( pAttr->GetAttribute("textoffset"), "%f,%f", &m_TextOffset.x,&m_TextOffset.y );
	m_TextOffset.y += m_nTopLineHeight;

	long nWidth = m_pTable->m_anColsWidth[ nColIndex ] - 2*m_pTable->m_pntSpaceSize.x - m_nLeftLineWidth - (long)m_TextOffset.x;
	if( nWidth <= 0 ) {
		nWidth = 32;
	}

	pcStr = pAttr->GetAttribute( "str" );
	array<string> asStr(_FL);
	CXI_UTILS::SplitStringByWidth( pcStr, m_nFontID, m_fScale, nWidth, asStr );

	if( m_nVAlignment != ALIGN_TOP )
	{
		float fVOffset = (float)( m_pLine->GetLineHeight() - 2*m_pTable->m_pntSpaceSize.y - m_nTopLineHeight ) -
						m_fScale * m_pTable->m_rs->CharHeight( m_nFontID ) * asStr.Size();
		if( m_nVAlignment == ALIGN_CENTER )
			fVOffset *= .5f;
		m_TextOffset.y += fVOffset;
	}

	m_aStrings.DelAll();
	pA = pAttr->GetAttributeClass( "textoffsets" );
	for( long n=0; n<asStr; n++ )
	{
		m_aStrings.Add();
		asStr[n].TrimRight();
		m_aStrings[n].str = asStr[n];

		m_aStrings[n].offset.y = CXI_UTILS::GetByStrNumFromAttribute_Float( pA, "s",n+1, NOTUSE_OFFSET );
		m_aStrings[n].offset.x = (float)m_nLeftLineWidth;
		if( m_nAlignment == ALIGN_CENTER )
			m_aStrings[n].offset.x += (float)((nWidth - m_pTable->m_rs->StringWidth( (char*)asStr[n].GetBuffer(), m_nFontID, m_fScale)) / 2);
		else if( m_nAlignment == ALIGN_RIGHT )
			m_aStrings[n].offset.x += (float)(nWidth - m_pTable->m_rs->StringWidth( (char*)asStr[n].GetBuffer(), m_nFontID, m_fScale ));
	}
}

void XI_TableCellDescribe::LoadImageParam(ImgDescribe* pImg,ATTRIBUTES* pA)
{
	Assert( pImg && pA );
	const char* pcStr;
	long nW,nH,nImgAlign;

	if( !pImg->pImage ) {
		pImg->pImage = NEW CXI_IMAGE;
		Assert( pImg->pImage );
	}

	if( pA->GetAttribute("group") ) {
		pImg->pImage->LoadFromBase( pA->GetAttribute("group"), pA->GetAttribute("image"), true );
	} else {
		if( pA->GetAttribute("texture") )
			pImg->pImage->LoadFromFile( pA->GetAttribute("texture") );

		if( pA->GetAttribute("texturepointer") )
			pImg->pImage->SetPointerToTexture( (IDirect3DTexture8*)pA->GetAttributeAsDword("texturepointer") );

		float fL=0.f, fT=0.f, fR=1.f, fB=1.f;
		if( pA->GetAttribute("uv") )
			sscanf( pA->GetAttribute("uv"), "%f,%f,%f,%f", &fL,&fT, &fR,&fB );
		pImg->pImage->SetUV( fL,fT, fR,fB );
	}
	nW = pA->GetAttributeAsDword("width",0);
	nH = pA->GetAttributeAsDword("height",0);
	if( pA->GetAttributeAsDword("cellsize",0)>0 )
	{
		nW = m_pTable->m_anColsWidth[m_nColIndex] - m_nLeftLineWidth;
		nH = m_pLine->GetLineHeight() - m_nTopLineHeight;
	}
	if( nW>0 && nH>0 )
		pImg->pImage->SetSize( nW, nH );
	pImg->offset.x = pImg->offset.y = 0;
	if( pA->GetAttribute("offset") )
		sscanf( pA->GetAttribute("offset"), "%d,%d", &pImg->offset.x,&pImg->offset.y );
	if( (pcStr=pA->GetAttribute("valign")) != null ) {
		nImgAlign = ALIGN_TOP;
		if( stricmp(pcStr,"center")==0 ) nImgAlign = ALIGN_CENTER;
		else if( stricmp(pcStr,"bottom")==0 ) nImgAlign = ALIGN_BOTTOM;
		if( nImgAlign!=ALIGN_TOP ) {
			nH = m_pLine->GetLineHeight() - 2*m_pTable->m_pntSpaceSize.y - m_nTopLineHeight - pImg->pImage->GetHeight();
			if( nImgAlign==ALIGN_CENTER ) nH /= 2;
			pImg->offset.y += nH;
		}
	}
}



CXI_TABLE::CXI_TABLE() :
	m_aLine(_FL),
	m_anColsWidth(_FL),
	m_anRowsHeights(_FL),
	m_anFontList(_FL)
{
	m_rs = null;
	m_bClickable = true;
	m_nNodeType = NODETYPE_TABLE;

	m_bBackPresent = false;

	m_idBorderTexture = -1;
	m_idBorderVBuf = -1;
	m_idBorderIBuf = -1;
	m_nBorderSubQ = 0;

	m_EditData.bAllEditable = true;
	m_EditData.bColsEditable = true;
	m_EditData.nEditableIndex = 0;

	m_nTopIndex = 0;

	m_SelectImg.DisableDraw( true );
	m_pHeader = null;
	m_bDoColsSelect = false;

	m_bFirstFrame = true;
	m_bVariableLineHeight = false;
	m_nNormalLineHeight = 32;

	m_nFontCellID = -1;
	m_nFontTitleID = -1;
}

CXI_TABLE::~CXI_TABLE()
{
	ReleaseAll();
}

void CXI_TABLE::Draw(bool bSelected,dword Delta_Time)
{
	m_bFirstFrame = false;

	// Прорисовка задника
	if( m_bBackPresent )
	{
		m_BackImg.Draw();
	}

	// отрисовка спец цвета
	float fY = (float)m_rect.top;
	if( m_pHeader )
	{
		m_pHeader->DrawSpecColor( fY );
		fY += m_anRowsHeights[0];
	}
	for( long n=0; n<m_aLine; n++ )
	{
		m_aLine[n]->DrawSpecColor( fY );
		fY += m_anRowsHeights[n+(m_pHeader?1:0)];
	}

	// отрисовка выделения выбранной строки
	m_SelectImg.Draw();

	// Прорисовка рамки
	if( m_idBorderTexture!=-1 && m_idBorderVBuf!=-1 && m_idBorderIBuf!=-1 )
	{
		m_rs->TextureSet( 0, m_idBorderTexture );
		m_rs->DrawBuffer( m_idBorderVBuf, sizeof(XI_ONETEX_VERTEX), m_idBorderIBuf, 0, m_nBorderSubQ*4, 0, m_nBorderSubQ*2, "iIcon" );
	}

	// Вывод линий
	fY = (float)m_rect.top;
	if( m_pHeader )
	{
		m_pHeader->Draw( fY );
		fY += m_anRowsHeights[0];
	}
	for( long n=0; n<m_aLine; n++ )
	{
		m_aLine[n]->Draw( fY );
		fY += m_anRowsHeights[n+(m_pHeader?1:0)];
	}
}

bool CXI_TABLE::Init(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2, VDX8RENDER *rs, XYRECT &hostRect, XYPOINT &ScreenSize)
{
	if( !CINODE::Init(ini1,name1, ini2,name2, rs,hostRect,ScreenSize) ) return false;
	return true;
}

void CXI_TABLE::ReleaseAll()
{
	// release all rows
	m_aLine.DelAllWithPointers();
	DELETE( m_pHeader );

	// release back image
	m_bBackPresent = false;
	m_BackImg.Unload();

	// release border data
	PICTURE_TEXTURE_RELEASE( pPictureService, m_sBorderIconGroupName, m_idBorderTexture );
	VERTEX_BUF_RELEASE( m_rs, m_idBorderVBuf );
	INDEX_BUF_RELEASE( m_rs, m_idBorderIBuf );

	// release fonts from list
	FONT_RELEASE( m_rs, m_nFontCellID );
	FONT_RELEASE( m_rs, m_nFontTitleID );
	for( long n=0; n<m_anFontList; n++ ) {
		FONT_RELEASE( m_rs, m_anFontList[n] );
	}
	m_anFontList.DelAll();

	m_nBorderSubQ = 0;
}

int CXI_TABLE::CommandExecute(int wActCode)
{
	if(m_bUse)
	{
		switch(wActCode)
		{
		case ACTION_ACTIVATE:
			if( m_nSelectIndex >= 0 )
				api->Event( "TableActivate", "sl", m_nodeName, m_nSelectIndex );
			break;

		case ACTION_MOUSEDBLCLICK:
			if( m_nSelectIndex >= 0 )
				api->Event( "TableActivate", "sl", m_nodeName, m_nSelectIndex );
			break;

		case ACTION_UPSTEP:
			if( m_nSelectIndex > 0 )
			{
				m_nSelectIndex--;
				SetTopIndexForSelect( m_nSelectIndex );
				SelectLine( m_nSelectIndex );
			}
			break;

		case ACTION_SPEEDUP:
			if( m_nSelectIndex > 0 )
			{
				m_nSelectIndex -= m_anRowsHeights.Size() - (m_pHeader?1:0);
				if( m_nSelectIndex < 0 ) m_nSelectIndex = 0;
				SetTopIndexForSelect( m_nSelectIndex );
				SelectLine( m_nSelectIndex );
			}
			break;

		case ACTION_DOWNSTEP:
			if( m_nSelectIndex < m_nLineQuantity-1 )
			{
				m_nSelectIndex++;
				SetTopIndexForSelect( m_nSelectIndex );
				SelectLine( m_nSelectIndex );
			}
			break;

		case ACTION_SPEEDDOWN:
			if( m_nSelectIndex < m_nLineQuantity-1 )
			{
				m_nSelectIndex += m_anRowsHeights.Size() - (m_pHeader?1:0);
				if( m_nSelectIndex > m_nLineQuantity-1 ) m_nSelectIndex = m_nLineQuantity - 1;
				SetTopIndexForSelect( m_nSelectIndex );
				SelectLine( m_nSelectIndex );
			}
			break;

		case ACTION_LEFTSTEP:
		case ACTION_SPEEDLEFT:
			if( m_bDoColsSelect && m_nSelectIndex!=-1 && m_nSelectColIndex>0 )
			{
				m_nSelectColIndex--;
				SelectCol( m_nSelectColIndex );
			}
			break;

		case ACTION_RIGHTSTEP:
		case ACTION_SPEEDRIGHT:
			if( m_bDoColsSelect && m_nSelectIndex!=-1 && m_nSelectColIndex<(long)m_anColsWidth.Size()-1 )
			{
				m_nSelectColIndex++;
				SelectCol( m_nSelectColIndex );
			}
			break;
		// boal -->
        case ACTION_MOUSERCLICK :  // просто спозиционировать курсор
        	{
				long n = GetLineByPoint( ptrOwner->GetMousePoint() );
				if( n>=0 && n<=m_nLineQuantity-m_nTopIndex-(m_pHeader?0:1) )
				{
					long nCol = GetColByX( (long)ptrOwner->GetMousePoint().x );
					if( m_bDoColsSelect ) SelectRow( n, nCol );
					else SelectRow( n );
					//api->Event( "OnTableClick", "sll", m_nodeName, m_nSelectIndex, nCol+1 );
				}
			}
			break;
		// boal <--
		case ACTION_MOUSECLICK:
			{
				long n = GetLineByPoint( ptrOwner->GetMousePoint() );
				if( n>=0 && n<=m_nLineQuantity-m_nTopIndex-(m_pHeader?0:1) )
				{
					long nCol = GetColByX( (long)ptrOwner->GetMousePoint().x );
					if( m_bDoColsSelect ) SelectRow( n, nCol );
					else SelectRow( n );
					//api->Event( "OnTableClick", "sll", m_nodeName, (m_pHeader?n:(n+1)), nCol+1 );
					api->Event( "OnTableClick", "sll", m_nodeName, m_nSelectIndex, nCol+1 );
				}
			}
			break;
		}
	}
	return -1;
}

bool CXI_TABLE::IsClick(int buttonID,long xPos,long yPos)
{
	if( xPos>=m_rect.left && xPos<=m_rect.right &&
		yPos>=m_rect.top  && yPos<=m_rect.bottom && m_bClickable && m_bSelected && m_bUse )		return true;
	return false;
}

void CXI_TABLE::ChangePosition( XYRECT &rNewPos )
{
	long n, nWAdd, nHAdd;

	if( m_EditData.bAllEditable )
	{
		nWAdd = (rNewPos.right-rNewPos.left) - (m_rect.right-m_rect.left);
		nHAdd = (rNewPos.bottom-rNewPos.top) - (m_rect.bottom-m_rect.top);
		n = m_anColsWidth.Size() - 1;
		if( n > 0 && nWAdd > 0 )
			m_anColsWidth[n] += nWAdd;
		for(;n>=0&&nWAdd<0;n--)
		{
			m_anColsWidth[n] += nWAdd;
			if( m_anColsWidth[n] < 0 )
			{
				nWAdd = m_anColsWidth[n];
				m_anColsWidth[n] = 0;
			} else nWAdd=0;
		}
		n = m_anRowsHeights.Size() - 1;
		if( n > 0 && nHAdd > 0 )
			m_anRowsHeights[n] += nHAdd;
		for(;n>=0&&nHAdd<0;n--)
		{
			m_anRowsHeights[n] += nHAdd;
			if( m_anRowsHeights[n] < 0 )
			{
				nHAdd = m_anRowsHeights[n];
				m_anRowsHeights[n] = 0;
			} else nHAdd=0;
		}
		//
		m_rect.right = m_rect.left = rNewPos.left;
		for(n=0;n<m_anColsWidth;n++)
			m_rect.right += m_anColsWidth[n];
		m_rect.right += m_nBorderWidth;
		//
		m_rect.bottom = m_rect.top = rNewPos.top;
		for(n=0;n<m_anRowsHeights;n++)
			m_rect.bottom += m_anRowsHeights[n];
		m_rect.bottom += m_nBorderWidth;
	}

	else
	{
		if( m_EditData.bColsEditable )
		{
			nWAdd = rNewPos.right - m_rect.right;
			if( nWAdd == 0 ) nWAdd = rNewPos.left - m_rect.left;
			if( m_EditData.nEditableIndex < m_anColsWidth )
			{
				m_anColsWidth[m_EditData.nEditableIndex] += nWAdd;
				m_rect.right += nWAdd;
			}
		}
		else
		{
			nHAdd = rNewPos.bottom - m_rect.bottom;
			if( nHAdd == 0 ) nHAdd = rNewPos.top - m_rect.top;
			if( m_EditData.nEditableIndex < m_anRowsHeights )
			{
				m_anRowsHeights[m_EditData.nEditableIndex] += nHAdd;
				m_rect.bottom += nHAdd;
			}
		}
	}

	UpdateBorders();
	UpdateTableCells();
	m_BackImg.SetPosition( m_rect );
	UpdateSelectImage();
}

void CXI_TABLE::SaveParametersToIni()
{
	char pcWriteParam[2048];

	INIFILE * pIni = api->fio->OpenIniFile( (char*)ptrOwner->m_sDialogFileName.GetBuffer() );
	if( !pIni ) {
		api->Trace( "Warning! Can`t open ini file name %s", ptrOwner->m_sDialogFileName.GetBuffer() );
		return;
	}

	// save position
	_snprintf( pcWriteParam, sizeof(pcWriteParam), "%d,%d,%d,%d", m_rect.left, m_rect.top, m_rect.right, m_rect.bottom );
	pIni->WriteString( m_nodeName, "position", pcWriteParam );

	// save cols width
	string sTmp = "";
	for( long n=0; n<m_anColsWidth; n++ )
	{
		if( n>0 ) sTmp += ",";
		sTmp += m_anColsWidth[n];
	}
	pIni->WriteString( m_nodeName, "colswidth", (char*)sTmp.GetBuffer() );

	// save rows height
	sTmp = "";
	for( n=0; n<m_anRowsHeights; n++ )
	{
		if( n>0 ) sTmp += ",";
		sTmp += m_anRowsHeights[n];
	}
	pIni->WriteString( m_nodeName, "rowsheight", (char*)sTmp.GetBuffer() );

	delete pIni;
}

dword _cdecl CXI_TABLE::MessageProc(long msgcode, MESSAGE & message)
{
	switch(msgcode)
	{
	case 0: // обновить строки в таблице
		UpdateTableCells();
	break;
	case 1: // получить номер строки на которой находится указатель мыши (-1 - указатель за пределами)
		return GetLineByPoint( ptrOwner->GetMousePoint() );
	break;
	}

	return 0;
}

bool CXI_TABLE::GetInternalNameList( array<string>& aStr )
{
	aStr.DelAll();
	string sTmp = "all";
	aStr.Add( sTmp );
	for( long n=0; n<m_nColQuantity; n++ )
	{
		sTmp = "col";
		sTmp += (n+1);
		aStr.Add( sTmp );
	}
	for( n=0; n<m_nRowQuantity; n++ )
	{
		sTmp = "row";
		sTmp += (n+1);
		aStr.Add( sTmp );
	}
	return true;
}

void CXI_TABLE::SetInternalName( string& sName )
{
	if( sName == "all" ) {
		m_EditData.bAllEditable = true;
	} else {
		m_EditData.bAllEditable = false;
		m_EditData.bColsEditable = (strnicmp(sName.GetBuffer(),"col",3) == 0);
		m_EditData.nEditableIndex = atoi( &sName.GetBuffer()[3] ) - 1;
	}
}

void CXI_TABLE::ScrollerChanged( float fRelativeScrollPos )
{
	if( m_nLineQuantity <= 1 ) return; // все одно
	long n = (long)( fRelativeScrollPos * (m_nLineQuantity-1) );
	if( n != m_nSelectIndex )
	{
		SetTopIndexForSelect( n );
		SelectLine( n );
	}
}



void CXI_TABLE::LoadIni(INIFILE *ini1,char *name1, INIFILE *ini2,char *name2)
{
	SetGlowCursor( false );

	char param[1024], pctmp[128];
	long n, nCommonWidth, nUsedQ;

	// font list
	if( ini1 && name1 ) {
		for(n=0; n<100; n++) {
			_snprintf( pctmp, sizeof(pctmp), "fontlist%d", n+1 );
			if( !ini1->ReadString(name1,pctmp, param,sizeof(param),"") )
				break;
			m_anFontList.Add( m_rs->LoadFont(param) );
		}
	}

	if( ReadIniString( ini1,name1, ini2,name2, "fontcell", param, sizeof(param), "" ) )
		m_nFontCellID = m_rs->LoadFont( param );

	// cell font
	if( ReadIniString( ini1,name1, ini2,name2, "fontcell", param, sizeof(param), "" ) )
		m_nFontCellID = m_rs->LoadFont( param );
	m_dwFontCellColor = GetIniARGB( ini1,name1, ini2,name2, "fontcellcolor", ARGB(255,255,255,255) );
	m_fFontCellScale = GetIniFloat( ini1,name1, ini2,name2, "fontcellscale", 1.f );
	m_nFontCellAlignment = ALIGN_LEFT;
	m_nFontCellVAlignment = ALIGN_TOP;
	if( ReadIniString( ini1,name1, ini2,name2, "fontcellalignment", param, sizeof(param), "" ) )
	{
		if( stricmp(param,"center") == 0 ) m_nFontCellAlignment = ALIGN_CENTER;
		if( stricmp(param,"right") == 0 ) m_nFontCellAlignment = ALIGN_RIGHT;
	}
	if( ReadIniString( ini1,name1, ini2,name2, "fontcellvalignment", param, sizeof(param), "" ) )
	{
		if( stricmp(param,"center") == 0 ) m_nFontCellVAlignment = ALIGN_CENTER;
		if( stricmp(param,"bottom") == 0 ) m_nFontCellVAlignment = ALIGN_BOTTOM;
	}

	// title font
	if( ReadIniString( ini1,name1, ini2,name2, "fonttitle", param, sizeof(param), "" ) )
		m_nFontTitleID = m_rs->LoadFont( param );
	m_dwFontTitleColor = GetIniARGB( ini1,name1, ini2,name2, "fonttitlecolor", ARGB(255,255,255,255) );
	m_fFontTitleScale = GetIniFloat( ini1,name1, ini2,name2, "fonttitlescale", 1.f );
	m_nFontTitleAlignment = ALIGN_LEFT;
	m_nFontTitleVAlignment = ALIGN_TOP;
	if( ReadIniString( ini1,name1, ini2,name2, "fonttitlealignment", param, sizeof(param), "" ) )
	{
		if( stricmp(param,"center") == 0 ) m_nFontTitleAlignment = ALIGN_CENTER;
		if( stricmp(param,"right") == 0 ) m_nFontTitleAlignment = ALIGN_RIGHT;
	}
	if( ReadIniString( ini1,name1, ini2,name2, "fonttitlevalignment", param, sizeof(param), "" ) )
	{
		if( stricmp(param,"center") == 0 ) m_nFontTitleVAlignment = ALIGN_CENTER;
		if( stricmp(param,"bottom") == 0 ) m_nFontTitleVAlignment = ALIGN_BOTTOM;
	}

	// select image
	if( ReadIniString( ini1,name1, ini2,name2, "selectimage", param,sizeof(param), "" ) )
	{
		m_SelectImg.LoadAccordingToString( param );
	}
	m_bDoColsSelect = GetIniBool( ini1,name1, ini2,name2, "iscolselect", false );
	if( m_bDoColsSelect ) m_nSelectColIndex = 0;

	// border
	if( ReadIniString( ini1,name1, ini2,name2, "bordericongroup", param,sizeof(param), "" ) )
	{
		m_sBorderIconGroupName = param;
		m_idBorderTexture = pPictureService->GetTextureID( param );
	}
	//
	m_nBorderIcon_LeftTop = -1;
	if( ReadIniString( ini1,name1, ini2,name2, "borderlefttop", param,sizeof(param), "" ) )
		m_nBorderIcon_LeftTop = pPictureService->GetImageNum( m_sBorderIconGroupName, param );
	//
	m_nBorderIcon_LeftBottom = -1;
	if( ReadIniString( ini1,name1, ini2,name2, "borderleftbottom", param,sizeof(param), "" ) )
		m_nBorderIcon_LeftBottom = pPictureService->GetImageNum( m_sBorderIconGroupName, param );
	//
	m_nBorderIcon_RightTop = -1;
	if( ReadIniString( ini1,name1, ini2,name2, "borderrighttop", param,sizeof(param), "" ) )
		m_nBorderIcon_RightTop = pPictureService->GetImageNum( m_sBorderIconGroupName, param );
	//
	m_nBorderIcon_RightBottom = -1;
	if( ReadIniString( ini1,name1, ini2,name2, "borderrightbottom", param,sizeof(param), "" ) )
		m_nBorderIcon_RightBottom = pPictureService->GetImageNum( m_sBorderIconGroupName, param );
	//
	m_nBorderIcon_Left = -1;
	if( ReadIniString( ini1,name1, ini2,name2, "borderleft", param,sizeof(param), "" ) )
		m_nBorderIcon_Left = pPictureService->GetImageNum( m_sBorderIconGroupName, param );
	//
	m_nBorderIcon_Right = -1;
	if( ReadIniString( ini1,name1, ini2,name2, "borderright", param,sizeof(param), "" ) )
		m_nBorderIcon_Right = pPictureService->GetImageNum( m_sBorderIconGroupName, param );
	//
	m_nBorderIcon_Top = -1;
	if( ReadIniString( ini1,name1, ini2,name2, "bordertop", param,sizeof(param), "" ) )
		m_nBorderIcon_Top = pPictureService->GetImageNum( m_sBorderIconGroupName, param );
	//
	m_nBorderIcon_Bottom = -1;
	if( ReadIniString( ini1,name1, ini2,name2, "borderbottom", param,sizeof(param), "" ) )
		m_nBorderIcon_Bottom = pPictureService->GetImageNum( m_sBorderIconGroupName, param );
	//
	m_nBorderIcon_VLine = -1;
	if( ReadIniString( ini1,name1, ini2,name2, "bordervline", param,sizeof(param), "" ) )
		m_nBorderIcon_VLine = pPictureService->GetImageNum( m_sBorderIconGroupName, param );
	//
	m_nBorderIcon_HLine = -1;
	if( ReadIniString( ini1,name1, ini2,name2, "borderhline", param,sizeof(param), "" ) )
		m_nBorderIcon_HLine = pPictureService->GetImageNum( m_sBorderIconGroupName, param );

	m_dwBorderColor = GetIniARGB( ini1,name1, ini2,name2, "bordercolor", ARGB(255,128,128,128) );
	m_nBorderWidth = GetIniLong( ini1,name1, ini2,name2, "borderwidth", 0 );
	m_nVLineWidth = GetIniLong( ini1,name1, ini2,name2, "vlinewidth", 0 );
	m_nHLineHeight = GetIniLong( ini1,name1, ini2,name2, "hlineheight", 0 );
	m_nHeaderLineHeight = GetIniLong( ini1,name1, ini2,name2, "headerlineheight", 0 );
	m_bHLineIsBreakable = GetIniBool( ini1,name1, ini2,name2, "hlineisbreakable", true );
	m_pntBorderCornerSize.x = m_pntBorderCornerSize.y = 9;
	m_pntBorderCornerSize = GetIniLongPoint( ini1,name1, ini2,name2, "bordercornersize", m_pntBorderCornerSize );
	m_pntSpaceSize.x = m_pntSpaceSize.y = 4;
	m_pntSpaceSize = GetIniLongPoint( ini1,name1, ini2,name2, "cellspacesize", m_pntSpaceSize );

	m_nRowQuantity = GetIniLong( ini1,name1, ini2,name2, "rowquantity", 1 );
	m_nColQuantity = GetIniLong( ini1,name1, ini2,name2, "colquantity", 1 );

	m_bVariableLineHeight = GetIniBool( ini1,name1, ini2,name2, "IsVariableLineHeight", false );
	m_nNormalLineHeight = GetIniLong( ini1,name1, ini2,name2, "NormalLineHeight", 32 );

	nUsedQ = 0; // жестко заданных строк
	nCommonWidth = 0; // общая высота этих строк
	m_anRowsHeights.DelAll();
	if( ReadIniString( ini1,name1, ini2,name2, "rowsheight", param,sizeof(param), "" ) )
	{
		char * pcTmp = param;
		char pcTmpBuf[256];
		while( pcTmp && pcTmp[0] )
		{
			pcTmp = GetSubStr( pcTmp, pcTmpBuf, sizeof(pcTmpBuf) );
			long nTmp = atol( pcTmpBuf );
			if( nTmp <= 0 ) nTmp = 0;
			else nUsedQ++;
			nCommonWidth += nTmp;
			m_anRowsHeights.Add( nTmp );
		}
	}
	for( n=m_anRowsHeights; n<m_nRowQuantity; n++ )
		m_anRowsHeights.Add( 0 );
	if( m_nRowQuantity>nUsedQ && m_rect.bottom-m_rect.top>nCommonWidth )
	{
		long nH = (m_rect.bottom-m_rect.top - nCommonWidth) / (m_nRowQuantity-nUsedQ);
		for( n=0; n<m_anRowsHeights; n++ )
			if( m_anRowsHeights[n] == 0 )
				m_anRowsHeights[n] = nH;
	}
	// приведем высоту таблицы к общему значению
	m_rect.bottom = m_rect.top + m_nBorderWidth;
	for( n=0; n<m_anRowsHeights; n++ )
	{
		m_rect.bottom += m_anRowsHeights[n];
	}

	// заполнение колонок
	nUsedQ = 0; // жестко заданных колонок
	nCommonWidth = 0; // общая ширина этих колонок
	m_anColsWidth.DelAll();
	// зачитаем из ИНИ
	if( ReadIniString( ini1,name1, ini2,name2, "colswidth", param,sizeof(param), "" ) )
	{
		char * pcTmp = param;
		char pcTmpBuf[256];
		while( pcTmp && pcTmp[0] )
		{
			pcTmp = GetSubStr( pcTmp, pcTmpBuf, sizeof(pcTmpBuf) );
			long nTmp = atol( pcTmpBuf );
			if( nTmp <= 0 ) nTmp = 0; // ширина 0 - по умолчанию
			else nUsedQ++; // в другом случае это значение жестко задано
			nCommonWidth += nTmp;
			m_anColsWidth.Add( nTmp );
		}
	}
	for( n=m_anColsWidth; n<m_nColQuantity; n++ )
		m_anColsWidth.Add( 0 );
	if( m_nColQuantity>nUsedQ && m_rect.right-m_rect.left>nCommonWidth )
	{
		long nW = (m_rect.right-m_rect.left - m_nBorderWidth - nCommonWidth) / (m_nColQuantity-nUsedQ);
		for( n=0; n<m_anColsWidth; n++ )
			if( m_anColsWidth[n] == 0 )
				m_anColsWidth[n] = nW;
	}
	// приведем ширину таблицы к общему значению
	m_rect.right = m_rect.left + m_nBorderWidth;
	for( n=0; n<m_anColsWidth; n++ )
	{
		m_rect.right += m_anColsWidth[n];
	}

	// back
	m_bBackPresent = false;
	if( ReadIniString( ini1,name1, ini2,name2, "backimage", param,sizeof(param), "" ) )
	{
		m_bBackPresent = true;
		m_BackImg.LoadAccordingToString( param );
		m_BackImg.SetPosition( m_rect );
	}

	if( ReadIniString( ini1,name1, ini2,name2, "scroller", param,sizeof(param),"" ) )
		m_sScrollerName = param;
	m_nSelectIndex = 0;

	UpdateBorders();

	// читаем строки таблицы
	UpdateTableCells();
	SelectLine( 0 );
}

void CXI_TABLE::UpdateBorders()
{
	long n, q, r,c, nTop,nLeft;

	// подсчет количества линий в рамке
	q = 0;
	if( m_nHeaderLineHeight > 0 && m_nRowQuantity > 0 ) // горизонтальная линия под заголовком
		q += 1;
	if( m_nHLineHeight > 0 && m_nRowQuantity > 2 ) // горизонтальные линии
		q += m_nRowQuantity-2;
	if( m_bHLineIsBreakable ) q *= m_nColQuantity; // разрывная линия дробится по количеству колонок
	if(	m_nVLineWidth > 0 && m_nColQuantity > 1 ) // вертикальные линии
		q += m_nColQuantity-1;
	q += 4 + 4; // 4 угла и линии по сторонам

	if( m_nBorderSubQ != q ) // перестраиваем буферы
	{
		m_nBorderSubQ = q;
		VERTEX_BUF_RELEASE( m_rs, m_idBorderVBuf );
		INDEX_BUF_RELEASE( m_rs, m_idBorderIBuf );

		// индекс буфер
		m_idBorderIBuf = m_rs->CreateIndexBuffer( q * 6 * sizeof(WORD) );
		Assert( m_idBorderIBuf != -1 );
		// заполняем
		WORD* pT = (WORD*)m_rs->LockIndexBuffer( m_idBorderIBuf );
		for( n=0; n<q; n++ )
		{
			pT[n*6 + 0] = (WORD)(n*4 + 0);
			pT[n*6 + 1] = (WORD)(n*4 + 1);
			pT[n*6 + 2] = (WORD)(n*4 + 2);
			pT[n*6 + 3] = (WORD)(n*4 + 3);
			pT[n*6 + 4] = (WORD)(n*4 + 1);
			pT[n*6 + 5] = (WORD)(n*4 + 2);
		}
		m_rs->UnLockIndexBuffer( m_idBorderIBuf );

		// вертекс буфер
		m_idBorderVBuf = m_rs->CreateVertexBuffer( XI_ONETEX_FVF, q * 4 * sizeof(XI_ONETEX_VERTEX), D3DUSAGE_WRITEONLY );
		Assert( m_idBorderVBuf != -1 );
	}

	// заполняем вертекс буфер
	XI_ONETEX_VERTEX* pV = (XI_ONETEX_VERTEX*)m_rs->LockVertexBuffer( m_idBorderVBuf );
	// horizontal lines
	nTop = m_rect.top;
	for( r=0,n=0; r<m_nRowQuantity-1; r++ )
	{
		nTop += m_anRowsHeights[r];
		nLeft = m_rect.left;
		q = (r==0 ? m_nHeaderLineHeight : m_nHLineHeight);
		if( q > 0 )
		{
			if( m_bHLineIsBreakable )
			{ // отдельные линии для каждой колонки
				for( c=0; c<m_nColQuantity; c++ )
				{
					WriteSquare( &pV[n], m_nBorderIcon_HLine, m_dwBorderColor,
						nLeft, nTop, m_anColsWidth[c], q );
					nLeft += m_anColsWidth[c];
					n += 4;
				}
			} // одна линия на все колонки
			else
			{
				WriteSquare( &pV[n], m_nBorderIcon_HLine, m_dwBorderColor,
					m_rect.left+m_nBorderWidth, nTop, m_rect.right-m_rect.left-2*m_nBorderWidth, q );
				n += 4;
			}
		}
	}
	// vertical lines
	if( m_nVLineWidth > 0 )
	{
		nLeft = m_rect.left;
		for( c=0; c<m_nColQuantity-1; c++ )
		{
			nLeft += m_anColsWidth[c];
			WriteSquare( &pV[n], m_nBorderIcon_VLine, m_dwBorderColor,
				nLeft, m_rect.top+m_nBorderWidth, m_nVLineWidth, m_rect.bottom-m_rect.top-2*m_nBorderWidth );
			n += 4;
		}
	}
	// main border
	WriteSquare( &pV[n], m_nBorderIcon_LeftTop, m_dwBorderColor,
		m_rect.left, m_rect.top, m_pntBorderCornerSize.x, m_pntBorderCornerSize.y );
	n += 4;
	WriteSquare( &pV[n], m_nBorderIcon_RightTop, m_dwBorderColor,
		m_rect.right-m_pntBorderCornerSize.x, m_rect.top, m_pntBorderCornerSize.x, m_pntBorderCornerSize.y );
	n += 4;
	WriteSquare( &pV[n], m_nBorderIcon_LeftBottom, m_dwBorderColor,
		m_rect.left, m_rect.bottom-m_pntBorderCornerSize.y, m_pntBorderCornerSize.x, m_pntBorderCornerSize.y );
	n += 4;
	WriteSquare( &pV[n], m_nBorderIcon_RightBottom, m_dwBorderColor,
		m_rect.right-m_pntBorderCornerSize.x, m_rect.bottom-m_pntBorderCornerSize.y, m_pntBorderCornerSize.x, m_pntBorderCornerSize.y );
	n += 4;
	WriteSquare( &pV[n], m_nBorderIcon_Top, m_dwBorderColor,
		m_rect.left+m_pntBorderCornerSize.x, m_rect.top, m_rect.right-m_rect.left-2*m_pntBorderCornerSize.x, m_nBorderWidth );
	n += 4;
	WriteSquare( &pV[n], m_nBorderIcon_Bottom, m_dwBorderColor,
		m_rect.left+m_pntBorderCornerSize.x, m_rect.bottom-m_nBorderWidth, m_rect.right-m_rect.left-2*m_pntBorderCornerSize.x, m_nBorderWidth );
	n += 4;
	WriteSquare( &pV[n], m_nBorderIcon_Left, m_dwBorderColor,
		m_rect.left, m_rect.top+m_pntBorderCornerSize.y, m_nBorderWidth, m_rect.bottom-m_rect.top-2*m_pntBorderCornerSize.y);
	n += 4;
	WriteSquare( &pV[n], m_nBorderIcon_Right, m_dwBorderColor,
		m_rect.right-m_nBorderWidth, m_rect.top+m_pntBorderCornerSize.y, m_nBorderWidth, m_rect.bottom-m_rect.top-2*m_pntBorderCornerSize.y);
	n += 4;
	// finish
	m_rs->UnLockVertexBuffer( m_idBorderVBuf );
}

void CXI_TABLE::WriteSquare( XI_ONETEX_VERTEX* pV, long nImgID, dword dwCol, long nX, long nY, long nW, long nH )
{
	if( !pV ) return;

	FXYRECT uv;
	pPictureService->GetTexturePos( nImgID, uv );

	pV[0].color = dwCol;
	pV[0].pos.x = (float)nX;
	pV[0].pos.y = (float)nY;
	pV[0].pos.z = 1.f;
	pV[0].tu = uv.left;
	pV[0].tv = uv.top;

	pV[1].color = dwCol;
	pV[1].pos.x = (float)nX;
	pV[1].pos.y = (float)(nY + nH);
	pV[1].pos.z = 1.f;
	pV[1].tu = uv.left;
	pV[1].tv = uv.bottom;

	pV[2].color = dwCol;
	pV[2].pos.x = (float)(nX + nW);
	pV[2].pos.y = (float)nY;
	pV[2].pos.z = 1.f;
	pV[2].tu = uv.right;
	pV[2].tv = uv.top;

	pV[3].color = dwCol;
	pV[3].pos.x = (float)(nX + nW);
	pV[3].pos.y = (float)(nY + nH);
	pV[3].pos.z = 1.f;
	pV[3].tu = uv.right;
	pV[3].tv = uv.bottom;
}

void CXI_TABLE::UpdateTableCells()
{
	// Set new current image
	ATTRIBUTES* pARoot = api->Entity_GetAttributeClass( &g_idInterface, m_nodeName );
	if( !pARoot ) {
		m_nLineQuantity = 0;
		return;
	}

	long r,q;
	char pcTmp[64];

	long nY = m_rect.top;

	long nNewSel = pARoot->GetAttributeAsDword( "select", m_nSelectIndex+1 ) - 1;

	// зачитываем строки из атрибутов
	m_nTopIndex = pARoot->GetAttributeAsDword( "top", 0 ); // первый индекс зачитываемого аттрибута
	// сначала проверим наличие заголовка
	ATTRIBUTES* pAttr = pARoot->GetAttributeClass( "hr" );
	if( pAttr )
	{
		if( !m_pHeader ) m_pHeader = NEW XI_TableLineDescribe( this );
		Assert( m_pHeader );
		m_pHeader->SetData( 0, pAttr, true );
		nY += m_anRowsHeights[0];
	} else {
		DELETE( m_pHeader );
	}
	// потом
	q = m_bVariableLineHeight ? 1000 : (m_nRowQuantity - (m_pHeader?1:0));
	for( r=0; (nY<m_rect.bottom)&&(r<q); r++ )
	{
		sprintf( pcTmp, "tr%d", r + m_nTopIndex + 1 );
		pAttr = pARoot->GetAttributeClass( pcTmp );
		if( !pAttr ) break;
		XI_TableLineDescribe* pTL = 0;
		if( r >= m_aLine )
		{
			pTL = NEW XI_TableLineDescribe( this );
			m_aLine.Add( pTL );
		} else
			pTL = m_aLine[r];
		Assert( pTL );
		pTL->SetData( r+(m_pHeader?1:0), pAttr, false );
		nY += pTL->GetLineHeight();
	}
	// удаляем лишние строки
	while( (long)m_aLine.Size() > r )
	{
		DELETE( m_aLine[r] );
		m_aLine.DelIndex( r );
	}

	// если высота строк переменная, то пересчитаем ее
	if( m_bVariableLineHeight )
	{
		RecalculateLineHeights();
		UpdateSelectImage();
	}

	UpdateLineQuantity();
	if( m_bVariableLineHeight )
		UpdateBorders();
	if( nNewSel >= m_nLineQuantity ) nNewSel = m_nLineQuantity - 1;
	if( nNewSel != m_nSelectIndex ) {
		m_nSelectIndex = nNewSel;
		UpdateSelectImage();
	}
}

long CXI_TABLE::GetLineByPoint( FXYPOINT& pnt )
{
	if( pnt.x < m_rect.left || pnt.x > m_rect.right ||
		pnt.y < m_rect.top || pnt.y > m_rect.bottom ) return -1;
	long nTop = m_rect.top;
	for( long n=0; n<m_anRowsHeights; n++ )
	{
		nTop += m_anRowsHeights[n];
		if( pnt.y < nTop ) return n;
	}
	return -1;
}

long CXI_TABLE::GetColByX( long x )
{
	x -= m_rect.left;
	if( x<0 ) return -1;
	for( long n=0; n<m_anColsWidth; n++ )
		if( m_anColsWidth[n] >= x )
			return n;
		else
			x -= m_anColsWidth[n];
	return -1;
}

void CXI_TABLE::SelectRow( long nRowNum )
{
	if( nRowNum<(m_pHeader?1:0) || nRowNum>=m_anRowsHeights ) return;
	long n = m_nTopIndex + nRowNum - (m_pHeader?1:0);
	SelectLine( n );
}

void CXI_TABLE::SelectRow( long nRowNum, long nColNum )
{
	if( m_bDoColsSelect )
	{
		if( nColNum<0 || nColNum>=m_anColsWidth ) m_nSelectColIndex = -1;
		else m_nSelectColIndex = nColNum;
	}
	if( nRowNum<(m_pHeader?1:0) || nRowNum>=m_anRowsHeights ) return;
	long n = m_nTopIndex + nRowNum - (m_pHeader?1:0);
	SelectLine( n );
}

void CXI_TABLE::SelectLine( long nLineNum )
{
	if( nLineNum<0 ) m_nSelectIndex = -1;
	else m_nSelectIndex = nLineNum;
	UpdateSelectImage();
	UpdateScroller();
	if( !m_bFirstFrame )
		api->Event( "TableSelectChange", "sll", m_nodeName, m_nSelectIndex+1, m_nSelectColIndex+1 );
}

void CXI_TABLE::SelectCol( long nColNum )
{
	if( !m_bDoColsSelect ) return;
	if( nColNum<0 || nColNum>=m_anColsWidth ) m_nSelectColIndex = -1;
	else m_nSelectColIndex = nColNum;
	UpdateSelectImage();
	if( !m_bFirstFrame )
		api->Event( "TableSelectChange", "sll", m_nodeName, m_nSelectIndex+1, m_nSelectColIndex+1 );
}

void CXI_TABLE::UpdateSelectImage()
{
	long nRow = m_nSelectIndex - m_nTopIndex + (m_pHeader?1:0);
	if( m_nSelectIndex<0 || nRow<0 || nRow>=m_anRowsHeights ) {
		m_SelectImg.DisableDraw( true );
	}
	else
	{
		long nCol = m_nSelectColIndex;
		m_SelectImg.DisableDraw( false );
		XYRECT pos;
		pos.top = GetRowTop( nRow );
		pos.bottom = pos.top + m_anRowsHeights[nRow];
		if( m_bDoColsSelect && nCol>=0 && nCol<m_anColsWidth ) {
			pos.left = GetColLeft( nCol );
			pos.right = pos.left + m_anColsWidth[nCol];
		} else {
			pos.left = m_rect.left;
			pos.right = m_rect.right;
		}
		m_SelectImg.SetPosition( pos );
	}
	ATTRIBUTES* pA = ptrOwner->AttributesPointer->GetAttributeClass( m_nodeName );
	if( !pA ) pA = ptrOwner->AttributesPointer->CreateSubAClass( ptrOwner->AttributesPointer, m_nodeName );
	if( pA )
	{
		pA->SetAttributeUseDword( "select", m_nSelectIndex+1 );
		pA->SetAttributeUseDword( "selectcol", m_nSelectColIndex+1 );
	}
}

long CXI_TABLE::GetRowTop( long nRow )
{
	if( nRow < 0 || nRow >= m_anRowsHeights ) return m_rect.top; // ошибочная ситуация
	long nTop = m_rect.top;
	for( long n=0; n<nRow; n++ )
	{
		nTop += m_anRowsHeights[n];
	}
	return nTop;
}

long CXI_TABLE::GetColLeft( long nCol )
{
	if( nCol<0 || nCol>=m_anColsWidth ) return m_rect.left; // ошибочная ситуация
	long nLeft = m_rect.left;
	for( long n=0; n<nCol; n++ )
	{
		nLeft += m_anColsWidth[n];
	}
	return nLeft;
}

void CXI_TABLE::SetTopIndexForSelect( long nSelIndex )
{
	if( nSelIndex<0 ) return; // ошибочная ситуация

	if( nSelIndex < m_nTopIndex ) // до вершины - значит ставим выделение наверх
	{
		SetTopIndex( nSelIndex );
		UpdateTableCells();
	}
	else if( nSelIndex >= m_nTopIndex + (long)m_anRowsHeights.Size() - (m_pHeader?1:0) ) // после последней строки - значит ставим выделение вниз
	{
		nSelIndex = nSelIndex - m_anRowsHeights.Size() + (m_pHeader?1:0) + 1;
		if( nSelIndex < 0 ) nSelIndex = 0;
		SetTopIndex( nSelIndex );
		UpdateTableCells();
	}
}

void CXI_TABLE::UpdateLineQuantity()
{
	m_nLineQuantity = 0;
	ATTRIBUTES* pARoot = api->Entity_GetAttributeClass( &g_idInterface, m_nodeName );
	if( !pARoot ) return;

	if( !pARoot->GetAttributeClass( "tr1" ) ) return; // нет ни одного

	long nmin = 1;
	char pcAttrName[64];

	// поиск минимального элемента
	while(true)
	{
		sprintf( pcAttrName, "tr%d", nmin*2 );
		if( pARoot->GetAttributeClass( pcAttrName ) )
			nmin *= 2;
		else break; // следующий элемент бинарного дерева несуществует
	}

	// бинарный поиск между мин и макс индексами
	long nmax = nmin * 2;
	while(true)
	{
		long n = (nmin + nmax) / 2;
		if( n == nmin ) break;
		sprintf( pcAttrName, "tr%d", n );
		if( pARoot->GetAttributeClass( pcAttrName ) ) // элемент есть - значит старшая половина может иметь конец
		{
			nmin = n;
		}
		else // иначе конец где то в первой половине
		{
			nmax = n;
		}
	}

	m_nLineQuantity = nmin;
}

void CXI_TABLE::SetTopIndex( long nTopIndex )
{
	m_nTopIndex = nTopIndex;
	ATTRIBUTES* pA = api->Entity_GetAttributeClass( &g_idInterface, m_nodeName );
	if( pA ) pA->SetAttributeUseDword( "top", nTopIndex );
}

void CXI_TABLE::UpdateScroller()
{
	if( m_sScrollerName.IsEmpty() ) return;
	CINODE* pNode = ptrOwner->FindNode( m_sScrollerName, 0 );
	if( !pNode || pNode->m_nNodeType != NODETYPE_SCROLLER ) return;
	CXI_SCROLLER* pScroll = (CXI_SCROLLER*)pNode;

	if( m_nLineQuantity <= 1 )
		pScroll->SetRollerPos( 0.f );
	else
		pScroll->SetRollerPos( (float)m_nSelectIndex / (float)(m_nLineQuantity-1) );
}

void CXI_TABLE::RecalculateLineHeights()
{
	long nY = m_rect.top; // текущая вершина строки
	long n = 0; // индекс в массиве высот

	// учет заголовка
	if( m_pHeader )
		nY += m_anRowsHeights[n++];

	// берем реальные размеры строк
	for(long i=0; i<m_aLine; i++,n++)
	{
		if( n < m_anRowsHeights )
			m_anRowsHeights[n] = m_aLine[i]->GetLineHeight();
		else
			m_anRowsHeights.Add(m_aLine[i]->GetLineHeight());
		nY += m_aLine[i]->GetLineHeight();
	}

	// если последняя строка выходит за размеры, то укорачиваем ее по высоте
	if( nY>m_rect.bottom && m_aLine.Size()>0 ) {
		long nLine = m_aLine.Last();
		m_aLine[nLine]->SetLineHeight( m_aLine[nLine]->GetLineHeight() - (nY - m_rect.bottom) );
		nY = m_rect.bottom;
	}

	// недостающие строки ставим в стандартную высоту
	for( ; nY<m_rect.bottom; n++ ) {
		long nH = m_nNormalLineHeight;
		if( nY + nH > m_rect.bottom ) nH = m_rect.bottom - nY;
		if( n < m_anRowsHeights ) m_anRowsHeights[n] = nH;
		else m_anRowsHeights.Add(nH);
		nY += nH;
	}

	// оставшиеся строки удаляем
	while( n < m_anRowsHeights )
		m_anRowsHeights.DelIndex( n );
}
