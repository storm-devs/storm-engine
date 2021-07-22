#include "battle_navigator.h"
#include <stdio.h>

#include "core.h"

#include "../Utils.h"
#include "Entity.h"
#include "Island_Base.h"
#include "shared/battle_interface/msg_control.h"
#include "ship_base.h"
#include "ships_list.h"

#define NAVBACKGROUND_SIZESCALE 1.3f

#define MAP_TEXTURE_WIDTH 256
#define MAP_TEXTURE_HEIGHT 256
#define BIGMAP_TEXTURE_WIDTH 1024
#define BIGMAP_TEXTURE_HEIGHT 1024

inline bool FloatCompare(float a, float b)
{
    return ((a > b - .0001f) && (a < b + .0001f));
}

bool g_bExternMapCenter = false;
bool g_bMapScaling = false;
CVECTOR g_externPos;

void BATTLE_NAVIGATOR::CalculateTextureRect(FRECT &texRect, long num, long hq, long vq)
{
    const auto yNum = num / hq;
    const auto xNum = num - yNum * hq;

    texRect.left = 1.f / hq * xNum;
    texRect.right = 1.f / hq * (xNum + 1);
    texRect.top = 1.f / vq * yNum;
    texRect.bottom = 1.f / vq * (yNum + 1);
}

BATTLE_NAVIGATOR::BATTLE_NAVIGATOR()
{
    m_pOwnerEI = nullptr;
    m_dwFireZoneColor = 0x20FF0050;
    rs = nullptr;

    m_fShipSpeedScale = 1.f;

    m_pIslandTexture = nullptr;
    m_fCurScale = m_fDefaultScale = 1.f;
    m_bYesIsland = false;

    // textures
    m_idCompasTex = -1;
    m_idSpeedTex = -1;
    m_idCannonTex = -1;
    m_idEmptyTex = -1;
    m_idIslandTexture = -1;
    m_idChargeTexture = -1;
    m_idWindTex = -1;
    m_idBestCourseTex = -1;
    m_idWindTexture = -1;
    m_idSailTexture = -1;

    // buffers
    m_idEmptyVBuf = -1;
    m_idCourseVBuf = -1;
    m_idMapVBuf = -1;
    m_idFireZoneVBuf = -1;
    m_idCannonVBuf = -1;
    m_idSpeedVBuf = -1;
    m_idCurChargeVBuf = -1;

    m_idShipsVBuf = -1;
    m_nvShips = 0;

    m_speedFont = -1;
    m_wb = nullptr;
    m_pAWeather = nullptr;

    m_curCharge = -1;
    m_bNotEnoughBallFlag = false;
}

BATTLE_NAVIGATOR::~BATTLE_NAVIGATOR()
{
    ReleaseAll();
}

void BATTLE_NAVIGATOR::Draw() const
{
    if (rs == nullptr)
        return;
    int n;

    // set world matrix
    const CMatrix matw;
    rs->SetTransform(D3DTS_WORLD, matw);

    // gradient background
    // rs->DrawPrimitive(D3DPT_TRIANGLEFAN,m_idGradBackVBuf,sizeof(BI_COLORONLY_VERTEX),0,1,"battle_only_color");

    // Isle
    rs->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
    rs->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);

    rs->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
    if (m_idIslandTexture >= 0)
        rs->TextureSet(0, m_idIslandTexture);
    if (m_pIslandTexture != nullptr)
        rs->SetTexture(0, m_pIslandTexture);
    if (m_bYesIsland)
        rs->DrawPrimitive(D3DPT_TRIANGLEFAN, m_idMapVBuf, sizeof(BI_ONETEXTURE_VERTEX), 0, RADIAL_QUANTITY,
                          "battle_island");
    else
    {
        rs->SetRenderState(D3DRS_TEXTUREFACTOR, m_dwSeaColor);
        rs->DrawPrimitive(D3DPT_TRIANGLEFAN, m_idMapVBuf, sizeof(BI_ONETEXTURE_VERTEX), 0, RADIAL_QUANTITY,
                          "battle_only_tfactor");
    }
    rs->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
    rs->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

    // ships
    if (m_nvShips > 0)
        rs->DrawPrimitive(D3DPT_TRIANGLELIST, m_idShipsVBuf, sizeof(BI_COLORONLY_VERTEX), 0, m_nvShips / 3,
                          "battle_only_color");

    // cannons fire range
    if (m_idFireZoneVBuf != -1L)
    {
        rs->SetRenderState(D3DRS_TEXTUREFACTOR, m_dwFireZoneColor);
        rs->DrawPrimitive(D3DPT_TRIANGLEFAN, m_idFireZoneVBuf, sizeof(BI_NOTEXTURE_VERTEX), 0, FIRERANGE_QUANTITY - 2,
                          "battle_only_tfactor");
    }

    // compass and the background
    if (m_idEmptyVBuf != -1L)
    {
        // background of the minimap
        rs->TextureSet(0, m_idEmptyTex);
        rs->DrawPrimitive(D3DPT_TRIANGLESTRIP, m_idEmptyVBuf, sizeof(BI_ONETEXTURE_VERTEX), 0, 2, "battle_rectangle");
        // compass
        rs->TextureSet(0, m_idCompasTex);
        rs->DrawPrimitive(D3DPT_TRIANGLESTRIP, m_idEmptyVBuf, sizeof(BI_ONETEXTURE_VERTEX), 4, 2, "battle_rectangle");
    }

    // cannons charge indicator
    rs->TextureSet(0, m_idCannonTex);
    n = 0;
    if (m_nvCannonCharge > 0)
    {
        rs->SetRenderState(D3DRS_TEXTUREFACTOR, m_dwChargeCannon);
        rs->DrawPrimitive(D3DPT_TRIANGLEFAN, m_idCannonVBuf, sizeof(BI_ONETEXTURE_VERTEX), n, m_nvCannonCharge,
                          "battle_tf_rectangle");
        n += m_nvCannonCharge + 2;
    }
    if (m_nvCannonReady > 0)
    {
        rs->SetRenderState(D3DRS_TEXTUREFACTOR, m_dwReadyCannon);
        rs->DrawPrimitive(D3DPT_TRIANGLEFAN, m_idCannonVBuf, sizeof(BI_ONETEXTURE_VERTEX), n, m_nvCannonReady,
                          "battle_tf_rectangle");
        n += m_nvCannonReady + 2;
    }
    if (m_nvCannonDamage > 0)
    {
        rs->SetRenderState(D3DRS_TEXTUREFACTOR, m_dwDamagedCannon);
        rs->DrawPrimitive(D3DPT_TRIANGLEFAN, m_idCannonVBuf, sizeof(BI_ONETEXTURE_VERTEX), n, m_nvCannonDamage,
                          "battle_tf_rectangle");
    }

    // speed indicator
    /*rs->TextureSet(0,m_idSpeedTex);
    if(m_nvSpeed>0)
      rs->DrawPrimitive(D3DPT_TRIANGLEFAN,m_idSpeedVBuf,sizeof(BI_ONETEXTURE_VERTEX),0,m_nvSpeed,"battle_rectangle");*/

    // show wind direction arrow
    if (m_idEmptyVBuf != -1L)
    {
        rs->TextureSet(0, m_idWindTex);
        rs->DrawPrimitive(D3DPT_TRIANGLESTRIP, m_idEmptyVBuf, sizeof(BI_ONETEXTURE_VERTEX), 8, 2, "battle_rectangle");
    }

    // show heading angles
    if (m_idCourseVBuf != -1L && m_idBestCourseTex != -1L)
    {
        rs->TextureSet(0, m_idBestCourseTex);
        rs->DrawPrimitive(D3DPT_TRIANGLESTRIP, m_idCourseVBuf, sizeof(BI_ONETEXTURE_VERTEX), 0, 2, "battle_rectangle");
        rs->DrawPrimitive(D3DPT_TRIANGLESTRIP, m_idCourseVBuf, sizeof(BI_ONETEXTURE_VERTEX), 4, 2, "battle_rectangle");
    }

    // print wind and ship speed
    rs->ExtPrint(m_speedFont, 0xFFFFFFFF, 0, PR_ALIGN_CENTER, true, m_fFontScale, 0, 0, m_xWindSpeed, m_ySpeedShow,
                 "%.1f", m_fWindStrength);
    rs->ExtPrint(m_speedFont, 0xFFFFFFFF, 0, PR_ALIGN_CENTER, true, m_fFontScale, 0, 0, m_xShipSpeed, m_ySpeedShow,
                 "%.1f", m_fShipSpeed);

    // show current charge
    if (m_curCharge >= 0)
    {
        rs->TextureSet(0, m_idChargeTexture);
        rs->SetRenderState(D3DRS_TEXTUREFACTOR, (BIUtils::g_dwBlinkColor << 24L) | 0xFFFFFF);
        rs->DrawPrimitive(D3DPT_TRIANGLESTRIP, m_idCurChargeVBuf, sizeof(BI_ONETEXTURE_VERTEX), 0, 2,
                          m_bNotEnoughBallFlag ? "battle_texure_blend_tf" : "battle_rectangle");
    }
    // show wind icon
    rs->TextureSet(0, m_idWindTexture);
    rs->DrawPrimitive(D3DPT_TRIANGLESTRIP, m_idCurChargeVBuf, sizeof(BI_ONETEXTURE_VERTEX), 4, 2, "battle_rectangle");
    // show sails position icon
    rs->TextureSet(0, m_idSailTexture);
    rs->DrawPrimitive(D3DPT_TRIANGLESTRIP, m_idCurChargeVBuf, sizeof(BI_ONETEXTURE_VERTEX), 8, 2, "battle_rectangle");
}

