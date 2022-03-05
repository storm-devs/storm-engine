#include "xservice.h"
#include "defines.h"
#include "dx9render.h"
#include "v_file_service.h"

#define ERROR_MUL 1.0f

static const char *LISTS_INIFILE = "resource\\ini\\interfaces\\pictures.ini";

XSERVICE::XSERVICE()
    : m_fWScale(0), m_fHScale(0), m_fWAdd(0), m_fHAdd(0)
{
    m_dwListQuantity = 0;
    m_dwImageQuantity = 0;
    m_pList = nullptr;
    m_pImage = nullptr;

    m_pRS = nullptr;
}

XSERVICE::~XSERVICE()
{
}

void XSERVICE::Init(VDX9RENDER *pRS, int32_t lWidth, int32_t lHeight)
{
    m_pRS = pRS;

    // get the size of the output window
    /*    D3DVIEWPORT9 vp;
      m_pRS->GetViewport(&vp);
      m_fWAdd = m_fWScale = (float)vp.Width/lWidth;
      m_fHAdd = m_fHScale = (float)vp.Height/lHeight;

      while(m_fWAdd>1.f) m_fWAdd-=1.f;
      while(m_fWAdd<0.f) m_fWAdd+=1.f;
      while(m_fHAdd>1.f) m_fHAdd-=1.f;
      while(m_fHAdd<0.f) m_fHAdd+=1.f;
      if(m_fWAdd>0.5f) m_fWAdd = 2.f-m_fWAdd*2.f;
      else    m_fWAdd *= 2.f;
      if(m_fHAdd>0.5f) m_fHAdd = 2.f-m_fHAdd*2.f;
      else    m_fHAdd *= 2.f;
      m_fWAdd *= ERROR_MUL;
      m_fHAdd *= ERROR_MUL;
    */
    m_fWAdd = 0.5f;
    m_fHAdd = 0.5f;

    LoadAllPicturesInfo();
}

int32_t XSERVICE::GetTextureID(const char *sImageListName)
{
    if (sImageListName != nullptr)
    {
        for (auto i = 0; i < m_dwListQuantity; i++)
            if (storm::iEquals(m_pList[i].sImageListName, sImageListName))
            {
                if (m_pList[i].textureQuantity <= 0)
                {
                    char sTexName[256];
                    sprintf_s(sTexName, "INTERFACES\\%s", m_pList[i].sTextureName);
                    m_pList[i].textureID = m_pRS->TextureCreate(sTexName);
                    m_pList[i].textureQuantity = 1;
                }
                else
                    m_pList[i].textureQuantity++;
                return m_pList[i].textureID;
            }
    }

    return -1;
}

int32_t XSERVICE::FindGroup(const char *sImageListName) const
{
    if (!sImageListName)
        return -1;
    for (auto n = 0; n < m_dwListQuantity; n++)
        if (storm::iEquals(m_pList[n].sImageListName, sImageListName))
            return n;
    return -1;
}

bool XSERVICE::ReleaseTextureID(const char *sImageListName)
{
    if (sImageListName == nullptr)
        return false;

    for (auto i = 0; i < m_dwListQuantity; i++)
        if (storm::iEquals(m_pList[i].sImageListName, sImageListName))
            if (--m_pList[i].textureQuantity == 0)
            {
                m_pRS->TextureRelease(m_pList[i].textureID);
                return true;
            }

    return false;
}

bool XSERVICE::GetTexturePos(int32_t pictureNum, FXYRECT &texRect)
{
    if (pictureNum >= 0 && pictureNum < m_dwImageQuantity)
    {
        // find picture group
        int gn;
        for (gn = 0; gn < m_dwListQuantity; gn++)
            if (pictureNum >= m_pList[gn].pictureStart &&
                pictureNum < m_pList[gn].pictureStart + m_pList[gn].pictureQuantity)
                break;
        if (gn < m_dwListQuantity)
        {
            texRect.left =
                static_cast<float>(m_pImage[pictureNum].pTextureRect.left + m_fWAdd) / m_pList[gn].textureWidth;
            texRect.right =
                static_cast<float>(m_pImage[pictureNum].pTextureRect.right - m_fWAdd) / m_pList[gn].textureWidth;
            texRect.top =
                static_cast<float>(m_pImage[pictureNum].pTextureRect.top + m_fHAdd) / m_pList[gn].textureHeight;
            texRect.bottom =
                static_cast<float>(m_pImage[pictureNum].pTextureRect.bottom - m_fHAdd) / m_pList[gn].textureHeight;
            return true;
        }
    }

    PZERO(&texRect, sizeof(texRect));
    return false;
}

bool XSERVICE::GetTexturePos(int32_t pictureNum, XYRECT &texRect)
{
    if (pictureNum >= 0 && pictureNum < m_dwImageQuantity)
    {
        memcpy(&texRect, &m_pImage[pictureNum].pTextureRect, sizeof(XYRECT));
        return true;
    }

    PZERO(&texRect, sizeof(texRect));
    return false;
}

