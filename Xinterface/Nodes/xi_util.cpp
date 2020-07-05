#include <stdio.h>
#include "..\..\common_h\vmodule_api.h"
#include "..\..\common_h\defines.h"
#include "xi_util.h"
#include "..\inode.h"

CXI_UTILS * CXI_UTILS::pThis = 0;

CXI_UTILS::CXI_UTILS()
{
	pThis = this;
}

CXI_UTILS::~CXI_UTILS()
{
}

void CXI_UTILS::Init()
{
	for( long n=0; n<UTILS_KEYS_QUANTITY; n++ )
	{
		//keys[n].nAsyncKeyCode = -1; // not used
		keys[n].nPressedState = -1;
		keys[n].nAsyncKeyCode = n;
	}
	// language keys

	m_bLang2 = false;
	m_bIsKeyPressed = false;
	m_bFrameKeyPressedFlag = false;
}

void CXI_UTILS::Release()
{
}

void CXI_UTILS::FrameUpdate()
{
	if( m_bIsKeyPressed && m_bFrameKeyPressedFlag )
	{ // опроса на этом кадре не было - значит можно все клавиши сбросить (мы вышли из цикла опроса)
		for( long n=0; n<UTILS_KEYS_QUANTITY; n++ )
			if( keys[n].nAsyncKeyCode < 0 ) continue;
			else keys[n].nPressedState = -1;
		m_bIsKeyPressed = false;
	}
	m_bFrameKeyPressedFlag = true;
}

char CXI_UTILS::GetKeyInput()
{
	pThis->m_bFrameKeyPressedFlag = false; // не сбрасывать состояния клавиш пока на каждом кадре происходит опрос
	char cRetVal = 0;

	pThis->m_bIsKeyPressed = false;
	for( long n=0; n<UTILS_KEYS_QUANTITY; n++ )
	{
		if( pThis->keys[n].nAsyncKeyCode < 0 ) continue;

		if( GetAsyncKeyState( pThis->keys[n].nAsyncKeyCode ) < 0 )
		{
			pThis->m_bIsKeyPressed = true;

			if( pThis->keys[n].nPressedState < 0 )
			{
				pThis->keys[n].nPressedState = 0;

				switch( n )
				{
				case VK_LEFT: return SpecSymbol_left; break;
				case VK_RIGHT: return SpecSymbol_right; break;
				case VK_UP: return SpecSymbol_up; break;
				case VK_DOWN: return SpecSymbol_down; break;
				case VK_BACK: return SpecSymbol_back; break;
				case VK_DELETE: return SpecSymbol_delete; break;
				case VK_HOME: return SpecSymbol_home; break;
				case VK_END: return SpecSymbol_end; break;
				case VK_TAB: return SpecSymbol_tab; break;
				case VK_RETURN: return SpecSymbol_return; break;
				case VK_ESCAPE: return SpecSymbol_escape; break;
				}

				byte pKBState[256];
				WORD pcTmp[16]; // вообще то нужно только 2 символа (остальные на всякий случай)
				GetKeyboardState(pKBState);
				if( ToAscii( n, MapVirtualKey(n,0), pKBState, pcTmp, 0 ) == 1 )
				{
					cRetVal = (char)pcTmp[0];
					return cRetVal;
				}
			}
			cRetVal = 0;
		}
		else
		{
			pThis->keys[n].nPressedState = -1;
		}
	}

	return cRetVal;
}

void CXI_UTILS::StringLeftClamp( char*& pcString )
{
	if( !pcString ) return;
	while( pcString[0] )
	{
		if( pcString[0] <= 0x20 ) {pcString++; continue;}
		break;
	}
}

