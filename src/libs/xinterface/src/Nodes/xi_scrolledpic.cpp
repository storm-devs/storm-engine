#include "xi_scrolledpic.h"
#include <stdio.h>

CXI_SCROLLEDPICTURE::CXI_SCROLLEDPICTURE()
{
    m_nNodeType = NODETYPE_SCROLLEDPICTURE;
}

CXI_SCROLLEDPICTURE::~CXI_SCROLLEDPICTURE()
{
    ReleaseAll();
}

void CXI_SCROLLEDPICTURE::Draw(bool bSelected, uint32_t Delta_Time)
{
    CXI_PICTURE::Draw(bSelected, Delta_Time);

    for (long n = 0; n < m_aImg.size(); n++)
    {
        if (m_aImg[n].bShow)
        {
            m_aImg[n].pImg->Draw();
        }
    }
}

bool CXI_SCROLLEDPICTURE::Init(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2, VDX9RENDER *rs,
                               XYRECT &hostRect, XYPOINT &ScreenSize)
{
    const auto bSuccess = CXI_PICTURE::Init(ini1, name1, ini2, name2, rs, hostRect, ScreenSize);
    return bSuccess;
}

void CXI_SCROLLEDPICTURE::LoadIni(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2)
{
    CXI_PICTURE::LoadIni(ini1, name1, ini2, name2);

    m_fpBaseSize.x = m_fpBaseSize.y = 1000.f;
    m_fpBaseSize = GetIniFloatPoint(ini1, name1, ini2, name2, "basesize", m_fpBaseSize);
    if (m_fpBaseSize.x <= 0.f)
        m_fpBaseSize.x = 1.f;
    if (m_fpBaseSize.y <= 0.f)
        m_fpBaseSize.y = 1.f;

    long n;
    char keyName[128];
    m_aScale.clear();
    for (n = 1; n < 20; n++)
    {
        sprintf_s(keyName, "scale%d", n);
        FXYPOINT fpTemp;
        fpTemp.x = fpTemp.y = 2.f;
        fpTemp = GetIniFloatPoint(ini1, name1, ini2, name2, keyName, fpTemp);
        if (fpTemp.x > 1.f || fpTemp.y > 1.f)
            break; // not read or error
        m_aScale.push_back(fpTemp);
    }

    m_nScaleNum = GetIniLong(ini1, name1, ini2, name2, "startscale", 0) - 1;

    auto *pAttribute = core.Entity_GetAttributeClass(g_idInterface, m_nodeName);
    if (pAttribute)
    {
        auto *pAttr = pAttribute->GetAttributeClass("imagelist");
        if (pAttr)
        {
            const long q = pAttr->GetAttributesNum();
            for (n = 0; n < q; n++)
            {
                auto *pA = pAttr->GetAttributeClass(n);
                if (pA)
                {
                    // long i = m_aImg.Add();
                    m_aImg.push_back(BuildinImage{});
                    const long i = m_aImg.size() - 1;
                    m_aImg[i].bShow = false;
                    m_aImg[i].fpPos.x = pA->GetAttributeAsFloat("x", 0.f);
                    m_aImg[i].fpPos.y = pA->GetAttributeAsFloat("y", 0.f);
                    m_aImg[i].fpSize.x = pA->GetAttributeAsFloat("width", -1.f);
                    m_aImg[i].fpSize.y = pA->GetAttributeAsFloat("height", -1.f);

                    m_aImg[i].pImg = new CXI_IMAGE;
                    Assert(m_aImg[i].pImg);
                    const char *pcGroupName = pA->GetAttribute("group");
                    if (pcGroupName)
                    {
                        m_aImg[i].pImg->LoadFromBase(pcGroupName, pA->GetAttribute("pic"), true);
                        if (m_aImg[i].fpSize.x > 0.f && m_aImg[i].fpSize.y > 0.f)
                            m_aImg[i].pImg->SetSize(static_cast<long>(m_aImg[i].fpSize.x),
                                                    static_cast<long>(m_aImg[i].fpSize.y));
                    }
                    else
                    {
                        m_aImg[i].pImg->LoadFromFile(pA->GetAttribute("file"));
                    }
                    m_aImg[i].fpSize.x = static_cast<float>(m_aImg[i].pImg->GetWidth());
                    m_aImg[i].fpSize.y = static_cast<float>(m_aImg[i].pImg->GetHeight());
                }
            }
        }

        m_nScaleNum = pAttribute->GetAttributeAsDword("scale", m_nScaleNum);

        const auto fx = pAttribute->GetAttributeAsFloat("centerX", m_fpBaseSize.x * .5f);
        const auto fy = pAttribute->GetAttributeAsFloat("centerY", m_fpBaseSize.y * .5f);
        SetPosToCenter(fx, fy);
    }

    SetScale(m_nScaleNum);
}

