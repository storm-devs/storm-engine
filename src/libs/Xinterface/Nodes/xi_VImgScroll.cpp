#include "xi_VImgScroll.h"

#include "core.h"

#define MAXIMAGEQUANTITY 100

IDirect3DTexture9 *GetTexFromEvent(VDATA *vdat);

CXI_VIMAGESCROLL::CXI_VIMAGESCROLL()
{
    m_bClickable = true;

    m_texBorder = -1;
    m_nCurImage = 0;
    m_nListSize = 0;
    m_Image = nullptr;
    m_pScroll = nullptr;
    m_bDoMove = false;
    m_sBorderGroupName = nullptr;

    m_nStringQuantity = 0;
    m_pStrParam = nullptr;

    m_nSpeedMul = 5;
    m_nNodeType = NODETYPE_VIMGSCROLL;

    m_nGroupQuantity = 0;
    m_sGroupName = nullptr;
    m_nGroupTex = nullptr;
    m_nShowOrder = 100;
    m_nNotUsedQuantity = 0;
    m_sSpecTechniqueName = nullptr;
    m_dwSpecTechniqueARGB = 0xFFFFFFFF;

    m_nSlotsQnt = 0;
    m_idBadTexture = nullptr;
    m_idBadPic = nullptr;
    m_pPicOffset = nullptr;
    m_dwNormalColor = nullptr;
    m_dwSelectColor = nullptr;
    m_dwCurColor = nullptr;

    m_leftTextLimit = 0;
    m_rightTextLimit = 0;
}

CXI_VIMAGESCROLL::~CXI_VIMAGESCROLL()
{
    ReleaseAll();
}

void CXI_VIMAGESCROLL::Draw(bool bSelected, uint32_t Delta_Time)
{
    int n, l;
    if (m_bUse && m_Image != nullptr)
    {
        if (m_bDoMove)
        {
            m_fCurrentDistance += .01f * Delta_Time * m_fDeltaMove;
            if ((m_fCurrentDistance >= m_fMoveDistance && m_fDeltaMove >= 0.f) ||
                (m_fCurrentDistance <= m_fMoveDistance && m_fDeltaMove <= 0.f))
                m_fCurrentDistance = m_fMoveDistance;
            const auto fDelta = ChangeDinamicParameters(m_fCurrentDistance);
            m_fCurrentDistance += fDelta;
            m_fMoveDistance += fDelta;
            if (m_fMoveDistance == m_fCurrentDistance)
            {
                m_bLockStatus = false;
                m_bDoMove = false;

                // Set new current image
                auto *tmpAttr = core.Entity_GetAttributeClass(g_idInterface, m_nodeName);
                if (tmpAttr != nullptr)
                    tmpAttr->SetAttributeUseDword("current", m_nCurImage);

                ChangeDinamicParameters(0);
            }
            for (n = 0; n < m_nSlotsQnt; n++)
                m_dwCurColor[n] = m_dwNormalColor[n];
        }
        else
        {
            if (m_bDoBlind && bSelected)
            {
                if ((m_nBlindCounter -= Delta_Time) <= 0)
                {
                    m_nBlindCounter = m_nMaxBlindCounter;
                    m_bColorType = !m_bColorType;
                }

                for (n = 0; n < m_nSlotsQnt; n++)
                {
                    const int ad = ALPHA(m_dwNormalColor[n]);
                    const int rd = RED(m_dwNormalColor[n]);
                    const int gd = GREEN(m_dwNormalColor[n]);
                    const int bd = BLUE(m_dwNormalColor[n]);
                    const int al = ALPHA(m_dwSelectColor[n]);
                    const int rl = RED(m_dwSelectColor[n]);
                    const int gl = GREEN(m_dwSelectColor[n]);
                    const int bl = BLUE(m_dwSelectColor[n]);
                    int a, r, g, b;
                    if (m_bColorType)
                    {
                        a = (al - ad) * m_nBlindCounter / m_nMaxBlindCounter;
                        r = (rl - rd) * m_nBlindCounter / m_nMaxBlindCounter;
                        g = (gl - gd) * m_nBlindCounter / m_nMaxBlindCounter;
                        b = (bl - bd) * m_nBlindCounter / m_nMaxBlindCounter;
                        m_dwCurColor[n] = ARGB(ad + a, rd + r, gd + g, bd + b);
                    }
                    else
                    {
                        a = (al - ad) * m_nBlindCounter / m_nMaxBlindCounter;
                        r = (rl - rd) * m_nBlindCounter / m_nMaxBlindCounter;
                        g = (gl - gd) * m_nBlindCounter / m_nMaxBlindCounter;
                        b = (bl - bd) * m_nBlindCounter / m_nMaxBlindCounter;
                        m_dwCurColor[n] = ARGB(al - a, rl - r, gl - g, bl - b);
                    }
                }
            }
            else
            {
                for (n = 0; n < m_nSlotsQnt; n++)
                    m_dwCurColor[n] = m_dwNormalColor[n];
            }
        }

        // create select border
        XI_ONLYONETEX_VERTEX pV[4];
        FXYRECT textureRect;
        for (auto i = 0; i < 4; i++)
            pV[i].pos.z = 1.f;
        pPictureService->GetTexturePos(m_nBorderPicture, textureRect);
        pV[0].tu = textureRect.left;
        pV[0].tv = textureRect.top;
        pV[1].tu = textureRect.left;
        pV[1].tv = textureRect.bottom;
        pV[2].tu = textureRect.right;
        pV[2].tv = textureRect.top;
        pV[3].tu = textureRect.right;
        pV[3].tv = textureRect.bottom;
        pV[0].pos.x = static_cast<float>(m_pCenter.x - m_ImageSize.x / 2);
        pV[0].pos.y = static_cast<float>(m_pCenter.y - m_ImageSize.y / 2);
        pV[1].pos.x = static_cast<float>(m_pCenter.x - m_ImageSize.x / 2);
        pV[1].pos.y = static_cast<float>(m_pCenter.y + m_ImageSize.y / 2);
        pV[2].pos.x = static_cast<float>(m_pCenter.x + m_ImageSize.x / 2);
        pV[2].pos.y = static_cast<float>(m_pCenter.y - m_ImageSize.y / 2);
        pV[3].pos.x = static_cast<float>(m_pCenter.x + m_ImageSize.x / 2);
        pV[3].pos.y = static_cast<float>(m_pCenter.y + m_ImageSize.y / 2);

        // show select border
        if (m_bShowBorder /*&& !m_bLockStatus*/ && m_nShowOrder < 0)
        {
            m_rs->TextureSet(0, m_texBorder);
            m_rs->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, XI_ONLYONETEX_FVF, 2, pV, sizeof(XI_ONLYONETEX_VERTEX),
                                  "iScrollImages_border");
        }

        XI_ONETEX_VERTEX v[4];

        auto j = 0;
        FXYRECT rectTex;
        SCROLLEntity *pScroll;
        auto curShowOrder = m_nShowOrder;
        // if(m_bLockStatus) curShowOrder = m_nSlotsQnt-1;
        if (curShowOrder >= m_nSlotsQnt)
            curShowOrder = m_nSlotsQnt - 1;
        auto bDoShowBorder = false;

        for (n = 0; n < m_nSlotsQnt; n++)
        {
            pScroll = m_pScroll;
            while (pScroll != nullptr)
            {
                FXYRECT pos;

                if (m_Image[pScroll->imageNum].ptex[n] != nullptr)
                {
                    m_rs->SetTexture(0, m_Image[pScroll->imageNum].ptex[n]);
                    rectTex.left = 0.f;
                    rectTex.top = 0.f;
                    rectTex.right = 1.f;
                    rectTex.bottom = 1.f;
                }
                else if (m_Image[pScroll->imageNum].img[n] != -1)
                {
                    // get texture rectangle
                    pPictureService->GetTexturePos(m_Image[pScroll->imageNum].img[n], rectTex);
                    m_rs->TextureSet(0, m_nGroupTex[m_Image[pScroll->imageNum].tex[n]]);
                }
                else
                {
                    if (m_idBadPic[n] != -1 && m_idBadTexture[n] != -1)
                    // partial use of texture for a "bad" picture
                    {
                        m_rs->TextureSet(0, m_nGroupTex[m_idBadTexture[n]]);
                        pPictureService->GetTexturePos(m_idBadPic[n], rectTex);
                    }
                    else // "bad" picture for the whole texture
                    {
                        if (m_idBadTexture[n] != -1)
                        {
                            m_rs->TextureSet(0, m_idBadTexture[n]);
                            rectTex.left = 0.f;
                            rectTex.top = 0.f;
                            rectTex.right = 1.f;
                            rectTex.bottom = 1.f;
                        }
                        else // do not show non-existent picture
                        {
                            pScroll = pScroll->next;
                            continue;
                        }
                    }
                }

                pos.right = (pos.left = pScroll->pCenter.x - m_ImageSize.x / 2.f) + m_ImageSize.x;
                pos.bottom = (pos.top = pScroll->pCenter.y - m_ImageSize.y / 2.f + m_pPicOffset[n]) + m_ImageSize.y;
                if (pos.top < m_rect.top)
                {
                    const auto texCorrect = (m_rect.top - pos.top) / (pos.bottom - pos.top);
                    rectTex.top += texCorrect * (rectTex.bottom - rectTex.top);
                    pos.top = static_cast<float>(m_rect.top);
                }
                if (pos.bottom > m_rect.bottom)
                {
                    const auto texCorrect = (m_rect.bottom - pos.bottom) / (pos.bottom - pos.top);
                    rectTex.bottom += texCorrect * (rectTex.bottom - rectTex.top);
                    pos.bottom = static_cast<float>(m_rect.bottom);
                }
                if (pos.bottom > m_rect.top && pos.top < m_rect.bottom)
                {
                    v[0].pos = CVECTOR(static_cast<float>(pos.left), static_cast<float>(pos.top), 1.f);
                    v[1].pos = CVECTOR(static_cast<float>(pos.left), static_cast<float>(pos.bottom), 1.f);
                    v[2].pos = CVECTOR(static_cast<float>(pos.right), static_cast<float>(pos.top), 1.f);
                    v[3].pos = CVECTOR(static_cast<float>(pos.right), static_cast<float>(pos.bottom), 1.f);
                    v[0].tu = v[1].tu = rectTex.left;
                    v[2].tu = v[3].tu = rectTex.right;
                    v[0].tv = v[2].tv = rectTex.top;
                    v[1].tv = v[3].tv = rectTex.bottom;
                    if (pScroll == m_pScroll)
                        v[0].color = v[1].color = v[2].color = v[3].color = m_dwCurColor[n];
                    else
                        v[0].color = v[1].color = v[2].color = v[3].color = m_dwNormalColor[n];
                    if (m_Image[pScroll->imageNum].bUseSpecTechnique[n])
                    {
                        m_rs->SetRenderState(D3DRS_TEXTUREFACTOR, m_dwSpecTechniqueARGB);
                        m_rs->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, XI_ONETEX_FVF, 2, v, sizeof(XI_ONETEX_VERTEX),
                                              m_sSpecTechniqueName);
                    }
                    else
                        m_rs->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, XI_ONETEX_FVF, 2, v, sizeof(XI_ONETEX_VERTEX),
                                              "iScrollImages_main");
                    pScroll->bCurNotUse = false;
                }
                else
                    pScroll->bCurNotUse = true;

                pScroll = pScroll->next;
            }

            if (n == m_nSlotsQnt - 1) // show strings last
            {
                // out to screen the strings if that needed
                for (l = 0; l < m_nStringQuantity; l++)
                {
                    pScroll = m_pScroll;
                    while (pScroll != nullptr)
                    {
                        if (pScroll->pCenter.y + m_pStrParam[l].m_nStrY > m_rect.top && !pScroll->bCurNotUse)
                        {
                            if (m_Image[pScroll->imageNum].strNum[l] != -1)
                                ptrOwner->PrintIntoWindow(
                                    m_rect.left - m_leftTextLimit, m_rect.right + m_rightTextLimit,
                                    m_pStrParam[l].m_nFont, m_pStrParam[l].m_dwForeColor, m_pStrParam[l].m_dwBackColor,
                                    m_pStrParam[l].m_nAlign, true, m_pStrParam[l].m_fScale, m_screenSize.x,
                                    m_screenSize.y, static_cast<long>(pScroll->pCenter.x + m_pStrParam[l].m_nStrX),
                                    static_cast<long>(pScroll->pCenter.y + m_pStrParam[l].m_nStrY),
                                    pStringService->GetString(m_Image[pScroll->imageNum].strNum[l]),
                                    static_cast<int>(m_ImageSize.x * pScroll->fCurScale),
                                    static_cast<int>(-24 * m_pStrParam[l].m_fScale));
                            else if (m_Image[pScroll->imageNum].strSelf[l] != nullptr)
                                ptrOwner->PrintIntoWindow(
                                    m_rect.left - m_leftTextLimit, m_rect.right + m_rightTextLimit,
                                    m_pStrParam[l].m_nFont, m_pStrParam[l].m_dwForeColor, m_pStrParam[l].m_dwBackColor,
                                    m_pStrParam[l].m_nAlign, true, m_pStrParam[l].m_fScale, m_screenSize.x,
                                    m_screenSize.y, static_cast<long>(pScroll->pCenter.x + m_pStrParam[l].m_nStrX),
                                    static_cast<long>(pScroll->pCenter.y + m_pStrParam[l].m_nStrY),
                                    m_Image[pScroll->imageNum].strSelf[l],
                                    static_cast<int>(m_ImageSize.x * pScroll->fCurScale),
                                    static_cast<int>(-24 * m_pStrParam[l].m_fScale));
                        }
                        pScroll = pScroll->next;
                    }
                }
            }

            if (m_bShowBorder && n == curShowOrder)
            {
                // show select border
                m_rs->TextureSet(0, m_texBorder);
                m_rs->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, XI_ONLYONETEX_FVF, 2, pV, sizeof(XI_ONLYONETEX_VERTEX),
                                      "iScrollImages_border");
                bDoShowBorder = true;
            }
        }

        if (m_bShowBorder && !bDoShowBorder)
        {
            // show select border
            m_rs->TextureSet(0, m_texBorder);
            m_rs->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, XI_ONLYONETEX_FVF, 2, pV, sizeof(XI_ONLYONETEX_VERTEX),
                                  "iScrollImages_border");
        }
    }
}

