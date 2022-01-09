#include "spyglass.h"
#include "../utils.h"
#include "../image/image.h"
#include "../image/img_render.h"
#include "../sea/ships_list.h"
#include "controls.h"
#include "core.h"
#include "entity.h"
#include "math3d/sphere.h"
#include "message.h"
#include "shared/battle_interface/msg_control.h"
#include "shared/events.h"
#include "shared/messages.h"

void ISPYGLASS::ImageParam::Release()
{
    STORM_DELETE(pImage);
}

void ISPYGLASS::ImageParam::LoadFromAttr(BIImageRender *pImgRender, ATTRIBUTES *pA, const char *pcDefName,
                                         int32_t nDefLeftPos, int32_t nDefTopPos, int32_t nDefRightPos, int32_t nDefBottomPos,
                                         int32_t nPrior)
{
    sTextureName = BIUtils::GetStringFromAttr(pA, "texture", pcDefName);
    dwColor = BIUtils::GetIntFromAttr(pA, "color", 0xFF808080);
    FULLRECT(rUV);
    BIUtils::ReadRectFromAttr(pA, "uv", rUV, rUV);
    rPos.left = nDefLeftPos;
    rPos.top = nDefTopPos;
    rPos.right = nDefRightPos;
    rPos.bottom = nDefBottomPos;
    BIUtils::ReadRectFromAttr(pA, "pos", rPos, rPos);
    pImage = static_cast<BIImage *>(
        pImgRender->CreateImage(BIType_square, sTextureName.c_str(), dwColor, rUV, rPos, nPrior));
    Assert(pImage);
}

void ISPYGLASS::ImageParam::ChangeIcon(BIImageRender *pImgRender, const char *pcTextureName, FRECT &frUV)
{
    rUV = frUV;
    if (sTextureName == pcTextureName)
    {
        pImage->SetUV(frUV);
    }
    else
    {
        const auto nPrior = pImage ? pImage->GetPrioritet() : ImagePrioritet_DefaultValue;
        STORM_DELETE(pImage);
        sTextureName = pcTextureName;
        pImage = static_cast<BIImage *>(
            pImgRender->CreateImage(BIType_square, sTextureName.c_str(), dwColor, rUV, rPos, nPrior));
    }
}

void ISPYGLASS::TextParam::LoadFromAttr(VDX9RENDER *rs, ATTRIBUTES *pA, const char *pcDefText, int32_t nDefXPos,
                                        int32_t nDefYPos)
{
    this->rs = rs;
    nFontID = BIUtils::GetFontIDFromAttr(pA, "font", rs, "interface_normal");
    BIUtils::ReadPosFromAttr(pA, "pos", pos.x, pos.y, nDefXPos, nDefYPos);
    fScale = BIUtils::GetFloatFromAttr(pA, "scale", 1.f);
    dwColor = BIUtils::GetIntFromAttr(pA, "color", 0xFFFFFFFF);
    nAlign = BIUtils::GetAlignmentFromAttr(pA, "align", PR_ALIGN_LEFT);
    sText = BIUtils::GetStringFromAttr(pA, "text", pcDefText);
}

void ISPYGLASS::TextParam::Print() const
{
    if (rs && !sText.empty())
        rs->ExtPrint(nFontID, dwColor, 0, nAlign, true, fScale, 0, 0, pos.x, pos.y, "%s", sText.c_str());
}

ISPYGLASS::ISPYGLASS()
{
    rs = nullptr;
    m_pImgRender = nullptr;
    m_bIsOn = false;
    m_bIsPresentShipInfo = false;
    m_nInfoCharacterIndex = -1;

    m_pFortObj = nullptr;

    m_fInfoKeepDelay = 0.f;
    m_fMaxInfoKeepDelay = 1.5f;
}

ISPYGLASS::~ISPYGLASS()
{
    Release();
}

