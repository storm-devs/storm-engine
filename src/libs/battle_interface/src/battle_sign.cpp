#include "battle_sign.h"
#include "utils.h"
#include "controls.h"
#include "core.h"
#include "vma.hpp"

BISignIcon::BISignIcon(entid_t BIEntityID, VDX9RENDER *pRS)
{
    Assert(pRS);

    m_pARoot = nullptr;
    m_pAData = nullptr;

    m_idHostEntity = BIEntityID;
    m_pRS = pRS;

    m_nVBufID = -1;
    m_nIBufID = -1;

    m_nBackTextureID = -1;
    m_nSignFaceTextureID = -1;
    m_nSignStateTextureID = -1;
    m_nSignStarTextureID = -1;

    m_nBackSquareQ = 0;
    m_nSignFaceSquareQ = 0;
    m_nSignStateSquareQ = 0;
    m_nSignStarSquareQ = 0;

    m_nMaxSquareQ = 0;

    m_bMakeUpdate = true;
    m_bActive = false;

    m_idSignTextFont = -1;
}

BISignIcon::~BISignIcon()
{
    Release();
}

void BISignIcon::Draw()
{
    if (m_bMakeUpdate)
    {
        m_bMakeUpdate = false;
        Recollect();

        UpdateChildrens();
    }

    FillVertexBuffer();

    if (m_nVBufID != -1 && m_nIBufID != -1)
    {
        int32_t nStartV = 0;
        const int32_t nStartI = 0;

        // sign face
        if (m_nSignFaceSquareQ > 0)
        {
            m_pRS->TextureSet(0, m_nSignFaceTextureID);
            m_pRS->DrawBuffer(m_nVBufID, sizeof(BI_COLOR_VERTEX), m_nIBufID, nStartV, m_nSignFaceSquareQ * 4, nStartI,
                              m_nSignFaceSquareQ * 2, "battle_colorRectangle");
        }
        nStartV += m_nSignFaceSquareQ * 4;

        // back
        if (m_nBackSquareQ > 0)
        {
            m_pRS->TextureSet(0, m_nBackTextureID);
            m_pRS->DrawBuffer(m_nVBufID, sizeof(BI_COLOR_VERTEX), m_nIBufID, nStartV, m_nBackSquareQ * 4, nStartI,
                              m_nBackSquareQ * 2, "battle_colorRectangle");
        }
        nStartV += m_nBackSquareQ * 4;

        // sign state (left & right)
        if (m_nSignStateSquareQ > 0)
        {
            m_pRS->TextureSet(0, m_nSignStateTextureID);
            m_pRS->DrawBuffer(m_nVBufID, sizeof(BI_COLOR_VERTEX), m_nIBufID, nStartV, m_nSignStateSquareQ * 4, nStartI,
                              m_nSignStateSquareQ * 2, "battle_colorRectangle");
        }
        nStartV += m_nSignStateSquareQ * 4;

        // sign star
        if (m_nSignStarSquareQ > 0)
        {
            m_pRS->TextureSet(0, m_nSignStarTextureID);
            m_pRS->DrawBuffer(m_nVBufID, sizeof(BI_COLOR_VERTEX), m_nIBufID, nStartV, m_nSignStarSquareQ * 4, nStartI,
                              m_nSignStarSquareQ * 2, "battle_alphacutcolor");
        }
        nStartV += m_nSignStarSquareQ * 4;
    }

    for (int32_t n = 0; n < m_nSignQ; n++)
    {
        if (m_Sign[n].sText.length() > 0)
        {
            m_pRS->ExtPrint(m_idSignTextFont, m_dwSignTextFontColor, 0, PR_ALIGN_CENTER, false, m_fSignTextFontScale, 0,
                            0, static_cast<int32_t>(m_Sign[n].pntPos.x) + m_SignTextFontOffset.x,
                            static_cast<int32_t>(m_Sign[n].pntPos.y) + m_SignTextFontOffset.y, "%s",
                            m_Sign[n].sText.c_str());
        }
    }
}

