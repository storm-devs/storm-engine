#include "utils.h"
#include "..\common_h\defines.h"
#include "image\imgrender.h"

// extern data
ENTITY_ID BIUtils::idBattleInterface;
DWORD BIUtils::g_dwBlinkColor;

long BIUtils::GetLongFromAttr(ATTRIBUTES * pA, const char * name, long defVal)
{
	if(pA==null || name==null) return defVal;
	return pA->GetAttributeAsDword(name,defVal);
}

float BIUtils::GetFloatFromAttr(ATTRIBUTES * pA, const char * name, float defVal)
{
	if(pA==null || name==null) return defVal;
	return pA->GetAttributeAsFloat(name,defVal);
}

bool BIUtils::ReadStringFromAttr(ATTRIBUTES * pA, const char * name, char * buf, long bufSize, const char * defVal)
{
	if(buf==null || bufSize<1) return false;
	buf[0] = 0;
	const char * strGet;
	bool bRet = true;
	if( pA==null || (strGet=pA->GetAttribute(name))==null )
	{
		strGet = defVal;
		bRet = false;
	}

	if(strGet!=null)
	{
		if( (int)strlen(strGet)>bufSize-1 )
		{
			strncpy(buf,strGet,bufSize-1);
			buf[bufSize-1] = 0;
		}
		else strcpy(buf,strGet);
	} else bRet = false;

	return bRet;
}

char * BIUtils::GetStringFromAttr(ATTRIBUTES * pA, const char * name, const char * defVal)
{
	if(pA==null || name==null) return (char*)defVal;
	char * aVal = pA->GetAttribute(name);
	if(aVal==null) return (char*)defVal;
	return aVal;
}

long BIUtils::GetTextureFromAttr(VDX8RENDER * rs,ATTRIBUTES * pA,const char * sAttrName)
{
	if(!rs || !pA) return -1;
	char * sname = pA->GetAttribute(sAttrName);
	if(!sname || sname[0]==0) return -1;
	return rs->TextureCreate(sname);
}

bool BIUtils::ReadRectFromAttr(ATTRIBUTES * pA,const char * name,FRECT& rOut, FRECT& rDefault)
{
	rOut = rDefault;
	if( pA && name ) {
		char* pcStr = pA->GetAttribute(name);
		if( pcStr )
		{
			sscanf(pcStr,"%f,%f,%f,%f", &rOut.left,&rOut.top, &rOut.right,&rOut.bottom);
			return true;
		}
	}
	return false;
}

bool BIUtils::ReadRectFromAttr(ATTRIBUTES * pA,const char * name,RECT& rOut, RECT& rDefault)
{
	rOut = rDefault;
	if( pA && name ) {
		char* pcStr = pA->GetAttribute(name);
		if( pcStr ) {
			sscanf(pcStr,"%d,%d,%d,%d", &rOut.left,&rOut.top, &rOut.right,&rOut.bottom);
			return true;
		}
	}
	return false;
}

bool BIUtils::ReadPosFromAttr(ATTRIBUTES * pA,const char * name,float& fX,float& fY, float fXDef,float fYDef)
{
	fX = fXDef;
	fY = fYDef;
	if( pA && name ) {
		char* pcStr = pA->GetAttribute(name);
		if( pcStr ) {
			sscanf(pcStr,"%f,%f", &fX,&fY);
			return true;
		}
	}
	return false;
}

bool BIUtils::ReadPosFromAttr(ATTRIBUTES * pA,const char * name,long& nX,long& nY, long nXDef,long nYDef)
{
	nX = nXDef;
	nY = nYDef;
	if( pA && name ) {
		char* pcStr = pA->GetAttribute(name);
		if( pcStr ) {
			sscanf(pcStr,"%d,%d", &nX,&nY);
			return true;
		}
	}
	return false;
}

long BIUtils::GetAlignmentFromAttr(ATTRIBUTES * pA,const char * name,long nDefAlign)
{
	if( pA && name ) {
		char * pcTmp = pA->GetAttribute(name);
		if( pcTmp ) {
			if( stricmp(pcTmp,"left")==0 ) return ALIGN_LEFT;
			if( stricmp(pcTmp,"center")==0 ) return ALIGN_CENTER;
			if( stricmp(pcTmp,"right")==0 ) return ALIGN_RIGHT;
		}
	}
	return nDefAlign;
}

long BIUtils::GetFontIDFromAttr(ATTRIBUTES * pA,const char * name,VDX8RENDER* rs,const char* pcDefFontName)
{
	if( rs && pA && name ) {
		char* pcTmp = pA->GetAttribute(name);
		if( pcTmp ) return rs->LoadFont(pcTmp);
	}
	if( rs && pcDefFontName ) return rs->LoadFont((char*)pcDefFontName);
	return -1;
}

