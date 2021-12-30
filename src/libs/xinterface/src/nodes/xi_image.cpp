#include "xi_image.h"
#include "../xinterface.h"

#define DEFAULT_IMAGE_OBJECT_WIDTH 32
#define DEFAULT_IMAGE_OBJECT_HEIGHT 32
#define DEFAULT_IMAGE_OBJECT_COLOR 0xFF808080

CXI_IMAGE::CXI_IMAGE()
{
    m_rs = XINTERFACE::GetRenderService();
    if (!m_rs)
    {
        throw std::runtime_error("No service: dx9render");
    }

    m_bDisableDraw = false;

    m_pcPictureListName = nullptr;
    m_nPictureNum = -1;

    m_nTextureID = -1;
    m_pTexture = nullptr;

    m_frUV.left = 0.f;
    m_frUV.top = 0.f;
    m_frUV.right = 1.f;
    m_frUV.bottom = 1.f;

    SetColor(DEFAULT_IMAGE_OBJECT_COLOR);

    m_pntSize.x = DEFAULT_IMAGE_OBJECT_WIDTH;
    m_pntSize.y = DEFAULT_IMAGE_OBJECT_HEIGHT;

    m_pntLeftTop.x = m_pntLeftTop.y = 0;

    m_bThisIsColorRectangle = false;
}

CXI_IMAGE::~CXI_IMAGE()
{
    Unload();
}

void CXI_IMAGE::LoadFromFile(const char *sFileName)
{
    Unload();
    m_nTextureID = m_rs->TextureCreate(sFileName);
    UpdateTexture();
    UpdatePosition();
}

void CXI_IMAGE::LoadFromBase(const char *sListName, const char *sPictureName, bool bGetSizeFromSource)
{
    Unload();

    if (sListName)
    {
        const auto len = strlen(sListName) + 1;
        m_pcPictureListName = new char[len];
        if (!m_pcPictureListName)
        {
            throw std::runtime_error("allocate memory error");
        }
        memcpy(m_pcPictureListName, sListName, len);
    }
    m_nTextureID = XINTERFACE::GetPictureService()->GetTextureID(m_pcPictureListName);
    m_nPictureNum = XINTERFACE::GetPictureService()->GetImageNum(m_pcPictureListName, sPictureName);

    if (bGetSizeFromSource)
    {
        XYRECT rPos;
        if (XINTERFACE::GetPictureService()->GetTexturePos(m_nPictureNum, rPos))
        {
            m_pntSize.x = rPos.right - rPos.left;
            m_pntSize.y = rPos.bottom - rPos.top;
        }
    }

    UpdateTexture();
    UpdatePosition();
}

void CXI_IMAGE::LoadAccordingToString(const char *pcImageParam)
{
    if (!pcImageParam)
        return;
    char tokenID[512];
    char tokenString[512];
    auto bSetColor = false;
    m_bThisIsColorRectangle = true;
    for (auto *pcParam = (char *)pcImageParam; pcParam && pcParam[0];)
    {
        if (nullptr == CXI_UTILS::StringGetTokenID(pcParam, tokenID, sizeof(tokenID)))
            break;
        const auto nTokenCode = CXI_UTILS::StringGetTokenCode(tokenID);
        switch (nTokenCode)
        {
        case InterfaceToken_color:
            bSetColor = true;
            if (CXI_UTILS::StringGetTokenString(pcParam, tokenString, sizeof(tokenString)))
            {
                m_dwImageColor = CXI_UTILS::StringGetColor(tokenString);
                SetColor(m_dwImageColor);
            }
            break;
        case InterfaceToken_file:
            if (CXI_UTILS::StringGetTokenString(pcParam, tokenString, sizeof(tokenString)))
            {
                m_nTextureID = m_rs->TextureCreate(tokenString);
                UpdateTexture();
            }
            break;
        case InterfaceToken_picture_list:
            m_bThisIsColorRectangle = false;
            if (CXI_UTILS::StringGetTokenString(pcParam, tokenString, sizeof(tokenString)))
            {
                CXI_UTILS::StringDoublicate(tokenString, m_pcPictureListName);
                m_nTextureID = XINTERFACE::GetPictureService()->GetTextureID(m_pcPictureListName);
            }
            break;
        case InterfaceToken_picture_name:
            m_bThisIsColorRectangle = false;
            if (CXI_UTILS::StringGetTokenString(pcParam, tokenString, sizeof(tokenString)))
            {
                m_nPictureNum = XINTERFACE::GetPictureService()->GetImageNum(m_pcPictureListName, tokenString);
                UpdateTexture();
            }
            break;
        case InterfaceToken_picture_cut_uv:
            m_bThisIsColorRectangle = false;
            m_nLeftTopCutUV.x = CXI_UTILS::StringGetFloat((const char *&)pcParam);
            m_nLeftTopCutUV.y = CXI_UTILS::StringGetFloat((const char *&)pcParam);
            UpdateTexture();
            break;
        case InterfaceToken_size:
            if (CXI_UTILS::StringGetTokenString(pcParam, tokenString, sizeof(tokenString)))
            {
                CXI_UTILS::StringTwoLong(tokenString, m_pntSize.x, m_pntSize.y);
                SetSize(m_pntSize.x, m_pntSize.y);
            }
            break;
        case InterfaceToken_rectUV:
            if (CXI_UTILS::StringGetTokenString(pcParam, tokenString, sizeof(tokenString)))
            {
                CXI_UTILS::StringFourFloat(tokenString, m_frUV.left, m_frUV.top, m_frUV.right, m_frUV.bottom);
                UpdateTexture();
            }
            break;
        case InterfaceToken_pos:
            if (CXI_UTILS::StringGetTokenString(pcParam, tokenString, sizeof(tokenString)))
            {
                CXI_UTILS::StringTwoLong(tokenString, m_pntLeftTop.x, m_pntLeftTop.y);
                SetPosition(m_pntLeftTop.x, m_pntLeftTop.y);
            }
            break;
        }
    }

    // if there was no setting of color and pictures, then do not consider it as a colored rectangle
    if (m_bThisIsColorRectangle && !bSetColor)
        m_bThisIsColorRectangle = false;
}