void BISignIcon::Init(ATTRIBUTES *pRoot, ATTRIBUTES *pA)
{
    int32_t n;
    const char *pcTmp;
    char param[256];

    m_pARoot = pRoot;
    m_pAData = pA;

    // default value
    m_nBackTextureID = -1;
    m_dwBackColor = ARGB(255, 128, 128, 128);
    FULLRECT(m_rBackUV);
    ZERROPOINT(m_pntBackOffset);
    FILLPOINT(m_pntBackIconSize, 128, 128);

    m_nSignStateTextureID = -1;
    m_dwSignStateColor = ARGB(255, 128, 128, 128);
    FULLRECT(m_rSignStateLeftUV);
    ZERROPOINT(m_pntSignStateLeftOffset);
    FILLPOINT(m_pntSignStateLeftIconSize, 128, 128);
    FULLRECT(m_rSignStateRightUV);
    ZERROPOINT(m_pntSignStateRightOffset);
    FILLPOINT(m_pntSignStateRightIconSize, 128, 128);

    m_nSignStarTextureID = -1;
    m_dwSignStarColor = ARGB(255, 128, 128, 128);
    FULLRECT(m_rSignStarUV);
    ZERROPOINT(m_pntSignStarOffset);
    FILLPOINT(m_pntSignStarIconSize, 128, 128);

    m_nSignFaceTextureID = -1;
    m_dwSignFaceColor = ARGB(255, 128, 128, 128);
    ZERROPOINT(m_pntSignFaceOffset);
    FILLPOINT(m_pntSignFaceIconSize, 128, 128);

    m_idSignTextFont = -1;
    m_dwSignTextFontColor = ARGB(255, 64, 64, 64);
    m_fSignTextFontScale = 1.f;
    m_SignTextFontOffset.x = -14;
    m_SignTextFontOffset.y = 18;

    for (n = 0; n < MAX_SIGN_QUANTITY; n++)
    {
        m_Sign[n].pntPos.x = 20.f;
        m_Sign[n].pntPos.y = 20.f + (m_pntBackIconSize.y + 10.f) * n;
    }

    if (pA)
    {
        pcTmp = pA->GetAttribute("fontid");
        if (pcTmp)
            m_idSignTextFont = m_pRS->LoadFont(pcTmp);
        m_dwSignTextFontColor = pA->GetAttributeAsDword("fontcolor", m_dwSignTextFontColor);
        m_fSignTextFontScale = pA->GetAttributeAsFloat("fontscale", m_fSignTextFontScale);

        pcTmp = pA->GetAttribute("fontoffset");
        if (pcTmp)
            sscanf(pcTmp, "%ld,%ld", &m_SignTextFontOffset.x, &m_SignTextFontOffset.y);

        pcTmp = pA->GetAttribute("backtexturename");
        if (pcTmp)
            m_nBackTextureID = m_pRS->TextureCreate(pcTmp);
        m_dwBackColor = pA->GetAttributeAsDword("backcolor", m_dwBackColor);
        pcTmp = pA->GetAttribute("backuv");
        if (pcTmp)
            sscanf(pcTmp, "%f,%f,%f,%f", &m_rBackUV.left, &m_rBackUV.top, &m_rBackUV.right, &m_rBackUV.bottom);
        pcTmp = pA->GetAttribute("backoffset");
        if (pcTmp)
            sscanf(pcTmp, "%f,%f", &m_pntBackOffset.x, &m_pntBackOffset.y);
        pcTmp = pA->GetAttribute("backiconsize");
        if (pcTmp)
            sscanf(pcTmp, "%f,%f", &m_pntBackIconSize.x, &m_pntBackIconSize.y);

        pcTmp = pA->GetAttribute("shipstatetexturename");
        if (pcTmp)
            m_nSignStateTextureID = m_pRS->TextureCreate(pcTmp);
        m_dwSignStateColor = pA->GetAttributeAsDword("shipstatecolor", m_dwSignStateColor);
        pcTmp = pA->GetAttribute("shiphpuv");
        if (pcTmp)
            sscanf(pcTmp, "%f,%f,%f,%f", &m_rSignStateLeftUV.left, &m_rSignStateLeftUV.top, &m_rSignStateLeftUV.right,
                   &m_rSignStateLeftUV.bottom);
        pcTmp = pA->GetAttribute("shiphpoffset");
        if (pcTmp)
            sscanf(pcTmp, "%f,%f", &m_pntSignStateLeftOffset.x, &m_pntSignStateLeftOffset.y);
        pcTmp = pA->GetAttribute("shiphpiconsize");
        if (pcTmp)
            sscanf(pcTmp, "%f,%f", &m_pntSignStateLeftIconSize.x, &m_pntSignStateLeftIconSize.y);
        pcTmp = pA->GetAttribute("shipspuv");
        if (pcTmp)
            sscanf(pcTmp, "%f,%f,%f,%f", &m_rSignStateRightUV.left, &m_rSignStateRightUV.top,
                   &m_rSignStateRightUV.right, &m_rSignStateRightUV.bottom);
        pcTmp = pA->GetAttribute("shipspoffset");
        if (pcTmp)
            sscanf(pcTmp, "%f,%f", &m_pntSignStateRightOffset.x, &m_pntSignStateRightOffset.y);
        pcTmp = pA->GetAttribute("shipspiconsize");
        if (pcTmp)
            sscanf(pcTmp, "%f,%f", &m_pntSignStateRightIconSize.x, &m_pntSignStateRightIconSize.y);

        pcTmp = pA->GetAttribute("shipclasstexturename");
        if (pcTmp)
            m_nSignStarTextureID = m_pRS->TextureCreate(pcTmp);
        m_dwSignStarColor = pA->GetAttributeAsDword("shipclasscolor", m_dwSignStarColor);
        pcTmp = pA->GetAttribute("shipclassuv");
        if (pcTmp)
            sscanf(pcTmp, "%f,%f,%f,%f", &m_rSignStarUV.left, &m_rSignStarUV.top, &m_rSignStarUV.right,
                   &m_rSignStarUV.bottom);
        pcTmp = pA->GetAttribute("shipclassoffset");
        if (pcTmp)
            sscanf(pcTmp, "%f,%f", &m_pntSignStarOffset.x, &m_pntSignStarOffset.y);
        pcTmp = pA->GetAttribute("shipclassiconsize");
        if (pcTmp)
            sscanf(pcTmp, "%f,%f", &m_pntSignStarIconSize.x, &m_pntSignStarIconSize.y);

        pcTmp = pA->GetAttribute("shiptexturename");
        if (pcTmp)
            m_nSignFaceTextureID = m_pRS->TextureCreate(pcTmp);
        m_dwSignFaceColor = pA->GetAttributeAsDword("shipcolor", m_dwSignFaceColor);
        /*pcTmp = pA->GetAttribute( "shipuv" );
        if( pcTmp ) sscanf( pcTmp, "%f,%f,%f,%f",
         * &m_rSignStarUV.left,&m_rSignStarUV.top,
        &m_rSignStarUV.right,&m_rSignStarUV.bottom );*/
        pcTmp = pA->GetAttribute("shipoffset");
        if (pcTmp)
            sscanf(pcTmp, "%f,%f", &m_pntSignFaceOffset.x, &m_pntSignFaceOffset.y);
        pcTmp = pA->GetAttribute("shipiconsize");
        if (pcTmp)
            sscanf(pcTmp, "%f,%f", &m_pntSignFaceIconSize.x, &m_pntSignFaceIconSize.y);

        for (n = 0; n < MAX_SIGN_QUANTITY; n++)
        {
            sprintf_s(param, sizeof(param), "iconoffset%d", n + 1);
            pcTmp = pA->GetAttribute(param);
            if (pcTmp)
                sscanf(pcTmp, "%f,%f", &m_Sign[n].pntPos.x, &m_Sign[n].pntPos.y);
        }
    }

    m_bMakeUpdate = true;
}

