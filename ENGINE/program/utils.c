#define PARSE_STRING	"PARSE_STRING"

#event_handler(PARSE_STRING, "ParseString");

int AddStr2Array(ref rNames, int iNum, string sStr)
{
	SetArraySize(rNames, iNum + 1);
	rNames[iNum] = sStr;
	iNum++;
	return iNum;
}
// boal 101004 -->
string GetSubStringByNum(string sStr, int iSelect)
{
	string sTemp;
	int iFindPos = findSubStr(&sStr, ",", 0);
	if (iFindPos < 0) return sStr;	// one word in string
	int iNumFind = 1;
    while (iFindPos > 0)
	{
		iNumFind++;
		iFindPos = findSubStr(&sStr, ",", iFindPos + 1);
	}
	if (iNumFind <= iSelect)
	{
        return GetSubStringByNum(sStr, 0);
	}
	int	iLastPos = 0;
	iFindPos = 0;
	for (int i=0; i<iNumFind; i++)
	{
		iFindPos = findSubStr(&sStr, ",", iFindPos + 1);
		if (i == iSelect)
		{
			if (iFindPos < 0)
            {
                iFindPos = strlen(&sStr);
            }
            if (iLastPos >=  iFindPos) iFindPos = iLastPos + 2;
			sTemp = strcut(&sStr, iLastPos, iFindPos - 1);
			return sTemp;
		}
		iLastPos = iFindPos + 1;
	}

	return GetSubStringByNum(sStr, 0);
}
// boal <--
string GetRandSubString(string sStr)
{
	string sTemp;

    if (strlen(sStr) > 0)//fix
    {
    	//int iFindPos = findSubStr(&sStr, ",", 0);
    	int iFindPos = findSubStr(sStr, ",", 0);
    	if (iFindPos < 0) return sStr;	// one word in string
    	int iNumFind = 1;
    	while (iFindPos > 0)
    	{
    		iNumFind++;
    		//iFindPos = findSubStr(&sStr, ",", iFindPos + 1);
    		iFindPos = findSubStr(sStr, ",", iFindPos + 1);
    	}
    	int iSelect = rand(iNumFind - 1);
    	int	iLastPos = 0;
    	iFindPos = 0; // вот где собака порылась!!!!! считай я убил Вано :) Boal
    	for (int i=0; i<iNumFind; i++)
    	{
    		//iFindPos = findSubStr(&sStr, ",", iFindPos + 1);
    		iFindPos = findSubStr(sStr, ",", iFindPos + 1);
    		if (i == iSelect)
    		{
    			if (iFindPos == -1)
                {
                    //iFindPos = strlen(&sStr);
                    iFindPos = strlen(sStr);
                }
                // заколебало!!!
                if (iLastPos >=  iFindPos) return "";
    			//sTemp = strcut(&sStr, iLastPos, iFindPos - 1);
    			sTemp = strcut(sStr, iLastPos, iFindPos - 1);
    			return sTemp;
    		}
    		iLastPos = iFindPos + 1;
    	}

    	return "If you see this, you can kick Vano :)";
	}
	return   "";
}

float GetDotProduct(float fA1, float fA2)
{
    return cos(fA2 - fA1);  // boal бешенная оптимизация моря

	/*float x1, y1, x2, y2;
	x1 = sin(fA1);	y1 = cos(fA1);
	x2 = sin(fA2);	y2 = cos(fA2);
	return (x2 * x1 + y2 * y1); */
}

/*float Clampf(float fClamp)
{
	if (fClamp > 1.0) return 1.0;
	if (fClamp < 0.0) return 0.0;
	return fClamp;
}*/

/*float Bring2Range(float fMin1, float fMax1, float fMin2, float fMax2, float fValue)
{
	if (fValue < fMin2) { fValue = fMin2; }
	if (fValue > fMax2) { fValue = fMax2; }
	float fDelta = (fValue - fMin2) / (fMax2 - fMin2);
	return fMin1 + fDelta * (fMax1 - fMin1);
}*/