bool ISPYGLASS::Init()
{
    if ((rs = static_cast<VDX9RENDER *>(core.GetService("dx9render"))) == nullptr)
    {
        throw std::runtime_error("Can`t create render service");
    }

    m_pImgRender = new BIImageRender(rs);
    Assert(m_pImgRender);

    m_Lens.LoadFromAttr(m_pImgRender, GetAttr("lens"), "battle_interface\\spyglass\\eye_badTub.tga", 0, 0, 1024, 768,
                        8000);
    m_ShipImage.LoadFromAttr(m_pImgRender, GetAttr("shipsign.ship"), "battle_interface\\ship_icons1.tga", 80, 600, 144,
                             664, 9000);
    m_ShipBack.LoadFromAttr(m_pImgRender, GetAttr("shipsign.back"), "battle_interface\\shipbackicon.tga", 80, 600, 208,
                            728, 10000);
    m_ShipHP.LoadFromAttr(m_pImgRender, GetAttr("shipsign.hp"), "battle_interface\\ShipState.tga", 80, 600, 208, 728,
                          10000);
    m_ShipSP.LoadFromAttr(m_pImgRender, GetAttr("shipsign.sp"), "battle_interface\\ShipState.tga", 80, 600, 208, 728,
                          10000);
    m_Class.LoadFromAttr(m_pImgRender, GetAttr("shipsign.class"), "battle_interface\\ShipClass.tga", 80, 600, 208, 728,
                         10000);
    m_Nation.LoadFromAttr(m_pImgRender, GetAttr("info.nation"), "flagall.tga", 210, 600, 274, 632, 10000);
    m_Cannon.LoadFromAttr(m_pImgRender, GetAttr("info.cannon"), "battle_interface\\list_icon2.tga", 320, 600, 384, 664,
                          10000);
    m_Sail.LoadFromAttr(m_pImgRender, GetAttr("info.speed"), "battle_interface\\list_icon2.tga", 420, 600, 484, 664,
                        10000);
    m_Charge.LoadFromAttr(m_pImgRender, GetAttr("info.charge"), "battle_interface\\list_icon2.tga", 520, 600, 584, 664,
                          10000);
    m_SailTo.LoadFromAttr(m_pImgRender, GetAttr("info.sailto"), "battle_interface\\list_icon2.tga", 620, 600, 684, 664,
                          10000); // boal

    // captain image data
    m_CaptainBack.LoadFromAttr(m_pImgRender, GetAttr("captain.back"), "battle_interface\\shipbackicon.tga", 816, 600,
                               944, 728, 10000);
    m_CaptainFace.LoadFromAttr(m_pImgRender, GetAttr("captain.face"), "battle_interface\\portraits\\face_0.tga", 816,
                               600, 880, 664, 9000);
    m_ImgCaptainFencing.LoadFromAttr(m_pImgRender, GetAttr("captain.fencing"), "interfaces\\skills.tga", 680, 600, 764,
                                     664, 10000);
    m_ImgCaptainCannon.LoadFromAttr(m_pImgRender, GetAttr("captain.cannon"), "interfaces\\skills.tga", 552, 600, 616,
                                    664, 10000);
    m_ImgCaptainAccuracy.LoadFromAttr(m_pImgRender, GetAttr("captain.accuracy"), "interfaces\\skills.tga", 424, 600,
                                      488, 664, 10000);
    m_ImgCaptainNavigation.LoadFromAttr(m_pImgRender, GetAttr("captain.navigation"), "interfaces\\skills.tga", 296, 600,
                                        360, 664, 10000);
    m_ImgCaptainBoarding.LoadFromAttr(m_pImgRender, GetAttr("captain.boarding"), "interfaces\\skills.tga", 168, 600,
                                      232, 664, 10000);

    // captain text data
    m_TextCaptainName.LoadFromAttr(rs, GetAttr("captext.capname"), "?", 210, 640);
    m_TextCaptainFencing.LoadFromAttr(rs, GetAttr("captext.fencing"), "", 210, 640); // replaced "?" with empty
    m_TextCaptainCannon.LoadFromAttr(rs, GetAttr("captext.cannon"), "", 210, 640);
    m_TextCaptainAccuracy.LoadFromAttr(rs, GetAttr("captext.accuracy"), "", 210, 640);
    m_TextCaptainNavigation.LoadFromAttr(rs, GetAttr("captext.navigation"), "", 210, 640);
    m_TextCaptainBoarding.LoadFromAttr(rs, GetAttr("captext.boarding"), "", 210, 640);

    m_txtShipType.LoadFromAttr(rs, GetAttr("text.shiptype"), "Caravella", 210, 640);
    m_txtShipName.LoadFromAttr(rs, GetAttr("text.shipname"), "Noname", 210, 660);
    m_txtCannons.LoadFromAttr(rs, GetAttr("text.cannons"), "14/16", 390, 610);
    m_txtShipSpeed.LoadFromAttr(rs, GetAttr("text.speed"), "6.3", 490, 610);
    m_txtSailTo.LoadFromAttr(rs, GetAttr("text.sailto"), "6.3", 590, 610); // boal
    m_txtShipCrew.LoadFromAttr(rs, GetAttr("text.crew"), "", 144, 700);

    FillUVArrayFromAttributes(m_aNationUV, GetAttr("nationuvarray"));
    FillUVArrayFromAttributes(m_aChargeUV, GetAttr("chargeuvarray"));
    FillUVArrayFromAttributes(m_aSailUV, GetAttr("sailuvarray"));

    CVECTOR pos, ang;
    rs->GetCamera(pos, ang, m_Camera.fOldPerspective);

    return true;
}