void BATTLE_NAVIGATOR::Update()
{
    if (rs == nullptr)
        return;

    UpdateWindParam();
    SetMainCharacterData();
    UpdateCurrentCharge();

    // change compass buffer
    auto *pV = static_cast<BI_ONETEXTURE_VERTEX *>(rs->LockVertexBuffer(m_idEmptyVBuf));
    if (pV != nullptr)
    {
        SetRectangleVertexPos(&pV[4], static_cast<float>(m_XNavigator), static_cast<float>(m_YNavigator),
                              static_cast<float>(m_NavigationWidth), static_cast<float>(m_NavigationWidth), -m_fAngle);
        SetRectangleVertexPos(&pV[8], static_cast<float>(m_XNavigator), static_cast<float>(m_YNavigator),
                              static_cast<float>(m_windWidth), static_cast<float>(m_windHeight),
                              m_fWindAngle - m_fAngle);
        rs->UnLockVertexBuffer(m_idEmptyVBuf);
    }
    pV = (BI_ONETEXTURE_VERTEX *)rs->LockVertexBuffer(m_idCourseVBuf);
    if (pV != NULL)
    {
        SetRectangleVertexPos(&pV[0], (float)m_XNavigator, (float)m_YNavigator, (float)m_windWidth, (float)m_windHeight,
                              m_fWindAngle - m_fAngle + m_fShipWindAgainst);
        SetRectangleVertexPos(&pV[4], (float)m_XNavigator, (float)m_YNavigator, (float)m_windWidth, (float)m_windHeight,
                              m_fWindAngle - m_fAngle - m_fShipWindAgainst);
        rs->UnLockVertexBuffer(m_idCourseVBuf);
    }
    // change cannon charge buffer
    pV = static_cast<BI_ONETEXTURE_VERTEX *>(rs->LockVertexBuffer(m_idCannonVBuf));
    if (pV != nullptr)
    {
        m_nvCannonCharge = m_nvCannonReady = m_nvCannonDamage = 0;

        if (!FloatCompare(m_fCurAnglLeftCharge, m_fCurAnglLeftDamage))
        {
            SetRectangleSegVertexPos(pV, static_cast<float>(m_XNavigator), static_cast<float>(m_YNavigator),
                                     static_cast<float>(m_NavigationWidth), static_cast<float>(m_NavigationWidth),
                                     m_fBegAnglLeftCharge, m_fCurAnglLeftCharge);
            m_nvCannonCharge =
                SetRectangleSegVertexTex(pV, .5f, .5f, 1.f, 1.f, m_fBegAnglLeftCharge, m_fCurAnglLeftCharge);
        }
        if (!FloatCompare(m_fCurAnglRightCharge, m_fCurAnglRightDamage))
        {
            SetRectangleSegVertexPos(&pV[m_nvCannonCharge], static_cast<float>(m_XNavigator),
                                     static_cast<float>(m_YNavigator), static_cast<float>(m_NavigationWidth),
                                     static_cast<float>(m_NavigationWidth), m_fCurAnglRightCharge,
                                     m_fBegAnglRightCharge);
            m_nvCannonCharge += SetRectangleSegVertexTex(&pV[m_nvCannonCharge], .5f, .5f, 1.f, 1.f,
                                                         m_fCurAnglRightCharge, m_fBegAnglRightCharge);
        }
        if (!FloatCompare(m_fCurAnglForwardCharge, m_fCurAnglForwardDamage))
        {
            SetRectangleSegVertexPos(&pV[m_nvCannonCharge], static_cast<float>(m_XNavigator),
                                     static_cast<float>(m_YNavigator), static_cast<float>(m_NavigationWidth),
                                     static_cast<float>(m_NavigationWidth), m_fBegAnglForwardCharge,
                                     m_fCurAnglForwardCharge);
            m_nvCannonCharge += SetRectangleSegVertexTex(&pV[m_nvCannonCharge], .5f, .5f, 1.f, 1.f,
                                                         m_fBegAnglForwardCharge, m_fCurAnglForwardCharge);
        }
        if (!FloatCompare(m_fCurAnglBackCharge, m_fCurAnglBackDamage))
        {
            SetRectangleSegVertexPos(&pV[m_nvCannonCharge], static_cast<float>(m_XNavigator),
                                     static_cast<float>(m_YNavigator), static_cast<float>(m_NavigationWidth),
                                     static_cast<float>(m_NavigationWidth), m_fCurAnglBackCharge, m_fBegAnglBackCharge);
            m_nvCannonCharge += SetRectangleSegVertexTex(&pV[m_nvCannonCharge], .5f, .5f, 1.f, 1.f,
                                                         m_fCurAnglBackCharge, m_fBegAnglBackCharge);
        }

        if (FloatCompare(m_fCurAnglLeftCharge, m_fCurAnglLeftDamage))
        {
            SetRectangleSegVertexPos(&pV[m_nvCannonCharge], static_cast<float>(m_XNavigator),
                                     static_cast<float>(m_YNavigator), static_cast<float>(m_NavigationWidth),
                                     static_cast<float>(m_NavigationWidth), m_fBegAnglLeftCharge, m_fCurAnglLeftCharge);
            m_nvCannonReady = SetRectangleSegVertexTex(&pV[m_nvCannonCharge], .5f, .5f, 1.f, 1.f, m_fBegAnglLeftCharge,
                                                       m_fCurAnglLeftCharge);
        }
        if (FloatCompare(m_fCurAnglRightCharge, m_fCurAnglRightDamage))
        {
            SetRectangleSegVertexPos(&pV[m_nvCannonCharge + m_nvCannonReady], static_cast<float>(m_XNavigator),
                                     static_cast<float>(m_YNavigator), static_cast<float>(m_NavigationWidth),
                                     static_cast<float>(m_NavigationWidth), m_fCurAnglRightCharge,
                                     m_fBegAnglRightCharge);
            m_nvCannonReady += SetRectangleSegVertexTex(&pV[m_nvCannonCharge + m_nvCannonReady], .5f, .5f, 1.f, 1.f,
                                                        m_fCurAnglRightCharge, m_fBegAnglRightCharge);
        }
        if (FloatCompare(m_fCurAnglForwardCharge, m_fCurAnglForwardDamage))
        {
            SetRectangleSegVertexPos(&pV[m_nvCannonCharge + m_nvCannonReady], static_cast<float>(m_XNavigator),
                                     static_cast<float>(m_YNavigator), static_cast<float>(m_NavigationWidth),
                                     static_cast<float>(m_NavigationWidth), m_fBegAnglForwardCharge,
                                     m_fCurAnglForwardCharge);
            m_nvCannonReady += SetRectangleSegVertexTex(&pV[m_nvCannonCharge + m_nvCannonReady], .5f, .5f, 1.f, 1.f,
                                                        m_fBegAnglForwardCharge, m_fCurAnglForwardCharge);
        }
        if (FloatCompare(m_fCurAnglBackCharge, m_fCurAnglBackDamage))
        {
            SetRectangleSegVertexPos(&pV[m_nvCannonCharge + m_nvCannonReady], static_cast<float>(m_XNavigator),
                                     static_cast<float>(m_YNavigator), static_cast<float>(m_NavigationWidth),
                                     static_cast<float>(m_NavigationWidth), m_fCurAnglBackCharge, m_fBegAnglBackCharge);
            m_nvCannonReady += SetRectangleSegVertexTex(&pV[m_nvCannonCharge + m_nvCannonReady], .5f, .5f, 1.f, 1.f,
                                                        m_fCurAnglBackCharge, m_fBegAnglBackCharge);
        }

        if (!FloatCompare(m_fCurAnglLeftDamage, m_fEndAnglLeftCharge))
        {
            SetRectangleSegVertexPos(&pV[m_nvCannonCharge + m_nvCannonReady], static_cast<float>(m_XNavigator),
                                     static_cast<float>(m_YNavigator), static_cast<float>(m_NavigationWidth),
                                     static_cast<float>(m_NavigationWidth), m_fCurAnglLeftDamage, m_fEndAnglLeftCharge);
            m_nvCannonDamage = SetRectangleSegVertexTex(&pV[m_nvCannonCharge + m_nvCannonReady], .5f, .5f, 1.f, 1.f,
                                                        m_fCurAnglLeftDamage, m_fEndAnglLeftCharge);
        }
        if (!FloatCompare(m_fCurAnglRightDamage, m_fEndAnglRightCharge))
        {
            SetRectangleSegVertexPos(&pV[m_nvCannonCharge + m_nvCannonReady + m_nvCannonDamage],
                                     static_cast<float>(m_XNavigator), static_cast<float>(m_YNavigator),
                                     static_cast<float>(m_NavigationWidth), static_cast<float>(m_NavigationWidth),
                                     m_fEndAnglRightCharge, m_fCurAnglRightDamage);
            m_nvCannonDamage +=
                SetRectangleSegVertexTex(&pV[m_nvCannonCharge + m_nvCannonReady + m_nvCannonDamage], .5f, .5f, 1.f, 1.f,
                                         m_fEndAnglRightCharge, m_fCurAnglRightDamage);
        }
        if (!FloatCompare(m_fCurAnglForwardDamage, m_fEndAnglForwardCharge))
        {
            SetRectangleSegVertexPos(&pV[m_nvCannonCharge + m_nvCannonReady + m_nvCannonDamage],
                                     static_cast<float>(m_XNavigator), static_cast<float>(m_YNavigator),
                                     static_cast<float>(m_NavigationWidth), static_cast<float>(m_NavigationWidth),
                                     m_fCurAnglForwardDamage, m_fEndAnglForwardCharge);
            m_nvCannonDamage +=
                SetRectangleSegVertexTex(&pV[m_nvCannonCharge + m_nvCannonReady + m_nvCannonDamage], .5f, .5f, 1.f, 1.f,
                                         m_fCurAnglForwardDamage, m_fEndAnglForwardCharge);
        }
        if (!FloatCompare(m_fCurAnglBackDamage, m_fEndAnglBackCharge))
        {
            SetRectangleSegVertexPos(&pV[m_nvCannonCharge + m_nvCannonReady + m_nvCannonDamage],
                                     static_cast<float>(m_XNavigator), static_cast<float>(m_YNavigator),
                                     static_cast<float>(m_NavigationWidth), static_cast<float>(m_NavigationWidth),
                                     m_fEndAnglBackCharge, m_fCurAnglBackDamage);
            m_nvCannonDamage +=
                SetRectangleSegVertexTex(&pV[m_nvCannonCharge + m_nvCannonReady + m_nvCannonDamage], .5f, .5f, 1.f, 1.f,
                                         m_fEndAnglBackCharge, m_fCurAnglBackDamage);
        }

        m_nvCannonCharge -= 2;
        m_nvCannonReady -= 2;
        m_nvCannonDamage -= 2;

        rs->UnLockVertexBuffer(m_idCannonVBuf);
    }
    // change speed buffer
    pV = static_cast<BI_ONETEXTURE_VERTEX *>(rs->LockVertexBuffer(m_idSpeedVBuf));
    if (pV != nullptr)
    {
        SetRectangleSegVertexPos(pV, static_cast<float>(m_XNavigator), static_cast<float>(m_YNavigator),
                                 static_cast<float>(m_NavigationWidth), static_cast<float>(m_NavigationWidth),
                                 m_fBegAnglWindSpeed, m_fCurAnglWindSpeed);
        m_nvSpeed = SetRectangleSegVertexTex(pV, .5f, .5f, 1.f, 1.f, m_fBegAnglWindSpeed, m_fCurAnglWindSpeed);
        SetRectangleSegVertexPos(&pV[m_nvSpeed], static_cast<float>(m_XNavigator), static_cast<float>(m_YNavigator),
                                 static_cast<float>(m_NavigationWidth), static_cast<float>(m_NavigationWidth),
                                 m_fCurAnglShipSpeed, m_fBegAnglShipSpeed);
        m_nvSpeed +=
            SetRectangleSegVertexTex(&pV[m_nvSpeed], .5f, .5f, 1.f, 1.f, m_fCurAnglShipSpeed, m_fBegAnglShipSpeed);
        m_nvSpeed -= 2;

        rs->UnLockVertexBuffer(m_idSpeedVBuf);
    }
    // change island buffer
    if (m_bYesIsland)
        UpdateMiniMap();

    UpdateFireRangeBuffer();

    SetAnotherShip();
}