bool BIUtils::ReadVectorFormAttr(ATTRIBUTES * pA,const char * name,CVECTOR& vOut,const CVECTOR& vDef)
{
	vOut = vDef;
	if( pA && name ) {
		ATTRIBUTES* pAttr = pA->CreateSubAClass(pA,name);
		if( pAttr ) {
			vOut.x = pAttr->GetAttributeAsFloat("x",vDef.x);
			vOut.y = pAttr->GetAttributeAsFloat("y",vDef.y);
			vOut.z = pAttr->GetAttributeAsFloat("z",vDef.z);
			return true;
		}
	}
	return false;
}

bool BIUtils::ComparePoint(POINT& p1,POINT& p2)
{
	return ((p1.x==p2.x) && (p1.y==p2.y));
}

ATTRIBUTES * _cdecl BIUtils::GetAttributesFromPath(ATTRIBUTES * pA,...)
{
	va_list arglist;

	char * sName;
	ATTRIBUTES *pTmpAttr = pA;
	va_start( arglist, pA );
	while( (sName=va_arg(arglist,char*))!=NULL )
	{
		if(pTmpAttr==NULL) return NULL;
		pTmpAttr = pTmpAttr->GetAttributeClass(sName);
	}
	va_end(arglist);

	return pTmpAttr;
}

DWORD BIUtils::GetIntervalColor(DWORD minV,DWORD maxV, float fpar)
{
	long a = minV>>24L;
	long ad = (long)(maxV>>24L) - a;
	long r = (minV&0xFF0000)>>16;
	long rd = (long)((maxV&0xFF0000)>>16) - r;
	long g = (minV&0xFF00)>>8;
	long gd = (long)((maxV&0xFF00)>>8) - g;
	long b = minV&0xFF;
	long bd = (long)(maxV&0xFF) - b;

	a += long(ad*fpar);
	r += long(rd*fpar);
	g += long(gd*fpar);
	b += long(bd*fpar);

	return ARGB(a,r,g,b);
}

bool BIUtils::GetIntervalRect(float fk, const FRECT& r1, const FRECT& r2, FRECT& rOut)
{
	rOut.left = r1.left + fk*(r2.left-r1.left);
	rOut.top = r1.top + fk*(r2.top-r1.top);
	rOut.right = r1.right + fk*(r2.right-r1.right);
	rOut.bottom = r1.bottom + fk*(r2.bottom-r1.bottom);
	return true;
}

long BIUtils::GetMaxFromFourLong( long n1, long n2, long n3, long n4 )
{
	if( n1>=n2 && n1>=n3 && n1>=n4 ) return n1;
	if( n2>=n3 && n2>=n4 ) return n2;
	if( n3>=n4 ) return n3;
	return n4;
}

float BIUtils::GetFromStr_Float(const char* &pcStr, float fDefault)
{
	if( !pcStr ) return fDefault;
	long n;
	char ctmp[64];
	for( n=0; pcStr[0]!=',' && pcStr[0]; pcStr++ )
	{
		if( pcStr[0]<=32 ) continue;
		if( n<sizeof(ctmp)-1 )
			ctmp[n++] = pcStr[0];
	}
	ctmp[n] = 0;
	while( pcStr[0] == ',' ) pcStr++;
	return (float)atof(ctmp);
}

void BIUtils::FillTextInfoArray(VDX8RENDER* pRS, ATTRIBUTES* pA, array<BITextInfo>& tia)
{
	if( !pA ) return;
	tia.DelAll();

	long q = pA->GetAttributesNum();
	for( long n=0; n<q; n++ )
	{
		long i = tia.Add();
		tia[i].Init( pRS, pA->GetAttributeClass(n) );
	}
}

void BIUtils::PrintTextInfoArray(array<BITextInfo>& tia)
{
	for( long n=0; n<tia; n++ )
		tia[n].Print();
}


BITextInfo::BITextInfo()
{
	pRS = 0;
	nFont = -1;
}

BITextInfo::~BITextInfo()
{
	Release();
}

void BITextInfo::Release()
{
	sText = "";
	FONT_RELEASE(pRS,nFont);
}

