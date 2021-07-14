#include "Weather.h"
#include "Astronomy.h"
#include "Lightning.h"
#include "SunGlow.h"
#include "WaterFlare.h"
#include "inlines.h"

CREATE_CLASS(WEATHER)

CREATE_CLASS(RAIN)

CREATE_CLASS(SUNGLOW)

CREATE_CLASS(LIGHTNING)

CREATE_CLASS(SKY)

CREATE_CLASS(WATERFLARE)

CREATE_CLASS(Astronomy)

WEATHER::WEATHER()
{
}

WEATHER::~WEATHER()
{
    CleanUP();
}

void WEATHER::SetDevice()
{
    pRS = static_cast<VDX9RENDER *>(core.CreateService("dx9render"));
    Assert(pRS);

    // LoadWeatherIni();
}

bool WEATHER::Init()
{
    SetDevice();
    SetBeginData();
    return true;
}

void WEATHER::SetBeginData()
{
    fFloats[whf_time_counter] = 0.f;
    fFloats[whf_time_speed] = 0.f;
    fUpdateFrequence = 0.f;

    // sun moving data
    bSunPresent = false;
    fSunHeight = 3.f / 4.f * PId2;
    fSunBegAngle = 0.f;
    fSunBegTime = 5.f;
    fSunEndAngle = PI;
    fSunEndTime = 23.f;
    // moon moving data
    bMoonPresent = false;
    fMoonHeight = 5.f / 4.f * PId2;
    fMoonBegAngle = PId2;
    fMoonBegTime = 23.5f;
    fMoonEndAngle = -PId2;
    fMoonEndTime = 4.5f;
}

void WEATHER::Move()
{
}

void WEATHER::Realize(uint32_t Delta_Time)
{
}

void WEATHER::Execute(uint32_t Delta_Time)
{
    /*    if (dwFrames) SetLong(whi_weather_update,0);

      WIN32_FIND_DATA    wfd;
      HANDLE h = fio->_FindFirstFile(WHT_INI_FILE,&wfd);
      if (INVALID_HANDLE_VALUE != h)
      {
        FILETIME ft_new = wfd.ftLastWriteTime;
        fio->_FindClose(h);

        if (CompareFileTime(&ft_old,&ft_new)!=0) LoadWeatherIni();
      }

      dwFrames--;
    */
    if (fFloats[whf_time_speed] != 0.f)
    {
        const auto fOldTimer = fFloats[whf_time_counter];
        fFloats[whf_time_counter] += core.GetDeltaTime() * fFloats[whf_time_speed];
        // change of day
        if (fFloats[whf_time_counter] > 24.f)
            fFloats[whf_time_counter] -= 24.f;

        UpdateSunMoonPos();

        if (static_cast<long>(fFloats[whf_time_counter] * fUpdateFrequence) !=
            static_cast<long>(fOldTimer * fUpdateFrequence))
        {
            core.Event("WeatherTimeUpdate", "f", fFloats[whf_time_counter]);
        }
        // updating date in scripts
        // if( fFloats[whf_time_counter] fOldTimer
    }
}