void BATTLE_NAVIGATOR::UpdateFireRangeBuffer() const
{
    auto *pv = static_cast<BI_NOTEXTURE_VERTEX *>(rs->LockVertexBuffer(m_idFireZoneVBuf));
    if (pv == nullptr)
        return;

    auto *const psd = g_ShipList.GetMainCharacterShip();
    auto *const pAttr = g_ShipList.GetMainCharacterShipAttr();
    if (psd != nullptr && pAttr != nullptr)
    {
        FillOneSideFireRange(&pv[0], pAttr, psd->pAttr, "cannonf");
        FillOneSideFireRange(&pv[(BI_ONESIDE_SIZE + 1)], pAttr, psd->pAttr, "cannonr");
        FillOneSideFireRange(&pv[2 * (BI_ONESIDE_SIZE + 1)], pAttr, psd->pAttr, "cannonb");
        FillOneSideFireRange(&pv[3 * (BI_ONESIDE_SIZE + 1)], pAttr, psd->pAttr, "cannonl");
    }
    rs->UnLockVertexBuffer(m_idFireZoneVBuf);
}

void BATTLE_NAVIGATOR::FillOneSideFireRange(BI_NOTEXTURE_VERTEX *pv, ATTRIBUTES *pShip, ATTRIBUTES *pChar,
                                            const char *pstr) const
{
    if (pv == nullptr || pShip == nullptr || pChar == nullptr || pstr == nullptr)
        return;

    auto fDirAng = 0.f, fSizeAng = 0.f, fFireZone = 0.f;
    ATTRIBUTES *pA;
    if ((pA = BIUtils::GetAttributesFromPath(pShip, "Cannons", "Borts", pstr, 0)) != nullptr)
    {
        fSizeAng = pA->GetAttributeAsFloat("FireZone", 0.f);
        fDirAng = pA->GetAttributeAsFloat("FireDir", 0.f);
    }

    if ((pA = BIUtils::GetAttributesFromPath(pChar, "Cannons", "Borts", pstr, 0)) != nullptr)
        fFireZone = pA->GetAttributeAsFloat("MaxFireDistance", 0.f);
    fFireZone *= m_fMapRadius / (m_fWorldRad * m_fCurScale);
    if (fFireZone > m_fMapRadius)
        fFireZone = m_fMapRadius;

    auto curAng = fDirAng - fSizeAng / 2.f;
    const auto dAng = fSizeAng / static_cast<float>((BI_ONESIDE_SIZE - 1));
    for (auto i = 0; i < BI_ONESIDE_SIZE; i++, curAng += dAng)
    {
        pv[i + 1].pos.x = m_XNavigator + sinf(curAng) * fFireZone;
        pv[i + 1].pos.y = m_YNavigator - cosf(curAng) * fFireZone * m_fAspectRatio;
    }
}

