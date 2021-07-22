#include "ActivePerkShower.h"
#include "bi_defines.h"
#include "core.h"
#include "shared/battle_interface/msg_control.h"
#include "vmodule_api.h"
#include <exception>

ActivePerkShower::ActivePerkShower()
{
    rs = nullptr;

    m_idVBuf = -1;
    m_idIBuf = -1;

    m_nTextureQ = 0;
    m_pTexDescr = nullptr;

    m_nShowPlaceQ = 0;
    m_pShowPlaces = nullptr;

    m_nIShowQ = 0;
    m_pIconsList = nullptr;
}

ActivePerkShower::~ActivePerkShower()
{
    ReleaseAll();
}

bool ActivePerkShower::Init()
{
    if ((rs = static_cast<VDX9RENDER *>(core.CreateService("dx9render"))) == nullptr)
    {
        throw std::runtime_error("Can`t create render service");
    }

    if (AttributesPointer == nullptr)
        return false;

    if (!CreateTextures(AttributesPointer->GetAttributeClass("Textures")))
        return false;

    if (!CreateShowPlaces(AttributesPointer->GetAttributeClass("ShowParam")))
    {
        ReleaseAll();
        return false;
    }
    if (!InitIconsList(AttributesPointer->FindAClass(AttributesPointer, "PerkList.list")))
    {
        ReleaseAll();
        return false;
    }
    return true;
}

void ActivePerkShower::Execute(uint32_t delta_time)
{
}

void ActivePerkShower::Realize(uint32_t delta_time) const
{
    if (m_pTexDescr == nullptr)
        return;
    rs->MakePostProcess();

    for (auto i = 0; i < m_nTextureQ; i++)
    {
        if (m_pTexDescr[i].m_nPicsQ == 0)
            continue;
        rs->TextureSet(0, m_pTexDescr[i].m_idTexture);
        rs->DrawBuffer(m_idVBuf, sizeof(BI_ONETEXTURE_VERTEX), m_idIBuf, m_pTexDescr[i].m_nVertStart,
                       m_pTexDescr[i].m_nPicsQ * 4, m_pTexDescr[i].m_nIndxStart, m_pTexDescr[i].m_nPicsQ * 2,
                       "battle_rectangle");
    }
}

uint64_t ActivePerkShower::ProcessMessage(MESSAGE &message)
{
    switch (message.Long())
    {
    case MSG_ACTIVE_PERK_LIST_REFRESH: {
        const std::string &param = message.String();
        auto *const pA = message.AttributePointer();
        if (_stricmp(param.c_str(), "add") == 0)
            AddIconToList(pA);
        else if (_stricmp(param.c_str(), "del") == 0)
            DelIconFromList(pA);
    }
    break;
    case MSG_ACTIVE_PERK_ICON_REFRESH:
        RefreshShowPlaces(AttributesPointer->GetAttributeClass("ShowParam"));
        break;
    }
    return 0;
}

bool ActivePerkShower::CreateTextures(ATTRIBUTES *pATextureRoot)
{
    if (pATextureRoot == nullptr)
        return false;

    const size_t q = pATextureRoot->GetAttributesNum();
    if (q <= 0)
        return false;

    m_pTexDescr = new _TEXTURE_DESCR[q];
    if (m_pTexDescr == nullptr)
    {
        throw std::runtime_error("allocate memory error");
    }

    for (auto i = 0; i < q; i++)
    {
        auto *pA = pATextureRoot->GetAttributeClass(i);
        if (pA == nullptr)
        {
            m_pTexDescr[i].m_idTexture = -1;
            m_pTexDescr[i].m_nCol = 1;
            m_pTexDescr[i].m_nRow = 1;
        }
        else
        {
            m_pTexDescr[i].m_idTexture = rs->TextureCreate(pA->GetAttribute("file"));
            m_pTexDescr[i].m_nCol = pA->GetAttributeAsDword("horz", 1);
            m_pTexDescr[i].m_nRow = pA->GetAttributeAsDword("vert", 1);
        }
        m_pTexDescr[i].m_nPicsQ = 0;
        m_pTexDescr[i].m_nVertStart = 0;
        m_pTexDescr[i].m_nIndxStart = 0;
    }

    m_nTextureQ = q;
    return true;
}