void ISPYGLASS::Execute(uint32_t delta_time)
{
    CONTROL_STATE cs;
    core.Controls->GetControlState("TelescopeIn", cs);
    if (cs.state == CST_ACTIVATED)
        core.Event("MSG_TELESCOPE_REQUEST", "l", 1);
    core.Controls->GetControlState("TelescopeOut", cs);
    if (cs.state == CST_ACTIVATED)
        core.Event("MSG_TELESCOPE_REQUEST", "l", 0);

    if (m_bIsOn)
    {
        UpdateCamera();
        auto bFindNewTarget = false;
        if (m_Camera.fCurUpdatingTime >= m_Camera.fUpdateTime)
            bFindNewTarget = true;
        if (m_nInfoCharacterIndex >= 0 && m_fInfoKeepDelay > 0.f)
        {
            m_fInfoKeepDelay -= 0.001f * delta_time;
            if (m_fInfoKeepDelay <= 0.f)
                bFindNewTarget = true;
        }
        if (bFindNewTarget)
        {
            m_Camera.fCurUpdatingTime = 0.f;
            FindNewTargetShip();
        }
    }
}

void ISPYGLASS::Realize(uint32_t delta_time) const
{
    if (m_bIsOn)
    {
        rs->MakePostProcess();
        if (m_pImgRender)
            m_pImgRender->Render();

        if (m_bIsPresentShipInfo)
        {
            m_txtShipType.Print();
            m_txtShipName.Print();
            m_txtCannons.Print();
            m_txtShipSpeed.Print();
            m_txtSailTo.Print(); // boal
            m_txtShipCrew.Print();

            m_TextCaptainName.Print();
            m_TextCaptainFencing.Print();
            m_TextCaptainCannon.Print();
            m_TextCaptainAccuracy.Print();
            m_TextCaptainNavigation.Print();
            m_TextCaptainBoarding.Print();
        }
    }
}

