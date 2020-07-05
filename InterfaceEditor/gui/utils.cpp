#include "utils.h"

VDX8RENDER* EdUtils::m_pRender = 0;

bool EdUtils::CheckPointInsideRect( const EdPoint& epnt, const EdRect& erect )
{
	if( epnt.x < erect.x || epnt.y < erect.y ) return false;
	if( epnt.x > erect.Right() || epnt.y > erect.Bottom() ) return false;
	return true;
}

void EdUtils::DrawColorRect( const EdRect& erPlace, dword dwColor )
{
}

void EdUtils::DrawColorSprite( const EdRect& erPlace, dword dwColor, long nTexture, const EdRect& erUV )
{
	EdVertexColorUV v[4];
	m_pRender->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, EdVF_ColorUV, 2, v, sizeof(EdVertexColorUV), "?technique" );
}