void BATTLE_NAVIGATOR::Init(VDX9RENDER *RenderService, Entity *pOwnerEI)
{
    int i;
    char *tmpstr;

    m_pOwnerEI = pOwnerEI;
    if (RenderService == nullptr)
        return;
    rs = RenderService;

    m_dwChargeCannon = ARGB(255, 255, 0, 0);
    m_dwReadyCannon = ARGB(255, 0, 255, 0);
    m_dwDamagedCannon = ARGB(255, 64, 64, 64);

    // maximum wind speed
    m_fWindMaxStrength = core.Entity_GetAttributeAsFloat(BIUtils::idBattleInterface, "MaxWind", 30.f);
    // maximum ship speed
    m_fMaxShipSpeed = core.Entity_GetAttributeAsFloat(BIUtils::idBattleInterface, "MaxShipSpeed", 20.f);
    //
    m_fShipSpeedScale = core.Entity_GetAttributeAsFloat(BIUtils::idBattleInterface, "ShipSpeedScaler", 1.f);

    auto *const pARoot = core.Entity_GetAttributeClass(BIUtils::idBattleInterface, "navigation");

    m_fAspectRatio = BIUtils::GetFloatFromAttr(pARoot, "aspectRatio", 1.f);

    // Get navigation window parameters
    m_NavigationWidth = BIUtils::GetLongFromAttr(pARoot, "navigatorWidth", 128);
    m_NavigationHeight = BIUtils::GetLongFromAttr(pARoot, "navigatorHeight", 128);
    m_XNavigator = BIUtils::GetLongFromAttr(pARoot, "rightPos", 640) - m_NavigationWidth / 2;
    m_YNavigator = BIUtils::GetLongFromAttr(pARoot, "topPos", 0) + m_NavigationHeight / 2;

    // visibility radius on the minimap
    m_fWorldRad = BIUtils::GetFloatFromAttr(pARoot, "horizontRadius", 400.f);
    m_fMinScale = BIUtils::GetFloatFromAttr(pARoot, "minScale", .01f);
    m_fMaxScale = BIUtils::GetFloatFromAttr(pARoot, "maxScale", 2.f);
    m_fCurScale = m_fDefaultScale = BIUtils::GetFloatFromAttr(pARoot, "curScale", 1.f);
    m_fScaleStep = BIUtils::GetFloatFromAttr(pARoot, "scaleStep", .01f);
    m_fShipShowRad = BIUtils::GetFloatFromAttr(pARoot, "shipShowRadius", 2.f);

    m_windWidth = BIUtils::GetLongFromAttr(pARoot, "windWidth", 20);
    m_windHeight = BIUtils::GetLongFromAttr(pARoot, "windHeight", 158);

    // color of loaded cannons
    m_dwReadyCannon = BIUtils::GetLongFromAttr(pARoot, "argbReadyCannonColor", m_dwReadyCannon);
    // color of loading cannons
    m_dwChargeCannon = BIUtils::GetLongFromAttr(pARoot, "argbChargeCannonColor", m_dwChargeCannon);
    // color of damaged cannons
    m_dwDamagedCannon = BIUtils::GetLongFromAttr(pARoot, "argbDamageCannonColor", m_dwDamagedCannon);
    // color of the sea
    m_dwSeaColor = BIUtils::GetLongFromAttr(pARoot, "argbSeaColor", ARGB(255, 255, 255, 255));
    // color of the cannon fire zone
    m_dwFireZoneColor = BIUtils::GetLongFromAttr(pARoot, "argbFireZoneColor", ARGB(255, 255, 255, 255));
    // enemy ship color
    m_dwEnemyShipColor = BIUtils::GetLongFromAttr(pARoot, "argbEnemyShipColor", ARGB(255, 255, 255, 255));
    // color of your ship
    m_dwFrendShipColor = BIUtils::GetLongFromAttr(pARoot, "argbFrendShipColor", ARGB(255, 255, 255, 255));
    // neutral ship color
    m_dwNeutralShipColor = BIUtils::GetLongFromAttr(pARoot, "argbNeutralShipColor", ARGB(255, 255, 255, 255));
    // color of a sinking ship
    m_dwDeadShipColor = BIUtils::GetLongFromAttr(pARoot, "argbDeadShipColor", ARGB(255, 255, 255, 255));
    // background gradient color
    m_dwBackGradColor1 = BIUtils::GetLongFromAttr(pARoot, "argbBackMaxColor", ARGB(255, 0, 0, 128));
    m_dwBackGradColor2 = BIUtils::GetLongFromAttr(pARoot, "argbBackMinColor", ARGB(55, 0, 0, 128));

    // get strings parameters
    tmpstr = BIUtils::GetStringFromAttr(pARoot, "speedShowFont", nullptr);
    if (tmpstr == nullptr)
        m_speedFont = -1;
    else
        m_speedFont = rs->LoadFont(tmpstr);
    m_ySpeedShow = m_YNavigator + BIUtils::GetLongFromAttr(pARoot, "speedOutYOffset", -m_NavigationHeight / 2);
    m_xShipSpeed = m_XNavigator + BIUtils::GetLongFromAttr(pARoot, "shipSpeedXOffset", 10);
    m_xWindSpeed = m_XNavigator + BIUtils::GetLongFromAttr(pARoot, "windSpeedXOffset", -20);
    m_fFontScale = BIUtils::GetFloatFromAttr(pARoot, "fontScale", 1.f);

    tmpstr = BIUtils::GetStringFromAttr(pARoot, "compasTexture", nullptr);
    if (tmpstr == nullptr)
        m_idCompasTex = -1;
    else
        m_idCompasTex = rs->TextureCreate(tmpstr);

    tmpstr = BIUtils::GetStringFromAttr(pARoot, "speedTexture", nullptr);
    if (tmpstr == nullptr)
        m_idSpeedTex = -1;
    else
        m_idSpeedTex = rs->TextureCreate(tmpstr);

    tmpstr = BIUtils::GetStringFromAttr(pARoot, "cannonsTexture", nullptr);
    if (tmpstr == nullptr)
        m_idCannonTex = -1;
    else
        m_idCannonTex = rs->TextureCreate(tmpstr);

    tmpstr = BIUtils::GetStringFromAttr(pARoot, "emptyTexture", nullptr);
    if (tmpstr == nullptr)
        m_idEmptyTex = -1;
    else
        m_idEmptyTex = rs->TextureCreate(tmpstr);

    tmpstr = BIUtils::GetStringFromAttr(pARoot, "windTexture", nullptr);
    if (tmpstr == nullptr)
        m_idWindTex = -1;
    else
        m_idWindTex = rs->TextureCreate(tmpstr);

    tmpstr = BIUtils::GetStringFromAttr(pARoot, "bestCourseTexture", nullptr);
    if (tmpstr == nullptr)
        m_idBestCourseTex = -1;
    else
        m_idBestCourseTex = rs->TextureCreate(tmpstr);

    tmpstr = BIUtils::GetStringFromAttr(pARoot, "chargeTexture", nullptr);
    if (tmpstr == nullptr)
        m_idChargeTexture = -1;
    else
        m_idChargeTexture = rs->TextureCreate(tmpstr);

    tmpstr = BIUtils::GetStringFromAttr(pARoot, "sailstateTexture", nullptr);
    if (tmpstr == nullptr)
        m_idSailTexture = -1;
    else
        m_idSailTexture = rs->TextureCreate(tmpstr);

    tmpstr = BIUtils::GetStringFromAttr(pARoot, "windStateTexture", nullptr);
    if (tmpstr == nullptr)
        m_idWindTexture = -1;
    else
        m_idWindTexture = rs->TextureCreate(tmpstr);

    // get cannon charge angles
    m_fBegAnglLeftCharge = static_cast<float>(BIUtils::GetLongFromAttr(pARoot, "leftChargeBegAngle", 0)) / 180.f * PI;
    m_fEndAnglLeftCharge = static_cast<float>(BIUtils::GetLongFromAttr(pARoot, "leftChargeEndAngle", 0)) / 180.f * PI;

    m_fBegAnglRightCharge = static_cast<float>(BIUtils::GetLongFromAttr(pARoot, "rightChargeBegAngle", 0)) / 180.f * PI;
    m_fEndAnglRightCharge = static_cast<float>(BIUtils::GetLongFromAttr(pARoot, "rightChargeEndAngle", 0)) / 180.f * PI;

    m_fBegAnglForwardCharge =
        static_cast<float>(BIUtils::GetLongFromAttr(pARoot, "forwardChargeBegAngle", 0)) / 180.f * PI;
    m_fEndAnglForwardCharge =
        static_cast<float>(BIUtils::GetLongFromAttr(pARoot, "forwardChargeEndAngle", 0)) / 180.f * PI;

    m_fBegAnglBackCharge =
        static_cast<float>(BIUtils::GetLongFromAttr(pARoot, "backwardChargeBegAngle", 0)) / 180.f * PI;
    m_fEndAnglBackCharge =
        static_cast<float>(BIUtils::GetLongFromAttr(pARoot, "backwardChargeEndAngle", 0)) / 180.f * PI;

    // get speed angles
    m_fBegAnglShipSpeed = static_cast<float>(BIUtils::GetLongFromAttr(pARoot, "shipSpeedBegAngle", 0)) / 180.f * PI;
    m_fCurAnglShipSpeed = m_fEndAnglShipSpeed =
        static_cast<float>(BIUtils::GetLongFromAttr(pARoot, "shipSpeedEndAngle", 0)) / 180.f * PI;
    m_fBegAnglWindSpeed = static_cast<float>(BIUtils::GetLongFromAttr(pARoot, "windSpeedBegAngle", 0)) / 180.f * PI;
    m_fCurAnglWindSpeed = m_fEndAnglWindSpeed =
        static_cast<float>(BIUtils::GetLongFromAttr(pARoot, "windSpeedEndAngle", 0)) / 180.f * PI;

    // current charge type
    m_ChargeGreed.x = 1;
    m_ChargeGreed.y = 1;
    if ((tmpstr = BIUtils::GetStringFromAttr(pARoot, "chargeTextureGreed", nullptr)) != nullptr)
        sscanf(tmpstr, "%d,%d", &m_ChargeGreed.x, &m_ChargeGreed.y);
    if (m_ChargeGreed.x < 1)
        m_ChargeGreed.x = 1;
    if (m_ChargeGreed.y < 1)
        m_ChargeGreed.y = 1;
    m_ChargePos.x = 160;
    m_ChargePos.y = 160;
    if ((tmpstr = BIUtils::GetStringFromAttr(pARoot, "chargePos", nullptr)) != nullptr)
        sscanf(tmpstr, "%d,%d", &m_ChargePos.x, &m_ChargePos.y);
    m_ChargePos.x += m_XNavigator; // - m_NavigationWidth/2;
    m_ChargePos.y += m_YNavigator; // - m_NavigationHeight/2;
    m_ChargeSize.x = 32;
    m_ChargeSize.y = 32;
    if ((tmpstr = BIUtils::GetStringFromAttr(pARoot, "chargePictureSize", nullptr)) != nullptr)
        sscanf(tmpstr, "%d,%d", &m_ChargeSize.x, &m_ChargeSize.y);

    // wind icon
    m_curSailState = 0;
    m_WindGreed.x = 1;
    m_WindGreed.y = 1;
    if ((tmpstr = BIUtils::GetStringFromAttr(pARoot, "windTextureGreed", nullptr)) != nullptr)
        sscanf(tmpstr, "%d,%d", &m_WindGreed.x, &m_WindGreed.y);
    if (m_WindGreed.x < 1)
        m_WindGreed.x = 1;
    if (m_WindGreed.y < 1)
        m_WindGreed.y = 1;
    m_WindPos.x = 160;
    m_WindPos.y = 160;
    if ((tmpstr = BIUtils::GetStringFromAttr(pARoot, "windPos", nullptr)) != nullptr)
        sscanf(tmpstr, "%d,%d", &m_WindPos.x, &m_WindPos.y);
    m_WindPos.x += m_XNavigator;
    m_WindPos.y += m_YNavigator;
    m_WindSize.x = 32;
    m_WindSize.y = 32;
    if ((tmpstr = BIUtils::GetStringFromAttr(pARoot, "windPictureSize", nullptr)) != nullptr)
        sscanf(tmpstr, "%d,%d", &m_WindSize.x, &m_WindSize.y);

    // sail position icon
    m_curSailState = 0;
    m_SailGreed.x = 1;
    m_SailGreed.y = 1;
    if ((tmpstr = BIUtils::GetStringFromAttr(pARoot, "sailstateTextureGreed", nullptr)) != nullptr)
        sscanf(tmpstr, "%d,%d", &m_SailGreed.x, &m_SailGreed.y);
    if (m_SailGreed.x < 1)
        m_SailGreed.x = 1;
    if (m_SailGreed.y < 1)
        m_SailGreed.y = 1;
    m_SailPos.x = 160;
    m_SailPos.y = 160;
    if ((tmpstr = BIUtils::GetStringFromAttr(pARoot, "sailstatePos", nullptr)) != nullptr)
        sscanf(tmpstr, "%d,%d", &m_SailPos.x, &m_SailPos.y);
    m_SailPos.x += m_XNavigator;
    m_SailPos.y += m_YNavigator;
    m_SailSize.x = 32;
    m_SailSize.y = 32;
    if ((tmpstr = BIUtils::GetStringFromAttr(pARoot, "sailstatePictureSize", nullptr)) != nullptr)
        sscanf(tmpstr, "%d,%d", &m_SailSize.x, &m_SailSize.y);

    // create buffers
    m_idEmptyVBuf =
        rs->CreateVertexBuffer(BI_ONETEX_VERTEX_FORMAT, (4 + 4 + 4) * sizeof(BI_ONETEXTURE_VERTEX), D3DUSAGE_WRITEONLY);
    m_idCourseVBuf =
        rs->CreateVertexBuffer(BI_ONETEX_VERTEX_FORMAT, (4 + 4) * sizeof(BI_ONETEXTURE_VERTEX), D3DUSAGE_WRITEONLY);
    m_idCannonVBuf =
        rs->CreateVertexBuffer(BI_ONETEX_VERTEX_FORMAT, 7 * 4 * sizeof(BI_ONETEXTURE_VERTEX), D3DUSAGE_WRITEONLY);
    m_idSpeedVBuf =
        rs->CreateVertexBuffer(BI_ONETEX_VERTEX_FORMAT, 7 * 2 * sizeof(BI_ONETEXTURE_VERTEX), D3DUSAGE_WRITEONLY);
    m_idMapVBuf = rs->CreateVertexBuffer(BI_ONETEX_VERTEX_FORMAT, (RADIAL_QUANTITY + 3) * sizeof(BI_ONETEXTURE_VERTEX),
                                         D3DUSAGE_WRITEONLY);
    m_idFireZoneVBuf = rs->CreateVertexBuffer(BI_NOTEX_VERTEX_FORMAT, FIRERANGE_QUANTITY * sizeof(BI_NOTEXTURE_VERTEX),
                                              D3DUSAGE_WRITEONLY);
    m_idShipsVBuf = rs->CreateVertexBuffer(
        BI_COLORONLY_VERTEX_FORMAT, MAX_ENEMY_SHIP_QUANTITY * 3 * sizeof(BI_COLORONLY_VERTEX), D3DUSAGE_WRITEONLY);
    m_idGradBackVBuf =
        rs->CreateVertexBuffer(BI_COLORONLY_VERTEX_FORMAT, 3 * sizeof(BI_COLORONLY_VERTEX), D3DUSAGE_WRITEONLY);
    m_idCurChargeVBuf =
        rs->CreateVertexBuffer(BI_ONETEX_VERTEX_FORMAT, 3 * 4 * sizeof(BI_ONETEXTURE_VERTEX), D3DUSAGE_WRITEONLY);
    if (m_idEmptyVBuf == -1 || m_idCourseVBuf == -1 || m_idCannonVBuf == -1 || m_idSpeedVBuf == -1 ||
        m_idMapVBuf == -1 || m_idFireZoneVBuf == -1 || m_idShipsVBuf == -1 || m_idGradBackVBuf == -1 ||
        m_idCurChargeVBuf == -1)
    {
        throw std::runtime_error("Can`t create vertex or index buffer");
    }
    m_nvShips = 0;

    // fill constant value for vertex buffer
    auto *pV = static_cast<BI_ONETEXTURE_VERTEX *>(rs->LockVertexBuffer(m_idEmptyVBuf));
    if (pV != nullptr)
    {
        for (i = 0; i < 12; i++)
        {
            pV[i].w = .5f;
            pV[i].pos.z = 1.f;
        }
        SetRectangleVertexPos(&pV[0], static_cast<float>(m_XNavigator), static_cast<float>(m_YNavigator),
                              static_cast<float>(m_NavigationWidth), static_cast<float>(m_NavigationWidth));
        SetRectangleVertexTex(&pV[0]);
        SetRectangleVertexTex(&pV[4]);
        SetRectangleVertexTex(&pV[8]);
        rs->UnLockVertexBuffer(m_idEmptyVBuf);
    }
    // best course vertices
    pV = (BI_ONETEXTURE_VERTEX *)rs->LockVertexBuffer(m_idCourseVBuf);
    if (pV != NULL)
    {
        for (i = 0; i < 8; i++)
        {
            pV[i].w = .5f;
            pV[i].pos.z = 1.f;
        }
        SetRectangleVertexPos(&pV[0], (float)m_XNavigator, (float)m_YNavigator, (float)m_NavigationWidth,
                              (float)m_NavigationWidth);
        SetRectangleVertexTex(&pV[0]);
        SetRectangleVertexTex(&pV[4]);
        rs->UnLockVertexBuffer(m_idCourseVBuf);
    }
    // cannon charge vertexs
    pV = static_cast<BI_ONETEXTURE_VERTEX *>(rs->LockVertexBuffer(m_idCannonVBuf));
    if (pV != nullptr)
    {
        for (i = 0; i < 7 * 4; i++)
        {
            pV[i].pos.z = 1.f;
            pV[i].w = .5f;
        }
        rs->UnLockVertexBuffer(m_idCannonVBuf);
    }
    // speed show vertexs
    pV = static_cast<BI_ONETEXTURE_VERTEX *>(rs->LockVertexBuffer(m_idSpeedVBuf));
    if (pV != nullptr)
    {
        for (i = 0; i < 7 * 2; i++)
        {
            pV[i].pos.z = 1.f;
            pV[i].w = .5f;
        }
        rs->UnLockVertexBuffer(m_idSpeedVBuf);
    }
    // map vertexes
    m_fMapRadius = BIUtils::GetFloatFromAttr(pARoot, "mapRadius", 100.f);
    pV = static_cast<BI_ONETEXTURE_VERTEX *>(rs->LockVertexBuffer(m_idMapVBuf));
    if (pV != nullptr)
    {
        for (i = 0; i < RADIAL_QUANTITY + 2; i++)
        {
            pV[i].w = .5f;
            pV[i].pos.z = 1.f;
        }
        SetCircleVertexPos(pV, static_cast<float>(m_XNavigator), static_cast<float>(m_YNavigator), m_fMapRadius);
        SetCircleVertexTex(pV);
        rs->UnLockVertexBuffer(m_idMapVBuf);
    }
    // cannon fire zone
    auto *pv = static_cast<BI_NOTEXTURE_VERTEX *>(rs->LockVertexBuffer(m_idFireZoneVBuf));
    if (pv != nullptr)
    {
        const auto vCenter = CVECTOR(static_cast<float>(m_XNavigator), static_cast<float>(m_YNavigator), 1.f);
        for (i = 0; i < FIRERANGE_QUANTITY; i++)
        {
            pv[i].w = .5f;
            pv[i].pos = vCenter;
        }
        rs->UnLockVertexBuffer(m_idFireZoneVBuf);
    }
    // ships on the map
    auto *pcv = static_cast<BI_COLORONLY_VERTEX *>(rs->LockVertexBuffer(m_idShipsVBuf));
    if (pcv != nullptr)
    {
        for (i = 0; i < MAX_ENEMY_SHIP_QUANTITY * 3; i++)
        {
            pcv[i].w = .5f;
            pcv[i].pos.z = 1.f;
        }
        rs->UnLockVertexBuffer(m_idShipsVBuf);
    }

    // triangular gradient for the background
    pcv = static_cast<BI_COLORONLY_VERTEX *>(rs->LockVertexBuffer(m_idGradBackVBuf));
    if (pcv != nullptr)
    {
        pcv[0].w = pcv[1].w = pcv[2].w = 0.5f;
        pcv[0].pos.z = pcv[1].pos.z = pcv[2].pos.z = 1.f;
        pcv[0].pos.x = static_cast<float>(m_XNavigator) + m_NavigationWidth / 2;
        pcv[0].pos.y = static_cast<float>(m_YNavigator) - m_NavigationHeight / 2;
        pcv[1].pos.x = static_cast<float>(m_XNavigator) - NAVBACKGROUND_SIZESCALE * m_NavigationWidth / 2;
        pcv[1].pos.y = static_cast<float>(m_YNavigator) - m_NavigationHeight / 2;
        pcv[2].pos.x = static_cast<float>(m_XNavigator) + m_NavigationWidth / 2;
        pcv[2].pos.y = static_cast<float>(m_YNavigator) + NAVBACKGROUND_SIZESCALE * m_NavigationHeight / 2;
        pcv[0].col = m_dwBackGradColor1;
        pcv[1].col = pcv[2].col = m_dwBackGradColor2;
        rs->UnLockVertexBuffer(m_idGradBackVBuf);
    }

    // Current charge type
    pV = static_cast<BI_ONETEXTURE_VERTEX *>(rs->LockVertexBuffer(m_idCurChargeVBuf));
    if (pV != nullptr)
    {
        for (i = 0; i < 3 * 4; i++)
        {
            pV[i].pos.z = 1.f;
            pV[i].w = .5f;
        }
        SetRectangleVertexPos(&pV[0], static_cast<float>(m_ChargePos.x), static_cast<float>(m_ChargePos.y),
                              static_cast<float>(m_ChargeSize.x), static_cast<float>(m_ChargeSize.y));
        SetRectangleVertexPos(&pV[4], static_cast<float>(m_WindPos.x), static_cast<float>(m_WindPos.y),
                              static_cast<float>(m_WindSize.x), static_cast<float>(m_WindSize.y));
        SetRectangleVertexPos(&pV[8], static_cast<float>(m_SailPos.x), static_cast<float>(m_SailPos.y),
                              static_cast<float>(m_SailSize.x), static_cast<float>(m_SailSize.y));
        rs->UnLockVertexBuffer(m_idCurChargeVBuf);
    }
}