bool ActivePerkShower::CreateShowPlaces(ATTRIBUTES *pAPlacesRoot)
{
    if (pAPlacesRoot == nullptr)
        return false;

    RefreshShowPlaces(pAPlacesRoot);

    return InitCommonBuffers();
}

void ActivePerkShower::RefreshShowPlaces(ATTRIBUTES *pAPlacesRoot)
{
    ATTRIBUTES *pAttr;

    if (m_pShowPlaces)
        STORM_DELETE(m_pShowPlaces);

    m_nIconWidth = 64;
    m_nIconHeight = 64;
    pAttr = pAPlacesRoot->GetAttributeClass("IconSize");
    if (pAttr != nullptr)
    {
        m_nIconWidth = pAttr->GetAttributeAsDword("horz", 64);
        m_nIconHeight = pAttr->GetAttributeAsDword("vert", 64);
    }

    m_nSpaceHorz = 4;
    m_nSpaceVert = 4;
    pAttr = pAPlacesRoot->GetAttributeClass("IconSpace");
    if (pAttr != nullptr)
    {
        m_nSpaceHorz = pAttr->GetAttributeAsDword("horz", 4);
        m_nSpaceVert = pAttr->GetAttributeAsDword("vert", 4);
    }

    RECT rectBound;
    rectBound.left = 488;
    rectBound.top = 192;
    rectBound.right = 624;
    rectBound.bottom = 464;
    pAttr = pAPlacesRoot->GetAttributeClass("PosRect");
    if (pAttr != nullptr)
    {
        rectBound.left = pAttr->GetAttributeAsDword("left", rectBound.left);
        rectBound.top = pAttr->GetAttributeAsDword("top", rectBound.top);
        rectBound.right = pAttr->GetAttributeAsDword("right", rectBound.right);
        rectBound.bottom = pAttr->GetAttributeAsDword("bottom", rectBound.bottom);
    }

    int nHorzQ = (rectBound.right - rectBound.left) / (m_nIconWidth + m_nSpaceHorz);
    int nVertQ = (rectBound.bottom - rectBound.top) / (m_nIconHeight + m_nSpaceVert);
    if (nHorzQ <= 0)
        nHorzQ = 1;
    if (nVertQ <= 0)
        nVertQ = 1;

    m_nShowPlaceQ = nHorzQ * nVertQ;
    m_pShowPlaces = new _SHOW_PLACE[m_nShowPlaceQ];
    if (m_pShowPlaces == nullptr)
    {
        throw std::runtime_error("allocate memory error");
    }

    for (auto ih = 0; ih < nHorzQ; ih++)
    {
        for (auto iv = 0; iv < nVertQ; iv++)
        {
            const auto idx = iv + ih * nVertQ;
            m_pShowPlaces[idx].right = static_cast<float>(rectBound.right - ih * (m_nIconWidth + m_nSpaceHorz));
            m_pShowPlaces[idx].left = static_cast<float>(m_pShowPlaces[idx].right - m_nIconWidth);
            m_pShowPlaces[idx].top = static_cast<float>(rectBound.top + iv * (m_nIconHeight + m_nSpaceVert));
            m_pShowPlaces[idx].bottom = static_cast<float>(m_pShowPlaces[idx].top + m_nIconHeight);
        }
    }
}

bool ActivePerkShower::InitIconsList(ATTRIBUTES *pAIconsRoot)
{
    if (pAIconsRoot == nullptr)
        return true;

    const size_t q = pAIconsRoot->GetAttributesNum();
    m_nIShowQ = q;
    if (m_nIShowQ == 0)
        return true;
    m_pIconsList = new _PICTURE_DESCR[q];
    if (m_pIconsList == nullptr)
    {
        throw std::runtime_error("allocate memory error");
    }
    for (auto i = 0; i < q; i++)
    {
        m_pIconsList[i].m_nPicNum = 0;
        m_pIconsList[i].m_nPicTexIdx = 0;
        auto *pA = pAIconsRoot->GetAttributeClass(i);
        if (pA != nullptr)
        {
            m_pIconsList[i].m_nPicNum = pA->GetAttributeAsDword("texture", 0);
            m_pIconsList[i].m_nPicTexIdx = pA->GetAttributeAsDword("pic_idx", 0);
        }
    }

    FillVIBuffers();
    return true;
}