bool CXI_VIMAGESCROLL::Init(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2, VDX9RENDER *rs,
                            XYRECT &hostRect, XYPOINT &ScreenSize)
{
    if (!CINODE::Init(ini1, name1, ini2, name2, rs, hostRect, ScreenSize))
        return false;
    return true;
}

void CXI_VIMAGESCROLL::LoadIni(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2)
{
    int i, n, k;
    char param[256];
    char param1[256];
    const char *tmpstr;

    // Set space
    m_rAbsolutePosition = GetIniLongRect(ini1, name1, ini2, name2, "position", m_hostRect);
    GetAbsoluteRect(m_rAbsolutePosition, 0);
    m_pCenter.x = (m_rAbsolutePosition.right + m_rAbsolutePosition.left) / 2;
    m_pCenter.y = (m_rAbsolutePosition.top + m_rAbsolutePosition.bottom) / 2;
    m_pntCenterOffset.x = GetIniLong(ini1, name1, ini2, name2, "centerXoffset", m_pCenter.x - m_rAbsolutePosition.left);
    m_pntCenterOffset.y = GetIniLong(ini1, name1, ini2, name2, "centerYoffset", m_pCenter.y - m_rAbsolutePosition.top);
    m_pCenter.x = m_rAbsolutePosition.left + m_pntCenterOffset.x;
    m_pCenter.y = m_rAbsolutePosition.top + m_pntCenterOffset.y;

    // set text limit
    m_leftTextLimit = GetIniLong(ini1, name1, ini2, name2, "leftTextLimit", 0);
    m_rightTextLimit = GetIniLong(ini1, name1, ini2, name2, "rightTextLimit", 0);

    // set center of scrolling list
    m_fDeltaMoveBase = GetIniFloat(ini1, name1, ini2, name2, "fMoveDelta", 1.f);
    m_nSpeedMul = GetIniLong(ini1, name1, ini2, name2, "speedMul", 5);

    // set image size
    m_ImageSize = GetIniLongPoint(ini1, name1, ini2, name2, "imageSize", XYPOINT(128, 128));

    // set parameters for blend & minimize far images
    m_fScale = GetIniFloat(ini1, name1, ini2, name2, "fBoundScale", 1.f);
    m_nVDelta = GetIniLong(ini1, name1, ini2, name2, "wDelta", 0);
    m_dwBlendColor = GetIniARGB(ini1, name1, ini2, name2, "blendColor", 0xFFFFFFFF);

    //
    m_nMaxBlindCounter = GetIniLong(ini1, name1, ini2, name2, "blindDelay", 2000);
    m_bDoBlind = true;
    m_bColorType = true;
    m_nBlindCounter = m_nMaxBlindCounter;

    m_nSlotsQnt = GetIniLong(ini1, name1, ini2, name2, "LayerQuantity", 0);

    if (m_nSlotsQnt > 0)
    {
        m_dwCurColor = new uint32_t[m_nSlotsQnt];
        m_dwNormalColor = new uint32_t[m_nSlotsQnt];
        m_dwSelectColor = new uint32_t[m_nSlotsQnt];
        m_pPicOffset = new long[m_nSlotsQnt];
        if (!m_dwCurColor || !m_dwNormalColor || !m_dwSelectColor || !m_pPicOffset)
        {
            throw std::exception("allocate memory error");
        }
    }

    // set parameters for blind
    for (i = 0; i < m_nSlotsQnt; i++)
    {
        sprintf_s(param, "dwNormalColorARGB%d", i + 1);
        m_dwCurColor[i] = m_dwNormalColor[i] = GetIniARGB(ini1, name1, ini2, name2, param, ARGB(255, 128, 128, 128));
        sprintf_s(param, "dwSelectColorARGB%d", i + 1);
        m_dwSelectColor[i] = GetIniARGB(ini1, name1, ini2, name2, param, ARGB(255, 64, 64, 64));
        sprintf_s(param, "PicOffset%d", i + 1);
        m_pPicOffset[i] = GetIniLong(ini1, name1, ini2, name2, param, 0);
    }

    // set stringes
    m_nStringQuantity = GetIniLong(ini1, name1, ini2, name2, "StringsQuantity", 0);
    if (m_nStringQuantity > 0)
    {
        m_pStrParam = new StringParams[m_nStringQuantity];
        Assert(m_pStrParam);
    }

    for (i = 0; i < m_nStringQuantity; i++)
    {
        sprintf_s(param1, sizeof(param1), "scale%d", i + 1);
        m_pStrParam[i].m_fScale = GetIniFloat(ini1, name1, ini2, name2, param1, 1.f);
        sprintf_s(param1, sizeof(param1), "font%d", i + 1);
        if (ReadIniString(ini1, name1, ini2, name2, param1, param, sizeof(param), ""))
            if ((m_pStrParam[i].m_nFont = m_rs->LoadFont(param)) == -1)
                core.Trace("can not load font:'%s'", param);
        sprintf_s(param1, sizeof(param1), "dwXOffset%d", i + 1);
        m_pStrParam[i].m_nStrX = GetIniLong(ini1, name1, ini2, name2, param1, 0);
        if (m_pStrParam[i].m_nStrX > 0)
            m_pStrParam[i].m_nAlign = PR_ALIGN_RIGHT;
        else if (m_pStrParam[i].m_nStrX < 0)
            m_pStrParam[i].m_nAlign = PR_ALIGN_LEFT;
        else
            m_pStrParam[i].m_nAlign = PR_ALIGN_CENTER;
        sprintf_s(param1, sizeof(param1), "align%d", i + 1);
        if (ReadIniString(ini1, name1, ini2, name2, param1, param, sizeof(param), ""))
        {
            if (_stricmp(param, "left") == 0)
                m_pStrParam[i].m_nAlign = PR_ALIGN_LEFT;
            else if (_stricmp(param, "right") == 0)
                m_pStrParam[i].m_nAlign = PR_ALIGN_RIGHT;
            else if (_stricmp(param, "center") == 0)
                m_pStrParam[i].m_nAlign = PR_ALIGN_CENTER;
            else
                core.Trace("Warning! unknown align: %s", param);
        }
        sprintf_s(param1, sizeof(param1), "dwYOffset%d", i + 1);
        m_pStrParam[i].m_nStrY = GetIniLong(ini1, name1, ini2, name2, param1, 0);
        sprintf_s(param1, sizeof(param1), "dwForeColor%d", i + 1);
        m_pStrParam[i].m_dwForeColor = GetIniARGB(ini1, name1, ini2, name2, param1, 0xFFFFFFFF);
        sprintf_s(param1, sizeof(param1), "dwBackColor%d", i + 1);
        m_pStrParam[i].m_dwBackColor = GetIniARGB(ini1, name1, ini2, name2, param1, 0);
    }

    auto *pAttribute = core.Entity_GetAttributeClass(g_idInterface, m_nodeName);
    if (pAttribute != nullptr)
    {
        // get special technique name and color
        m_dwSpecTechniqueARGB = pAttribute->GetAttributeAsDword("SpecTechniqueColor");
        auto *const sTechnique = pAttribute->GetAttribute("SpecTechniqueName");
        if (sTechnique != nullptr)
        {
            const auto len = strlen(sTechnique) + 1;
            if ((m_sSpecTechniqueName = new char[len]) == nullptr)
            {
                throw std::exception("Allocate memory error");
            }
            memcpy(m_sSpecTechniqueName, sTechnique, len);
        }
        // get images quantity
        m_nListSize = pAttribute->GetAttributeAsDword("ListSize", 0);
        m_nNotUsedQuantity = pAttribute->GetAttributeAsDword("NotUsed", 0);
        m_nListSize += m_nNotUsedQuantity;
        // create images array
        if (m_nListSize > 0)
            m_Image = new IMAGEDESCRIBE[m_nListSize];
        else
        {
            m_Image = nullptr;
            return;
        }
        m_nCurImage = pAttribute->GetAttributeAsDword("current", 0);
        if (m_nCurImage >= m_nListSize || m_nCurImage < 0)
            m_nCurImage = 0;

        // get textures
        auto *pA = pAttribute->GetAttributeClass("ImagesGroup");
        if (pA != nullptr)
        {
            m_nGroupQuantity = pA->GetAttributesNum();
            if (m_nGroupQuantity != 0)
            {
                m_nGroupTex = new long[m_nGroupQuantity];
                m_sGroupName = new char *[m_nGroupQuantity];
                if (m_nGroupTex == nullptr || m_sGroupName == nullptr)
                {
                    throw std::exception("allocate memory error");
                }
                for (i = 0; i < m_nGroupQuantity; i++)
                {
                    auto *const stmp = pA->GetAttribute(i);
                    if (stmp == nullptr)
                        continue;
                    const auto len = strlen(stmp) + 1;
                    m_sGroupName[i] = new char[len];
                    if (m_sGroupName[i] == nullptr)
                    {
                        throw std::exception("allocate memory error");
                    }
                    memcpy(m_sGroupName[i], stmp, len);
                    m_nGroupTex[i] = pPictureService->GetTextureID(m_sGroupName[i]);
                }
            }
        }

        // get bad picture
        if (m_nSlotsQnt > 0)
        {
            m_idBadTexture = new long[m_nSlotsQnt];
            m_idBadPic = new long[m_nSlotsQnt];
            if (!m_idBadTexture || !m_idBadPic)
            {
                throw std::exception("allocate memory error");
            }
        }
        for (n = 0; n < m_nSlotsQnt; n++)
        {
            char *sBadPict;
            sprintf_s(param, "BadPicture%d", n + 1);
            if ((sBadPict = pAttribute->GetAttribute(param)) != nullptr)
            {
                m_idBadTexture[n] = m_rs->TextureCreate(sBadPict);
                m_idBadPic[n] = -1;
            }
            else
            {
                sprintf_s(param, "BadTex%d", n + 1);
                m_idBadTexture[n] = pAttribute->GetAttributeAsDword(param, -1);
                if (m_idBadTexture[n] >= 0)
                {
                    sprintf_s(param, "BadPic%d", n + 1);
                    m_idBadPic[n] =
                        pPictureService->GetImageNum(m_sGroupName[m_idBadTexture[n]], pAttribute->GetAttribute(param));
                }
                else
                    m_idBadPic[n] = -1;
                if (m_idBadPic[n] == -1)
                    m_idBadTexture[n] = -1;
            }
        }

        // get all scroll entity
        for (i = 0; i < m_nListSize; i++)
        {
            char attrName[256];
            char *sStringName;
            sprintf_s(attrName, "pic%d", i + 1);
            auto *pListEntity = pAttribute->GetAttributeClass(attrName);

            // Fill image descriptor by default value
            //------------------------------------------------------
            if (m_nStringQuantity > 0)
            {
                m_Image[i].strNum = new long[m_nStringQuantity];
                m_Image[i].strSelf = new char *[m_nStringQuantity];
            }
            else
            {
                m_Image[i].strNum = nullptr;
                m_Image[i].strSelf = nullptr;
            }
            for (k = 0; k < m_nStringQuantity; k++)
            {
                m_Image[i].strNum[k] = -1;
                m_Image[i].strSelf[k] = nullptr;
            }
            if (m_nSlotsQnt > 0)
            {
                m_Image[i].bUseSpecTechnique = new bool[m_nSlotsQnt];
                m_Image[i].img = new long[m_nSlotsQnt];
                m_Image[i].ptex = new IDirect3DTexture9 *[m_nSlotsQnt];
                m_Image[i].saveName = new char *[m_nSlotsQnt];
                m_Image[i].tex = new long[m_nSlotsQnt];
                if (!m_Image[i].bUseSpecTechnique || !m_Image[i].img || !m_Image[i].ptex || !m_Image[i].saveName ||
                    !m_Image[i].tex)
                {
                    throw std::exception("allocate memory error");
                }
                for (n = 0; n < m_nSlotsQnt; n++)
                {
                    m_Image[i].bUseSpecTechnique[n] = false;
                    m_Image[i].img[n] = -1;
                    m_Image[i].ptex[n] = nullptr;
                    m_Image[i].saveName[n] = nullptr;
                    m_Image[i].tex[n] = -1;
                }
            }
            else
            {
                m_Image[i].bUseSpecTechnique = nullptr;
                m_Image[i].img = nullptr;
                m_Image[i].ptex = nullptr;
                m_Image[i].saveName = nullptr;
                m_Image[i].tex = nullptr;
            }

            if (pListEntity != nullptr)
            {
                // set strings
                for (k = 0; k < m_nStringQuantity; k++)
                {
                    sprintf_s(param1, sizeof(param1) - 1, "str%d", k + 1);
                    sStringName = pListEntity->GetAttribute(param1);
                    if (sStringName != nullptr && sStringName[0] == '#')
                    {
                        const auto len = strlen(sStringName);
                        m_Image[i].strSelf[k] = new char[len];
                        if (m_Image[i].strSelf[k] == nullptr)
                            throw std::exception("allocate memory error");
                        memcpy(m_Image[i].strSelf[k], &(sStringName[1]), len);
                    }
                    else
                        m_Image[i].strNum[k] = pStringService->GetStringNum(sStringName);
                }

                // set pictures
                char *tmpStr;
                for (n = 0; n < m_nSlotsQnt; n++)
                {
                    sprintf_s(param, "name%d", n + 1);
                    tmpStr = pListEntity->GetAttribute(param);
                    if (tmpStr != nullptr)
                    {
                        const auto len = strlen(tmpStr) + 1;
                        if ((m_Image[i].saveName[n] = new char[len]) == nullptr)
                        {
                            throw std::exception("allocate memory error");
                        }
                        memcpy(m_Image[i].saveName[n], tmpStr, len);
                    }
                    sprintf_s(param, "tex%d", n + 1);
                    m_Image[i].tex[n] = pListEntity->GetAttributeAsDword(param, -1);
                    sprintf_s(param, "img%d", n + 1);
                    if (m_Image[i].tex[n] != -1)
                        m_Image[i].img[n] = pPictureService->GetImageNum(m_sGroupName[m_Image[i].tex[n]],
                                                                         pListEntity->GetAttribute(param));
                    else
                        m_Image[i].img[n] = -1;
                    sprintf_s(param, "spec%d", n + 1);
                    m_Image[i].bUseSpecTechnique[n] = pListEntity->GetAttributeAsDword(param, 0) != 0;
                }
            }
        }
    }
    else
    {
        m_nSlotsQnt = 0;
    }

    // get border picture
    m_nShowOrder = GetIniLong(ini1, name1, ini2, name2, "borderShowOrder", 100); // boredrShowUp
    if (ReadIniString(ini1, name1, ini2, name2, "border", param, sizeof(param), ""))
    {
        tmpstr = GetSubStr(param, param1, sizeof(param1));
        const auto len = strlen(param1) + 1;
        if ((m_sBorderGroupName = new char[len]) == nullptr)
            throw std::exception("allocate memory error");
        memcpy(m_sBorderGroupName, param1, len);
        m_texBorder = pPictureService->GetTextureID(m_sBorderGroupName);
        m_nBorderPicture = pPictureService->GetImageNum(m_sBorderGroupName, tmpstr);
        m_bShowBorder = m_texBorder != -1;
    }
    else
    {
        m_bShowBorder = false;
        m_texBorder = -1;
        m_sBorderGroupName = nullptr;
    }

    // set images parameters
    ChangeDinamicParameters(0);
}