void BISignIcon::Recollect()
{
    const auto n = CalculateSignQuantity();
    UpdateBuffers(n);
}

void BISignIcon::SetActive(bool bActive)
{
    if (m_pARoot)
    {
        m_pARoot->SetAttributeUseDword("ComState", (bActive ? 1 : 0));
    }

    if (m_bActive == bActive)
        return;
    m_bActive = bActive;

    m_bMakeUpdate = true;
}

void BISignIcon::MakeControl()
{
    CONTROL_STATE cs;

    core.Controls->GetControlState(BI_COMMANDS_CONFIRM, cs);
    if (cs.state == CST_ACTIVATED)
        ExecuteCommand(Command_confirm);

    core.Controls->GetControlState(BI_COMMANDS_LEFTSTEP, cs);
    if (cs.state == CST_ACTIVATED)
        ExecuteCommand(Command_left);

    core.Controls->GetControlState(BI_COMMANDS_RIGHTSTEP, cs);
    if (cs.state == CST_ACTIVATED)
        ExecuteCommand(Command_right);

    core.Controls->GetControlState(BI_COMMANDS_UPSTEP, cs);
    if (cs.state == CST_ACTIVATED)
        ExecuteCommand(Command_up);

    core.Controls->GetControlState(BI_COMMANDS_DOWNSTEP, cs);
    if (cs.state == CST_ACTIVATED)
        ExecuteCommand(Command_down);

    core.Controls->GetControlState(BI_COMMANDS_CANCEL, cs);
    if (cs.state == CST_ACTIVATED)
        ExecuteCommand(Command_cancel);
}