const char* CXI_UTILS::StringGetTokenID( char*& pcString, char* pcBuffer, long nBufferSize )
{
	if( !pcString || !pcBuffer || nBufferSize<=0 ) return 0;
	pcBuffer[0] = 0;

	StringLeftClamp( pcString );

	long n = 0;
	long q = nBufferSize - 1;
	for( char cCur=pcString[0]; cCur && n<q; pcString++,cCur=pcString[0] )
	{
		if( cCur == '=' ) break;
		if( cCur == ',' ) {
			n = 0;
			pcString++;
			StringLeftClamp( pcString );
			pcString--;
			continue;
		}
		if( cCur < 0x20 ) continue; // пропустим неиспользуемые символы
		pcBuffer[n++] = cCur;
	}
	// уберем последние пробелы
	while( n>0 && pcBuffer[n-1]<=0x20 ) n--;
	pcBuffer[n] = 0;

	// избавимся от знака '='
	while( pcString[0] == '=' ) pcString++;

	if( pcBuffer[0] ) return pcBuffer;
	return 0;
}

const char* CXI_UTILS::StringGetTokenString( char*& pcString, char* pcBuffer, long nBufferSize )
{
	if( !pcString || !pcBuffer || nBufferSize<=0 ) return 0;
	pcBuffer[0] = 0;

	StringLeftClamp( pcString );

	long n = 0;
	long q = nBufferSize - 1;
	long nQuote = 0;
	for( char cCur=pcString[0]; cCur && n<q; pcString++,cCur=pcString[0]  )
	{
		if( cCur == ',' && nQuote==0 ) break;
		if( cCur=='(' || cCur=='{' || cCur=='[' ) nQuote++;
		if( cCur==')' || cCur=='}' || cCur==']' ) nQuote--;
		if( cCur < 0x20 ) continue; // пропустим неиспользуемые символы
		pcBuffer[n++] = cCur;
	}
	// уберем последние пробелы
	while( n>0 && pcBuffer[n-1]<=0x20 ) n--;
	pcBuffer[n] = 0;

	// избавимся от знака ','
	while( pcString[0] == ',' ) pcString++;

	if( pcBuffer[0] ) return pcBuffer;
	return 0;
}

long CXI_UTILS::StringGetTokenCode( const char* pcTokenID )
{
	if( stricmp(pcTokenID,"color") == 0 ) return InterfaceToken_color;
	if( stricmp(pcTokenID,"file") == 0 ) return InterfaceToken_file;
	if( stricmp(pcTokenID,"piclist") == 0 ) return InterfaceToken_picture_list;
	if( stricmp(pcTokenID,"picname") == 0 ) return InterfaceToken_picture_name;
	if( stricmp(pcTokenID,"piccutuv") == 0 ) return InterfaceToken_picture_cut_uv;
	if( stricmp(pcTokenID,"size") == 0 ) return InterfaceToken_size;
	if( stricmp(pcTokenID,"rectUV") == 0 ) return InterfaceToken_rectUV;
	if( stricmp(pcTokenID,"pos") == 0 ) return InterfaceToken_pos;
	if( stricmp(pcTokenID,"text") == 0 ) return InterfaceToken_text;
	if( stricmp(pcTokenID,"width") == 0 ) return InterfaceToken_width;
	if( stricmp(pcTokenID,"class") == 0 ) return InterfaceToken_class;

	return InterfaceToken_unknown;
}

DWORD CXI_UTILS::StringGetColor( const char* pcARGBString )
{
	long nA = StringGetLong( pcARGBString );
	long nR = StringGetLong( pcARGBString );
	long nG = StringGetLong( pcARGBString );
	long nB = StringGetLong( pcARGBString );
	return ARGB( nA, nR, nG, nB );
}

void CXI_UTILS::StringDoublicate( const char* pcSrc, char*& pcDst )
{
	if( pcDst ) delete pcDst;
	pcDst = 0;
	if( pcSrc )
	{
		pcDst = NEW char[strlen(pcSrc)+1];
		if( pcDst ) strcpy( pcDst, pcSrc );
		else _THROW("allocate memory error");
	}
}

void CXI_UTILS::StringTwoLong( const char* pcString, long & nLong1, long & nLong2 )
{
	nLong1 = StringGetLong( pcString );
	nLong2 = StringGetLong( pcString );
}

long CXI_UTILS::StringGetLong( const char*& pcString )
{
	if( !pcString ) return 0;
	long nRetVal = 0;
	for( ; pcString[0]; pcString++ )
	{
		if( pcString[0] <= 0x20 ) continue;
		if( pcString[0] == ',' ) {pcString++; break;}
		if( pcString[0] >= '0' && pcString[0] <= '9' )
		{
			nRetVal = nRetVal * 10 + pcString[0] - '0';
		}
	}
	return nRetVal;
}

