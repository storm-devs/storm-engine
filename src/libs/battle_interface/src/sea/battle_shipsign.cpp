#include "battle_shipsign.h"

#include "core.h"

#include "../Utils.h"
#include "battle_shipcommand.h"
#include "controls.h"
#include "shared/battle_interface/msg_control.h"

BIShipIcon::BIShipIcon(entid_t BIEntityID, VDX9RENDER *pRS)
{
    Assert(pRS);

    m_idHostEntity = BIEntityID;
    m_pRS = pRS;
    m_pCommandList = nullptr;

    m_nVBufID = -1;
    m_nIBufID = -1;
    m_nBackTextureID = -1;
    m_nShipTextureID = -1;
    m_nShipStateTextureID = -1;
    m_nShipClassTextureID = -1;
    m_nBackSquareQ = 0;
    m_nShipSquareQ = 0;
    m_nShipStateSquareQ = 0;
    m_nShipClassSquareQ = 0;

    m_nMaxSquareQ = 0;

    m_bMakeUpdate = true;
    m_nCommandMode = 0;

    m_nCommandListVerticalOffset = -48;

    m_idSailorFont = -1;
    m_idShipNameFont = -1;

    m_nCurrentShipIndex = 0;
}

BIShipIcon::~BIShipIcon()
{
    Release();
}

void BIShipIcon::Draw()
{
    if (m_bMakeUpdate)
    {
        m_bMakeUpdate = false;
        Recollect();

        UpdateCommandList();
        if (m_pCommandList)
            m_pCommandList->SetUpDown(m_nCurrentShipIndex > 0, m_nCurrentShipIndex < m_nShipQuantity - 1);
        if (!IsActive())
            core.Event("evntBISelectShip", "ll", -1, true);
    }

    FillVertexBuffer();

    if (m_nVBufID != -1 && m_nIBufID != -1)
    {
        long nStartV = 0;
        const long nStartI = 0;

        // ship
        if (m_nShipSquareQ > 0)
        {
            m_pRS->TextureSet(0, m_nShipTextureID);
            m_pRS->DrawBuffer(m_nVBufID, sizeof(BI_COLOR_VERTEX), m_nIBufID, nStartV, m_nShipSquareQ * 4, nStartI,
                              m_nShipSquareQ * 2, "battle_colorRectangle");
        }
        nStartV += m_nShipSquareQ * 4;

        // back
        if (m_nBackSquareQ > 0)
        {
            m_pRS->TextureSet(0, m_nBackTextureID);
            m_pRS->DrawBuffer(m_nVBufID, sizeof(BI_COLOR_VERTEX), m_nIBufID, nStartV, m_nBackSquareQ * 4, nStartI,
                              m_nBackSquareQ * 2, "battle_colorRectangle");
        }
        nStartV += m_nBackSquareQ * 4;

        // ship state (hp & sp)
        if (m_nShipStateSquareQ > 0)
        {
            m_pRS->TextureSet(0, m_nShipStateTextureID);
            m_pRS->DrawBuffer(m_nVBufID, sizeof(BI_COLOR_VERTEX), m_nIBufID, nStartV, m_nShipStateSquareQ * 4, nStartI,
                              m_nShipStateSquareQ * 2, "battle_colorRectangle");
        }
        nStartV += m_nShipStateSquareQ * 4;

        // ship class
        if (m_nShipClassSquareQ > 0)
        {
            m_pRS->TextureSet(0, m_nShipClassTextureID);
            m_pRS->DrawBuffer(m_nVBufID, sizeof(BI_COLOR_VERTEX), m_nIBufID, nStartV, m_nShipClassSquareQ * 4, nStartI,
                              m_nShipClassSquareQ * 2, "battle_alphacutcolor");
        }
        nStartV += m_nShipClassSquareQ * 4;
    }

    for (long n = 0; n < m_nShipQuantity; n++)
    {
        if (m_Ship[n].pASailorQuantity)
        {
            m_pRS->ExtPrint(m_idSailorFont, m_dwSailorFontColor, 0, PR_ALIGN_CENTER, true, m_fSailorFontScale, 0, 0,
                            // shadow
                            static_cast<long>(m_Ship[n].pntPos.x) + m_SailorFontOffset.x,
                            static_cast<long>(m_Ship[n].pntPos.y) + m_SailorFontOffset.y, "%d",
                            static_cast<long>(atof(m_Ship[n].pASailorQuantity->GetThisAttr())));
        }
        if (!m_Ship[n].sShipName.empty())
        {
            m_pRS->ExtPrint(m_idShipNameFont, m_dwShipNameFontColor, 0, PR_ALIGN_CENTER, true, m_fShipNameFontScale, 0,
                            0, // font shadow
                            static_cast<long>(m_Ship[n].pntPos.x) + m_ShipNameFontOffset.x,
                            static_cast<long>(m_Ship[n].pntPos.y) + m_ShipNameFontOffset.y, "%s",
                            m_Ship[n].sShipName.c_str());
        }
    }

    if (m_pCommandList)
        m_pCommandList->Draw();
}