void CXI_IMAGE::SetPointerToTexture(IDirect3DTexture9 *pTex)
{
    Unload();
    if (!pTex)
        return;
    m_pTexture = pTex;
    pTex->AddRef();
}

void CXI_IMAGE::Draw()
{
    if (m_bDisableDraw)
        return;

    if (m_bThisIsColorRectangle)
    {
        m_rs->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, XI_ONETEX_FVF, 2, m_vrtx, sizeof(XI_ONETEX_VERTEX), "iRectangle");
        return;
    }

    if (IsImagePresent())
    {
        if (m_pTexture)
            m_rs->SetTexture(0, m_pTexture);
        else
            m_rs->TextureSet(0, m_nTextureID);
        m_rs->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, XI_ONETEX_FVF, 2, m_vrtx, sizeof(XI_ONETEX_VERTEX), "iVideo");
    }
}

void CXI_IMAGE::Draw(long nX, long nY, ImagePointType alignment)
{
    auto nLeft = nX;
    auto nTop = nY;
    if (alignment == IPType_Center)
    {
        nLeft -= m_pntSize.x / 2;
        nTop -= m_pntSize.y / 2;
    }
    else if (alignment == IPType_RightTop)
    {
        nLeft -= m_pntSize.x;
    }

    if (m_pntLeftTop.x != nLeft || m_pntLeftTop.y != nTop)
        SetPosition(nLeft, nTop);

    Draw();
}

void CXI_IMAGE::SetSize(long nWidth, long nHeight)
{
    m_pntSize.x = nWidth;
    m_pntSize.y = nHeight;

    UpdatePosition();
    if (m_nPictureNum == -1 && m_pcPictureListName)
        UpdateTexture();
}

void CXI_IMAGE::SetPosition(long nX, long nY, ImagePointType ptype)
{
    switch (ptype)
    {
    case IPType_LeftTop:
        m_pntLeftTop.x = nX;
        m_pntLeftTop.y = nY;
        break;
    case IPType_Center:
        m_pntLeftTop.x = nX - m_pntSize.x / 2;
        m_pntLeftTop.y = nY - m_pntSize.y / 2;
        break;
    case IPType_RightTop:
        m_pntLeftTop.x = nX - m_pntSize.x;
        m_pntLeftTop.y = nY;
        break;
    }

    UpdatePosition();
}

void CXI_IMAGE::SetPosition(XYRECT &pos)
{
    const auto bDoTextureUpdate = (m_pntSize.x != pos.right - pos.left) || (m_pntSize.y != pos.bottom - pos.top);
    if (m_pntLeftTop.x != pos.left || m_pntLeftTop.y != pos.top || bDoTextureUpdate)
    {
        m_pntLeftTop.x = pos.left;
        m_pntLeftTop.y = pos.top;
        m_pntSize.x = pos.right - pos.left;
        m_pntSize.y = pos.bottom - pos.top;
        UpdatePosition();
        if (bDoTextureUpdate && m_nPictureNum == -1 && m_pcPictureListName)
            UpdateTexture();
    }
}