void BITextInfo::Init(VDX8RENDER* rs, ATTRIBUTES *pA)
{
	FONT_RELEASE(pRS,nFont);
	pRS = rs;
	if( !pRS || !pA ) return;

	nFont = pRS->LoadFont( pA->GetAttribute("font") );
	fScale = pA->GetAttributeAsFloat( "scale", 1.f );
	dwColor = pA->GetAttributeAsDword( "color", 0xFFFFFFFF );
	bShadow = pA->GetAttributeAsDword( "shadow", 1 )!=0;

	ATTRIBUTES* pAttr = pA->GetAttributeClass( "pos" );
	if( pAttr ) {
		pos.x = pAttr->GetAttributeAsDword( "x", 0 );
		pos.y = pAttr->GetAttributeAsDword( "y", 0 );
	} else {
		pos.x = pos.y = 0;
	}

	sText = pA->GetAttribute( "text" );

	pARefresh = 0;
	if( pA->GetAttributeAsDword("refreshable",0) )
		pARefresh = pA;
}

void BITextInfo::Print()
{
	if( nFont!=-1 )
	{
		if( pARefresh )
			sText = pARefresh->GetAttribute( "text" );
		if( !sText.IsEmpty() )
			pRS->ExtPrint( nFont, dwColor, 0, ALIGN_CENTER, bShadow, fScale, 0,0, pos.x,pos.y, "%s", sText.GetBuffer() );
	}
}

BILinesInfo::BILinesInfo() :
	lines(_FL_)
{
	pRS = 0;
}

BILinesInfo::~BILinesInfo()
{
	Release();
}

void BILinesInfo::Release()
{
	lines.DelAll();
}

void BILinesInfo::Init(VDX8RENDER* rs, ATTRIBUTES *pA)
{
	pRS = rs;
	if( !pA ) return;

	long q = pA->GetAttributesNum();
	for( long n=0; n<q; n++ )
	{
		ATTRIBUTES* pAttr = pA->GetAttributeClass(n);
		if( !pAttr ) break;

		long bi = lines.Add();
		long ei = lines.Add();

		lines[bi].rhw = lines[ei].rhw = 0.5f;
		lines[bi].vPos.z = lines[ei].vPos.z = 1.f;
		lines[bi].dwColor = lines[ei].dwColor = pAttr->GetAttributeAsDword("color");
		ATTRIBUTES* pAPos = pAttr->GetAttributeClass("begin");
		if( pAPos ) {
			lines[bi].vPos.x = (float)pAPos->GetAttributeAsDword("x",0);
			lines[bi].vPos.y = (float)pAPos->GetAttributeAsDword("y",0);
		} else {
			lines[bi].vPos.x = lines[bi].vPos.y = 0.f;
		}
		pAPos = pAttr->GetAttributeClass("end");
		if( pAPos ) {
			lines[ei].vPos.x = lines[bi].vPos.x + pAPos->GetAttributeAsDword("x",0);
			lines[ei].vPos.y = lines[bi].vPos.y + pAPos->GetAttributeAsDword("y",0);
		} else {
			lines[ei].vPos.x = lines[ei].vPos.y = 0.f;
		}
	}
}

void BILinesInfo::Draw()
{
	pRS->DrawLines2D( &lines[0], lines/2, "AILine" );
}



BIImagesInfo::BIImagesInfo() :
	images(_FL)
{
	pRS = 0;
	pImgRender = 0;
}

BIImagesInfo::~BIImagesInfo()
{
	Release();
}

void BIImagesInfo::Release()
{
	images.DelAllWithPointers();
	DELETE( pImgRender );
}

void BIImagesInfo::Init(VDX8RENDER* rs, ATTRIBUTES *pA)
{
	if(!pA || !rs) return;

	pRS = rs;
	pImgRender = NEW BIImageRender(rs);
	if(!pImgRender) return;

	long q = pA->GetAttributesNum();
	for(long n=0; n<q; n++)
	{
		ATTRIBUTES* pAImg = pA->GetAttributeClass( n );
		if( !pAImg ) continue;
		FRECT rUV; FULLRECT(rUV);
		BIUtils::ReadRectFromAttr( pAImg,"uv",rUV,rUV );
		RECT rPos; ZERO(rPos);
		BIUtils::ReadRectFromAttr( pAImg,"pos",rPos,rPos );
		IBIImage* pCurImg = pImgRender->CreateImage(BIType_square,
			pAImg->GetAttribute("texture"),
			pAImg->GetAttributeAsDword("color",ARGB(255,128,128,128)),
			rUV, rPos );
		if( pCurImg ) images.Add( pCurImg );
	}
}

void BIImagesInfo::Draw()
{
	if( pImgRender )
		pImgRender->Render();
}




BIBorderInfo::BIBorderInfo()
{
	pRS = 0;
	nVBuf = -1;
	nTexID = -1;
	bUp = true;
	bUsed = false;
}