void BIShipIcon::Init(ATTRIBUTES *pRoot, ATTRIBUTES *pA)
{
    long n;
    char *pcTmp;
    char param[256];

    m_pARoot = pRoot;

    m_pCommandList = new BIShipCommandList(m_idHostEntity, pRoot, m_pRS);

    // default value
    m_nBackTextureID = -1;
    m_dwBackColor = ARGB(255, 128, 128, 128);
    FULLRECT(m_rBackUV);
    ZERROPOINT(m_pntBackOffset);
    FILLPOINT(m_pntBackIconSize, 128, 128);

    m_nShipStateTextureID = -1;
    m_dwShipStateColor = ARGB(255, 128, 128, 128);
    FULLRECT(m_rShipHPUV);
    ZERROPOINT(m_pntShipHPOffset);
    FILLPOINT(m_pntShipHPIconSize, 128, 128);
    FULLRECT(m_rShipSPUV);
    ZERROPOINT(m_pntShipSPOffset);
    FILLPOINT(m_pntShipSPIconSize, 128, 128);

    m_nShipClassTextureID = -1;
    m_dwShipClassColor = ARGB(255, 128, 128, 128);
    FULLRECT(m_rShipClassUV);
    ZERROPOINT(m_pntShipClassOffset);
    FILLPOINT(m_pntShipClassIconSize, 128, 128);

    m_nShipTextureID = -1;
    m_dwShipColor = ARGB(255, 128, 128, 128);
    // FULLRECT( m_rShipUV );
    ZERROPOINT(m_pntShipOffset);
    FILLPOINT(m_pntShipIconSize, 128, 128);

    m_idSailorFont = -1;
    m_dwSailorFontColor = ARGB(255, 64, 64, 64);
    m_fSailorFontScale = 1.f;
    m_SailorFontOffset.x = -14;
    m_SailorFontOffset.y = 18;

    m_idShipNameFont = -1;
    m_dwShipNameFontColor = ARGB(255, 64, 64, 64);
    m_fShipNameFontScale = 1.f;
    m_ShipNameFontOffset.x = -14;
    m_ShipNameFontOffset.y = 40;

    m_nCommandListVerticalOffset = -48;

    for (n = 0; n < MAX_SHIP_QUANTITY; n++)
    {
        m_Ship[n].pntPos.x = 20.f;
        m_Ship[n].pntPos.y = 20.f + (m_pntBackIconSize.y + 10.f) * n;
    }

    if (pA)
    {
        pcTmp = pA->GetAttribute("sailorfontid");
        if (pcTmp)
            m_idSailorFont = m_pRS->LoadFont(pcTmp);
        m_dwSailorFontColor = pA->GetAttributeAsDword("sailorfontcolor", m_dwSailorFontColor);
        m_fSailorFontScale = pA->GetAttributeAsFloat("sailorfontscale", m_fSailorFontScale);

        // ugeen 150920
        pcTmp = pA->GetAttribute("sailorfontoffset");
        if (pcTmp)
            sscanf(pcTmp, "%ld,%ld", &m_SailorFontOffset.x, &m_SailorFontOffset.y);

        pcTmp = pA->GetAttribute("shipnamefontid");
        if (pcTmp)
            m_idShipNameFont = m_pRS->LoadFont(pcTmp);
        m_dwShipNameFontColor = pA->GetAttributeAsDword("shipnamefontcolor", m_dwShipNameFontColor);
        m_fShipNameFontScale = pA->GetAttributeAsFloat("shipnamefontscale", m_fShipNameFontScale);

        pcTmp = pA->GetAttribute("shipnamefontoffset");
        if (pcTmp)
            sscanf(pcTmp, "%ld,%ld", &m_ShipNameFontOffset.x, &m_ShipNameFontOffset.y);

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
            m_nShipStateTextureID = m_pRS->TextureCreate(pcTmp);
        m_dwShipStateColor = pA->GetAttributeAsDword("shipstatecolor", m_dwShipStateColor);
        pcTmp = pA->GetAttribute("shiphpuv");
        if (pcTmp)
            sscanf(pcTmp, "%f,%f,%f,%f", &m_rShipHPUV.left, &m_rShipHPUV.top, &m_rShipHPUV.right, &m_rShipHPUV.bottom);
        pcTmp = pA->GetAttribute("shiphpoffset");
        if (pcTmp)
            sscanf(pcTmp, "%f,%f", &m_pntShipHPOffset.x, &m_pntShipHPOffset.y);
        pcTmp = pA->GetAttribute("shiphpiconsize");
        if (pcTmp)
            sscanf(pcTmp, "%f,%f", &m_pntShipHPIconSize.x, &m_pntShipHPIconSize.y);
        pcTmp = pA->GetAttribute("shipspuv");
        if (pcTmp)
            sscanf(pcTmp, "%f,%f,%f,%f", &m_rShipSPUV.left, &m_rShipSPUV.top, &m_rShipSPUV.right, &m_rShipSPUV.bottom);
        pcTmp = pA->GetAttribute("shipspoffset");
        if (pcTmp)
            sscanf(pcTmp, "%f,%f", &m_pntShipSPOffset.x, &m_pntShipSPOffset.y);
        pcTmp = pA->GetAttribute("shipspiconsize");
        if (pcTmp)
            sscanf(pcTmp, "%f,%f", &m_pntShipSPIconSize.x, &m_pntShipSPIconSize.y);

        pcTmp = pA->GetAttribute("shipclasstexturename");
        if (pcTmp)
            m_nShipClassTextureID = m_pRS->TextureCreate(pcTmp);
        m_dwShipClassColor = pA->GetAttributeAsDword("shipclasscolor", m_dwShipClassColor);
        pcTmp = pA->GetAttribute("shipclassuv");
        if (pcTmp)
            sscanf(pcTmp, "%f,%f,%f,%f", &m_rShipClassUV.left, &m_rShipClassUV.top, &m_rShipClassUV.right,
                   &m_rShipClassUV.bottom);
        pcTmp = pA->GetAttribute("shipclassoffset");
        if (pcTmp)
            sscanf(pcTmp, "%f,%f", &m_pntShipClassOffset.x, &m_pntShipClassOffset.y);
        pcTmp = pA->GetAttribute("shipclassiconsize");
        if (pcTmp)
            sscanf(pcTmp, "%f,%f", &m_pntShipClassIconSize.x, &m_pntShipClassIconSize.y);
        pcTmp = pA->GetAttribute("gunchargeprogress");
        if (pcTmp)
        {
            do
            {
                m_aClassProgress.push_back(BIUtils::GetFromStr_Float((const char *&)pcTmp, 0.f));
            } while (pcTmp[0]);
        }

        m_nCommandListVerticalOffset = pA->GetAttributeAsDword("commandlistverticaloffset");

        pcTmp = pA->GetAttribute("shiptexturename");
        if (pcTmp)
            m_nShipTextureID = m_pRS->TextureCreate(pcTmp);
        m_dwShipColor = pA->GetAttributeAsDword("shipcolor", m_dwShipColor);
        /*pcTmp = pA->GetAttribute( "shipuv" );
        if( pcTmp ) sscanf( pcTmp, "%f,%f,%f,%f", &m_rShipClassUV.left,&m_rShipClassUV.top,
        &m_rShipClassUV.right,&m_rShipClassUV.bottom );*/
        pcTmp = pA->GetAttribute("shipoffset");
        if (pcTmp)
            sscanf(pcTmp, "%f,%f", &m_pntShipOffset.x, &m_pntShipOffset.y);
        pcTmp = pA->GetAttribute("shipiconsize");
        if (pcTmp)
            sscanf(pcTmp, "%f,%f", &m_pntShipIconSize.x, &m_pntShipIconSize.y);

        for (n = 0; n < MAX_SHIP_QUANTITY; n++)
        {
            sprintf_s(param, sizeof(param), "iconoffset%d", n + 1);
            pcTmp = pA->GetAttribute(param);
            if (pcTmp)
                sscanf(pcTmp, "%f,%f", &m_Ship[n].pntPos.x, &m_Ship[n].pntPos.y);
        }
    }

    m_bMakeUpdate = true;
}