void CXI_SCROLLEDPICTURE::ReleaseAll()
{
    CXI_PICTURE::ReleaseAll();
}

int CXI_SCROLLEDPICTURE::CommandExecute(int wActCode)
{
    return -1;
}

bool CXI_SCROLLEDPICTURE::IsClick(int buttonID, long xPos, long yPos)
{
    if (buttonID == MOUSE_RBUTTON)
    {
        if (xPos >= m_rect.left && xPos <= m_rect.right && yPos >= m_rect.top && yPos <= m_rect.bottom)
        {
            m_nScaleNum++;
            if (m_nScaleNum >= m_aScale.size())
                m_nScaleNum = 0;
            SetScale(m_nScaleNum);
        }
    }
    return true;
}

void CXI_SCROLLEDPICTURE::ChangePosition(XYRECT &rNewPos)
{
    CXI_PICTURE::ChangePosition(rNewPos);
    RecalculateTexPerPixel();
    UpdateBuildenImages();
}

void CXI_SCROLLEDPICTURE::SaveParametersToIni()
{
    CXI_PICTURE::SaveParametersToIni();
}

void CXI_SCROLLEDPICTURE::SetNewPicture(bool video, char *sNewTexName)
{
    CXI_PICTURE::SetNewPicture(video, sNewTexName);
}

void CXI_SCROLLEDPICTURE::SetNewPictureFromDir(char *dirName)
{
    CXI_PICTURE::SetNewPictureFromDir(dirName);
}

uint32_t CXI_SCROLLEDPICTURE::MessageProc(long msgcode, MESSAGE &message)
{
    switch (msgcode)
    {
    case 10: // Set a new center
    {
        const auto fx = message.Float();
        const auto fy = message.Float();
        SetPosToCenter(fx, fy);
    }
        return 0;
        break;
    }
    return CXI_PICTURE::MessageProc(msgcode, message);
}

void CXI_SCROLLEDPICTURE::MoveMouseOutScreen(float fX, float fY)
{
    if (fX == 0.f && fY == 0.f)
        return;

    auto fDeltaU = fX * m_fUTexPerPixel;
    auto fDeltaV = fY * m_fVTexPerPixel;

    if (m_v[0].tu + fDeltaU < 0.f)
        fDeltaU = -m_v[0].tu;
    if (m_v[0].tv + fDeltaV < 0.f)
        fDeltaV = -m_v[0].tv;

    if (m_v[3].tu + fDeltaU > 1.f)
        fDeltaU = 1.f - m_v[3].tu;
    if (m_v[3].tv + fDeltaV > 1.f)
        fDeltaV = 1.f - m_v[3].tv;

    m_v[0].tu = (m_v[1].tu += fDeltaU);
    m_v[2].tu = (m_v[3].tu += fDeltaU);
    m_v[0].tv = (m_v[2].tv += fDeltaV);
    m_v[1].tv = (m_v[3].tv += fDeltaV);

    UpdateBuildenImages();
}

void CXI_SCROLLEDPICTURE::ChangeUV(FXYRECT &frNewUV)
{
    CXI_PICTURE::ChangeUV(frNewUV);
    RecalculateTexPerPixel();
    UpdateBuildenImages();
    auto *pAttribute = core.Entity_GetAttributeClass(g_idInterface, m_nodeName);
    if (pAttribute)
    {
        auto *pA = pAttribute->GetAttributeClass("offset");
        if (!pA)
            pA = pAttribute->CreateSubAClass(pAttribute, "offset");
        if (pA)
        {
            pA->SetAttributeUseFloat("x", frNewUV.left * m_fpBaseSize.x);
            pA->SetAttributeUseFloat("y", frNewUV.top * m_fpBaseSize.y);
        }
        pA = pAttribute->GetAttributeClass("size");
        if (!pA)
            pA = pAttribute->CreateSubAClass(pAttribute, "size");
        if (pA)
        {
            pA->SetAttributeUseFloat("x", (frNewUV.right - frNewUV.left) * m_fpBaseSize.x);
            pA->SetAttributeUseFloat("y", (frNewUV.bottom - frNewUV.top) * m_fpBaseSize.y);
        }
        pA = pAttribute->GetAttributeClass("scale");
        if (!pA)
            pA = pAttribute->CreateSubAClass(pAttribute, "scale");
        if (pA)
        {
            pA->SetAttributeUseFloat("x", (frNewUV.right - frNewUV.left) * m_fpBaseSize.x /
                                              static_cast<float>(m_rect.right - m_rect.left));
            pA->SetAttributeUseFloat("y", (frNewUV.bottom - frNewUV.top) * m_fpBaseSize.y /
                                              static_cast<float>(m_rect.bottom - m_rect.top));
        }
    }
}