void ParseString()
{
	int		i, iLen;
	string	sParams[2];
	string	sDst = "";

	aref aRes = GetEventData();
	string rSourceString = GetEventData();
	int	iNumParameters = GetEventData();

	//Trace("parser started: " + rSourceString + ", params = " + iNumParameters);

	if (iNumParameters == 0)
	{
		aRes.Str = rSourceString;
		return;
	}

	SetArraySize(&sParams, iNumParameters + 1);
	for (i=0; i<iNumParameters; i++) { sParams[i] = GetEventData();	}

	iLen = strlen(&rSourceString);
	for (i=0; i<iLen; i++)
	{
		string sChar = GetSymbol(&rSourceString, i);
		if (sChar == "%")
		{
			if (GetSymbol(&rSourceString, i + 1) == "%")
			{
				sDst = sDst + "%"; 
				i++;
				continue;
			}
			// get parameter number 
			string sNumber = "";
			i++;
			while (i < iLen)
			{
				string sTmp = GetSymbol(&rSourceString, i);
				if (!isDigit(&sTmp, 0)) { break; }
				sNumber = sNumber + sTmp;
				i++;
			}
			sDst = sDst + sParams[sti(sNumber) - 1]; i--;
			continue;
		}
		sDst = sDst + sChar;
	}

	aRes.Str = sDst;
}

float GetDistance2D(float x1, float y1, float x2, float y2)
{
	return sqrt(sqr(x1 - x2) + sqr(y1 - y2));
}

void RotateAroundY(ref rX, ref rZ, float fCos, float fSin)
{
	float xxx = rX;
	float zzz = rZ;
	float xx = xxx * fCos + zzz * fSin;
	float zz = zzz * fCos - xxx * fSin;
	rX = xx;
	rZ = zz;
}

int Tonnes2CWT(int iTonnes)
{
	int a = iTonnes * 10000 / 508;
	a = 100 * ((a + 99) / 100);
	return a;
}

int i_min(int a1, int a2)
{
	if (a1 < a2) { return a1; }
	return a2;
}

/*float frand(float fMin, float fMax, int digit)
{
	int nDiv = pow(10,digit);
	int n = rand(100000);
	float fRes = fMin+(fMax-fMin)*n/100000.0;
	fRes = makefloat( makeint(fRes*ndiv+0.5) )/nDiv;
	return fRes;
}*/

// boal -->
string GetVerNum()
{
 	string  VERSION_STR = VERSION_NUMBER2;

    if (MOD_BETTATESTMODE == "On")
    {
       VERSION_STR = VERSION_STR + "  DEV";
    }
    if (MOD_BETTATESTMODE == "Test")
    {
       VERSION_STR = VERSION_STR + "  TST";
    }
    return VERSION_STR;
}

void Log_Info(string _str)
{
    Log_SetStringToLog(_str);
}

void Log_TestInfo(string logtext)
{
	if (bBettaTestMode)
	{
		Log_SetStringToLog(logtext);
		trace("TestInfo: " + GetQuestBookDataDigit() + " " + logtext)
	}
}
void Log_QuestInfo(string _info)
{
    if (bQuestLogShow)
	{
		Log_SetStringToLog(_info);
	}
}

string NewStr()
{
    int idLngFile = LanguageOpenFile("ItemsDescribe.txt");
    string sTemp;
    sTemp = LanguageConvertString(idLngFile,"new_string");
    LanguageCloseFile(idLngFile);

    return sTemp;
}

Object objGrass;