void WEATHER::UpdateSunMoonPos()
{
    // sun
    auto fK = (fFloats[whf_time_counter] - fSunBegTime) / (fSunEndTime - fSunBegTime);
    if (fK < 0.f || fK > 1.f)
    {
        bSunPresent = false;
        fFloats[whf_sun_height_angle] = -PId2;
        fFloats[whf_sun_azimuth_angle] = 0.f;

        vVectors[whv_sun_pos] = CVECTOR(0, -5000.f, 0);
    }
    else
    {
        bSunPresent = true;
        fFloats[whf_sun_height_angle] = (fSunHeight + 0.2f) * sinf(fK * PI) - 0.2f;
        fFloats[whf_sun_azimuth_angle] = fSunBegAngle + fK * (fSunEndAngle - fSunBegAngle);

        CVECTOR vSun;
        vSun.x = 5000.0f;
        vSun.y = 0.0f;
        vSun.z = 0.0f;
        RotateAroundY(vSun.x, vSun.y, cosf(-fFloats[whf_sun_height_angle]), sinf(-fFloats[whf_sun_height_angle]));
        RotateAroundY(vSun.x, vSun.z, cosf(fFloats[whf_sun_azimuth_angle]), sinf(fFloats[whf_sun_azimuth_angle]));
        vVectors[whv_sun_pos] = vSun;
    }

    // moon
    if (fMoonBegTime < fMoonEndTime)
        fK = (fFloats[whf_time_counter] - fMoonBegTime) / (fMoonEndTime - fMoonBegTime);
    else
    {
        fK = (fFloats[whf_time_counter] - fMoonBegTime);
        if (fK < 0.f)
            fK += 24.f;
        fK /= 24.f + fMoonEndTime - fMoonBegTime;
    }
    if (fK < 0.f || fK > 1.f)
    {
        bMoonPresent = false;
        /*fFloats[whf_sun_height_angle] = -PId2;
        fFloats[whf_sun_azimuth_angle] = 0.f;

        vVectors[whv_sun_pos] = CVECTOR(0,-5000.f,0);*/
    }
    else
    {
        bMoonPresent = true;
        fFloats[whf_sun_height_angle] = (fK < 0.5f) ? (fMoonHeight * fK * 2.f) : (fMoonHeight * (1.f - fK) * 2.f);
        fFloats[whf_sun_azimuth_angle] = fMoonBegAngle + fK * (fMoonEndAngle - fMoonBegAngle);

        CVECTOR vMoon;
        vMoon.x = 5000.0f;
        vMoon.y = 0.0f;
        RotateAroundY(vMoon.x, vMoon.y, cosf(-fFloats[whf_sun_height_angle]), sinf(-fFloats[whf_sun_height_angle]));
        vMoon.z = 0.0f;
        RotateAroundY(vMoon.x, vMoon.z, cosf(fFloats[whf_sun_azimuth_angle]), sinf(fFloats[whf_sun_azimuth_angle]));
        vVectors[whv_sun_pos] = vMoon;
    }
}

bool WEATHER::CreateState(ENTITY_STATE_GEN *state_gen)
{
    return true;
}

bool WEATHER::LoadState(ENTITY_STATE *state)
{
    return true;
}

void WEATHER::CleanUP()
{
    pRS->SetRenderState(D3DRS_FOGENABLE, false);
    pRS->LightEnable(0, false);
}

/*void WEATHER::LoadWeatherIni()
{
    char    section[256],param[256],str[256];
    long    i;
    uint32_t    r,g,b;

    CleanUP();

    INIFILE * ini;
    WIN32_FIND_DATA    wfd;
    HANDLE h = fio->_FindFirstFile(WHT_INI_FILE,&wfd);
    if (INVALID_HANDLE_VALUE != h)
    {
        ft_old = wfd.ftLastWriteTime;
        fio->_FindClose(h);
    }
    ini = fio->OpenIniFile(WHT_INI_FILE);
    if(!ini) throw std::runtime_error("weather.ini file not found!");

    //iHour = ini->GetLong(0,"iCurHour",0);
    iHour = AttributesPointer->GetAttributeAsuint32_t("Hour",0);

    sprintf_s(section,"%s%d:00",(iHour<10) ? "0" : "", iHour);

    ZERO4(fFloats,iLongs,dwColors,vVectors);

    // load float parameters
    for (i=0;i<iNumFloatNames;i++)
    {
        fFloats[FloatNames[i].dwCode] = (float)ini->GetDouble(section,FloatNames[i].name,1000.0);
    }

    // load long parameters
    for (i=0;i<iNumLongNames;i++)
    {
        iLongs[LongNames[i].dwCode] = ini->GetLong(section,LongNames[i].name,0);
    }

    // load colors parameters
    for (i=0;i<iNumColorNames;i++)
    {
        str[0] = 0;
        ini->ReadString(section,ColorNames[i].name,str,sizeof(str)-1,"255,255,255");
        sscanf(str,"%d,%d,%d",&r,&g,&b);
        dwColors[ColorNames[i].dwCode] = RGB(r,g,b);
    }

    // load vectors parameters

    // load harmonics
    if (GetLong(whi_harmonics_num)>0)
    {
        pHarmonics = new sea_harmonic_t[GetLong(whi_harmonics_num)];
        for (i=0;i<iLongs[whi_harmonics_num];i++)
        {
            str[0] = 0;
            sprintf_s(param,"Harmonic%d",i);
            ini->ReadString(section,param,str,sizeof(str)-1,"1,0.0,20.0,0.1202,80.87,-28.00");
            sea_harmonic_t *pH = &pHarmonics[i];
            pH->bUse = true;
            sscanf(str,"%d,%f,%f,%f,%f,%f",&pH->bUse,&pH->fAng,&pH->fLen,&pH->fAmp,&pH->fPhase,&pH->fPhaseDelta);
        }
    }
    // load strings
    for (i=0;i<iNumStringNames;i++)
    {
        str[0] = 0;
        ini->ReadString(section,StringNames[i].name,str,sizeof(str)-1,"");
        pStrings[StringNames[i].dwCode] = (char*)new char[strlen(str)+1];
        strcpy_s(pStrings[StringNames[i].dwCode],str);
    }

    SetLong(whi_weather_update,1);
    dwFrames = 0;

    delete ini;

    SetCommonStates();

    core.Trace("Weather: Load ini complete");
}*/