size_t BIShipIcon::AddTexture(const char *pcTextureName, long nCols, long nRows) const
{
    if (m_pCommandList)
        return m_pCommandList->AddTexture(pcTextureName, nCols, nRows);
    return -1;
}

void BIShipIcon::Recollect()
{
    const auto n = CalculateShipQuantity();
    UpdateBuffers(n);
}

bool BIShipIcon::IsActive() const
{
    if (!m_pCommandList)
        return false;
    return m_pCommandList->GetActive();
}

void BIShipIcon::SetActive(bool bActive)
{
    const auto bSameActive = (IsActive() == bActive);
    if (m_pCommandList)
        m_pCommandList->SetActive(bActive);
    if (bSameActive)
        return;
    m_nCommandMode = BI_COMMODE_COMMAND_SELECT;
    m_bMakeUpdate = true;
}

void BIShipIcon::MakeControl()
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

void BIShipIcon::ExecuteCommand(CommandType command)
{
    switch (command)
    {
    case Command_confirm:
        if (m_pCommandList)
        {
            const auto nTmp = m_pCommandList->ExecuteConfirm();
            if (nTmp != -1)
                m_nCommandMode = nTmp;
        }
        if (m_nCommandMode == 0)
            SetActive(false);
        break;

    case Command_left:
        if (m_pCommandList)
            m_pCommandList->ExecuteLeft();
        break;

    case Command_right:
        if (m_pCommandList)
            m_pCommandList->ExecuteRight();
        break;

    case Command_up:
        if (m_nCurrentShipIndex > 0)
        {
            m_nCurrentShipIndex--;
            m_nCommandMode = BI_COMMODE_COMMAND_SELECT;
            UpdateCommandList();
            if (m_pCommandList)
                m_pCommandList->SetUpDown(m_nCurrentShipIndex > 0, m_nCurrentShipIndex < m_nShipQuantity - 1);
        }
        break;

    case Command_down:
        if (m_nCurrentShipIndex < m_nShipQuantity - 1)
        {
            m_nCurrentShipIndex++;
            m_nCommandMode = BI_COMMODE_COMMAND_SELECT;
            UpdateCommandList();
            if (m_pCommandList)
                m_pCommandList->SetUpDown(m_nCurrentShipIndex > 0, m_nCurrentShipIndex < m_nShipQuantity - 1);
        }
        break;

    case Command_cancel:
        m_nCommandMode = m_pCommandList->ExecuteCancel();
        if (m_nCommandMode == 0)
            SetActive(false);
        else
            UpdateCommandList();
        break;

    default:
        core.Trace("Warning! Unknown executing command: %d", command);
    }
}