float CXI_VIMAGESCROLL::ChangeDinamicParameters(float fYDelta)
{
    if (m_Image == nullptr)
        return 0.f;
    int n;

    float curScale;
    auto bIncrement = true;
    auto curYCenter = m_pCenter.y + fYDelta;
    auto curImage = m_nCurImage;
    const auto lbBorderDist = m_rect.bottom - m_pCenter.y;
    const auto ltBorderDist = m_pCenter.y - m_rect.top;

    auto newCurImage = m_nCurImage;
    auto fNewCurCenter = curYCenter;

    SCROLLEntity *pPrevScroll = nullptr;
    auto *pScroll = m_pScroll;

    while (true)
    {
        if (pScroll == nullptr)
        {
            pScroll = new SCROLLEntity;
            if (pScroll == nullptr)
                throw std::exception("allocate memory error");

            if (pPrevScroll == nullptr)
                m_pScroll = pScroll;
            else
                pPrevScroll->next = pScroll;

            pScroll->next = nullptr;
        }

        while (true)
        {
            pScroll->imageNum = curImage;
            for (n = 0; n < m_nSlotsQnt; n++)
            {
                if (m_Image[curImage].saveName[n] != nullptr)
                {
                    if (m_Image[curImage].ptex[n] == nullptr)
                    {
                        m_Image[curImage].ptex[n] = GetTexFromEvent(
                            core.Event("GetInterfaceTexture", "sl", m_Image[curImage].saveName[n], curImage));
                        break;
                    }
                }
            }
            if (n == m_nSlotsQnt || (uintptr_t)m_Image[curImage].ptex[n] != -1)
                break;

            // delete current save from list
            m_Image[curImage].Release(m_nSlotsQnt, m_nStringQuantity);
            if (curImage < m_nListSize - 1)
                memcpy(&m_Image[curImage], &m_Image[curImage + 1],
                       sizeof(IMAGEDESCRIBE) * (m_nListSize - 1 - curImage));
            m_nListSize--;
            // move all already used pictures
            for (auto *pSTmp = m_pScroll; pSTmp != nullptr && pSTmp != pScroll; pSTmp = pSTmp->next)
                if (pSTmp->imageNum > curImage)
                    pSTmp->imageNum--;
            if (!bIncrement)
                curImage--;
            //
            if (curImage < 0 || curImage >= m_nListSize)
                break;
        }
        if (curImage < 0 || curImage >= m_nListSize)
            break;

        // Calculate color
        if (curYCenter >= m_pCenter.y)
        {
            pScroll->colorMul = static_cast<float>(curYCenter - m_pCenter.y) / lbBorderDist;
        }
        else
        {
            pScroll->colorMul = static_cast<float>(m_pCenter.y - curYCenter) / ltBorderDist;
        }

        // Calculate current scale
        if (curYCenter >= m_pCenter.y)
        {
            curScale = 1.f - (curYCenter - m_pCenter.y) / lbBorderDist * (1.f - m_fScale);
        }
        else
        {
            curScale = 1.f - (m_pCenter.y - curYCenter) / ltBorderDist * (1.f - m_fScale);
        }

        // set position
        pScroll->fCurScale = curScale;
        pScroll->pCenter.x = static_cast<float>(m_pCenter.x);
        pScroll->pCenter.y = curYCenter;

        auto tpos = curYCenter - m_ImageSize.y * .5f;
        const auto bpos = curYCenter + m_ImageSize.y * .5f;

        if (m_pCenter.y >= tpos && m_pCenter.y <= bpos)
        {
            newCurImage = curImage;
            fNewCurCenter = curYCenter;
        }

        if (bIncrement && bpos > m_rect.bottom - m_nVDelta) // end pass to right
        {
            // remove the next (unused texture)
            curImage++;
            if (curImage >= m_nListSize)
                curImage = 0;
            for (n = 0; n < m_nSlotsQnt; n++)
            {
                if (m_Image[curImage].saveName[n] != nullptr && m_Image[curImage].ptex[n] != nullptr)
                {
                    core.Event("DelInterfaceTexture", "s", m_Image[curImage].saveName[n]);
                    m_Image[curImage].ptex[n] = nullptr;
                }
            }
            // continue displaying the next icon from the center
            curImage = m_nCurImage;
            curYCenter = m_pCenter.y + fYDelta;
            if (curYCenter >= m_pCenter.y)
            {
                curScale = 1.f - (curYCenter - m_pCenter.y) / lbBorderDist * (1.f - m_fScale);
            }
            else
            {
                curScale = 1.f - (m_pCenter.y - curYCenter) / ltBorderDist * (1.f - m_fScale);
            }
            tpos = curYCenter - m_ImageSize.y * .5f * curScale;
            bIncrement = false;
        }
        else if (!bIncrement && tpos < m_rect.top + m_nVDelta) // end pass to left is all end
        {
            // remove the previous (unused texture)
            curImage--;
            if (curImage < 0)
                curImage = m_nListSize - 1;
            for (n = 0; n < m_nSlotsQnt; n++)
            {
                if (m_Image[curImage].saveName[n] != nullptr && m_Image[curImage].ptex[n] != nullptr)
                {
                    core.Event("DelInterfaceTexture", "s", m_Image[curImage].saveName[n]);
                    m_Image[curImage].ptex[n] = nullptr;
                }
            }
            if (m_pScroll->next == nullptr)
            {
                m_pScroll->next = nullptr;
            }
            break;
        }

        // next image
        if (bIncrement)
        {
            curImage++;
            if (curImage >= m_nListSize)
                curImage = 0;
            curYCenter = bpos + m_nVDelta + m_ImageSize.y * 0.5f;
        }
        else
        {
            curImage--;
            if (curImage < 0)
                curImage = m_nListSize - 1;
            curYCenter = tpos - m_nVDelta - m_ImageSize.y * 0.5f;
        }

        if (curYCenter + m_ImageSize.y * 0.5f <= m_rect.top + m_nVDelta)
            break;

        pPrevScroll = pScroll;
        pScroll = pScroll->next;
    }

    if (pScroll->next != nullptr)
    {
        auto *pScr = pScroll->next;
        pScroll->next = nullptr;
        while (pScr != nullptr)
        {
            pScroll = pScr;
            pScr = pScr->next;
            delete pScroll;
        }
    }

    if (newCurImage != m_nCurImage)
    {
        m_nCurImage = newCurImage;
        return fNewCurCenter - m_pCenter.y - fYDelta;
    }

    return 0;
}