BIBorderInfo::~BIBorderInfo()
{
	Release();
}

void BIBorderInfo::Release()
{
	VERTEX_BUFFER_RELEASE(pRS, nVBuf);
	TEXTURE_RELEASE(pRS, nTexID);
}

void BIBorderInfo::Init(VDX8RENDER* rs, ATTRIBUTES *pA)
{
	pRS = rs;
	nVBuf = rs->CreateVertexBuffer( BI_COLOR_VERTEX_FORMAT, 2*5*sizeof(BI_COLOR_VERTEX), D3DUSAGE_WRITEONLY );
	dwColor1 = dwColor2 = ARGB(255,255,255,255);
	ext_pos.left = 0.f; ext_pos.top = 0.f; ext_pos.right = 1024.f; ext_pos.bottom = 768.f;
	int_pos1.left = 20.f; int_pos1.top = 20.f; int_pos1.right = 1004.f; int_pos1.bottom = 748.f;
	int_pos2 = int_pos1;
	fCur = 0.f;
	fSpeed = 1.f;
	nTexID = -1;
	bUp = true;
	bUsed = true;
	if( !pA ) return;
	dwColor1 = pA->GetAttributeAsDword( "color1", dwColor1 );
	dwColor2 = pA->GetAttributeAsDword( "color2", dwColor2 );
	BIUtils::ReadRectFromAttr(pA,"extpos",ext_pos,ext_pos);
	BIUtils::ReadRectFromAttr(pA,"intpos1",int_pos1,int_pos1);
	BIUtils::ReadRectFromAttr(pA,"intpos2",int_pos2,int_pos2);
	fSpeed = pA->GetAttributeAsFloat( "speed", fSpeed*1000.f ) * 0.001f;
	nTexID = pRS->TextureCreate( pA->GetAttribute("texture") );
	bUsed = pA->GetAttributeAsDword("used",0)!=0;
}

void BIBorderInfo::Draw()
{
	if( !bUsed || nVBuf<0 ) return;
	BI_COLOR_VERTEX* pV = (BI_COLOR_VERTEX*)pRS->LockVertexBuffer( nVBuf );
	if( !pV ) return;

	if( bUp ) {
		fCur += fSpeed * api->GetDeltaTime();
		if( fCur > 1.f ) {fCur = 1.f; bUp=false;}
	} else {
		fCur -= fSpeed * api->GetDeltaTime();
		if( fCur < 0.f ) {fCur = 0.f; bUp=true;}
	}
	dword dwColor = BIUtils::GetIntervalColor( dwColor1, dwColor2, fCur );
	FRECT int_pos;
	BIUtils::GetIntervalRect( fCur, int_pos1,int_pos2, int_pos );

	for( long n=0; n<10; n++ ) {
		pV[n].col = dwColor;
		pV[n].w = 0.5f;
		pV[n].pos.z = 1.f;
	}
	//
	pV[0].tu = pV[1].tu = pV[4].tu = pV[5].tu = pV[8].tu = pV[9].tu = 0.f;
	pV[2].tu = pV[3].tu = pV[6].tu = pV[7].tu = 0.96f;
	pV[0].tv = pV[2].tv = pV[4].tv = pV[6].tv = pV[8].tv = 0.f;
	pV[1].tv = pV[3].tv = pV[5].tv = pV[7].tv = pV[9].tv = 0.96f;
	//
	pV[0].pos.x = ext_pos.left; pV[0].pos.y = ext_pos.top;
	pV[1].pos.x = int_pos.left; pV[1].pos.y = int_pos.top;
	pV[2].pos.x = ext_pos.right; pV[2].pos.y = ext_pos.top;
	pV[3].pos.x = int_pos.right; pV[3].pos.y = int_pos.top;
	pV[4].pos.x = ext_pos.right; pV[4].pos.y = ext_pos.bottom;
	pV[5].pos.x = int_pos.right; pV[5].pos.y = int_pos.bottom;
	pV[6].pos.x = ext_pos.left; pV[6].pos.y = ext_pos.bottom;
	pV[7].pos.x = int_pos.left; pV[7].pos.y = int_pos.bottom;
	pV[8].pos.x = ext_pos.left; pV[8].pos.y = ext_pos.top;
	pV[9].pos.x = int_pos.left; pV[9].pos.y = int_pos.top;
	pRS->UnLockVertexBuffer( nVBuf );

	pRS->TextureSet( 0, nTexID );
	pRS->DrawPrimitive( D3DPT_TRIANGLESTRIP, nVBuf, sizeof(BI_COLOR_VERTEX), 0, 8, "battle_msg" );
}