long BATTLE_NAVIGATOR::SetRectangleVertexPos(BI_ONETEXTURE_VERTEX *v, float x, float y, float width, float height,
                                             float angle) const
{
    if (v == nullptr)
        return 0;

    if (angle == 0)
    {
        v[2].pos.x = v[3].pos.x = (v[0].pos.x = v[1].pos.x = x - width / 2) + width;
        v[1].pos.y = v[3].pos.y = (v[0].pos.y = v[2].pos.y = y - height / 2 * m_fAspectRatio) + height * m_fAspectRatio;
    }
    else
    {
        const auto ca = cosf(angle);
        const auto sa = sinf(angle);
        const auto wca = width / 2 * ca;
        const auto wsa = width / 2 * sa;
        const auto hca = height / 2 * ca;
        const auto hsa = height / 2 * sa;
        v[0].pos.x = x + (-wca + hsa);
        v[0].pos.y = y + (-wsa - hca) * m_fAspectRatio;
        v[1].pos.x = x + (-wca - hsa);
        v[1].pos.y = y + (-wsa + hca) * m_fAspectRatio;
        v[2].pos.x = x + (wca + hsa);
        v[2].pos.y = y + (wsa - hca) * m_fAspectRatio;
        v[3].pos.x = x + (wca - hsa);
        v[3].pos.y = y + (wsa + hca) * m_fAspectRatio;
    }

    return 4;
}