void CXI_VIMAGESCROLL::ReleaseAll()
{
    int i;

    STORM_DELETE(m_sSpecTechniqueName);

    for (i = 0; i < m_nSlotsQnt; i++)
    {
        if (m_idBadPic[i] == -1)
        {
            TEXTURE_RELEASE(m_rs, m_idBadTexture[i]);
        }
        else
            m_idBadTexture[i] = -1;
        m_idBadPic[i] = -1;
    }
    STORM_DELETE(m_idBadPic);
    STORM_DELETE(m_idBadTexture);

    if (m_Image != nullptr)
    {
        for (i = 0; i < m_nListSize; i++)
            m_Image[i].Release(m_nSlotsQnt, m_nStringQuantity);
        STORM_DELETE(m_Image);
    }

    while (m_pScroll != nullptr)
    {
        SCROLLEntity *rootScroll = m_pScroll;
        m_pScroll = m_pScroll->next;
        delete rootScroll;
    }

    for (i = 0; i < m_nGroupQuantity; i++)
    {
        PICTURE_TEXTURE_RELEASE(pPictureService, m_sGroupName[i], m_nGroupTex[i]);
        STORM_DELETE(m_sGroupName[i]);
    }
    STORM_DELETE(m_sGroupName);
    STORM_DELETE(m_nGroupTex);

    PICTURE_TEXTURE_RELEASE(pPictureService, m_sBorderGroupName, m_texBorder);
    STORM_DELETE(m_sBorderGroupName);

    // release all strings parameters
    for (i = 0; i < m_nStringQuantity; i++)
    {
        FONT_RELEASE(m_rs, m_pStrParam[i].m_nFont);
    }

    STORM_DELETE(m_dwCurColor);
    STORM_DELETE(m_dwNormalColor);
    STORM_DELETE(m_dwSelectColor);
    STORM_DELETE(m_pPicOffset);
    STORM_DELETE(m_pStrParam);

    m_nSlotsQnt = 0;
    m_nStringQuantity = 0;
    m_nGroupQuantity = 0;
    m_nListSize = 0;
}