uint64_t ISPYGLASS::ProcessMessage(MESSAGE &message)
{
    auto nMsgCode = message.Long();

    switch (nMsgCode)
    {
    case MSG_TELESCOPE_REQUEST: {
        auto nIsOn = message.Long();
        TurnOnTelescope(nIsOn != 0);
    }
    break;

    case MSG_TELESCOPE_SET_TYPE: // "lsfll"
    {
        const std::string &param = message.String();
        auto fZoomScale = message.Float();
        auto fActivateTime = message.Long() * .001f;
        auto fUpdateTime = message.Long() * .001f;
        ChangeTelescopeType(param.c_str(), fZoomScale, fActivateTime, fUpdateTime);
        m_pFortObj = nullptr;
    }
    break;

    case MSG_ISG_UPDATE: // "lsslllfflllllllllllss"
    {
        const std::string &shipname = message.String();
        const std::string &shiptype = message.String();
        auto fRelativeHP = message.Long() * .01f;
        auto fRelativeSP = message.Long() * .01f;
        auto nShipCrew = message.Long();
        auto fShipSpeed = message.Float();
        auto fSailTo = message.Float(); // boal
        auto nCurCannons = message.Long();
        auto nMaxCannons = message.Long();
        auto nCharge = message.Long();
        auto nNation = message.Long();

        auto nSailState = message.Long();
        auto nFace = message.Long();
        auto nFencingSkl = message.Long();
        auto nCannonSkl = message.Long();
        auto nAccuracySkl = message.Long();
        auto nNavigationSkl = message.Long();
        auto nBoardingSkl = message.Long();
        const std::string &captainname = message.String();
        const std::string &facetexture = message.String();
        auto nShipClass = message.Long();
        ChangeTargetData(shipname.c_str(), shiptype.c_str(), fRelativeHP, fRelativeSP, nShipCrew, fShipSpeed, fSailTo,
                         nCurCannons, nMaxCannons, nCharge, nNation, nSailState, nFace, nFencingSkl, nCannonSkl,
                         nAccuracySkl, nNavigationSkl, nBoardingSkl, captainname.c_str(), facetexture.c_str(),
                         nShipClass);
    }
    break;

    case MSG_ISG_SET_SHIPICON: // "sffff"
    {
        FRECT frUV;
        const std::string &texturename = message.String();
        frUV.left = message.Float();
        frUV.top = message.Float();
        frUV.right = message.Float();
        frUV.bottom = message.Float();
        m_ShipImage.ChangeIcon(m_pImgRender, texturename.c_str(), frUV);
    }
    break;
    case MSG_ISG_SET_CHARICON: // "sffff"
    {
        FRECT frUV;
        const std::string &captexturename = message.String();
        frUV.left = message.Float();
        frUV.top = message.Float();
        frUV.right = message.Float();
        frUV.bottom = message.Float();
        m_CaptainBack.ChangeIcon(m_pImgRender, captexturename.c_str(), frUV);
    }
    break;
    }
    return 0;
}

void ISPYGLASS::Release()
{
    TurnOnTelescope(false);

    m_Lens.Release();
    m_ShipImage.Release();
    m_ShipBack.Release();
    m_ShipHP.Release();
    m_ShipSP.Release();
    m_Class.Release();
    m_Nation.Release();
    m_Cannon.Release();
    m_Sail.Release();
    m_Charge.Release();
    m_SailTo.Release(); // boal
    m_txtShipType.Release();
    m_txtShipName.Release();
    m_txtCannons.Release();
    m_txtShipSpeed.Release();
    m_txtSailTo.Release(); // boal

    m_CaptainBack.Release();
    m_CaptainFace.Release();
    m_TextCaptainName.Release();
    m_ImgCaptainFencing.Release();
    m_TextCaptainFencing.Release();
    m_ImgCaptainCannon.Release();
    m_TextCaptainCannon.Release();
    m_ImgCaptainAccuracy.Release();
    m_TextCaptainAccuracy.Release();
    m_ImgCaptainNavigation.Release();
    m_TextCaptainNavigation.Release();
    m_ImgCaptainBoarding.Release();
    m_TextCaptainBoarding.Release();

    STORM_DELETE(m_pImgRender);
}

ATTRIBUTES *ISPYGLASS::GetAttr(const char *pcAttrName) const
{
    if (AttributesPointer)
        return AttributesPointer->FindAClass(AttributesPointer, pcAttrName);
    return nullptr;
}

void ISPYGLASS::TurnOnTelescope(bool bTurnOn)
{
    if (bTurnOn == m_bIsOn)
        return;
    m_bIsOn = bTurnOn;
    if (m_bIsOn)
    {
        m_Camera.bIsActive = false;
        m_Camera.bIsGrow = true;
        m_Camera.fCurActivateTime = 0.f;
        m_Camera.fCurUpdatingTime = m_Camera.fUpdateTime;

        m_pFortObj = nullptr;

        core.Event(TELESCOPE_ACTIVE, "l", 1);
        core.Event("BI_VISIBLE", "l", 0);
    }
    else
    {
        m_bIsPresentShipInfo = false;
        m_nInfoCharacterIndex = -1;
        m_Camera.bIsActive = false;
        m_Camera.bIsGrow = true;
        m_Camera.fCurUpdatingTime = 0.f;

        core.Event(TELESCOPE_ACTIVE, "l", 0);
        core.Event("BI_VISIBLE", "l", 1);
    }
}

