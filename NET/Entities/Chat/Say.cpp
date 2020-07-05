#include "Say.h"

NetSay::NetSay() : aSay(_FL_)
{
}

NetSay::~NetSay()
{
}

bool NetSay::Init()
{
	return true;
}

void NetSay::Execute(DWORD __DeltaTime)
{
	float fDeltaTime = float(__DeltaTime) * 0.001f;

	for (dword i=0; i<aSay.Size(); i++)
	{
		say_t & S = *aSay[i];

		if (S.fTime > 0.0f) 
		{
			S.fTime -= fDeltaTime;
			if (S.fTime < 0.0f)	S.fTime = 0.0f;
		}

		if (S.fTime < 3.0f)
			S.fAlpha = S.fTime / 3.0f;
	}
}

void NetSay::Realize(DWORD __DeltaTime)
{
	if (aSay.IsEmpty()) return;

	float fFontScale = AttributesPointer->GetAttributeAsFloat("FontScale", 0.9f);
	float fStartY = AttributesPointer->GetAttributeAsFloat("StartY", 0.9f);
	long iLines = (long)AttributesPointer->GetAttributeAsDword("ChatLines", 10);

	int iLast = aSay.Len();
	int iFirst = Max(long(0), aSay.Len() - iLines);

	D3DVIEWPORT9 vp8;
	Render().GetViewport(&vp8);
	long y = long(float(vp8.Height) * fStartY) - ((aSay.Len() >= iLines) ? iLines * 16 : aSay.Len() * 16);
	long x = 20;
	for (long i=iFirst; i<iLast; i++)
	{
		say_t & S = *aSay[i];

		long dy = (i - iFirst) * 16;
		dword dwAlpha = Min(long(S.fAlpha * 255.0f), long(255)) << 24L;
		if (S.Type == CHAT_MESSAGE)
		{
			say_t & S = *aSay[i];
			string sRes = S.sName + " : ";
			long iWidth = Render().StringWidth((char*)sRes.GetBuffer(), FONT_DEFAULT, fFontScale);
			Render().ExtPrint(FONT_DEFAULT, dwAlpha | 0x62AD00, 0x00000000, ALIGN_LEFT, 0, fFontScale, 0, 0, x, y + dy, (char*)sRes.GetBuffer());
			Render().ExtPrint(FONT_DEFAULT, dwAlpha | S.dwColor, 0x00000000, ALIGN_LEFT, 0, fFontScale, 0, 0, x + iWidth, y + dy, (char*)S.sMessage.GetBuffer());
		}
		if (S.Type == GAME_MESSAGE)
		{
			Render().ExtPrint(FONT_DEFAULT, dwAlpha | S.dwColor, 0x00000000, ALIGN_LEFT, 0, fFontScale, 0, 0, x, y + dy, (char*)S.sMessage.c_str());
		}
	}
}

NetSay::say_t * NetSay::GetSay()
{
	if (aSay.Size() > 100) 
	{
		DELETE(aSay[0]);
		aSay.Extract(0);
	}
	return aSay[aSay.Add(NEW say_t())];
}

dword NetSay::AttributeChanged(ATTRIBUTES * pA)
{
	if (*pA == "ChatMessage")
	{
		say_t * pS = GetSay();
		pS->dwColor = AttributesPointer->GetAttributeAsDword("Color");		// 0xFFFBFBFB
		pS->sMessage = pA->GetThisAttr();
		pS->sName = AttributesPointer->GetAttribute("SenderName");
		pS->fTime = AttributesPointer->GetAttributeAsFloat("Time", 30.0f);
		pS->Type = CHAT_MESSAGE;
		return 0;
	}

	if (*pA == "PersonMessage")
	{
		say_t * pS = GetSay();
		pS->dwColor = AttributesPointer->GetAttributeAsDword("Color");		// 0xFFEAFF00
		pS->sMessage = pA->GetThisAttr();
		pS->sName = AttributesPointer->GetAttribute("SenderName");
		pS->fTime = AttributesPointer->GetAttributeAsFloat("Time", 30.0f);
		pS->Type = CHAT_MESSAGE;
		return 0;
	}

	if (*pA == "TeamMessage")
	{
		say_t * pS = GetSay();
		pS->dwColor = AttributesPointer->GetAttributeAsDword("Color");		// 0xFF79DEFF
		pS->sMessage = pA->GetThisAttr();
		pS->sName = AttributesPointer->GetAttribute("SenderName");
		pS->fTime = AttributesPointer->GetAttributeAsFloat("Time", 30.0f);
		pS->Type = CHAT_MESSAGE;
		return 0;
	}

	if (*pA == "GameMessage")
	{
		say_t * pS = GetSay();
		pS->dwColor = AttributesPointer->GetAttributeAsDword("Color");		// 0xFF83F033
		pS->sMessage = pA->GetThisAttr();
		pS->fTime = AttributesPointer->GetAttributeAsFloat("Time", 30.0f);
		pS->Type = GAME_MESSAGE;
	}

	return 0;
}