int CXI_VIMAGESCROLL::CommandExecute(int wActCode)
{
    int i;
    if (m_bUse && m_Image != nullptr && m_pScroll != nullptr)
    {
        if (m_bLockStatus)
            return -1;

        switch (wActCode)
        {
        case ACTION_DOWNSTEP:
        case ACTION_SPEEDDOWN:
            i = GetBottomQuantity();
            if (i == 0)
                break;
            if (wActCode == ACTION_DOWNSTEP)
                i = 1;
            m_bLockStatus = true;
            m_bDoMove = true;
            m_fCurrentDistance = 0.f;
            m_fMoveDistance = GetShiftDistance(i);
            if (wActCode == ACTION_SPEEDDOWN)
                m_fDeltaMove = -m_fDeltaMoveBase * m_nSpeedMul;
            else
                m_fDeltaMove = -m_fDeltaMoveBase;
            break;

        case ACTION_UPSTEP:
        case ACTION_SPEEDUP:
            i = GetTopQuantity();
            if (i == 0)
                break;
            if (wActCode == ACTION_UPSTEP)
                i = 1;
            m_bLockStatus = true;
            m_bDoMove = true;
            m_fCurrentDistance = 0.f;
            m_fMoveDistance = GetShiftDistance(-i);
            if (wActCode == ACTION_SPEEDUP)
                m_fDeltaMove = m_fDeltaMoveBase * m_nSpeedMul;
            else
                m_fDeltaMove = m_fDeltaMoveBase;
            break;

        case ACTION_MOUSECLICK:
            if (IsCurrentNode())
            {
                i = FindClickedImageNum();
                if (i == 0)
                    return ACTION_ACTIVATE;
                if (i == 10000)
                    return -1;
                m_bLockStatus = true;
                m_bDoMove = true;
                m_fCurrentDistance = 0.f;
                m_fMoveDistance = GetShiftDistance(i);
                if (i > 0)
                    m_fDeltaMove = -m_fDeltaMoveBase * m_nSpeedMul;
                else
                    m_fDeltaMove = m_fDeltaMoveBase * m_nSpeedMul;
            }
            break;
        }
    }
    return -1;
}

bool CXI_VIMAGESCROLL::IsClick(int buttonID, long xPos, long yPos)
{
    if (xPos >= m_rect.left && xPos <= m_rect.right && yPos >= m_rect.top && yPos <= m_rect.bottom && m_bClickable &&
        m_bUse)
    {
        return true;
    }

    return false;
}

void CXI_VIMAGESCROLL::ChangePosition(XYRECT &rNewPos)
{
    const long nLeftOffset = rNewPos.left - m_rect.left;
    const long nTopOffset = rNewPos.top - m_rect.top;
    const long nRightOffset = rNewPos.right - m_rect.right;
    const long nBottomOffset = rNewPos.bottom - m_rect.bottom;

    m_rAbsolutePosition.left += nLeftOffset;
    m_rAbsolutePosition.top += nTopOffset;
    m_rAbsolutePosition.right += nRightOffset;
    m_rAbsolutePosition.bottom += nBottomOffset;

    m_rect = rNewPos;

    m_pCenter.x = (m_rAbsolutePosition.right + m_rAbsolutePosition.left) / 2;
    m_pCenter.y = (m_rAbsolutePosition.top + m_rAbsolutePosition.bottom) / 2;
    m_pCenter.x = m_rAbsolutePosition.left + m_pntCenterOffset.x;
    m_pCenter.y = m_rAbsolutePosition.top + m_pntCenterOffset.y;

    ChangeDinamicParameters(0.f);
}