void ISPYGLASS::SetShipInfo(int32_t nCharIndex)
{
    // if( m_nInfoCharacterIndex == nCharIndex ) return;
    m_nInfoCharacterIndex = nCharIndex;
    if (m_nInfoCharacterIndex < 0)
        m_bIsPresentShipInfo = false;
    else
        m_bIsPresentShipInfo = true;

    core.Event("SetTelescopeInfo", "l", m_nInfoCharacterIndex);

    if (m_bIsPresentShipInfo)
    {
        m_ShipImage.pImage->SetColor(m_ShipImage.dwColor);
        m_ShipBack.pImage->SetColor(m_ShipBack.dwColor);
        m_ShipHP.pImage->SetColor(m_ShipHP.dwColor);
        m_ShipSP.pImage->SetColor(m_ShipSP.dwColor);
        m_Class.pImage->SetColor(m_Class.dwColor);
        m_Nation.pImage->SetColor(m_Nation.dwColor);
        m_Cannon.pImage->SetColor(m_Cannon.dwColor);
        m_Sail.pImage->SetColor(m_Sail.dwColor);
        m_Charge.pImage->SetColor(m_Charge.dwColor);
        m_SailTo.pImage->SetColor(m_SailTo.dwColor); // boal

        m_CaptainBack.pImage->SetColor(m_CaptainBack.dwColor);
        m_CaptainFace.pImage->SetColor(m_CaptainFace.dwColor);
        m_ImgCaptainFencing.pImage->SetColor(m_ImgCaptainFencing.dwColor);
        m_ImgCaptainCannon.pImage->SetColor(m_ImgCaptainCannon.dwColor);
        m_ImgCaptainAccuracy.pImage->SetColor(m_ImgCaptainAccuracy.dwColor);
        m_ImgCaptainNavigation.pImage->SetColor(m_ImgCaptainNavigation.dwColor);
        m_ImgCaptainBoarding.pImage->SetColor(m_ImgCaptainBoarding.dwColor);
    }
    else
    {
        m_ShipImage.pImage->SetColor(m_ShipImage.dwColor & 0xFFFFFF);
        m_ShipBack.pImage->SetColor(m_ShipBack.dwColor & 0xFFFFFF);
        m_ShipHP.pImage->SetColor(m_ShipHP.dwColor & 0xFFFFFF);
        m_ShipSP.pImage->SetColor(m_ShipSP.dwColor & 0xFFFFFF);
        m_Class.pImage->SetColor(m_Class.dwColor & 0xFFFFFF);
        m_Nation.pImage->SetColor(m_Nation.dwColor & 0xFFFFFF);
        m_Cannon.pImage->SetColor(m_Cannon.dwColor & 0xFFFFFF);
        m_Sail.pImage->SetColor(m_Sail.dwColor & 0xFFFFFF);
        m_Charge.pImage->SetColor(m_Charge.dwColor & 0xFFFFFF);
        m_SailTo.pImage->SetColor(m_SailTo.dwColor & 0xFFFFFF); // boal

        m_CaptainBack.pImage->SetColor(0xFFFFFF);
        m_CaptainFace.pImage->SetColor(0xFFFFFF);
        m_ImgCaptainFencing.pImage->SetColor(0xFFFFFF);
        m_ImgCaptainCannon.pImage->SetColor(0xFFFFFF);
        m_ImgCaptainAccuracy.pImage->SetColor(0xFFFFFF);
        m_ImgCaptainNavigation.pImage->SetColor(0xFFFFFF);
        m_ImgCaptainBoarding.pImage->SetColor(0xFFFFFF);
    }
}