void CXI_IMAGE::SetDifferentPosition(long nLeft, long nTop, long nWidth, long nHeight, ImagePointType ptype)
{
    long nX = nLeft;
    long nY = nTop;
    switch (ptype)
    {
    case IPType_Center:
        nX -= nWidth / 2;
        nY -= nHeight / 2;
        break;
    case IPType_RightTop:
        nX -= nWidth;
        break;
    }

    const bool bDoTextureUpdate = (nWidth != m_pntSize.x || nHeight != m_pntSize.y);
    if (nX != m_pntLeftTop.x || nY != m_pntLeftTop.y || bDoTextureUpdate)
    {
        m_pntLeftTop.x = nX;
        m_pntLeftTop.y = nY;
        m_pntSize.x = nWidth;
        m_pntSize.y = nHeight;

        UpdatePosition();
        if (bDoTextureUpdate && m_nPictureNum == -1 && m_pcPictureListName)
            UpdateTexture();
    }
}

void CXI_IMAGE::SetUV(float fLeft, float fTop, float fRight, float fBottom)
{
    m_frUV.left = fLeft;
    m_frUV.top = fTop;
    m_frUV.right = fRight;
    m_frUV.bottom = fBottom;

    UpdateTexture();
}

void CXI_IMAGE::SetColor(uint32_t dwColor)
{
    m_dwImageColor = dwColor;
    for (long n = 0; n < 4; n++)
        m_vrtx[n].color = m_dwImageColor;
}

void CXI_IMAGE::Unload()
{
    if (m_nTextureID != -1)
    {
        m_nPictureNum = -1;
        if (m_pcPictureListName)
        {
            PICTURE_TEXTURE_RELEASE(XINTERFACE::GetPictureService(), m_pcPictureListName, m_nTextureID);
        }
        else
        {
            TEXTURE_RELEASE(m_rs, m_nTextureID);
        }
    }
    RELEASE(m_pTexture);
    STORM_DELETE(m_pcPictureListName);
}

bool CXI_IMAGE::IsPointInside(long nX, long nY) const
{
    if (nX < m_pntLeftTop.x || nY < m_pntLeftTop.y || nX > m_pntLeftTop.x + m_pntSize.x ||
        nY > m_pntLeftTop.y + m_pntSize.y)
        return false;
    return true;
}

void CXI_IMAGE::UpdateTexture()
{
    FXYRECT frTex;
    if (m_nPictureNum != -1)
    {
        XINTERFACE::GetPictureService()->GetTexturePos(m_nPictureNum, frTex);
    }
    else
    {
        if (m_pcPictureListName)
        {
            XINTERFACE::GetPictureService()->GetTextureCutForSize(m_pcPictureListName, m_nLeftTopCutUV, m_pntSize, -1,
                                                                  -1, frTex);
        }
        else
            frTex = m_frUV;
    }

    m_vrtx[1].tu = m_vrtx[0].tu = frTex.left;
    m_vrtx[2].tu = m_vrtx[3].tu = frTex.right;
    m_vrtx[2].tv = m_vrtx[0].tv = frTex.top;
    m_vrtx[3].tv = m_vrtx[1].tv = frTex.bottom;
}

void CXI_IMAGE::UpdatePosition()
{
    m_vrtx[0].pos.x = static_cast<float>(m_pntLeftTop.x);
    m_vrtx[0].pos.y = static_cast<float>(m_pntLeftTop.y);
    m_vrtx[0].pos.z = 1.f;

    m_vrtx[1].pos.x = static_cast<float>(m_pntLeftTop.x);
    m_vrtx[1].pos.y = static_cast<float>(m_pntLeftTop.y + m_pntSize.y);
    m_vrtx[1].pos.z = 1.f;

    m_vrtx[2].pos.x = static_cast<float>(m_pntLeftTop.x + m_pntSize.x);
    m_vrtx[2].pos.y = static_cast<float>(m_pntLeftTop.y);
    m_vrtx[2].pos.z = 1.f;

    m_vrtx[3].pos.x = static_cast<float>(m_pntLeftTop.x + m_pntSize.x);
    m_vrtx[3].pos.y = static_cast<float>(m_pntLeftTop.y + m_pntSize.y);
    m_vrtx[3].pos.z = 1.f;
}
