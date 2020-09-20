
#include "locations\locations_loader.c"
#include "locations\locations_camera.c"


extern void InitLocations();


native int NativeFindCharacter(ref charactersArray, string id);
native int NativeFindLocation(ref charactersArray, string id);
native int NativeFindLoadCharacter();
native void SetReloadProgressBackImage(string imageName);
native void ReloadProgressStart();
native void ReloadProgressUpdate();
native void ReloadProgressEnd();
native void SystemDelay(int millsec);
native void ExecuteTechnique(string techName);
native string GetNextLineString();
native void SetReloadNextTipsImage();
#libriary "ScriptLocationLibrary"


void LocationInit()
{
	locCameraInit();
	if(LoadSegment("locations\locations_init.c"))
	{
		InitLocations();
		UnloadSegment("locations\locations_init.c");
	}
	locDisableUpdateTime = false;
	locTmpTime = 0.0;
}
/*
void LocationTestProcess()
{
	SetReloadProgressBackImage("Loading\initialization.tga.tx");
	ReloadProgressStart();
	LocationInit();
	Locations_TestAll();
	ReloadProgressEnd();
}
*/

int FindLoadedLocation()
{
	return NativeFindLoadCharacter();
}

bool SetLocatorGroupRadius(ref loc, string grpName, float rad)
{
	return SendMessage(loc, "lsf", MSG_LOCATION_GRP_RADIUS, grpName, rad);
}

bool SetLocatorRadius(ref loc, string grpName, string locName, float rad)
{
	return SendMessage(loc, "lssf", MSG_LOCATION_LOC_RADIUS, grpName, locName, rad);
}


bool VisibleLocatorsGroup(string group, float kRadius, float viewLabelsDist, int a, int r, int g, int b)
{
	
	int i = FindLoadedLocation();
	if(i < 0) return false;
	int color = argb(a, r, g, b);
	return SendMessage(&Locations[i], "lsffl", MSG_LOCATION_VIEWLGROUP, group, kRadius, viewLabelsDist, color);
}

void HideLocatorsGroup(string group)
{
	int i = FindLoadedLocation();
	if(i < 0) return;
	SendMessage(&Locations[i], "ls", MSG_LOCATION_HIDELGROUP, group);
}

bool WaitNight()
{
	return true;
}


bool WaitNightPause(bool paused)
{
	return true;
}

void LocationWaitNihgtEndFadeOut()
{
	bool isFadeIn = GetEventData();
	aref fader = GetEventData();
	if(isFadeIn == false)
	{
		SendMessage(&fader, "lfl", FADER_IN, 1.0, true);
	}
	SetEventHandler("FaderEvent_EndFadeIn", "LocationWaitNihgtEndFadeIn", 0);
	DelEventHandler("FaderEvent_EndFade", "LocationWaitNihgtEndFadeOut");
}

void LocationWaitNihgtEndFadeIn()
{
	bool isFadeIn = GetEventData();
	aref fader = GetEventData();
	if(isFadeIn != false)
	{
		DelEventHandler("FaderEvent_EndFadeIn", "LocationWaitNihgtEndFadeIn");
		int i = FindLoadedLocation();
		if(i >= 0)
		{
			SendMessage(&Locations[i], "ll", MSG_LOCATION_PAUSE, false);
		}
		PostEvent("LocationWaitNihgtEnd", 1);
	}	
}

//navy -->
void LocationMakeClone(string _locId)
{
	ref rOrg, rClone;
	int iOrg, iClone;

	iOrg = FindLocation(_locId);
	iClone = FindLocation("Clone_location");

	makeref(rOrg, Locations[iOrg]);
	makeref(rClone, Locations[iClone]);

	DeleteAttribute(rClone, "");
	CopyAttributes(rClone, rOrg);
	rClone.id = "Clone_location";
	rClone.type = "clone";
	rClone.index = iClone;
//а так не работает... :) проверка только на аттрибут ;)	
//	rClone.habitues = false;
	DeleteAttribute(rClone, "habitues");

	pchar.questTemp.LocationClone = true;
	pchar.questTemp.LocationClone.id = rOrg.id;
}
//navy <--