void CXI_VIMAGESCROLL::SaveParametersToIni()
{
    char pcWriteParam[2048];

    auto pIni = fio->OpenIniFile(ptrOwner->m_sDialogFileName.c_str());
    if (!pIni)
    {
        core.Trace("Warning! Can`t open ini file name %s", ptrOwner->m_sDialogFileName.c_str());
        return;
    }

    // save position
    sprintf_s(pcWriteParam, sizeof(pcWriteParam), "%d,%d,%d,%d", m_rAbsolutePosition.left, m_rAbsolutePosition.top,
              m_rAbsolutePosition.right, m_rAbsolutePosition.bottom);
    pIni->WriteString(m_nodeName, "position", pcWriteParam);
}

void CXI_VIMAGESCROLL::ChangeScroll(int nScrollItemNum)
{
    ATTRIBUTES *pAttr = core.Entity_GetAttributeClass(g_idInterface, m_nodeName);
    if (pAttr != nullptr)
    {
        // check maybe the whole list needs to be changed
        if (nScrollItemNum == -1 || m_nListSize != static_cast<long>(pAttr->GetAttributeAsDword("NotUsed", 0) +
                                                                     pAttr->GetAttributeAsDword("ListSize", 0)))
        {
            RefreshScroll();
            return;
        }

        int nScrollLastNum;
        if (nScrollItemNum != -1) //~!~
            nScrollLastNum = nScrollItemNum + 1;
        else
        {
            nScrollLastNum = m_nListSize;
            nScrollItemNum = 0;
        }

        int i, n;
        char sAttrName[256];
        char param[256];
        ATTRIBUTES *pAttribute;
        char *sStringName;
        for (i = nScrollItemNum; i < nScrollLastNum; i++)
        {
            m_Image[i].Clear(m_nSlotsQnt, m_nStringQuantity);

            sprintf_s(sAttrName, "pic%d", i + 1);
            pAttribute = pAttr->GetAttributeClass(sAttrName);

            if (pAttribute != nullptr)
            {
                for (n = 0; n < m_nStringQuantity; n++)
                {
                    sprintf_s(param, sizeof(param), "str%d", n + 1);
                    sStringName = pAttribute->GetAttribute(param);
                    if (sStringName != nullptr && sStringName[0] == '#')
                    {
                        const auto len = strlen(sStringName);
                        m_Image[i].strSelf[n] = new char[len];
                        if (m_Image[i].strSelf[n] == nullptr)
                            throw std::exception("allocate memory error");
                        memcpy(m_Image[i].strSelf[n], &(sStringName[1]), len);
                    }
                    else
                        m_Image[i].strNum[n] = pStringService->GetStringNum(sStringName);
                }

                // set pictures
                char *tmpStr;
                for (n = 0; n < m_nSlotsQnt; n++)
                {
                    sprintf_s(param, "name%d", n + 1);
                    tmpStr = pAttribute->GetAttribute(param);
                    if (tmpStr != nullptr)
                    {
                        const auto len = strlen(tmpStr) + 1;
                        if ((m_Image[i].saveName[n] = new char[len]) == nullptr)
                            throw std::exception("allocate memory error");
                        memcpy(m_Image[i].saveName[n], tmpStr, len);
                    }
                    sprintf_s(param, "tex%d", n + 1);
                    m_Image[i].tex[n] = pAttribute->GetAttributeAsDword(param, -1);
                    sprintf_s(param, "img%d", n + 1);
                    if (m_Image[i].tex[n] != -1)
                        m_Image[i].img[n] = pPictureService->GetImageNum(m_sGroupName[m_Image[i].tex[n]],
                                                                         pAttribute->GetAttribute(param));
                    else
                        m_Image[i].img[n] = -1;
                    sprintf_s(param, "spec%d", n + 1);
                    m_Image[i].bUseSpecTechnique[n] = pAttribute->GetAttributeAsDword(param, 0) != 0;
                }
            }
        }
    }
}

void CXI_VIMAGESCROLL::DeleteImage(int imgNum)
{
    if (imgNum < 0 || imgNum >= m_nListSize)
        return;
    if (m_nListSize <= m_nNotUsedQuantity)
        return;
    m_Image[imgNum].Release(m_nSlotsQnt, m_nStringQuantity);
    m_nListSize--;
    if (m_nListSize <= 0)
    {
        STORM_DELETE(m_Image);
        return;
    }

    IMAGEDESCRIBE *pOldImgs = m_Image;
    m_Image = new IMAGEDESCRIBE[m_nListSize];
    if (m_Image == nullptr)
        throw std::exception("memory allocate error");
    if (imgNum > 0)
        memcpy(m_Image, pOldImgs, imgNum * sizeof(IMAGEDESCRIBE));
    if (imgNum < m_nListSize)
        memcpy(&m_Image[imgNum], &pOldImgs[imgNum + 1], (m_nListSize - imgNum) * sizeof(IMAGEDESCRIBE));
    delete pOldImgs;

    if (m_nCurImage >= m_nListSize - m_nNotUsedQuantity)
        m_nCurImage = m_nListSize - m_nNotUsedQuantity - 1;
    if (m_nCurImage < 0)
        m_nCurImage = 0;
    ATTRIBUTES *pA = core.Entity_GetAttributeClass(g_idInterface, m_nodeName);
    if (pA != nullptr)
    {
        pA->SetAttributeUseDword("current", m_nCurImage);
    }
    ChangeDinamicParameters(0);
}