bool XSERVICE::GetTexturePos(const char *sImageListName, const char *sImageName, FXYRECT &texRect)
{
    return GetTexturePos(GetImageNum(sImageListName, sImageName), texRect);
}

bool XSERVICE::GetTexturePos(const char *sImageListName, const char *sImageName, XYRECT &texRect)
{
    return GetTexturePos(GetImageNum(sImageListName, sImageName), texRect);
}

bool XSERVICE::GetTexturePos(int nTextureModify, int32_t pictureNum, FXYRECT &texRect)
{
    FXYRECT rectTmp;

    if (pictureNum >= 0 && pictureNum < m_dwImageQuantity)
    {
        // find picture group
        int gn;
        for (gn = 0; gn < m_dwListQuantity; gn++)
            if ((pictureNum >= m_pList[gn].pictureStart) &&
                (pictureNum < m_pList[gn].pictureStart + m_pList[gn].pictureQuantity))
                break;
        if (gn < m_dwListQuantity)
        {
            rectTmp.left = static_cast<float>(m_pImage[pictureNum].pTextureRect.left);
            rectTmp.top = static_cast<float>(m_pImage[pictureNum].pTextureRect.top);
            rectTmp.right = static_cast<float>(m_pImage[pictureNum].pTextureRect.right);
            rectTmp.bottom = static_cast<float>(m_pImage[pictureNum].pTextureRect.bottom);
            if (nTextureModify & TEXTURE_MODIFY_HORZFLIP)
            {
                const auto tmp = rectTmp.left + m_fWAdd * 2.f;
                rectTmp.left = rectTmp.right - m_fWAdd * 2.f;
                rectTmp.right = tmp;
            }
            if (nTextureModify & TEXTURE_MODIFY_VERTFLIP)
            {
                const auto tmp = rectTmp.top + m_fHAdd * 2.f;
                rectTmp.top = rectTmp.bottom - m_fHAdd * 2.f;
                rectTmp.bottom = tmp;
            }
            texRect.left = (rectTmp.left + m_fWAdd) / m_pList[gn].textureWidth;
            texRect.right = static_cast<float>(rectTmp.right - m_fWAdd) / m_pList[gn].textureWidth;
            texRect.top = (rectTmp.top + m_fHAdd) / m_pList[gn].textureHeight;
            texRect.bottom = static_cast<float>(rectTmp.bottom - m_fHAdd) / m_pList[gn].textureHeight;
            return true;
        }
    }

    PZERO(&texRect, sizeof(texRect));
    return false;
}

bool XSERVICE::GetTexturePos(int nTextureModify, const char *sImageListName, const char *sImageName, FXYRECT &texRect)
{
    return GetTexturePos(nTextureModify, GetImageNum(sImageListName, sImageName), texRect);
}

void XSERVICE::GetTextureCutForSize(const char *pcImageListName, const FXYPOINT &pntLeftTopUV, const XYPOINT &pntSize,
                                    int32_t nSrcWidth, int32_t nSrcHeight, FXYRECT &outUV)
{
    const auto n = FindGroup(pcImageListName);
    if (n >= 0)
    {
        if (nSrcWidth < m_pList[n].textureWidth)
            nSrcWidth = m_pList[n].textureWidth;
        if (nSrcHeight < m_pList[n].textureHeight)
            nSrcHeight = m_pList[n].textureHeight;
    }
    auto fW = 1.f;
    if (nSrcWidth > 0)
        fW = static_cast<float>(pntSize.x) / nSrcWidth + pntLeftTopUV.x;
    auto fH = 1.f;
    if (nSrcHeight > 0)
        fH = static_cast<float>(pntSize.y) / nSrcHeight + pntLeftTopUV.y;
    if (fW > 1.f)
        fW = 1.f;
    if (fH > 1.f)
        fH = 1.f;
    outUV.left = pntLeftTopUV.x;
    outUV.top = pntLeftTopUV.y;
    outUV.right = fW;
    outUV.bottom = fH;
}