void CreateGrass(string sDataFile, string sTextureFile, float fScale, float fW,float fH, float fMinDist,float fMaxDist, float fMinLod)
{
	CreateEntity(&objGrass,"Grass");
	if( bSeaActive && !bAbordageStarted )
	{
		LayerAddObject(SEA_EXECUTE,&objGrass,1000);
		LayerAddObject(SEA_REALIZE,&objGrass,1000);
	}
	else
	{
		LayerAddObject("execute",&objGrass,1000);
		LayerAddObject("realize",&objGrass,1000);
	}	
	SendMessage(objGrass,"ls",41666, sTextureFile); // #define MSG_GRASS_SET_TEXTURE 41666
	SendMessage(objGrass,"lffffff",42666, fScale, fW,fH, fMinDist,fMaxDist, fMinLod); // #define MSG_GRASS_SET_PARAM 42666
	SendMessage(objGrass,"ls",40666, sDataFile); // #define MSG_GRASS_LOAD_DATA 40666
}

void DeleteGrass()
{
	if (IsEntity(&objGrass))
	{
		DeleteClass(&objGrass); // boal грохнуть конкретный объект
	}
	DeleteEntitiesByType("Grass");
}

void HideGrass()
{
	if (IsEntity(&objGrass))
	{
        LayerDelObject("execute", &objGrass);
		LayerDelObject("realize", &objGrass);
		LayerDelObject(SEA_EXECUTE, &objGrass);
		LayerDelObject(SEA_REALIZE, &objGrass);
	}
}

void ShowGrass()
{
	if (IsEntity(&objGrass))
	{
        LayerAddObject(SEA_EXECUTE, &objGrass, 1000);
		LayerAddObject(SEA_REALIZE, &objGrass, 1000);
	}
}
// boal 16.03.2004 -->
void CalcLocalTime(float _inc)
{
	if (locTmpTime >= _inc)
	{
		/*int hour = GetHour();
		if (AddTimeToCurrent(0, makeint(locTmpTime / _inc)) > 0)
		{
			//вызов уже внутри метода времени PostEvent("NextDay", 20); // наступил новый день
		}        */
        locTmpTime = 0.0;
        /*if (hour != GetHour() && !bStorm && !bTornado && !bDisableMapEnter && !LAi_grp_alarmactive) // не в бою
		{
            RefreshWeather();
		} */
    }
}
void RefreshWeather()
{
    Whr_UpdateWeather();
	// звуки
	/*
	if (bSeaActive && !bAbordageStarted)
	{
	    SetSchemeForSea();
	    if (Whr_IsDay() && sti(pchar.Ship.Lights) == 1)
	    {
	        //выключаем фонари.
			Ship_SetLightsOff(pchar, 15.0, true, true, false);
	    }
	}
	else
	{ // суша
	    LoadSceneSound();
	} */
}
void RefreshLandTime()
{
    objLandInterface.textinfo.datatext.text = XI_convertString("Date:") + GetQuestBookData();
}
// boal 16.03.2004 <--
string stripblank(string str)
{
	string retstr = "";
	int iMax = strlen(str);
	for(int i = 0; i < iMax; i++) {
		if(GetSymbol(str,i) != " ") { retstr += GetSymbol(str,i); }}
	return retstr;
}
// boal <--

int func_max(int a, int b)
{
	if (a > b) return a;
	return b;
}
int func_min(int a, int b)
{
	if (a > b) return b;
	return a;
}

void ResetTimeScale()
{
    SetTimeScale(1.0);
	TimeScaleCounter = 0;
	if(IsPerkIntoList("TimeSpeed"))
	{	DelPerkFromActiveList("TimeSpeed");
	}
}

void FreeChangeFlagMode(string _tmp)
{
	DeleteAttribute(pchar, "DisableChangeFlagMode");
}

string  xiStr(string _str) // просто сокращение
{
	return XI_ConvertString(_str);
}

string  xiDStr(string _str) // просто сокращение
{
	return GetConvertStr(_str, "DialogSource.txt");
}

void Dev_Trace(string _str) // логи только в дев версии
{
    if (MOD_BETTATESTMODE	==	"On")	Trace(_str);
}