void BISignIcon::Release()
{
    SetActive(false); // disable control

    TEXTURE_RELEASE(m_pRS, m_nBackTextureID);
    TEXTURE_RELEASE(m_pRS, m_nSignFaceTextureID);
    TEXTURE_RELEASE(m_pRS, m_nSignStateTextureID);
    TEXTURE_RELEASE(m_pRS, m_nSignStarTextureID);
    VERTEX_BUFFER_RELEASE(m_pRS, m_nVBufID);
    INDEX_BUFFER_RELEASE(m_pRS, m_nIBufID);
    FONT_RELEASE(m_pRS, m_idSignTextFont);

    m_nMaxSquareQ = 0;
    m_nBackSquareQ = 0;
    m_nSignFaceSquareQ = 0;
    m_nSignStateSquareQ = 0;
    m_nSignStarSquareQ = 0;
}

int32_t BISignIcon::CalculateSignQuantity()
{
    m_nSignQ = 0;
    for (int32_t n = 0; n < MAX_SIGN_QUANTITY; n++)
    {
        if (!m_Sign[n].bUse)
            break;
        m_nSignQ++;
    }
    return m_nSignQ;
}

void BISignIcon::UpdateBuffers(int32_t nQ)
{
    const auto nBackSquareQ = nQ;
    const auto nSignStateSquareQ = nQ * 2;
    const auto nSignStarSquareQ = nQ;
    const auto nSignFaceSquareQ = nQ;

    const auto nMaxSignQ =
        BIUtils::GetMaxFromFourLong(nBackSquareQ, nSignStateSquareQ, nSignStarSquareQ, nSignFaceSquareQ);
    if (m_nMaxSquareQ != nMaxSignQ)
    {
        m_nMaxSquareQ = nMaxSignQ;
        INDEX_BUFFER_RELEASE(m_pRS, m_nIBufID);
        m_nIBufID = m_pRS->CreateIndexBuffer(m_nMaxSquareQ * 6 * sizeof(uint16_t));
        FillIndexBuffer();
    }

    if ((nBackSquareQ + nSignStateSquareQ + nSignStarSquareQ + nSignFaceSquareQ) !=
        (m_nBackSquareQ + m_nSignStateSquareQ + m_nSignStarSquareQ + m_nSignFaceSquareQ))
    {
        m_nBackSquareQ = nBackSquareQ;
        m_nSignStateSquareQ = nSignStateSquareQ;
        m_nSignStarSquareQ = nSignStarSquareQ;
        m_nSignFaceSquareQ = nSignFaceSquareQ;
        VERTEX_BUFFER_RELEASE(m_pRS, m_nVBufID);
        m_nVBufID =
            m_pRS->CreateVertexBuffer(BI_COLOR_VERTEX_FORMAT,
                                      (m_nBackSquareQ + m_nSignStateSquareQ + m_nSignStarSquareQ + m_nSignFaceSquareQ) *
                                          4 * sizeof(BI_COLOR_VERTEX),
                                      D3DUSAGE_WRITEONLY);
    }
    FillVertexBuffer();
}