void XSERVICE::LoadAllPicturesInfo()
{
    char section[255];
    char param[255];

    // initialize ini file
    auto ini = fio->OpenIniFile(LISTS_INIFILE);
    if (!ini)
    {
        throw std::runtime_error("ini file not found!");
    }

    m_dwListQuantity = 0;
    m_dwImageQuantity = 0;

    // calculate lists quantity
    if (ini->GetSectionName(section, sizeof(section) - 1))
    {
        do
        {
            m_dwListQuantity++;
        } while (ini->GetSectionNameNext(section, sizeof(section) - 1));
    }
    // create list pointers array
    if (m_dwListQuantity > 0)
    {
        m_pList = new IMAGELISTDESCR[m_dwListQuantity];
        if (m_pList == nullptr)
        {
            throw std::runtime_error("memory allocate error");
        }
    }

    // fill lists
    if (ini->GetSectionName(section, sizeof(section) - 1))
    {
        for (auto i = 0; true; i++)
        {
            m_pList[i].textureQuantity = 0;
            m_pList[i].textureID = -1L;

            // get list name
            m_pList[i].sImageListName = new char[sizeof section];
            strcpy_s(m_pList[i].sImageListName, sizeof section, section);
            // get texture name
            ini->ReadString(section, "sTextureName", param, sizeof(param) - 1, "");
            m_pList[i].sTextureName = new char[sizeof param];
            strcpy_s(m_pList[i].sTextureName, sizeof param, param);

            // get texture width & height
            m_pList[i].textureWidth = ini->GetInt(section, "wTextureWidth", 1024);
            m_pList[i].textureHeight = ini->GetInt(section, "wTextureHeight", 1024);

            m_pList[i].pictureStart = m_dwImageQuantity;
            // get pictures quantity
            m_pList[i].pictureQuantity = 0;
            if (ini->ReadString(section, "picture", param, sizeof(param) - 1, ""))
                do
                {
                    m_pList[i].pictureQuantity++;
                } while (ini->ReadStringNext(section, "picture", param, sizeof(param) - 1));

            // resize image list
            auto *const oldpImage = m_pImage;
            m_pImage = new PICTUREDESCR[m_dwImageQuantity + m_pList[i].pictureQuantity];
            if (m_pImage == nullptr)
                throw std::runtime_error("allocate memory error");
            if (oldpImage != nullptr)
            {
                memcpy(m_pImage, oldpImage, m_dwImageQuantity * sizeof(PICTUREDESCR));
                delete oldpImage;
            }
            m_dwImageQuantity += m_pList[i].pictureQuantity;

            // set pictures
            char picName[sizeof(param)];
            ini->ReadString(section, "picture", param, sizeof(param) - 1, "");
            for (int j = m_pList[i].pictureStart; j < m_dwImageQuantity; j++)
            {
                // get texture coordinates
                int nLeft, nTop, nRight, nBottom;

                sscanf(param, "%[^,],%d,%d,%d,%d", picName, &nLeft, &nTop, &nRight, &nBottom);
                m_pImage[j].pTextureRect.left = nLeft;
                m_pImage[j].pTextureRect.top = nTop;
                m_pImage[j].pTextureRect.right = nRight;
                m_pImage[j].pTextureRect.bottom = nBottom;

                const auto len = strlen(picName) + 1;
                m_pImage[j].sPictureName = new char[len];
                memcpy(m_pImage[j].sPictureName, picName, len);

                ini->ReadStringNext(section, "picture", param, sizeof(param) - 1);
            }

            if (!ini->GetSectionNameNext(section, sizeof(section) - 1))
                break;
        }
    }
}

void XSERVICE::ReleaseAll()
{
    if (m_pList != nullptr)
    {
        for (auto i = 0; i < m_dwListQuantity; i++)
        {
            if (m_pList[i].textureQuantity != 0)
                m_pRS->TextureRelease(m_pList[i].textureID);

            delete m_pList[i].sImageListName;

            delete m_pList[i].sTextureName;
        }

        delete m_pList;
    }

    if (m_pImage != nullptr)
    {
        for (auto i = 0; i < m_dwImageQuantity; i++)
        {
            delete m_pImage[i].sPictureName;
        }

        delete m_pImage;
    }

    m_dwListQuantity = 0;
    m_dwImageQuantity = 0;
}

int32_t XSERVICE::GetImageNum(const char *sImageListName, const char *sImageName)
{
    int32_t retVal = -1;

    if (sImageName != nullptr)
        if (sImageListName != nullptr)
        {
            for (int i = 0; i < m_dwListQuantity; i++)
                if (storm::iEquals(m_pList[i].sImageListName, sImageListName))
                {
                    for (int j = m_pList[i].pictureStart; j < m_pList[i].pictureStart + m_pList[i].pictureQuantity; j++)
                        if (storm::iEquals(m_pImage[j].sPictureName, sImageName))
                        {
                            retVal = j;
                            break;
                        }
                    break;
                }
        }
        else
        {
            for (int i = 0; i < m_dwImageQuantity; i++)
                if (storm::iEquals(m_pImage[i].sPictureName, sImageName))
                {
                    retVal = i;
                    break;
                }
        }

    return retVal;
}

//============================================================================
//    COMBINE STRINGS CLASS
//============================================================================

ComboString::ComboString()
{
}

ComboString::~ComboString()
{
}

void ComboString::AddToCombo(char *fontName, const XYPOINT &posStrStart, char *str)
{
}

void ComboString::AddToCombo(XYRECT posPic, char *picTexName, FXYRECT picUV)
{
}

void ComboString::PrintComboString(int comboStrID)
{
}

int ComboString::GetComboString(int align, int x, int y, int needWidth, int needHeight, int *allHeight, char *formatStr,
                                char *fontlist)
{
    return -1;
}

void ComboString::ComboStringRelease(int comboStrID)
{
}