void WEATHER::SetCommonStates()
{
    pRS->SetRenderState(D3DRS_FOGENABLE, GetLong(whi_fog_enable));

    auto fDensity = GetFloat(whf_fog_density);
    pRS->SetRenderState(D3DRS_FOGCOLOR, GetColor(whc_fog_color));
    pRS->SetRenderState(D3DRS_FOGDENSITY, *((uint32_t *)&fDensity));

    const auto dwAmbient = GetColor(whc_sun_ambient);
    pRS->SetRenderState(D3DRS_AMBIENT, dwAmbient);

    // setup sun light
    const auto fSunHeightAngle = GetFloat(whf_sun_height_angle);
    const auto fSunAzimuthAngle = GetFloat(whf_sun_azimuth_angle);
    CVECTOR vSun, vSunColor, vSunLight;
    D3DLIGHT9 sun;

    if (fFloats[whf_time_speed] == 0.f)
    {
        vSun.x = 5000.0f;
        vSun.y = 0.0f;
        RotateAroundY(vSun.x, vSun.y, cosf(-fSunHeightAngle), sinf(-fSunHeightAngle));

        vSun.z = 0.0f;
        RotateAroundY(vSun.x, vSun.z, cosf(fSunAzimuthAngle), sinf(fSunAzimuthAngle));

        vVectors[whv_sun_pos] = vSun;
    }
    else
    {
        UpdateSunMoonPos();
        vSun = vVectors[whv_sun_pos];
    }

    GetColor(whc_sun_color, &vSunColor);

    sun.Type = D3DLIGHT_DIRECTIONAL;
    sun.Diffuse.r = vSunColor.x;
    sun.Diffuse.g = vSunColor.y;
    sun.Diffuse.b = vSunColor.z;
    sun.Diffuse.a = 1.0f;
    sun.Specular.r = vSunColor.x;
    sun.Specular.g = vSunColor.y;
    sun.Specular.b = vSunColor.z;
    sun.Specular.a = 1.0f;
    sun.Ambient.r = sun.Ambient.g = sun.Ambient.b = sun.Ambient.a = 0.0f;
    vSunLight = -(!(CVECTOR(vSun.x, vSun.y, vSun.z)));
    sun.Direction.x = vSunLight.x;
    sun.Direction.y = vSunLight.y;
    sun.Direction.z = vSunLight.z;
    sun.Range = 100000;
    sun.Attenuation0 = 1;
    sun.Attenuation1 = 0;
    sun.Attenuation2 = 0;
    sun.Falloff = 0;
    sun.Theta = 1;
    sun.Phi = 1;

    pRS->SetLight(0, &sun);
    pRS->LightEnable(0, true);

    pRS->SetRenderState(D3DRS_LIGHTING, false);
}

/*bool WEATHER::SetHour(long hour)
{
    if (iHour == hour) return false;
    iHour = hour;

    INIFILE *ini = fio->OpenIniFile(WHT_INI_FILE);
    if(!ini) throw std::runtime_error("weather.ini file not found!");
    ini->WriteLong(0,"iCurHour",iHour);
    delete ini;

    AttributesPointer->SetAttributeUseuint32_t("Hour",iHour);

    LoadWeatherIni();
    return true;
}*/

long WEATHER::GetLong(uint32_t dwCode)
{
    return iLongs[dwCode];
}

uint32_t WEATHER::GetColor(uint32_t dwCode, CVECTOR *vOut)
{
    vOut->x = static_cast<float>((dwColors[dwCode] >> 0x10) & 0xFF) / 255.0f;
    vOut->y = static_cast<float>((dwColors[dwCode] >> 0x8) & 0xFF) / 255.0f;
    vOut->z = static_cast<float>((dwColors[dwCode]) & 0xFF) / 255.0f;
    return dwColors[dwCode];
}

uint32_t WEATHER::GetColor(uint32_t dwCode)
{
    return dwColors[dwCode];
}