void BISignIcon::FillIndexBuffer() const
{
    if (m_nIBufID < 0)
        return;
    auto *pI = static_cast<uint16_t *>(m_pRS->LockIndexBuffer(m_nIBufID));
    if (pI)
    {
        for (int32_t n = 0; n < m_nMaxSquareQ; n++)
        {
            pI[n * 6 + 0] = static_cast<uint16_t>(n * 4 + 0);
            pI[n * 6 + 1] = static_cast<uint16_t>(n * 4 + 1);
            pI[n * 6 + 2] = static_cast<uint16_t>(n * 4 + 2);

            pI[n * 6 + 3] = static_cast<uint16_t>(n * 4 + 2);
            pI[n * 6 + 4] = static_cast<uint16_t>(n * 4 + 1);
            pI[n * 6 + 5] = static_cast<uint16_t>(n * 4 + 3);
        }
        m_pRS->UnLockIndexBuffer(m_nIBufID);
    }
}

void BISignIcon::FillVertexBuffer()
{
    int32_t n;
    if (m_nVBufID < 0)
        return;
    auto *pV = static_cast<BI_COLOR_VERTEX *>(m_pRS->LockVertexBuffer(m_nVBufID));
    if (pV)
    {
        int32_t vn = 0;

        for (n = 0; n < m_nSignFaceSquareQ; n++)
        {
            vn += WriteSquareToVBuff(&pV[vn], m_Sign[n].rFaceUV, m_dwSignFaceColor,
                                     m_Sign[n].pntPos + m_pntSignFaceOffset, m_pntSignFaceIconSize);
        }

        for (n = 0; n < m_nSignQ; n++)
            vn += WriteSquareToVBuff(&pV[vn], m_rBackUV, m_dwBackColor, m_Sign[n].pntPos + m_pntBackOffset,
                                     m_pntBackIconSize);

        for (n = 0; n < m_nSignQ; n++)
        {
            vn += WriteSquareToVBuffWithProgress(&pV[vn], m_rSignStateLeftUV, m_dwSignStateColor,
                                                 m_Sign[n].pntPos + m_pntSignStateLeftOffset,
                                                 m_pntSignStateLeftIconSize, 1.f - m_Sign[n].fLeftState, 0.f, 0.f, 0.f);
            vn += WriteSquareToVBuffWithProgress(
                &pV[vn], m_rSignStateRightUV, m_dwSignStateColor, m_Sign[n].pntPos + m_pntSignStateRightOffset,
                m_pntSignStateRightIconSize, 1.f - m_Sign[n].fRightState, 0.f, 0.f, 0.f);
        }

        for (n = 0; n < m_nSignQ; n++)
            vn += WriteSquareToVBuffWithProgress(&pV[vn], m_rSignStarUV, m_dwSignStarColor,
                                                 m_Sign[n].pntPos + m_pntSignStarOffset, m_pntSignStarIconSize, 0.f,
                                                 0.f, 0.f, 1.f - m_Sign[n].fStarProgress);

        m_pRS->UnLockVertexBuffer(m_nVBufID);
    }
}