long BATTLE_NAVIGATOR::SetRectangleVertexTex(BI_ONETEXTURE_VERTEX *v, float x, float y, float width, float height,
                                             float angle)
{
    if (v == nullptr)
        return 0;

    if (angle == 0)
    {
        v[2].tu = v[3].tu = (v[0].tu = v[1].tu = x - width / 2) + width;
        v[1].tv = v[3].tv = (v[0].tv = v[2].tv = y - height / 2) + height;
    }
    else
    {
        const auto ca = cosf(angle);
        const auto sa = sinf(angle);
        const auto wca = width / 2 * ca;
        const auto wsa = width / 2 * sa;
        const auto hca = height / 2 * ca;
        const auto hsa = height / 2 * sa;
        v[0].tu = x + (-wca + hsa);
        v[0].tv = y + (-wsa - hca);
        v[1].tu = x + (-wca - hsa);
        v[1].tv = y + (-wsa + hca);
        v[2].tu = x + (wca + hsa);
        v[2].tv = y + (wsa - hca);
        v[3].tu = x + (wca - hsa);
        v[3].tv = y + (wsa + hca);
    }

    return 4;
}

long BATTLE_NAVIGATOR::SetCircleVertexPos(BI_ONETEXTURE_VERTEX *v, float x, float y, float rad, float angle) const
{
    if (v == nullptr)
        return 0;

    auto addAngle = angle;
    auto idx = 0;
    v[idx].pos.x = x;
    v[idx].pos.y = y;
    idx++;

    for (auto i = 0; i < RADIAL_QUANTITY + 1; i++)
    {
        v[idx].pos.x = x + rad * sinf(addAngle);
        v[idx].pos.y = y + rad * cosf(addAngle) * m_fAspectRatio;
        addAngle += 2.f * PI / RADIAL_QUANTITY;
        idx++;
    }
    v[idx].pos.x = x + rad * sinf(angle);
    v[idx].pos.y = y + rad * cosf(angle) * m_fAspectRatio;

    return RADIAL_QUANTITY + 2;
}

long BATTLE_NAVIGATOR::SetCircleVertexTex(BI_ONETEXTURE_VERTEX *v, float x, float y, float rad, float angle)
{
    if (v == nullptr)
        return 0;

    auto addAngle = angle;
    auto idx = 0;
    v[idx].tu = x;
    v[idx].tv = y;
    idx++;

    for (auto i = 0; i < RADIAL_QUANTITY + 1; i++)
    {
        v[idx].tu = x + rad * sinf(addAngle);
        v[idx].tv = y + rad * cosf(addAngle);
        addAngle += 2.f * PI / RADIAL_QUANTITY;
        idx++;
    }
    v[idx].tu = x + rad * sinf(angle);
    v[idx].tv = y + rad * sinf(angle);

    return RADIAL_QUANTITY + 2;
}

void BATTLE_NAVIGATOR::SetMainCharacterData()
{
    m_fCurAnglLeftCharge = 0;
    m_fCurAnglRightCharge = 0;
    m_fCurAnglForwardCharge = 0;
    m_fCurAnglBackCharge = m_fBegAnglBackCharge;
    auto *psd = g_ShipList.GetMainCharacterShip();
    if (psd == nullptr)
        return;

    // get map center
    if (g_bExternMapCenter)
    {
        m_fXPos = g_externPos.x;
        m_fYPos = g_externPos.z;
    }
    else
    {
        const auto cPos = psd->pShip->GetPos();
        m_fXPos = cPos.x;
        m_fYPos = cPos.z;
    }
    if (g_bMapScaling)
    {
        g_bMapScaling = false;
        m_fCurScale = m_fDefaultScale; // m_fMaxScale;
    }
    // get ship y angle
    const auto cAng = psd->pShip->GetAng();
    m_fAngle = cAng.y;
    m_fShipSpeed = static_cast<SHIP_BASE *>(psd->pShip)->State.vSpeed.z;
    m_fShipSpeed *= m_fShipSpeedScale; // speed reduction is more necessary here
    if (m_fShipSpeed > m_fMaxShipSpeed)
        m_fCurAnglShipSpeed = m_fEndAnglShipSpeed;
    else
        m_fCurAnglShipSpeed = GetBetwinFloat(m_fBegAnglShipSpeed, m_fEndAnglShipSpeed, m_fShipSpeed / m_fMaxShipSpeed);
    m_fShipWindAgainst = ((SHIP_BASE *)psd->pShip)->GetWindAgainst() / 180.f * PI;
    // m_fShipSpeed *= m_fShipSpeedScale;

    // get the wind value
    UpdateWindParam();
    if (m_fWindStrength > m_fWindMaxStrength)
        m_fCurAnglWindSpeed = m_fEndAnglWindSpeed;
    else
        m_fCurAnglWindSpeed =
            GetBetwinFloat(m_fBegAnglWindSpeed, m_fEndAnglWindSpeed, m_fWindStrength / m_fWindMaxStrength);

    m_fCurAnglLeftCharge = m_fBegAnglLeftCharge;
    m_fCurAnglRightCharge = m_fBegAnglRightCharge;
    m_fCurAnglForwardCharge = m_fBegAnglForwardCharge;
    m_fCurAnglBackCharge = m_fBegAnglBackCharge;
    auto *pBortsAttr = BIUtils::GetAttributesFromPath(psd->pAttr, "cannons", "borts", 0);
    if (pBortsAttr != nullptr)
    {
        ATTRIBUTES *pTmpAttr;
        float fCharge, fDamage;
        // left cannons
        if ((pTmpAttr = pBortsAttr->GetAttributeClass("cannonl")) != nullptr)
        {
            fCharge = pTmpAttr->GetAttributeAsFloat("ChargeRatio", 0);
            fDamage = pTmpAttr->GetAttributeAsFloat("DamageRatio", 0);
            m_fCurAnglLeftDamage = GetBetwinFloat(m_fEndAnglLeftCharge, m_fBegAnglLeftCharge, fDamage); //~!~
            m_fCurAnglLeftCharge = GetBetwinFloat(m_fBegAnglLeftCharge, m_fCurAnglLeftDamage, fCharge);
        }
        // right cannons
        if ((pTmpAttr = pBortsAttr->GetAttributeClass("cannonr")) != nullptr)
        {
            fCharge = pTmpAttr->GetAttributeAsFloat("ChargeRatio", 0);
            fDamage = pTmpAttr->GetAttributeAsFloat("DamageRatio", 0);
            m_fCurAnglRightDamage = GetBetwinFloat(m_fEndAnglRightCharge, m_fBegAnglRightCharge, fDamage);
            m_fCurAnglRightCharge = GetBetwinFloat(m_fBegAnglRightCharge, m_fCurAnglRightDamage, fCharge);
        }
        // forward cannons
        if ((pTmpAttr = pBortsAttr->GetAttributeClass("cannonf")) != nullptr)
        {
            fCharge = pTmpAttr->GetAttributeAsFloat("ChargeRatio", 0);
            fDamage = pTmpAttr->GetAttributeAsFloat("DamageRatio", 0);
            m_fCurAnglForwardDamage = GetBetwinFloat(m_fEndAnglForwardCharge, m_fBegAnglForwardCharge, fDamage);
            m_fCurAnglForwardCharge = GetBetwinFloat(m_fBegAnglForwardCharge, m_fCurAnglForwardDamage, fCharge);
        }
        // backward cannons
        if ((pTmpAttr = pBortsAttr->GetAttributeClass("cannonb")) != nullptr)
        {
            fCharge = pTmpAttr->GetAttributeAsFloat("ChargeRatio", 0);
            fDamage = pTmpAttr->GetAttributeAsFloat("DamageRatio", 0);
            m_fCurAnglBackDamage = GetBetwinFloat(m_fEndAnglBackCharge, m_fBegAnglBackCharge, fDamage);
            m_fCurAnglBackCharge = GetBetwinFloat(m_fBegAnglBackCharge, m_fCurAnglBackDamage, fCharge);
        }
    }
}