void BIShipIcon::Release()
{
    SetActive(false); // disable control

    STORM_DELETE(m_pCommandList);
    TEXTURE_RELEASE(m_pRS, m_nBackTextureID);
    TEXTURE_RELEASE(m_pRS, m_nShipTextureID);
    TEXTURE_RELEASE(m_pRS, m_nShipStateTextureID);
    TEXTURE_RELEASE(m_pRS, m_nShipClassTextureID);
    VERTEX_BUFFER_RELEASE(m_pRS, m_nVBufID);
    INDEX_BUFFER_RELEASE(m_pRS, m_nIBufID);
    FONT_RELEASE(m_pRS, m_idSailorFont);

    m_nMaxSquareQ = 0;
    m_nBackSquareQ = 0;
    m_nShipSquareQ = 0;
    m_nShipStateSquareQ = 0;
    m_nShipClassSquareQ = 0;
}

long BIShipIcon::CalculateShipQuantity()
{
    long n;
    SHIP_DESCRIBE_LIST::SHIP_DESCR *pSD;

    // resetting all ships
    m_nShipQuantity = 0;
    for (n = 0; n < MAX_SHIP_QUANTITY; n++)
    {
        m_Ship[n].nCharacterIndex = -1;
        m_Ship[n].pASailorQuantity = nullptr;
        m_Ship[n].pAShip = nullptr;
        m_Ship[n].nMaxHP = 100;
        m_Ship[n].nMaxSP = 100;
        m_Ship[n].sShipName.clear();
    }

    // take the ship of the main character
    pSD = g_ShipList.GetMainCharacterShip();
    if (pSD)
    {
        m_Ship[0].nCharacterIndex = pSD->characterIndex;
        m_Ship[0].pASailorQuantity = GetSailorQuantityAttribute(pSD);
        m_Ship[0].pAShip = pSD->pAttr;
        m_Ship[0].nMaxHP = pSD->maxHP;
        m_Ship[0].nMaxSP = pSD->maxSP;
        m_Ship[0].nShipClass = GetShipClass(m_Ship[0].nCharacterIndex);
        GetShipUVFromPictureIndex(pSD->pictureNum, m_Ship[0].rUV);
        m_Ship[0].sShipName = pSD->pAttr ? pSD->pAttr->GetAttribute("name") : "noname";
        m_nShipQuantity++;
    }

    // take the following "our" ships
    for (pSD = g_ShipList.GetShipRoot(); pSD; pSD = pSD->next)
    {
        if (m_Ship[0].nCharacterIndex == pSD->characterIndex)
            continue; // the main character is already on the list
        if (pSD->isMyShip)
        {
            m_Ship[m_nShipQuantity].nCharacterIndex = pSD->characterIndex;
            m_Ship[m_nShipQuantity].pASailorQuantity = GetSailorQuantityAttribute(pSD);
            m_Ship[m_nShipQuantity].pAShip = pSD->pAttr;
            m_Ship[m_nShipQuantity].nMaxHP = pSD->maxHP;
            m_Ship[m_nShipQuantity].nMaxSP = pSD->maxSP;
            m_Ship[m_nShipQuantity].nShipClass = GetShipClass(m_Ship[m_nShipQuantity].nCharacterIndex);
            GetShipUVFromPictureIndex(pSD->pictureNum, m_Ship[m_nShipQuantity].rUV);
            m_Ship[m_nShipQuantity].sShipName = pSD->pAttr ? pSD->pAttr->GetAttribute("name") : "noname";
            m_nShipQuantity++;
        }
    }

    return m_nShipQuantity;
}

