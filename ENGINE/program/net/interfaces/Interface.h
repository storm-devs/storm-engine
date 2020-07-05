#ifndef NET_INTERFACE_H
#define NET_INTERFACE_H

#define NET_FG_LAN			0
#define NET_FG_INTERNET		1
#define NET_FG_FAVORITE		2

#define NET_MAX_SERVERS		4096

bool bFG_Deathmatch = true;
bool bFG_TeamDeathmatch = true;
bool bFG_DefendConvoy = true;
bool bFG_CaptureTheFort = true;
bool bFG_ActiveServers = false;
bool bFG_WOPassword = false;
bool bFG_FreeServers = false;
int	iFG_Credit = 1000;
int	iFG_MaxShipClass = 1;
int	iFG_Ping = 300;

int iCurrentServersList;
int iNumLanServers, iNumInternetServers;
int iNumLanServersList, iNumInternetServersList, iNumFavServersList;

object	LanServers[NET_MAX_SERVERS], InternetServers[NET_MAX_SERVERS];
int		LanServersList[NET_MAX_SERVERS], InternetServersList[NET_MAX_SERVERS], FavServersList[NET_MAX_SERVERS];

void Picture_SetPicture(string sPictureControl, string sTexture)
{
	SendMessage(&GameInterface, "lslls", MSG_INTERFACE_MSG_TO_NODE, sPictureControl, 2, false, sTexture);
}

void Picture_SetGroupPicture(string sPictureControl, string sGroup, string sPicture)
{
	SendMessage(&GameInterface, "lslss", MSG_INTERFACE_MSG_TO_NODE, sPictureControl, 6, sGroup, sPicture);
}

void Table_UpdateWindow(string sTableControl)
{
	SendMessage(&GameInterface, "lsl", MSG_INTERFACE_MSG_TO_NODE, sTableControl, 0);
}

void Table_Clear(string sTableControl, bool bClearHeader, bool bClearContent, bool bUpdateWindow)
{
	if (bClearHeader)
	{
	}

	if (bClearContent)
	{
		for (int i=1; i<100000; i++) 
		{
			string sA = sTableControl + "." + "tr" + i;
			if (!CheckAttribute(&GameInterface, sA)) { break; }
			DeleteAttribute(&GameInterface, sA);
		}
	}

	if (bUpdateWindow)
	{
		Table_UpdateWindow(sTableControl);
	}
}

void Button_SetText(string sButtonControl, string sText)
{
	SendMessage( &GameInterface, "lsls", MSG_INTERFACE_MSG_TO_NODE, sButtonControl, 0, sText); 
}

void Button_SetEnable(string sButtonControl, bool bEnable)
{
	SetSelectable(sButtonControl, bEnable);
}

void CheckButton_SetDisable(string sControl, int iControlIndex, bool bDisableState)
{
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE, sControl, 5, iControlIndex, bDisableState);
}

int CheckButton_GetState(string sControl, int iControlIndex)
{
	return SendMessage(&GameInterface, "lsll", MSG_INTERFACE_MSG_TO_NODE, sControl, 3, iControlIndex);
}

void CheckButton_SetState(string sControl, int iControlIndex, bool bState)
{
	SendMessage(&GameInterface, "lslll", MSG_INTERFACE_MSG_TO_NODE, sControl, 2, iControlIndex, bState);
}

void StringCollection_SetText(string sControl, int iControlIndex, string sText)
{
	SendMessage(&GameInterface, "lslls", MSG_INTERFACE_MSG_TO_NODE, sControl, 1, iControlIndex, sText);
}

void StringCollection_SetTextValue(string sControl, int iControlIndex, int iValue)
{
	string sValue = "#" + iValue;
	SendMessage(&GameInterface, "lslls", MSG_INTERFACE_MSG_TO_NODE, sControl, 1, iControlIndex, sValue);
}

void ImageCollection_ChangeTextureGroup(string sControl, string sNewTextureGroup)
{
	SendMessage(&GameInterface, "lsls", MSG_INTERFACE_MSG_TO_NODE, sControl, 1, sNewTextureGroup);
}

void ImageCollection_SetColor(string sControl, int iCIndex, int Color)
{
	SendMessage(&GameInterface, "lslll", MSG_INTERFACE_MSG_TO_NODE, sControl, 3, iCIndex, Color);
}

void ScrollImage_SetPosition(string sControl, int iPosition)
{
	SendMessage(&GameInterface, "lsll", MSG_INTERFACE_MSG_TO_NODE, sControl, 1, iPosition);
}

#endif