void CXI_VIMAGESCROLL::RefreshScroll()
{
    int i, n, k;
    char param[256];
    // UpdateTexturesGroup();

    STORM_DELETE(m_sSpecTechniqueName);

    for (i = 0; i < m_nSlotsQnt; i++)
    {
        if (m_idBadPic[i] == -1)
        {
            TEXTURE_RELEASE(m_rs, m_idBadTexture[i]);
        }
        else
            m_idBadTexture[i] = -1;
        m_idBadPic[i] = -1;
    }

    if (m_Image != nullptr)
    {
        for (i = 0; i < m_nListSize; i++)
            m_Image[i].Release(m_nSlotsQnt, m_nStringQuantity);
        STORM_DELETE(m_Image);
    }

    while (m_pScroll != nullptr)
    {
        SCROLLEntity *rootScroll = m_pScroll;
        m_pScroll = m_pScroll->next;
        delete rootScroll;
    }

    // release old groups
    for (i = 0; i < m_nGroupQuantity; i++)
    {
        PICTURE_TEXTURE_RELEASE(pPictureService, m_sGroupName[i], m_nGroupTex[i]);
        STORM_DELETE(m_sGroupName[i]);
    }
    STORM_DELETE(m_sGroupName);
    STORM_DELETE(m_nGroupTex);

    m_nListSize = 0;
    m_nNotUsedQuantity = 0;

    ATTRIBUTES *pAttribute = core.Entity_GetAttributeClass(g_idInterface, m_nodeName);
    if (pAttribute != nullptr)
    {
        // get special technique name and color
        m_dwSpecTechniqueARGB = pAttribute->GetAttributeAsDword("SpecTechniqueColor");
        char *sTechnique = pAttribute->GetAttribute("SpecTechniqueName");
        if (sTechnique != nullptr)
        {
            const auto len = strlen(sTechnique) + 1;
            if ((m_sSpecTechniqueName = new char[len]) == nullptr)
            {
                throw std::exception("Allocate memory error");
            }
            memcpy(m_sSpecTechniqueName, sTechnique, len);
        }
        // get images quantity
        m_nListSize = pAttribute->GetAttributeAsDword("ListSize", 0);
        m_nNotUsedQuantity = pAttribute->GetAttributeAsDword("NotUsed", 0);
        m_nListSize += m_nNotUsedQuantity;
        // create images array
        if (m_nListSize > 0)
        {
            m_Image = new IMAGEDESCRIBE[m_nListSize];
            for (i = 0; i < m_nListSize; i++)
            {
                m_Image[i].bUseSpecTechnique = nullptr;
            }
        }
        else
        {
            m_Image = nullptr;
            return;
        }
        m_nCurImage = pAttribute->GetAttributeAsDword("current", 0);
        if (m_nCurImage >= m_nListSize || m_nCurImage < 0)
            m_nCurImage = 0;

        // get textures
        ATTRIBUTES *pA = pAttribute->GetAttributeClass("ImagesGroup");
        if (pA != nullptr)
        {
            m_nGroupQuantity = pA->GetAttributesNum();
            if (m_nGroupQuantity != 0)
            {
                // set new groups
                m_nGroupTex = new long[m_nGroupQuantity];
                m_sGroupName = new char *[m_nGroupQuantity];
                if (m_nGroupTex == nullptr || m_sGroupName == nullptr)
                {
                    throw std::exception("allocate memory error");
                }
                for (i = 0; i < m_nGroupQuantity; i++)
                {
                    char *stmp = pA->GetAttribute(i);
                    if (stmp == nullptr)
                        continue;
                    const auto len = strlen(stmp) + 1;
                    m_sGroupName[i] = new char[len];
                    if (m_sGroupName[i] == nullptr)
                    {
                        throw std::exception("allocate memory error");
                    }
                    memcpy(m_sGroupName[i], stmp, len);
                    m_nGroupTex[i] = pPictureService->GetTextureID(m_sGroupName[i]);
                }
            }
        }

        // get bad picture
        for (n = 0; n < m_nSlotsQnt; n++)
        {
            char *sBadPict;
            sprintf_s(param, "BadPicture%d", n + 1);
            if ((sBadPict = pAttribute->GetAttribute(param)) != nullptr)
            {
                m_idBadTexture[n] = m_rs->TextureCreate(sBadPict);
                m_idBadPic[n] = -1;
            }
            else
            {
                sprintf_s(param, "BadTex%d", n + 1);
                m_idBadTexture[n] = pAttribute->GetAttributeAsDword(param, -1);
                if (m_idBadTexture[n] >= 0)
                {
                    sprintf_s(param, "BadPic%d", n + 1);
                    m_idBadPic[n] =
                        pPictureService->GetImageNum(m_sGroupName[m_idBadTexture[n]], pAttribute->GetAttribute(param));
                }
                else
                    m_idBadPic[n] = -1;
                if (m_idBadPic[n] == -1)
                    m_idBadTexture[n] = -1;
            }
        }

        // get all scroll entity
        for (i = 0; i < m_nListSize; i++)
        {
            char attrName[256];
            char *sStringName;
            sprintf_s(attrName, "pic%d", i + 1);
            ATTRIBUTES *pListEntity = pAttribute->GetAttributeClass(attrName);

            // Fill image descriptor by default value
            //------------------------------------------------------
            if (m_nStringQuantity > 0)
            {
                m_Image[i].strNum = new long[m_nStringQuantity];
                m_Image[i].strSelf = new char *[m_nStringQuantity];
            }
            else
            {
                m_Image[i].strNum = nullptr;
                m_Image[i].strSelf = nullptr;
            }
            for (k = 0; k < m_nStringQuantity; k++)
            {
                m_Image[i].strNum[k] = -1;
                m_Image[i].strSelf[k] = nullptr;
            }
            if (m_nSlotsQnt > 0)
            {
                m_Image[i].bUseSpecTechnique = new bool[m_nSlotsQnt];
                m_Image[i].img = new long[m_nSlotsQnt];
                m_Image[i].ptex = new IDirect3DTexture9 *[m_nSlotsQnt];
                m_Image[i].saveName = new char *[m_nSlotsQnt];
                m_Image[i].tex = new long[m_nSlotsQnt];
                if (!m_Image[i].bUseSpecTechnique || !m_Image[i].img || !m_Image[i].ptex || !m_Image[i].saveName ||
                    !m_Image[i].tex)
                {
                    throw std::exception("allocate memory error");
                }
                for (n = 0; n < m_nSlotsQnt; n++)
                {
                    m_Image[i].bUseSpecTechnique[n] = false;
                    m_Image[i].img[n] = -1;
                    m_Image[i].ptex[n] = nullptr;
                    m_Image[i].saveName[n] = nullptr;
                    m_Image[i].tex[n] = -1;
                }
            }
            else
            {
                m_Image[i].bUseSpecTechnique = nullptr;
                m_Image[i].img = nullptr;
                m_Image[i].ptex = nullptr;
                m_Image[i].saveName = nullptr;
                m_Image[i].tex = nullptr;
            }

            if (pListEntity != nullptr)
            {
                // set strings
                for (k = 0; k < m_nStringQuantity; k++)
                {
                    sprintf_s(param, sizeof(param) - 1, "str%d", k + 1);
                    sStringName = pListEntity->GetAttribute(param);
                    if (sStringName != nullptr && sStringName[0] == '#')
                    {
                        const auto len = strlen(sStringName);
                        m_Image[i].strSelf[k] = new char[len];
                        if (m_Image[i].strSelf[k] == nullptr)
                            throw std::exception("allocate memory error");
                        memcpy(m_Image[i].strSelf[k], &(sStringName[1]), len);
                    }
                    else
                        m_Image[i].strNum[k] = pStringService->GetStringNum(sStringName);
                }

                // set pictures
                char *tmpStr;
                for (n = 0; n < m_nSlotsQnt; n++)
                {
                    sprintf_s(param, "name%d", n + 1);
                    tmpStr = pListEntity->GetAttribute(param);
                    if (tmpStr != nullptr)
                    {
                        const auto len = strlen(tmpStr) + 1;
                        if ((m_Image[i].saveName[n] = new char[len]) == nullptr)
                        {
                            throw std::exception("allocate memory error");
                        }
                        memcpy(m_Image[i].saveName[n], tmpStr, len);
                    }
                    sprintf_s(param, "tex%d", n + 1);
                    m_Image[i].tex[n] = pListEntity->GetAttributeAsDword(param, -1);
                    sprintf_s(param, "img%d", n + 1);
                    if (m_Image[i].tex[n] != -1)
                        m_Image[i].img[n] = pPictureService->GetImageNum(m_sGroupName[m_Image[i].tex[n]],
                                                                         pListEntity->GetAttribute(param));
                    else
                        m_Image[i].img[n] = -1;
                    sprintf_s(param, "spec%d", n + 1);
                    m_Image[i].bUseSpecTechnique[n] = pListEntity->GetAttributeAsDword(param, 0) != 0;
                }
            }
        }
    }

    if (m_nCurImage >= m_nListSize - m_nNotUsedQuantity)
        m_nCurImage = m_nListSize - m_nNotUsedQuantity - 1;
    if (m_nCurImage < 0)
        m_nCurImage = 0;
    ATTRIBUTES *pA = core.Entity_GetAttributeClass(g_idInterface, m_nodeName);
    if (pA != nullptr)
    {
        pA->SetAttributeUseDword("current", m_nCurImage);
    }

    ChangeDinamicParameters(0);
}

int CXI_VIMAGESCROLL::FindClickedImageNum() const
{
    int n;
    int i = 0;

    const FXYPOINT fp = ptrOwner->GetMousePoint();
    SCROLLEntity *pscroll;
    for (pscroll = m_pScroll; pscroll != nullptr; pscroll = pscroll->next)
    {
        float flx = .5f * pscroll->fCurScale * m_ImageSize.x;
        const float frx = pscroll->pCenter.x + flx;
        flx = pscroll->pCenter.x - flx;

        float fty = .5f * pscroll->fCurScale * m_ImageSize.y;
        const float fby = pscroll->pCenter.y + fty;
        fty = pscroll->pCenter.y - fty;

        if (pscroll->pCenter.y >= m_pCenter.y)
            i++;
        else if (i >= 0)
            i = -1;
        else
            i--;

        if (fp.x >= flx && fp.x <= frx && fp.y >= fty && fp.y <= fby)
            break;
    }

    if (pscroll == nullptr)
        return 10000;
    for (n = 0; n < m_nSlotsQnt; n++)
    {
        if (m_Image[pscroll->imageNum].tex[n] != -1 || m_Image[pscroll->imageNum].ptex[n] != nullptr ||
            m_Image[pscroll->imageNum].saveName[n] != nullptr)
            break;
    }
    if (n >= m_nSlotsQnt)
        return 10000;
    if (i < 0)
        return i;
    if (i == 0)
        return 0;
    return i - 1;
}