void ISPYGLASS::FindNewTargetShip()
{
    // get trace ray
    CMatrix mtxv;
    rs->GetTransform(D3DTS_VIEW, (D3DMATRIX *)&mtxv);
    CVECTOR src, dst;
    mtxv.MulToInv(CVECTOR(0.0f, 0.0f, 0.0f), src);
    mtxv.MulToInv(CVECTOR(0.0f, 0.0f, 10000.f), dst);

    // find ships
    int32_t nFindedCharIndex = -1;
    auto fFindedDistance = 2.f;
    auto *pMainSD = g_ShipList.GetMainCharacterShip();
    for (auto *pSD = g_ShipList.GetShipRoot(); pSD; pSD = pSD->next)
    {
        if (pMainSD == pSD)
            continue;
        auto fTrace = pSD->pShip->Trace(src, dst);
        if (fTrace <= 1.f && fTrace < fFindedDistance)
        {
            nFindedCharIndex = pSD->characterIndex;
            fFindedDistance = fTrace;
        }
    }

    // find fort
    auto *pFort = GetFort();
    if (pFort)
    {
        float fTrace = pFort->Trace(src, dst);
        if (fTrace <= 1.f && fTrace < fFindedDistance)
        {
            fFindedDistance = fTrace;
            ATTRIBUTES *pA = pFort->GetACharacter();
            if (pA)
                nFindedCharIndex = pA->GetAttributeAsDword("index", -1);
        }
    }

    // nFindedCharIndex = 0;
    if (nFindedCharIndex == -1)
    {
        if (m_nInfoCharacterIndex != -1 && m_fInfoKeepDelay > 0.f)
        {
            SetShipInfo(m_nInfoCharacterIndex);
        }
        else
        {
            // find by ship sphere
            fFindedDistance = -1.f;
            Vector vsrc = *(Vector *)&src;
            Vector vdst = *(Vector *)&dst;
            for (SHIP_DESCRIBE_LIST::SHIP_DESCR *pSD = g_ShipList.GetShipRoot(); pSD; pSD = pSD->next)
            {
                if (pMainSD == pSD)
                    continue;
                Sphere sph;
                const auto &vec = pSD->pShip->GetPos();
                sph.pos = {vec.x, vec.y, vec.z};
                sph.r = 40.f;
                if (sph.Intersection(vsrc, vdst))
                {
                    float fTrace = ~(pSD->pShip->GetPos() - src);
                    if (fFindedDistance < 0.f || fTrace < fFindedDistance)
                    {
                        nFindedCharIndex = pSD->characterIndex;
                        fFindedDistance = fTrace;
                    }
                }
            }
            //
            SetShipInfo(nFindedCharIndex);
        }
    }
    else
    {
        m_fInfoKeepDelay = m_fMaxInfoKeepDelay;
        SetShipInfo(nFindedCharIndex);
    }
}

void ISPYGLASS::ChangeTelescopeType(const char *pcTextureName, float fZoomScale, float fActivateTime, float fUpdateTime)
{
    m_Camera.fActivateTime = fActivateTime;
    m_Camera.fUpdateTime = fUpdateTime;
    if (fZoomScale > 0.f)
        m_Camera.fSpyGlassPerspective = m_Camera.fOldPerspective / fZoomScale;
    else
        m_Camera.fSpyGlassPerspective = m_Camera.fOldPerspective;

    m_Camera.bIsActive = false;
    m_Camera.bIsGrow = true;
}

void ISPYGLASS::UpdateCamera()
{
    if (!m_bIsOn)
        return;
    const float fTime = core.GetDeltaTime() * .001f;
    m_Camera.fCurUpdatingTime += fTime;

    if (!m_Camera.bIsActive)
    {
        m_Camera.fCurActivateTime += fTime;
        if (m_Camera.fCurActivateTime >= m_Camera.fActivateTime)
        {
            m_Camera.fCurActivateTime = 0.f; // counter to zero
            if (m_Camera.bIsGrow)
                m_Camera.bIsActive = true;        // now working fully
            m_Camera.bIsGrow = !m_Camera.bIsGrow; // ready immediately
        }
    }

    if (m_Camera.bIsActive)
        rs->SetPerspective(m_Camera.fSpyGlassPerspective);
    else
    {
        float fPerspect = m_Camera.fOldPerspective;
        if (m_Camera.bIsGrow)
        {
            fPerspect = m_Camera.fOldPerspective + (m_Camera.fSpyGlassPerspective - m_Camera.fOldPerspective) *
                                                       (m_Camera.fCurActivateTime / m_Camera.fActivateTime);
        }
        else
        {
            fPerspect = m_Camera.fOldPerspective - (m_Camera.fSpyGlassPerspective - m_Camera.fOldPerspective) *
                                                       (1.f - m_Camera.fCurActivateTime / m_Camera.fActivateTime);
        }
        rs->SetPerspective(fPerspect);
    }
}