void BATTLE_NAVIGATOR::SetAnotherShip()
{
    auto *pv = static_cast<BI_COLORONLY_VERTEX *>(rs->LockVertexBuffer(m_idShipsVBuf));
    if (pv == nullptr)
        return;
    auto *const pMainCharacter = g_ShipList.GetMainCharacterShip();
    auto idx = 0;
    const auto fSqrMapRad = m_fMapRadius * m_fMapRadius;

    m_nvShips = 0L;
    // Fill ships buffer
    for (auto *psd = g_ShipList.GetShipRoot(); psd != nullptr; psd = psd->next)
    {
        // do not draw the main character's ship
        if (psd == pMainCharacter)
            continue;
        // determine the coordinates of the ship and if they are outside the map, then do not draw the ship
        auto fX = (psd->pShip->GetPos().x - m_fXPos) * m_fMapRadius / (m_fWorldRad * m_fCurScale);
        auto fY = (psd->pShip->GetPos().z - m_fYPos) * m_fMapRadius / (m_fWorldRad * m_fCurScale);
        if (fX * fX + fY * fY > fSqrMapRad)
            continue;
        const auto tmp = fX * cosf(m_fAngle) - fY * sinf(m_fAngle);
        fY = m_YNavigator - (fY * cosf(m_fAngle) + fX * sinf(m_fAngle)) * m_fAspectRatio;
        fX = tmp + m_XNavigator;
        // determine the color of the displayed ship
        auto dwColor = 0xFFFFFFFF;
        if (psd->dwShipColor == 0)
        {
            switch (psd->relation)
            {
            case BI_RELATION_ENEMY:
                dwColor = m_dwEnemyShipColor;
                break;
            case BI_RELATION_FRIEND:
                dwColor = m_dwFrendShipColor;
                break;
            case BI_RELATION_NEUTRAL:
                dwColor = m_dwNeutralShipColor;
                break;
            }
        }
        else
            dwColor = psd->dwShipColor;
        if (psd->isDead)
            dwColor = m_dwDeadShipColor;
        // determine the angle of the ship
        const auto fAngle = psd->pShip->GetAng().y - m_fAngle;
        // fill buffer
        pv[idx].col = pv[idx + 1].col = pv[idx + 2].col = dwColor;
        pv[idx + 0].pos.x = fX + m_fShipShowRad * sinf(fAngle);
        pv[idx + 0].pos.y = fY - m_fShipShowRad * cosf(fAngle) * m_fAspectRatio;
        pv[idx + 1].pos.x = fX + m_fShipShowRad * sinf(fAngle - PI * .9f);
        pv[idx + 1].pos.y = fY - m_fShipShowRad * cosf(fAngle - PI * .8f) * m_fAspectRatio;
        pv[idx + 2].pos.x = fX + m_fShipShowRad * sinf(fAngle + PI * .9f);
        pv[idx + 2].pos.y = fY - m_fShipShowRad * cosf(fAngle + PI * .8f) * m_fAspectRatio;
        idx += 3;
        m_nvShips += 3;
    }

    rs->UnLockVertexBuffer(m_idShipsVBuf);
}

void BATTLE_NAVIGATOR::ReleaseAll()
{
    m_wb = nullptr;
    m_pAWeather = nullptr;
    m_bYesIsland = false;

    // textures
    TEXTURE_RELEASE(rs, m_idCompasTex);
    TEXTURE_RELEASE(rs, m_idSpeedTex);
    TEXTURE_RELEASE(rs, m_idCannonTex);
    TEXTURE_RELEASE(rs, m_idEmptyTex);
    TEXTURE_RELEASE(rs, m_idIslandTexture);
    TEXTURE_RELEASE(rs, m_idChargeTexture);
    TEXTURE_RELEASE(rs, m_idWindTex);
    TEXTURE_RELEASE(rs, m_idBestCourseTex);
    TEXTURE_RELEASE(rs, m_idWindTexture);
    TEXTURE_RELEASE(rs, m_idSailTexture);

    if (m_pIslandTexture != nullptr)
    {
        rs->Release(m_pIslandTexture);
        m_pIslandTexture = nullptr;
    }

    VERTEX_BUFFER_RELEASE(rs, m_idEmptyVBuf);
    VERTEX_BUFFER_RELEASE(rs, m_idCourseVBuf);
    VERTEX_BUFFER_RELEASE(rs, m_idMapVBuf);
    VERTEX_BUFFER_RELEASE(rs, m_idCannonVBuf);
    VERTEX_BUFFER_RELEASE(rs, m_idSpeedVBuf);
    VERTEX_BUFFER_RELEASE(rs, m_idShipsVBuf);
    VERTEX_BUFFER_RELEASE(rs, m_idFireZoneVBuf);
    VERTEX_BUFFER_RELEASE(rs, m_idGradBackVBuf);
    VERTEX_BUFFER_RELEASE(rs, m_idCurChargeVBuf);

    if (m_speedFont != -1L)
    {
        rs->UnloadFont(m_speedFont);
        m_speedFont = -1L;
    }

    rs = nullptr;
}

float BATTLE_NAVIGATOR::CalculateCrossX(int side, float w, float h, float angl)
{
    switch (side)
    {
    case 0:
        return h / 2 * tanf(angl);
    case 1:
        return w / 2;
    case 2:
        return -h / 2 * tanf(angl);
    case 3:
        return -w / 2;
    }
    return 0.f;
}

float BATTLE_NAVIGATOR::CalculateCrossY(int side, float w, float h, float angl)
{
    switch (side)
    {
    case 0:
        return -h / 2;
    case 1:
        return w / 2 * tanf(angl - PI / 2);
    case 2:
        return h / 2;
    case 3:
        return -w / 2 * tanf(angl - PI / 2);
    }
    return 0.f;
}

long BATTLE_NAVIGATOR::SetRectangleSegVertexPos(BI_ONETEXTURE_VERTEX *v, float x, float y, float width, float height,
                                                float begAngle, float endAngle)
{
    if (v == nullptr)
        return 0;

    while (begAngle < 0)
        begAngle += 2 * PI;
    while (begAngle > 2 * PI)
        begAngle -= 2 * PI;
    while (endAngle < 0)
        endAngle += 2 * PI;
    while (endAngle > 2 * PI)
        endAngle -= 2 * PI;

    const auto alpha = atan2f(width / 2, height / 2);

    const auto begseg = begAngle < alpha              ? 0
                        : begAngle < (PI - alpha)     ? 1
                        : begAngle < (PI + alpha)     ? 2
                        : begAngle < (2 * PI - alpha) ? 3
                                                      : 0;
    const auto endseg = endAngle < alpha              ? 0
                        : endAngle < (PI - alpha)     ? 1
                        : endAngle < (PI + alpha)     ? 2
                        : endAngle < (2 * PI - alpha) ? 3
                                                      : 0;

    v[0].pos.x = x;
    v[0].pos.y = y;

    auto idx = 1;
    auto curSide = begseg;
    while (true)
    {
        if (curSide == begseg)
        {
            v[idx].pos.x = x + CalculateCrossX(curSide, width, height, begAngle);
            v[idx].pos.y = y + CalculateCrossY(curSide, width, height, begAngle) * m_fAspectRatio;
            idx++;
        }
        if (curSide == endseg)
        {
            v[idx].pos.x = x + CalculateCrossX(curSide, width, height, endAngle);
            v[idx].pos.y = y + CalculateCrossY(curSide, width, height, endAngle) * m_fAspectRatio;
            idx++;
            break;
        }

        switch (curSide)
        {
        case 0:
            v[idx].pos.x = x + width / 2;
            v[idx].pos.y = y - height / 2 * m_fAspectRatio;
            idx++;
            break;
        case 1:
            v[idx].pos.x = x + width / 2;
            v[idx].pos.y = y + height / 2 * m_fAspectRatio;
            idx++;
            break;
        case 2:
            v[idx].pos.x = x - width / 2;
            v[idx].pos.y = y + height / 2 * m_fAspectRatio;
            idx++;
            break;
        case 3:
            v[idx].pos.x = x - width / 2;
            v[idx].pos.y = y - height / 2 * m_fAspectRatio;
            idx++;
            break;
        }
        curSide++;
        if (curSide > 3)
            curSide = 0;
    }

    return idx;
}

long BATTLE_NAVIGATOR::SetRectangleSegVertexTex(BI_ONETEXTURE_VERTEX *v, float x, float y, float width, float height,
                                                float begAngle, float endAngle)
{
    if (v == nullptr)
        return 0;

    while (begAngle < 0)
        begAngle += 2 * PI;
    while (begAngle > 2 * PI)
        begAngle -= 2 * PI;
    while (endAngle < 0)
        endAngle += 2 * PI;
    while (endAngle > 2 * PI)
        endAngle -= 2 * PI;

    const auto alpha = atan2f(width / 2, height / 2);

    const auto begseg = begAngle < alpha              ? 0
                        : begAngle < (PI - alpha)     ? 1
                        : begAngle < (PI + alpha)     ? 2
                        : begAngle < (2 * PI - alpha) ? 3
                                                      : 0;
    const auto endseg = endAngle < alpha              ? 0
                        : endAngle < (PI - alpha)     ? 1
                        : endAngle < (PI + alpha)     ? 2
                        : endAngle < (2 * PI - alpha) ? 3
                                                      : 0;

    v[0].tu = x;
    v[0].tv = y;

    auto idx = 1;
    auto curSide = begseg;
    while (true)
    {
        if (curSide == begseg)
        {
            v[idx].tu = x + CalculateCrossX(curSide, width, height, begAngle);
            v[idx].tv = y + CalculateCrossY(curSide, width, height, begAngle);
            idx++;
        }
        if (curSide == endseg)
        {
            v[idx].tu = x + CalculateCrossX(curSide, width, height, endAngle);
            v[idx].tv = y + CalculateCrossY(curSide, width, height, endAngle);
            idx++;
            break;
        }

        switch (curSide)
        {
        case 0:
            v[idx].tu = x + width / 2;
            v[idx].tv = y - height / 2;
            idx++;
            break;
        case 1:
            v[idx].tu = x + width / 2;
            v[idx].tv = y + height / 2;
            idx++;
            break;
        case 2:
            v[idx].tu = x - width / 2;
            v[idx].tv = y + height / 2;
            idx++;
            break;
        case 3:
            v[idx].tu = x - width / 2;
            v[idx].tv = y - height / 2;
            idx++;
            break;
        }
        curSide++;
        if (curSide > 3)
            curSide = 0;
    }

    return idx;
}