int CXI_VIMAGESCROLL::GetBottomQuantity() const
{
    if (m_pScroll == nullptr || m_Image == nullptr)
        return 0;
    int q = 0;
    for (SCROLLEntity *pscr = m_pScroll; pscr != nullptr; pscr = pscr->next)
        q++;

    int i = m_pScroll->imageNum;
    int n;
    for (n = 0; n < q; n++)
    {
        int j;
        for (j = 0; j < m_nSlotsQnt; j++)
        {
            if (m_Image[i].img[j] != -1 || m_Image[i].saveName[j] != nullptr)
                break;
        }
        if (j >= m_nSlotsQnt)
            break;
        i++;
        if (i >= m_nListSize)
            i = 0;
        if (i == m_pScroll->imageNum)
            break;
    }
    if (n == 0)
        return 0;
    return n - 1;
}

int CXI_VIMAGESCROLL::GetTopQuantity() const
{
    if (m_pScroll == nullptr || m_Image == nullptr)
        return 0;
    int q = 0;
    for (SCROLLEntity *pscr = m_pScroll; pscr != nullptr; pscr = pscr->next)
        q++;

    int i = m_pScroll->imageNum;
    int n;
    for (n = 0; n < q; n++)
    {
        int j;
        for (j = 0; j < m_nSlotsQnt; j++)
        {
            if (m_Image[i].img[j] != -1 || m_Image[i].saveName[j] != nullptr)
                break;
        }
        if (j >= m_nSlotsQnt)
            break;
        i--;
        if (i < 0)
            i = m_nListSize - 1;
        if (i == m_pScroll->imageNum)
            break;
    }
    if (n == 0)
        return 0;
    return n - 1;
}

float CXI_VIMAGESCROLL::GetShiftDistance(int shiftIdx) const
{
    int i = 0;
    const int n = shiftIdx < 0 ? -shiftIdx : shiftIdx;

    bool bNoFindBottom = true;
    auto fbottom = static_cast<float>(m_pCenter.y);
    SCROLLEntity *pprev = m_pScroll;
    SCROLLEntity *pscr;
    for (pscr = m_pScroll; pscr != nullptr; pscr = pscr->next)
    {
        if (pscr->pCenter.y >= m_pCenter.y)
            pprev = pscr;
        else if (bNoFindBottom)
        {
            bNoFindBottom = false;
            fbottom = pprev->pCenter.y;
        }
        if (i == n)
            break;
        i++;
    }

    if (pscr != nullptr)
    {
        if (bNoFindBottom)
            fbottom -= pscr->pCenter.y;
        else
            fbottom = pscr->pCenter.y - fbottom;
        if (shiftIdx < 0)
            return -fbottom;
        return fbottom;
    }
    return 0.f;
}

XYRECT CXI_VIMAGESCROLL::GetCursorRect()
{
    XYRECT retVal;

    retVal.left = m_pCenter.x - m_ImageSize.x / 2;
    retVal.top = m_pCenter.y - m_ImageSize.y / 2;
    retVal.right = m_pCenter.x + m_ImageSize.x / 2;
    retVal.bottom = m_pCenter.y + m_ImageSize.y / 2;

    return retVal;
}

void CXI_VIMAGESCROLL::UpdateTexturesGroup()
{
    // m_sGroupName m_nGroupTex m_nGroupQuantity
    int i;

    char **pPrevGroup = m_sGroupName;
    long *prevTex = m_nGroupTex;
    const int nPrevQ = m_nGroupQuantity;

    // get textures
    ATTRIBUTES *pAttribute = core.Entity_GetAttributeClass(g_idInterface, m_nodeName);
    if (pAttribute == nullptr)
        return;

    ATTRIBUTES *pA = pAttribute->GetAttributeClass("ImagesGroup");
    if (pA != nullptr)
    {
        m_nGroupQuantity = pA->GetAttributesNum();
        if (m_nGroupQuantity != 0)
        {
            m_nGroupTex = new long[m_nGroupQuantity];
            m_sGroupName = new char *[m_nGroupQuantity];
            if (m_nGroupTex == nullptr || m_sGroupName == nullptr)
            {
                throw std::exception("allocate memory error");
            }
            for (i = 0; i < m_nGroupQuantity; i++)
            {
                char *stmp = pA->GetAttribute(i);
                if (stmp == nullptr)
                {
                    m_sGroupName[i] = nullptr;
                    m_nGroupTex[i] = -1;
                    continue;
                }

                const int itmp = FindTexGroupFromOld(pPrevGroup, stmp, nPrevQ);
                if (itmp != -1)
                {
                    m_sGroupName[i] = pPrevGroup[itmp];
                    m_nGroupTex[i] = prevTex[itmp];
                    pPrevGroup[itmp] = nullptr;
                    prevTex[itmp] = -1;
                }
                else
                {
                    const auto len = strlen(stmp) + 1;
                    m_sGroupName[i] = new char[len];
                    if (m_sGroupName[i] == nullptr)
                    {
                        throw std::exception("allocate memory error");
                    }
                    memcpy(m_sGroupName[i], stmp, len);
                    m_nGroupTex[i] = pPictureService->GetTextureID(m_sGroupName[i]);
                }
            }

            // delete old groups
            for (i = 0; i < nPrevQ; i++)
            {
                PICTURE_TEXTURE_RELEASE(pPictureService, pPrevGroup[i], prevTex[i]);
                STORM_DELETE(pPrevGroup[i]);
            }
            STORM_DELETE(pPrevGroup);
            STORM_DELETE(prevTex);
        }
    }
}

int CXI_VIMAGESCROLL::FindTexGroupFromOld(char **pGroupList, char *groupName, int listSize)
{
    if (pGroupList == nullptr || groupName == nullptr)
        return -1;
    for (int i = 0; i < listSize; i++)
    {
        if (pGroupList[i] != nullptr && _stricmp(pGroupList[i], groupName) == 0)
            return i;
    }
    return -1;
}

void CXI_VIMAGESCROLL::IMAGEDESCRIBE::Release(int nQnt, int nStr)
{
    int i;

    STORM_DELETE(bUseSpecTechnique);
    STORM_DELETE(tex);
    STORM_DELETE(ptex);
    STORM_DELETE(img);

    for (i = 0; i < nQnt; i++)
        STORM_DELETE(saveName[i]);
    STORM_DELETE(saveName);

    for (i = 0; i < nStr; i++)
    {
        strNum[i] = -1;
        STORM_DELETE(strSelf[i]);
    }
    STORM_DELETE(strNum);
    STORM_DELETE(strSelf);
}

void CXI_VIMAGESCROLL::IMAGEDESCRIBE::Clear(int nQnt, int nStr) const
{
    int i;

    for (i = 0; i < nQnt; i++)
    {
        bUseSpecTechnique[i] = false;
        tex[i] = -1;
        ptex[i] = nullptr;
        img[i] = -1;
        STORM_DELETE(saveName[i]);
    }

    for (i = 0; i < nStr; i++)
    {
        strNum[i] = -1;
        STORM_DELETE(strSelf[i]);
    }
}

uint32_t CXI_VIMAGESCROLL::MessageProc(long msgcode, MESSAGE &message)
{
    switch (msgcode)
    {
    case 0: // enable / disable display of the frame
        m_bShowBorder = message.Long() != 0;
        break;
    case 2:
        return GetMousePointedPictureNum();
        break;
    }

    return 0;
}

long CXI_VIMAGESCROLL::GetMousePointedPictureNum()
{
    const FXYPOINT mp = ptrOwner->GetMousePoint();
    if (mp.x < m_rect.left || mp.x > m_rect.right || mp.y < m_rect.top || mp.y > m_rect.bottom)
    {
        return -1;
    }

    const float curYCenter = m_pScroll ? m_pScroll->pCenter.y : (m_rect.top + m_rect.bottom) / 2;
    long n = 0;
    if (mp.y < curYCenter)
    {
        // number reduction
        float fTop = curYCenter - m_ImageSize.y * 0.5f;
        for (n = 0; mp.y < fTop; n--)
        {
            fTop -= m_nVDelta + m_ImageSize.y;
        }
    }
    else
    {
        // increase the number
        float fBottom = curYCenter + m_ImageSize.y * 0.5f;
        for (n = 0; mp.y > fBottom; n++)
        {
            fBottom += m_nVDelta + m_ImageSize.y;
        }
    }

    n += m_nCurImage;
    if (n >= m_nListSize)
        n -= m_nListSize;
    if (n < 0)
        n += m_nListSize;
    return n;
}