void ISPYGLASS::ChangeTargetData(const char *pcShipName, const char *pcShipType, float fRelativeHP, float fRelativeSP,
                                 int32_t nShipCrew, float fShipSpeed, float fSailTo, int32_t nCurCannons, int32_t nMaxCannons,
                                 int32_t nCharge, int32_t nNation, int32_t nSailState, int32_t nFace, int32_t nFencing, int32_t nCannon,
                                 int32_t nAccuracy, int32_t nNavigation, int32_t nBoarding, const char *pcCaptainName,
                                 const char *pcFaceTexture, int32_t nShipClass)
{
    char param[256];

    m_txtShipName.sText = pcShipName;
    m_txtShipType.sText = pcShipType;

    // distance to -->
    if (fSailTo >= 0.f)
    {
        sprintf_s(param, sizeof(param), "%.1f", fSailTo);
        m_txtSailTo.sText = param;
    }
    else
    {
        m_txtSailTo.sText = "";
    }
    // boal <--
    if (fShipSpeed >= 0.f)
    {
        sprintf_s(param, sizeof(param), "%.1f", fShipSpeed);
        m_txtShipSpeed.sText = param;
    }
    else
    {
        m_txtShipSpeed.sText = "";
    }

    if (nCurCannons >= 0 && nMaxCannons >= 0)
    {
        sprintf_s(param, sizeof(param), "%d/%d", nCurCannons, nMaxCannons);
        m_txtCannons.sText = param;
        if (m_Cannon.pImage)
            m_Cannon.pImage->CutSide(0.f, 0.f, 0.f, 0.f); // show
    }
    else
    {
        m_txtCannons.sText = "";
        if (m_Cannon.pImage)
            m_Cannon.pImage->CutSide(0.f, 1.f, 0.f, 0.f); // hide
    }

    if (nShipClass > 0)
    {
        float fCutVal = 1.f;
        switch (nShipClass)
        {
        case 1:
            fCutVal = 1.f - 13.f / 64.f;
            break;
        case 2:
            fCutVal = 1.f - 22.f / 64.f;
            break;
        case 3:
            fCutVal = 1.f - 32.f / 64.f;
            break;
        case 4:
            fCutVal = 1.f - 40.f / 64.f;
            break;
        case 5:
            fCutVal = 1.f - 50.f / 64.f;
            break;
        case 6:
            fCutVal = 0.f;
            break;
        case 7:
            fCutVal = 0.f;
            break;
        }
        if (m_Class.pImage)
            m_Class.pImage->CutSide(0.f, fCutVal, 0.f, 0.f);
    }
    else
    {
        if (m_Class.pImage)
            m_Class.pImage->CutSide(0.f, 1.f, 0.f, 0.f);
    }

    if (nShipCrew >= 0)
    {
        sprintf_s(param, sizeof(param), "%d", nShipCrew);
        m_txtShipCrew.sText = param;
    }
    else
    {
        m_txtShipCrew.sText = "";
    }

    if (fRelativeHP < 0)
        fRelativeHP = 0.f;
    if (m_ShipHP.pImage)
        m_ShipHP.pImage->CutSide(0.f, 0.f, 1.f - fRelativeHP, 0.f);
    if (fRelativeSP < 0)
        fRelativeSP = 0.f;
    if (m_ShipSP.pImage)
        m_ShipSP.pImage->CutSide(0.f, 0.f, 1.f - fRelativeSP, 0.f);

    if (nNation >= 0 && nNation < m_aNationUV.size())
    {
        if (m_Nation.pImage)
            m_Nation.pImage->SetUV(m_aNationUV[nNation]);
    }

    if (nCharge >= 0 && nCharge < m_aChargeUV.size())
    {
        if (m_Charge.pImage)
        {
            m_Charge.pImage->CutSide(0.f, 0.f, 0.f, 0.f); // show
            m_Charge.pImage->SetUV(m_aChargeUV[nCharge]);
        }
    }
    else
    {
        if (m_Charge.pImage)
            m_Charge.pImage->CutSide(0.f, 1.f, 0.f, 0.f); // hide
    }

    if (nSailState >= 0 && nSailState < m_aSailUV.size())
    {
        if (m_Sail.pImage)
        {
            m_Sail.pImage->CutSide(0.f, 0.f, 0.f, 0.f);
            m_Sail.pImage->SetUV(m_aSailUV[nSailState]);
        }
    }
    else
    {
        if (m_Sail.pImage)
            m_Sail.pImage->CutSide(0.f, 1.f, 0.f, 0.f);
    }

    if (m_CaptainFace.pImage)
    {
        if (nFace >= 0)
            sprintf_s(param, sizeof(param), "battle_interface\\portraits\\face_%d.tga", nFace);
        else
            sprintf_s(param, sizeof(param), "%s", pcFaceTexture);
        FRECT frUV;
        frUV.left = 0.f;
        frUV.right = 1.f;
        frUV.top = 0.f;
        frUV.bottom = 1.f;
        m_CaptainFace.ChangeIcon(m_pImgRender, param, frUV);
    }
    // further edit the skill icons to hide / show
    if (nFencing >= 0)
    {
        sprintf_s(param, sizeof(param), "%d", nFencing);
        m_TextCaptainFencing.sText = param;
        if (m_ImgCaptainFencing.pImage)
            m_ImgCaptainFencing.pImage->CutSide(0.f, 0.f, 0.f, 0.f);
    }
    else
    {
        if (m_ImgCaptainFencing.pImage)
            m_ImgCaptainFencing.pImage->CutSide(0.f, 1.f, 0.f, 0.f);
    }

    if (nCannon >= 0)
    {
        sprintf_s(param, sizeof(param), "%d", nCannon);
        m_TextCaptainCannon.sText = param;
        if (m_ImgCaptainCannon.pImage)
            m_ImgCaptainCannon.pImage->CutSide(0.f, 0.f, 0.f, 0.f);
    }
    else
    {
        if (m_ImgCaptainCannon.pImage)
            m_ImgCaptainCannon.pImage->CutSide(0.f, 1.f, 0.f, 0.f);
    }
    if (nAccuracy >= 0)
    {
        sprintf_s(param, sizeof(param), "%d", nAccuracy);
        m_TextCaptainAccuracy.sText = param;
        if (m_ImgCaptainAccuracy.pImage)
            m_ImgCaptainAccuracy.pImage->CutSide(0.f, 0.f, 0.f, 0.f);
    }
    else
    {
        if (m_ImgCaptainAccuracy.pImage)
            m_ImgCaptainAccuracy.pImage->CutSide(0.f, 1.f, 0.f, 0.f);
    }
    if (nNavigation >= 0)
    {
        sprintf_s(param, sizeof(param), "%d", nNavigation);
        m_TextCaptainNavigation.sText = param;
        if (m_ImgCaptainNavigation.pImage)
            m_ImgCaptainNavigation.pImage->CutSide(0.f, 0.f, 0.f, 0.f);
    }
    else
    {
        if (m_ImgCaptainNavigation.pImage)
            m_ImgCaptainNavigation.pImage->CutSide(0.f, 1.f, 0.f, 0.f);
    }
    if (nBoarding >= 0)
    {
        sprintf_s(param, sizeof(param), "%d", nBoarding);
        m_TextCaptainBoarding.sText = param;
        if (m_ImgCaptainBoarding.pImage)
            m_ImgCaptainBoarding.pImage->CutSide(0.f, 0.f, 0.f, 0.f);
    }
    else
    {
        if (m_ImgCaptainBoarding.pImage)
            m_ImgCaptainBoarding.pImage->CutSide(0.f, 1.f, 0.f, 0.f);
    }
    m_TextCaptainName.sText = pcCaptainName;
}

void ISPYGLASS::FillUVArrayFromAttributes(std::vector<FRECT> &m_aUV, ATTRIBUTES *pA) const
{
    m_aUV.clear();
    if (!pA)
        return;
    for (int32_t n = 0; n < static_cast<int32_t>(pA->GetAttributesNum()); n++)
    {
        FRECT rUV;
        rUV.left = rUV.top = 0.f;
        rUV.right = rUV.bottom = 1.f;
        BIUtils::ReadRectFromAttr(pA, pA->GetAttributeName(n), rUV, rUV);
        m_aUV.push_back(rUV);
    }
}

VAI_OBJBASE *ISPYGLASS::GetFort()
{
    if (!m_pFortObj)
    {
        m_pFortObj = static_cast<VAI_OBJBASE *>(EntityManager::GetEntityPointer(EntityManager::GetEntityId("AIFORT")));
    }
    return m_pFortObj;
}