void BATTLE_NAVIGATOR::SetIsland()
{
    CVECTOR posCenter;
    float islSize = 0;

    MODEL *pM = nullptr;
    auto *pIsl = static_cast<ISLAND_BASE *>(EntityManager::GetEntityPointer(EntityManager::GetEntityId("ISLAND")));
    if (pIsl != nullptr)
    {
        pM = static_cast<MODEL *>(EntityManager::GetEntityPointer(pIsl->GetModelEID()));
    }
    if (pM != nullptr)
    {
        pM->Update();
        // Get island size
        auto xMin = 0.f, yMin = 0.f, zMin = 0.f, xMax = 0.f, yMax = 0.f, zMax = 0.f;
        posCenter = pM->mtx.Pos();
        xMin = xMax = posCenter.x;
        zMin = zMax = posCenter.z;
        yMin = yMax = posCenter.y;
        auto i = 0;
        for (auto *pN = pM->GetNode(0); pN != nullptr; pN = pM->GetNode(++i))
        {
            GEOS::INFO gi;
            pN->geo->GetInfo(gi);
            posCenter = pN->glob_mtx * *(CVECTOR *)&gi.boxcenter;
            auto xmin = posCenter.x - gi.boxsize.x / 2;
            auto xmax = posCenter.x + gi.boxsize.x / 2;
            auto zmin = posCenter.z - gi.boxsize.z / 2;
            auto zmax = posCenter.z + gi.boxsize.z / 2;
            if (xmin < xMin)
                xMin = xmin;
            if (xmax > xMax)
                xMax = xmax;
            if (zmin < zMin)
                zMin = zmin;
            if (zmax > zMax)
                zMax = zmax;
        }
        posCenter.x = (xMin + xMax) / 2.f;
        posCenter.z = (zMin + zMax) / 2.f;
        posCenter.y = 0.f;
        islSize = (xMax - xMin) > (zMax - zMin) ? (xMax - xMin) : (zMax - zMin);
        islSize *= 1.1f;

        IDirect3DSurface9 *pRenderTarg = nullptr, *pOldRenderTarg = nullptr;
        if (rs->GetRenderTarget(&pOldRenderTarg) == D3D_OK)
        {
            if (rs->CreateTexture(MAP_TEXTURE_WIDTH, MAP_TEXTURE_HEIGHT, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R5G6B5,
                                  D3DPOOL_DEFAULT, &m_pIslandTexture) == D3D_OK)
            {
                // SIMPLE OUT
                if (rs->GetSurfaceLevel(m_pIslandTexture, 0, &pRenderTarg) == D3D_OK)
                {
                    IDirect3DSurface9 *pStencil;
                    rs->GetDepthStencilSurface(&pStencil);
                    if (rs->SetRenderTarget(pRenderTarg, nullptr) == D3D_OK)
                    {
                        CMatrix matw, matv, oldmatv;
                        D3DMATRIX matp, oldmatp;
                        matv.BuildViewMatrix(posCenter + CVECTOR(0.f, islSize / 2.f, 0.f), posCenter,
                                             CVECTOR(0.f, 0.f, 1.f));
                        rs->GetTransform(D3DTS_VIEW, oldmatv);
                        rs->SetTransform(D3DTS_VIEW, matv);
                        rs->GetTransform(D3DTS_PROJECTION, &oldmatp);
                        ZERO(matp);
                        matp._11 = matp._22 = 2.f / islSize;
                        matp._33 = 1.f / islSize;
                        matp._44 = 1.f;
                        rs->SetTransform(D3DTS_PROJECTION, &matp);
                        // fill fone color
                        rs->Clear(0, nullptr, D3DCLEAR_TARGET, m_dwSeaColor, 1.f, 0);
                        // show island
                        if (rs->TechniqueExecuteStart("battle_island_gettexture"))
                        {
                            pM->ProcessStage(Entity::Stage::realize, 1);
                            while (rs->TechniqueExecuteNext())
                                ;
                        }
                        rs->SetRenderTarget(pOldRenderTarg, pStencil);
                        rs->SetTransform(D3DTS_VIEW, oldmatv);
                        rs->SetTransform(D3DTS_PROJECTION, &oldmatp);
                    }
                    pStencil->Release();
                    pRenderTarg->Release();
                }
            }
            pOldRenderTarg->Release();
        }
    }

    if (m_pIslandTexture != nullptr)
        m_bYesIsland = true;
    else
        m_bYesIsland = false;
    m_fXIsland = posCenter.x;
    m_fYIsland = posCenter.z;
    m_fIslandWidth = m_fIslandHeight = islSize;
    m_fIslandRadius = sqrtf(SQR(m_fIslandWidth) + SQR(m_fIslandHeight));
}

void BATTLE_NAVIGATOR::UpdateMiniMap()
{
    float x, y, radius;

    x = .5f + (m_fXPos - m_fXIsland) / m_fIslandWidth;
    y = .5f - (m_fYPos - m_fYIsland) / m_fIslandHeight;
    radius = m_fWorldRad * m_fCurScale / m_fIslandRadius;

    auto *pV = static_cast<BI_ONETEXTURE_VERTEX *>(rs->LockVertexBuffer(m_idMapVBuf));
    if (pV != nullptr)
    {
        SetCircleVertexTex(pV, x, y, radius, -m_fAngle);
        rs->UnLockVertexBuffer(m_idMapVBuf);
    }
}

void BATTLE_NAVIGATOR::UpdateCurrentCharge()
{
    VDATA *pVD = core.Event("GetCurrentCharge");
    if (pVD == nullptr)
        return;
    long curCharge = m_curCharge;
    long curSailState = m_curSailState;
    long curWindPic = m_curWindPic;
    pVD->Get(curCharge, 0);
    pVD->Get(curSailState, 1);
    pVD->Get(curWindPic, 2);
    m_curCharge = curCharge;
    m_curSailState = curSailState;
    m_curWindPic = curWindPic;
    if (m_curCharge < 0)
        m_curCharge = 0;
    if (m_curSailState < 0)
        m_curSailState = 0;
    if (m_curWindPic < 0)
        m_curWindPic = 0;

    auto *pV = static_cast<BI_ONETEXTURE_VERTEX *>(rs->LockVertexBuffer(m_idCurChargeVBuf));
    if (pV != nullptr)
    {
        FRECT texRect;
        CalculateTextureRect(texRect, m_curCharge, m_ChargeGreed.x, m_ChargeGreed.y);
        pV[0].tu = texRect.left;
        pV[0].tv = texRect.top;
        pV[1].tu = texRect.left;
        pV[1].tv = texRect.bottom;
        pV[2].tu = texRect.right;
        pV[2].tv = texRect.top;
        pV[3].tu = texRect.right;
        pV[3].tv = texRect.bottom;

        CalculateTextureRect(texRect, m_curWindPic, m_WindGreed.x, m_WindGreed.y);
        pV[4].tu = texRect.left;
        pV[4].tv = texRect.top;
        pV[5].tu = texRect.left;
        pV[5].tv = texRect.bottom;
        pV[6].tu = texRect.right;
        pV[6].tv = texRect.top;
        pV[7].tu = texRect.right;
        pV[7].tv = texRect.bottom;

        CalculateTextureRect(texRect, m_curSailState, m_SailGreed.x, m_SailGreed.y);
        pV[8].tu = texRect.left;
        pV[8].tv = texRect.top;
        pV[9].tu = texRect.left;
        pV[9].tv = texRect.bottom;
        pV[10].tu = texRect.right;
        pV[10].tv = texRect.top;
        pV[11].tu = texRect.right;
        pV[11].tv = texRect.bottom;

        rs->UnLockVertexBuffer(m_idCurChargeVBuf);
    }
}

void BATTLE_NAVIGATOR::UpdateWindParam()
{
    if (!m_wb && !m_pAWeather)
    {
        m_wb = static_cast<WEATHER_BASE *>(EntityManager::GetEntityPointer(EntityManager::GetEntityId("weather")));
        /*if( core.IsNetActive() && !m_wb && m_pOwnerEI ) {
          VDATA * pSVWeather = (VDATA*)core.GetScriptVariable((m_pOwnerEI->IsServer()) ? "NSWeather" : "NCWeather");
        Assert(pSVWeather); m_pAWeather = pSVWeather->GetAClass(); Assert(m_pAWeather);
        }*/
    }

    if (m_wb)
    {
        m_fWindAngle = m_wb->GetFloat(whf_wind_angle);
        m_fWindStrength = m_wb->GetFloat(whf_wind_speed);
    }
    if (m_pAWeather)
    {
        m_fWindStrength = m_pAWeather->GetAttributeAsFloat("WindSpeed");
        m_fWindAngle = m_pAWeather->GetAttributeAsFloat("WindAngle");
    }
}

void BATTLE_NAVIGATOR::LostRender()
{
    rs->Release(m_pIslandTexture);
    m_pIslandTexture = nullptr;
}

void BATTLE_NAVIGATOR::RestoreRender()
{
    //~!~
    // rs->CreateTexture(MAP_TEXTURE_WIDTH, MAP_TEXTURE_HEIGHT, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R5G6B5,
    // D3DPOOL_DEFAULT, &m_pIslandTexture);
}