void ActivePerkShower::AddIconToList(ATTRIBUTES *pAItemDescr)
{
    if (pAItemDescr == nullptr)
        return;
    const int picNum = pAItemDescr->GetAttributeAsDword("pic_idx");
    const int texNum = pAItemDescr->GetAttributeAsDword("texture");

    if (m_pIconsList != nullptr)
    {
        for (auto i = 0; i < m_nIShowQ; i++)
        {
            if (texNum == m_pIconsList[i].m_nPicTexIdx && picNum == m_pIconsList[i].m_nPicNum)
                return; // there is already such an ability
        }
    }

    m_nIShowQ++;
    if (m_pIconsList == nullptr)
    {
        m_pIconsList = new _PICTURE_DESCR[m_nIShowQ];
    }
    else
    {
        auto *const old_pIconsList = m_pIconsList;
        m_pIconsList = new _PICTURE_DESCR[m_nIShowQ];
        if (m_pIconsList != nullptr)
        {
            memcpy(m_pIconsList, old_pIconsList, sizeof(_PICTURE_DESCR) * (m_nIShowQ - 1));
        }
        delete old_pIconsList;
    }
    if (m_pIconsList == nullptr)
    {
        throw std::runtime_error("allocate memory error");
    }
    m_pIconsList[m_nIShowQ - 1].m_nPicTexIdx = texNum;
    m_pIconsList[m_nIShowQ - 1].m_nPicNum = picNum;

    FillVIBuffers();
}

void ActivePerkShower::DelIconFromList(ATTRIBUTES *pAIconDescr)
{
    if (pAIconDescr == nullptr)
        return;
    const int picNum = pAIconDescr->GetAttributeAsDword("pic_idx");
    const int texNum = pAIconDescr->GetAttributeAsDword("texture");

    auto del_idx = m_nIShowQ;
    for (auto i = 0; i < m_nIShowQ; i++)
    {
        if (i > del_idx)
        {
            m_pIconsList[i - 1].m_nPicTexIdx = m_pIconsList[i].m_nPicTexIdx;
            m_pIconsList[i - 1].m_nPicNum = m_pIconsList[i].m_nPicNum;
            continue;
        }
        if (texNum == m_pIconsList[i].m_nPicTexIdx && picNum == m_pIconsList[i].m_nPicNum)
            del_idx = i;
    }
    if (del_idx == m_nIShowQ)
        return;

    m_nIShowQ--;
    FillVIBuffers();
}

void ActivePerkShower::FillVIBuffers()
{
    int pi, ti, start_idx;

    auto *pvb = static_cast<BI_ONETEXTURE_VERTEX *>(rs->LockVertexBuffer(m_idVBuf));
    if (pvb == nullptr)
        return;

    start_idx = 0;
    for (ti = 0; ti < m_nTextureQ; ti++)
    {
        m_pTexDescr[ti].m_nPicsQ = 0;
        m_pTexDescr[ti].m_nVertStart = start_idx * 4;
        for (pi = 0; pi < m_nIShowQ && start_idx < m_nShowPlaceQ; pi++)
        {
            if (m_pIconsList[pi].m_nPicTexIdx != ti)
                continue;
            m_pTexDescr[ti].m_nPicsQ++;
            FillRectData(&pvb[start_idx * 4], m_pShowPlaces[pi], GetTextureRect(ti, m_pIconsList[pi].m_nPicNum));
            start_idx++;
        }
    }

    rs->UnLockVertexBuffer(m_idVBuf);
}