float WEATHER::GetFloat(uint32_t dwCode)
{
    return fFloats[dwCode];
}

void WEATHER::GetVector(uint32_t dwCode, CVECTOR *vOut)
{
    *vOut = vVectors[dwCode];
}

uint32_t WEATHER::AttributeChanged(ATTRIBUTES *pAttribute)
{
    auto *const pParent = pAttribute->GetParent(); // if (*pAttribute == "Hour")
    if (*pParent == "fog")
    {
        if (*pAttribute == "Enable")
        {
            iLongs[whi_fog_enable] = pAttribute->GetAttributeAsDword();
            return 0;
        }
        if (*pAttribute == "Start")
        {
            fFloats[whf_fog_start] = pAttribute->GetAttributeAsFloat();
            return 0;
        }
        if (*pAttribute == "Density")
        {
            fFloats[whf_fog_density] = pAttribute->GetAttributeAsFloat();
            return 0;
        }
        if (*pAttribute == "Color")
        {
            dwColors[whc_fog_color] = pAttribute->GetAttributeAsDword();
            return 0;
        }
    }
    if (*pParent == "wind")
    {
        if (*pAttribute == "Angle")
        {
            fFloats[whf_wind_angle] = pAttribute->GetAttributeAsFloat();
            return 0;
        }
        if (*pAttribute == "Speed")
        {
            fFloats[whf_wind_speed] = pAttribute->GetAttributeAsFloat();
            return 0;
        }
        return 0;
    }
    if (*pParent == "sun")
    {
        if (*pAttribute == "Height")
        {
            fSunHeight = pAttribute->GetAttributeAsFloat();
            return 0;
        }
        if (*pAttribute == "BegAngle")
        {
            fSunBegAngle = pAttribute->GetAttributeAsFloat();
            return 0;
        }
        if (*pAttribute == "BegTime")
        {
            fSunBegTime = pAttribute->GetAttributeAsFloat();
            return 0;
        }
        if (*pAttribute == "EndAngle")
        {
            fSunEndAngle = pAttribute->GetAttributeAsFloat();
            return 0;
        }
        if (*pAttribute == "EndTime")
        {
            fSunEndTime = pAttribute->GetAttributeAsFloat();
            return 0;
        }
        if (*pAttribute == "Color")
        {
            dwColors[whc_sun_color] = pAttribute->GetAttributeAsDword();
            return 0;
        }
        if (*pAttribute == "Ambient")
        {
            dwColors[whc_sun_ambient] = pAttribute->GetAttributeAsDword();
            return 0;
        }
        if (fFloats[whf_time_speed] == 0.f)
        {
            if (*pAttribute == "HeightAngle")
            {
                fFloats[whf_sun_height_angle] = pAttribute->GetAttributeAsFloat();
                return 0;
            }
            if (*pAttribute == "AzimuthAngle")
            {
                fFloats[whf_sun_azimuth_angle] = pAttribute->GetAttributeAsFloat();
                return 0;
            }
        }
    }
    if (*pParent == "moon")
    {
        if (*pAttribute == "Height")
        {
            fMoonHeight = pAttribute->GetAttributeAsFloat();
            return 0;
        }
        if (*pAttribute == "BegAngle")
        {
            fMoonBegAngle = pAttribute->GetAttributeAsFloat();
            return 0;
        }
        if (*pAttribute == "BegTime")
        {
            fMoonBegTime = pAttribute->GetAttributeAsFloat();
            return 0;
        }
        if (*pAttribute == "EndAngle")
        {
            fMoonEndAngle = pAttribute->GetAttributeAsFloat();
            return 0;
        }
        if (*pAttribute == "EndTime")
        {
            fMoonEndTime = pAttribute->GetAttributeAsFloat();
            return 0;
        }
    }
    if (*pParent == "time")
    {
        if (*pAttribute == "time")
        {
            fFloats[whf_time_counter] = pAttribute->GetAttributeAsFloat();
            return 0;
        }
        if (*pAttribute == "speed")
        {
            fFloats[whf_time_speed] = pAttribute->GetAttributeAsFloat();
            if (fFloats[whf_time_speed] != 0.f)
                fFloats[whf_time_speed] = 0.001f / fFloats[whf_time_speed];
            return 0;
        }
        if (*pAttribute == "updatefrequence")
        {
            fUpdateFrequence = pAttribute->GetAttributeAsFloat();
            return 0;
        }
    }
    if (*pAttribute == "isDone")
    {
        SetCommonStates();
    }
    return 0;
}