void BIShipIcon::UpdateBuffers(long nShipQ)
{
    const auto nBackSquareQ = nShipQ;
    const auto nShipStateSquareQ = nShipQ * 2;
    const long nShipClassSquareQ = nShipQ;
    const long nShipSquareQ = nShipQ;

    const long nMaxSquareQ =
        BIUtils::GetMaxFromFourLong(nBackSquareQ, nShipStateSquareQ, nShipClassSquareQ, nShipSquareQ);
    if (m_nMaxSquareQ != nMaxSquareQ)
    {
        m_nMaxSquareQ = nMaxSquareQ;
        INDEX_BUFFER_RELEASE(m_pRS, m_nIBufID);
        m_nIBufID = m_pRS->CreateIndexBuffer(m_nMaxSquareQ * 6 * sizeof(uint16_t));
        FillIndexBuffer();
    }

    if ((nBackSquareQ + nShipStateSquareQ + nShipClassSquareQ + nShipSquareQ) !=
        (m_nBackSquareQ + m_nShipStateSquareQ + m_nShipClassSquareQ + m_nShipSquareQ))
    {
        m_nBackSquareQ = nBackSquareQ;
        m_nShipStateSquareQ = nShipStateSquareQ;
        m_nShipClassSquareQ = nShipClassSquareQ;
        m_nShipSquareQ = nShipSquareQ;
        VERTEX_BUFFER_RELEASE(m_pRS, m_nVBufID);
        m_nVBufID = m_pRS->CreateVertexBuffer(
            BI_COLOR_VERTEX_FORMAT,
            (m_nBackSquareQ + m_nShipStateSquareQ + m_nShipClassSquareQ + m_nShipSquareQ) * 4 * sizeof(BI_COLOR_VERTEX),
            D3DUSAGE_WRITEONLY);
    }
    FillVertexBuffer();
}