float CXI_UTILS::StringGetFloat( const char*& pcString )
{
	if( !pcString ) return 0.f;
	char sTmp[128];
	long n = 0;
	for( ; pcString[0] && n<sizeof(sTmp)-1; pcString++ )
	{
		if( pcString[0] <= 0x20 ) continue;
		if( pcString[0] == ',' ) {pcString++; break;}
		if( (pcString[0] >= '0' && pcString[0] <= '9') || pcString[0]=='.' )
		{
			sTmp[n] = pcString[0];
			n++;
		}
	}
	sTmp[n] = 0;
	return (float)atof(sTmp);
}

void CXI_UTILS::StringFourFloat( const char* pcString, float& f1, float& f2, float& f3, float& f4 )
{
	f1 = StringGetFloat( pcString );
	f2 = StringGetFloat( pcString );
	f3 = StringGetFloat( pcString );
	f4 = StringGetFloat( pcString );
}

void CXI_UTILS::StringFillStringArray( const char* pcString, array<string> & asStringsArray )
{
	char tmpstr[256];
	char * pcSrcStr = (char*)pcString;

	// delete old
	asStringsArray.DelAll();

	// create new
	while( 0 != CXI_UTILS::StringGetTokenString(pcSrcStr, tmpstr, sizeof(tmpstr)) )
	{
		if( !tmpstr[0] ) continue;
		asStringsArray.Add( string(tmpstr) );
	}
}

long CXI_UTILS::SplitStringByWidth( const char* pcText, long nFontID, float fFontScale, long nWidth, array<string>& asOutStr )
{
	long nMaxUsingWidth = 0;
	const char * pcSrcStr = pcText;
	if( pcSrcStr == 0 ) return nMaxUsingWidth;
	VDX8RENDER * rs = (VDX8RENDER*)api->CreateService("dx8render");
	asOutStr.DelAll();

	long nSrc = 0;
	long nDst = 0;
	long nDstPrev = 0;
	long nSrcPrev = 0;
	char param[1024];
	while( true )
	{
		long nToken = GetCurrentTokenIntoString( &pcSrcStr[nSrc] );
		if( nToken == StrTokenType_common )
		{
			param[nDst++] = pcSrcStr[nSrc++];
			continue;
		}

		param[nDst] = 0;
		long nW = rs->StringWidth( param, nFontID, fFontScale );
		if( nW<nWidth && nToken==StrTokenType_Space ) // пробел но может не последний
		{
			nSrcPrev = nSrc;
			nDstPrev = nDst;
			while( pcSrcStr[nSrc]==0x20 ) nSrc++;
			param[nDst++] = 0x20;
			continue;
		}

		if( nW>nWidth && nDstPrev>0 ) // либо последний пробел, либо просто конец строки
		{
			nSrc = nSrcPrev;
			nDst = nDstPrev;
			param[nDst++] = 0x20;
		}

		param[nDst] = 0;
		while( pcSrcStr[nSrc]==0x20 ) // убираем пробелы из строки
			nSrc++;
		if( GetCurrentTokenIntoString(&pcSrcStr[nSrc]) == StrTokenType_NextLine ) // при переводе строки считаем его
		{
			nSrc += 2;
			while( nDst>0 && param[nDst-1]==0x20 ) // результат строка не должна заканчиваться пробелом
				param[--nDst] = 0;
		}

		nDstPrev = nDst = 0;
		long n = asOutStr.Add();
		asOutStr[n] = param;
		nW = rs->StringWidth( param, nFontID, fFontScale );
		if( nW > nMaxUsingWidth )
			nMaxUsingWidth = nW;

		if( GetCurrentTokenIntoString(&pcSrcStr[nSrc]) == StrTokenType_End )
			break;
	}
	return nMaxUsingWidth;
}

float CXI_UTILS::GetByStrNumFromAttribute_Float( ATTRIBUTES* pA, const char* pStr, long num, float fDefValue )
{
	if( !pA ) return fDefValue;
	char stmp[64];
	_snprintf( stmp,sizeof(stmp), "%s%d", pStr, num );
	return pA->GetAttributeAsFloat( stmp, fDefValue );
}