void ActivePerkShower::FillRectData(void *vbuf, const FRECT &rectPos, const FRECT &rectTex)
{
    if (vbuf == nullptr)
        return;
    auto *ptmp = static_cast<BI_ONETEXTURE_VERTEX *>(vbuf);
    ptmp[0].pos.x = rectPos.left;
    ptmp[0].pos.y = rectPos.top;
    ptmp[1].pos.x = rectPos.left;
    ptmp[1].pos.y = rectPos.bottom;
    ptmp[2].pos.x = rectPos.right;
    ptmp[2].pos.y = rectPos.top;
    ptmp[3].pos.x = rectPos.right;
    ptmp[3].pos.y = rectPos.bottom;

    ptmp[0].tu = rectTex.left;
    ptmp[0].tv = rectTex.top;
    ptmp[1].tu = rectTex.left;
    ptmp[1].tv = rectTex.bottom;
    ptmp[2].tu = rectTex.right;
    ptmp[2].tv = rectTex.top;
    ptmp[3].tu = rectTex.right;
    ptmp[3].tv = rectTex.bottom;
}

FRECT ActivePerkShower::GetTextureRect(int textIdx, int picIdx) const
{
    FRECT retRect;

    const int vIdx = picIdx / m_pTexDescr[textIdx].m_nCol;
    const int hIdx = picIdx - vIdx * m_pTexDescr[textIdx].m_nCol;

    retRect.left = static_cast<float>(hIdx) / m_pTexDescr[textIdx].m_nCol;
    retRect.top = static_cast<float>(vIdx) / m_pTexDescr[textIdx].m_nRow;
    retRect.right = static_cast<float>(hIdx + 1.f) / m_pTexDescr[textIdx].m_nCol;
    retRect.bottom = static_cast<float>(vIdx + 1.f) / m_pTexDescr[textIdx].m_nRow;

    return retRect;
}

bool ActivePerkShower::InitCommonBuffers()
{
    m_idVBuf = rs->CreateVertexBuffer(BI_ONETEX_VERTEX_FORMAT, m_nShowPlaceQ * 4 * sizeof(BI_ONETEXTURE_VERTEX),
                                      D3DUSAGE_WRITEONLY);
    m_idIBuf = rs->CreateIndexBuffer(m_nShowPlaceQ * 6 * 2);
    if (m_idIBuf == -1 || m_idVBuf == -1)
        return false;

    int i;
    auto *pibuf = static_cast<uint16_t *>(rs->LockIndexBuffer(m_idIBuf));
    for (i = 0; i < m_nShowPlaceQ; i++)
    {
        pibuf[i * 6 + 0] = i * 4 + 0;
        pibuf[i * 6 + 1] = i * 4 + 1;
        pibuf[i * 6 + 2] = i * 4 + 2;
        pibuf[i * 6 + 3] = i * 4 + 2;
        pibuf[i * 6 + 4] = i * 4 + 1;
        pibuf[i * 6 + 5] = i * 4 + 3;
    }
    rs->UnLockIndexBuffer(m_idIBuf);

    auto *pvbuf = static_cast<BI_ONETEXTURE_VERTEX *>(rs->LockVertexBuffer(m_idVBuf));
    for (i = 0; i < m_nShowPlaceQ * 4; i++)
    {
        pvbuf[i].pos.z = 1.f;
        pvbuf[i].w = .5f;
    }
    rs->UnLockVertexBuffer(m_idVBuf);

    return true;
}

void ActivePerkShower::ReleaseAll()
{
    int i;

    VERTEX_BUFFER_RELEASE(rs, m_idVBuf);
    INDEX_BUFFER_RELEASE(rs, m_idIBuf);

    for (i = 0; i < m_nTextureQ; i++)
        TEXTURE_RELEASE(rs, m_pTexDescr[i].m_idTexture);
    STORM_DELETE(m_pTexDescr);
    m_nTextureQ = 0;

    STORM_DELETE(m_pShowPlaces);
    m_nShowPlaceQ = 0;

    STORM_DELETE(m_pIconsList);
    m_nIShowQ = 0;
}