int32_t BISignIcon::WriteSquareToVBuff(BI_COLOR_VERTEX *pv, const FRECT &uv, uint32_t color, const BIFPOINT &center,
                                    const FPOINT &size)
{
    if (!pv)
        return 0;

    const auto fLeft = static_cast<float>(center.x - size.x / 2);
    const auto fTop = static_cast<float>(center.y - size.y / 2);
    const auto fRight = fLeft + size.x;
    const auto fBottom = fTop + size.y;

    pv[0].pos.x = fLeft;
    pv[0].pos.y = fTop;
    pv[0].pos.z = 1.f;
    pv[0].w = 0.5f;
    pv[0].col = color;
    pv[0].tu = uv.left;
    pv[0].tv = uv.top;

    pv[1].pos.x = fLeft;
    pv[1].pos.y = fBottom;
    pv[1].pos.z = 1.f;
    pv[1].w = 0.5f;
    pv[1].col = color;
    pv[1].tu = uv.left;
    pv[1].tv = uv.bottom;

    pv[2].pos.x = fRight;
    pv[2].pos.y = fTop;
    pv[2].pos.z = 1.f;
    pv[2].w = 0.5f;
    pv[2].col = color;
    pv[2].tu = uv.right;
    pv[2].tv = uv.top;

    pv[3].pos.x = fRight;
    pv[3].pos.y = fBottom;
    pv[3].pos.z = 1.f;
    pv[3].w = 0.5f;
    pv[3].col = color;
    pv[3].tu = uv.right;
    pv[3].tv = uv.bottom;

    return 4;
}

int32_t BISignIcon::WriteSquareToVBuffWithProgress(BI_COLOR_VERTEX *pv, const FRECT &uv, uint32_t color,
                                                const BIFPOINT &center, const FPOINT &size, float fClampUp,
                                                float fClampDown, float fClampLeft, float fClampRight)
{
    if (!pv)
        return 0;

    auto fLeft = static_cast<float>(center.x - size.x / 2);
    auto fTop = static_cast<float>(center.y - size.y / 2);
    auto fRight = fLeft + size.x;
    auto fBottom = fTop + size.y;

    fLeft += size.x * fClampLeft;
    fRight -= size.x * fClampRight;
    fTop += size.y * fClampUp;
    fBottom += size.y * fClampDown;

    const auto fLeftUV = uv.left + (uv.right - uv.left) * fClampLeft;
    const auto fRightUV = uv.right - (uv.right - uv.left) * fClampRight;
    const auto fTopUV = uv.top + (uv.bottom - uv.top) * fClampUp;
    const auto fBottomUV = uv.bottom - (uv.bottom - uv.top) * fClampDown;

    pv[0].pos.x = fLeft;
    pv[0].pos.y = fTop;
    pv[0].pos.z = 1.f;
    pv[0].w = 0.5f;
    pv[0].col = color;
    pv[0].tu = fLeftUV;
    pv[0].tv = fTopUV;

    pv[1].pos.x = fLeft;
    pv[1].pos.y = fBottom;
    pv[1].pos.z = 1.f;
    pv[1].w = 0.5f;
    pv[1].col = color;
    pv[1].tu = fLeftUV;
    pv[1].tv = fBottomUV;

    pv[2].pos.x = fRight;
    pv[2].pos.y = fTop;
    pv[2].pos.z = 1.f;
    pv[2].w = 0.5f;
    pv[2].col = color;
    pv[2].tu = fRightUV;
    pv[2].tv = fTopUV;

    pv[3].pos.x = fRight;
    pv[3].pos.y = fBottom;
    pv[3].pos.z = 1.f;
    pv[3].w = 0.5f;
    pv[3].col = color;
    pv[3].tu = fRightUV;
    pv[3].tv = fBottomUV;

    return 4;
}