void BIShipIcon::FillIndexBuffer() const
{
    if (m_nIBufID < 0)
        return;
    auto *pI = static_cast<uint16_t *>(m_pRS->LockIndexBuffer(m_nIBufID));
    if (pI)
    {
        for (long n = 0; n < m_nMaxSquareQ; n++)
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

void BIShipIcon::FillVertexBuffer()
{
    long n;
    if (m_nVBufID < 0)
        return;
    auto *pV = static_cast<BI_COLOR_VERTEX *>(m_pRS->LockVertexBuffer(m_nVBufID));
    if (pV)
    {
        long vn = 0;

        // m_nShipQuantity = 0;
        for (n = 0; n < m_nShipQuantity; n++)
        {
            vn += WriteSquareToVBuff(&pV[vn], m_Ship[n].rUV, m_dwShipColor, m_Ship[n].pntPos + m_pntShipOffset,
                                     m_pntShipIconSize);
        }

        for (n = 0; n < m_nShipQuantity; n++)
            vn += WriteSquareToVBuff(&pV[vn], m_rBackUV, m_dwBackColor, m_Ship[n].pntPos + m_pntBackOffset,
                                     m_pntBackIconSize);

        for (n = 0; n < m_nShipQuantity; n++)
        {
            vn += WriteSquareToVBuffWithProgress(&pV[vn], m_rShipHPUV, m_dwShipStateColor,
                                                 m_Ship[n].pntPos + m_pntShipHPOffset, m_pntShipHPIconSize,
                                                 GetProgressShipHP(n), 0.f, 0.f, 0.f);
            vn += WriteSquareToVBuffWithProgress(&pV[vn], m_rShipSPUV, m_dwShipStateColor,
                                                 m_Ship[n].pntPos + m_pntShipSPOffset, m_pntShipSPIconSize,
                                                 GetProgressShipSP(n), 0.f, 0.f, 0.f);
        }

        for (n = 0; n < m_nShipQuantity; n++)
            vn += WriteSquareToVBuffWithProgress(&pV[vn], m_rShipClassUV, m_dwShipClassColor,
                                                 m_Ship[n].pntPos + m_pntShipClassOffset, m_pntShipClassIconSize, 0.f,
                                                 0.f, 0.f, 1.f - GetProgressShipClass(n));
        // vn += WriteSquareToVBuff( &pV[vn], m_rShipClassUV, m_dwShipClassColor, m_Ship[n].pntPos+m_pntShipClassOffset,
        // m_pntShipClassIconSize );

        m_pRS->UnLockVertexBuffer(m_nVBufID);
    }
}

long BIShipIcon::WriteSquareToVBuff(BI_COLOR_VERTEX *pv, const FRECT &uv, uint32_t color, const BIFPOINT &center,
                                    const FPOINT &size)
{
    if (!pv)
        return 0;

    const auto fLeft = static_cast<float>(center.x - size.x / 2);
    const auto fTop = static_cast<float>(center.y - size.y / 2);
    const float fRight = fLeft + size.x;
    const float fBottom = fTop + size.y;

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

long BIShipIcon::WriteSquareToVBuffWithProgress(BI_COLOR_VERTEX *pv, const FRECT &uv, uint32_t color,
                                                const BIFPOINT &center, const FPOINT &size, float fClampUp,
                                                float fClampDown, float fClampLeft, float fClampRight)
{
    if (!pv)
        return 0;

    auto fLeft = static_cast<float>(center.x - size.x / 2);
    auto fTop = static_cast<float>(center.y - size.y / 2);
    float fRight = fLeft + size.x;
    float fBottom = fTop + size.y;

    fLeft += size.x * fClampLeft;
    fRight -= size.x * fClampRight;
    fTop += size.y * fClampUp;
    fBottom += size.y * fClampDown;

    const float fLeftUV = uv.left + (uv.right - uv.left) * fClampLeft;
    const float fRightUV = uv.right - (uv.right - uv.left) * fClampRight;
    const float fTopUV = uv.top + (uv.bottom - uv.top) * fClampUp;
    const float fBottomUV = uv.bottom - (uv.bottom - uv.top) * fClampDown;

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

void BIShipIcon::UpdateCommandList()
{
    if (m_pCommandList)
        m_pCommandList->Update(GetCurrentCommandTopLine(), GetCurrentCommandCharacterIndex(), GetCurrentCommandMode());
    else
        core.Event("evntBISelectShip", "ll", -1, true);
}

long BIShipIcon::GetCurrentCommandTopLine()
{
    long n = m_nCurrentShipIndex;
    if (n < 0 || n >= m_nShipQuantity)
        n = 0;
    m_nCurrentShipIndex = n;
    return static_cast<long>(m_Ship[n].pntPos.y) + m_nCommandListVerticalOffset;
}

long BIShipIcon::GetCurrentCommandCharacterIndex()
{
    long n = m_nCurrentShipIndex;
    if (n < 0 || n >= m_nShipQuantity)
        n = 0;
    m_nCurrentShipIndex = n;
    return static_cast<long>(m_Ship[n].nCharacterIndex);
}

long BIShipIcon::GetCurrentCommandMode() const
{
    return m_nCommandMode;
}

ATTRIBUTES *BIShipIcon::GetSailorQuantityAttribute(SHIP_DESCRIBE_LIST::SHIP_DESCR *pSD)
{
    if (!pSD || !pSD->pAttr)
        return nullptr;
    ATTRIBUTES *pA = pSD->pAttr->GetAttributeClass("Crew");
    if (pA)
        pA = pA->GetAttributeClass("quantity");
    return pA;
}

float BIShipIcon::GetProgressShipHP(long nShipNum) const
{
    if (m_Ship[nShipNum].nMaxHP <= 0.f && !m_Ship[nShipNum].pAShip)
        return 0.f;
    float f = m_Ship[nShipNum].pAShip->GetAttributeAsFloat("HP", 0.f) / m_Ship[nShipNum].nMaxHP;
    if (f < 0.f)
        f = 0.f;
    if (f > 1.f)
        f = 1.f;
    return 1.f - f;
}

float BIShipIcon::GetProgressShipSP(long nShipNum)
{
    if (m_Ship[nShipNum].nMaxSP <= 0.f && !m_Ship[nShipNum].pAShip)
        return 0.f;
    float f = m_Ship[nShipNum].pAShip->GetAttributeAsFloat("SP", 0.f) / m_Ship[nShipNum].nMaxSP;
    if (f < 0.f)
        f = 0.f;
    if (f > 1.f)
        f = 1.f;
    return 1.f - f;
}

float BIShipIcon::GetProgressShipClass(long nShipNum)
{
    if (m_Ship[nShipNum].nShipClass < 0 || m_Ship[nShipNum].nShipClass >= m_aClassProgress.size())
        return 0.f;
    const float f = m_aClassProgress[m_Ship[nShipNum].nShipClass];
    return f;
}

void BIShipIcon::GetShipUVFromPictureIndex(long nPicIndex, FRECT &rUV)
{
    // TODO: Make picture count configurable
    const float pictureCount = core.GetTargetEngineVersion() >= storm::ENGINE_VERSION::TO_EACH_HIS_OWN ? 16.f : 8.f;
    const float pictureWidth = 1.0f / pictureCount;
    const float pictureHeight = 1.0f / pictureCount;

    const float ny = std::floor(static_cast<float>(nPicIndex) / pictureCount);
    const float nx = static_cast<float>(nPicIndex) - ny * pictureCount;
    rUV.left = nx * pictureWidth;
    rUV.top = ny * pictureHeight; // boal
    rUV.right = rUV.left + pictureWidth;
    rUV.bottom = rUV.top + pictureHeight; // boal
}

long BIShipIcon::GetShipClass(long nCharIdx)
{
    VDATA *pVDat = core.Event("evntGetCharacterShipClass", "l", nCharIdx);
    if (!pVDat)
        return 1;
    return pVDat->GetLong();
}