void CXI_UTILS::WriteSquareToVertexBuffer( XI_ONETEX_VERTEX *pv, dword color, FXYRECT& uv, XYRECT& rect )
{
	pv[0].color = color;
	pv[0].pos.x = (float)rect.left;
	pv[0].pos.y = (float)rect.top;
	pv[0].pos.z = 1.f;
	pv[0].tu = uv.left;
	pv[0].tv = uv.top;

	pv[1].color = color;
	pv[1].pos.x = (float)rect.left;
	pv[1].pos.y = (float)rect.bottom;
	pv[1].pos.z = 1.f;
	pv[1].tu = uv.left;
	pv[1].tv = uv.bottom;

	pv[2].color = color;
	pv[2].pos.x = (float)rect.right;
	pv[2].pos.y = (float)rect.top;
	pv[2].pos.z = 1.f;
	pv[2].tu = uv.right;
	pv[2].tv = uv.top;

	pv[3].color = color;
	pv[3].pos.x = (float)rect.right;
	pv[3].pos.y = (float)rect.bottom;
	pv[3].pos.z = 1.f;
	pv[3].tu = uv.right;
	pv[3].tv = uv.bottom;
}

void CXI_UTILS::WriteSquareToVertexBuffer( XI_ONETEX_VERTEX *pv, dword color, FXYRECT& uv, long left,long top,long right,long bottom )
{
	pv[0].color = color;
	pv[0].pos.x = (float)left;
	pv[0].pos.y = (float)top;
	pv[0].pos.z = 1.f;
	pv[0].tu = uv.left;
	pv[0].tv = uv.top;

	pv[1].color = color;
	pv[1].pos.x = (float)left;
	pv[1].pos.y = (float)bottom;
	pv[1].pos.z = 1.f;
	pv[1].tu = uv.left;
	pv[1].tv = uv.bottom;

	pv[2].color = color;
	pv[2].pos.x = (float)right;
	pv[2].pos.y = (float)top;
	pv[2].pos.z = 1.f;
	pv[2].tu = uv.right;
	pv[2].tv = uv.top;

	pv[3].color = color;
	pv[3].pos.x = (float)right;
	pv[3].pos.y = (float)bottom;
	pv[3].pos.z = 1.f;
	pv[3].tu = uv.right;
	pv[3].tv = uv.bottom;
}

void CXI_UTILS::PrintTextIntoWindow(VDX8RENDER* pRender,
									long nFont, dword dwColor, long wAlignment, bool bShadow, float fScale,
									long scrWidth, long scrHeight, long x, long y,
									const char* pcString, long left,long top, long width,long height)
{
	long nStrWidth = pRender->StringWidth((char*)pcString,nFont,fScale,0);
	if( nStrWidth == 0 ) return;
	long right = left + width;

	long nL,nR;
	if( wAlignment==ALIGN_RIGHT ) nL = x - nStrWidth;
	else if( wAlignment==ALIGN_CENTER ) nL = x - nStrWidth/2;
	else nL = x;
	nR = nL + nStrWidth;

	// влезаем в окно -> выводим как обычно
	if( nL >= left && nR <= right ) {
		pRender->ExtPrint( nFont,dwColor,0,wAlignment,bShadow,fScale, scrWidth,scrHeight, x,y, "%s", pcString );
		return;
	}

	char tmpstr[4096];
	_snprintf(tmpstr,sizeof(tmpstr),"%s",pcString);
	char* pc = tmpstr;

	// режем левый край
	while( pc[0] && nL<left ) {
		pc++;
		nL = nR - pRender->StringWidth(pc,nFont,fScale,0);
	}

	// режем правый край
	if( nR > right )
	{
		long n = strlen(pc);
		while( n>0 && nR>right )
		{
			pc[--n] = 0;
			nR = nL + pRender->StringWidth(pc,nFont,fScale,0);
		}
	}

	pRender->ExtPrint( nFont,dwColor,0,ALIGN_LEFT,bShadow,fScale, scrWidth,scrHeight, nL,y, "%s", pc );
}