void CXI_SCROLLEDPICTURE::RecalculateTexPerPixel()
{
    if (m_rect.right - m_rect.left <= 0)
        m_fUTexPerPixel = 0.f;
    else
        m_fUTexPerPixel = (m_v[3].tu - m_v[0].tu) / (m_rect.right - m_rect.left);

    if (m_rect.bottom - m_rect.top <= 0)
        m_fVTexPerPixel = 0.f;
    else
        m_fVTexPerPixel = (m_v[3].tv - m_v[0].tv) / (m_rect.bottom - m_rect.top);
}

void CXI_SCROLLEDPICTURE::UpdateBuildenImages()
{
    for (long n = 0; n < m_aImg.size(); n++)
    {
        m_aImg[n].bShow = false;

        auto fx = m_aImg[n].fpPos.x / m_fpBaseSize.x;
        auto fy = m_aImg[n].fpPos.y / m_fpBaseSize.y;
        if (fx < m_v[0].tu || fy < m_v[0].tv || fx > m_v[3].tu || fy > m_v[3].tv || m_fUTexPerPixel <= 0.f ||
            m_fVTexPerPixel <= 0.f)
            continue;
        fx = m_v[0].pos.x + (fx - m_v[0].tu) / m_fUTexPerPixel;
        fy = m_v[0].pos.y + (fy - m_v[0].tv) / m_fVTexPerPixel;
        m_aImg[n].pImg->SetPosition(static_cast<long>(fx), static_cast<long>(fy), IPType_Center);
        m_aImg[n].bShow = true;
    }
}

void CXI_SCROLLEDPICTURE::SetPosToCenter(float fX, float fY)
{
    RecalculateTexPerPixel();

    auto fLeft = fX / m_fpBaseSize.x;
    auto fTop = fY / m_fpBaseSize.y;
    const auto fWidth = (m_rect.right - m_rect.left) * m_fUTexPerPixel;
    const auto fHeight = (m_rect.bottom - m_rect.top) * m_fVTexPerPixel;

    if (fLeft <= fWidth * .5f)
        fLeft = 0.f;
    else
        fLeft -= fWidth * .5f;
    if (fTop <= fHeight * .5f)
        fTop = 0.f;
    else
        fTop -= fHeight * .5f;

    if (fLeft + fWidth > 1.f)
        fLeft = 1.f - fWidth;
    if (fTop + fHeight > 1.f)
        fTop = 1.f - fHeight;

    FXYRECT frNewUV;
    frNewUV.left = fLeft;
    frNewUV.top = fTop;
    frNewUV.right = fLeft + fWidth;
    frNewUV.bottom = fTop + fHeight;

    ChangeUV(frNewUV);
}

void CXI_SCROLLEDPICTURE::SetScale(long nScaleIdx)
{
    if (nScaleIdx < 0 || nScaleIdx >= m_aScale.size())
        return;
    SetScale(m_aScale[nScaleIdx].x, m_aScale[nScaleIdx].y);
}

void CXI_SCROLLEDPICTURE::SetScale(float fsx, float fsy)
{
    if (fsx < 0.f)
        fsx = 0.f;
    if (fsx > 1.f)
        fsx = 1.f;
    if (fsy < 0.f)
        fsy = 0.f;
    if (fsy > 1.f)
        fsy = 1.f;

    const auto fCenterX = (m_v[3].tu + m_v[0].tu) * .5f;
    const auto fCenterY = (m_v[3].tv + m_v[0].tv) * .5f;

    auto fLeft = fCenterX - fsx * .5f;
    auto fTop = fCenterY - fsy * .5f;
    if (fLeft < 0.f)
        fLeft = 0.f;
    if (fTop < 0.f)
        fTop = 0.f;
    if (fLeft + fsx > 1.f)
        fLeft = 1.f - fsx;
    if (fTop + fsy > 1.f)
        fTop = 1.f - fsy;

    FXYRECT frNewUV;
    frNewUV.left = fLeft;
    frNewUV.top = fTop;
    frNewUV.right = fLeft + fsx;
    frNewUV.bottom = fTop + fsy;
    ChangeUV(frNewUV);
}
