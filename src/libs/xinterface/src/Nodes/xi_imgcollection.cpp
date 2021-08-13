#include "xi_imgcollection.h"

CXI_IMGCOLLECTION::CXI_IMGCOLLECTION()
    : m_bRelativeRect(false)
{
    texl = -1;
    vBuf = -1;
    iBuf = -1;
    nVert = 0;
    nIndx = 0;
    m_rs = nullptr;
    sGroupName = nullptr;
    m_nNodeType = NODETYPE_IMAGECOLLECTION;
    m_nCurSection = 0;
    m_xyCommonOffset.x = 0;
    m_xyCommonOffset.y = 0;
}

CXI_IMGCOLLECTION::~CXI_IMGCOLLECTION()
{
    ReleaseAll();
}

void CXI_IMGCOLLECTION::Draw(bool bSelected, uint32_t Delta_Time)
{
    if (m_bUse)
    {
        m_rs->TextureSet(0, texl);
        m_rs->DrawBuffer(vBuf, sizeof(XI_ONETEX_VERTEX), iBuf, 0, nVert, 0, nIndx, "iImageCollection");
    }
}

bool CXI_IMGCOLLECTION::Init(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2, VDX9RENDER *rs,
                             XYRECT &hostRect, XYPOINT &ScreenSize)
{
    if (!CINODE::Init(ini1, name1, ini2, name2, rs, hostRect, ScreenSize))
        return false;
    // screen position for that is host screen position
    memcpy(&m_rect, &m_hostRect, sizeof(m_hostRect));
    SetGlowCursor(false);
    return true;
}

void CXI_IMGCOLLECTION::ReleaseAll()
{
    PICTURE_TEXTURE_RELEASE(pPictureService, sGroupName, texl);
    VERTEX_BUFFER_RELEASE(m_rs, vBuf);
    INDEX_BUFFER_RELEASE(m_rs, iBuf);
    STORM_DELETE(sGroupName);
}

int CXI_IMGCOLLECTION::CommandExecute(int wActCode)
{
    return -1;
}

void CXI_IMGCOLLECTION::AddImage(const char *pcPicName, uint32_t dwColor, XYRECT pos)
{
    // long n = m_aEditInfo;
    // m_aEditInfo.Add();
    PicEditInfo info;
    info.dwColor = dwColor;
    info.sName = pcPicName;
    info.nLeft = pos.left;
    info.nTop = pos.top;
    info.nRight = pos.right;
    info.nBottom = pos.bottom;
    info.bNative = false;
    m_aEditInfo.push_back(info);

    // redrawing the index and vertex buffers
    VERTEX_BUFFER_RELEASE(m_rs, vBuf);
    INDEX_BUFFER_RELEASE(m_rs, iBuf);

    // Calculate vertex and index quantity
    nVert = m_aEditInfo.size() * 4;
    nIndx = m_aEditInfo.size() * 6;
    // Create vertex and index buffers
    vBuf = m_rs->CreateVertexBuffer(XI_ONETEX_FVF, nVert * sizeof(XI_ONETEX_VERTEX), D3DUSAGE_WRITEONLY);
    iBuf = m_rs->CreateIndexBuffer(nIndx * 2);
    nIndx /= 3;

    UpdateBuffers();
}

void CXI_IMGCOLLECTION::LoadIni(INIFILE *ini1, const char *name1, INIFILE *ini2, const char *name2)
{
    char param[256];
    long n;

    // Get texture name and load that texture
    sGroupName = nullptr;
    texl = -1;
    if (ReadIniString(ini1, name1, ini2, name2, "groupName", param, sizeof(param), ""))
    {
        const auto len = strlen(param) + 1;
        sGroupName = new char[len];
        memcpy(sGroupName, param, len);
        texl = pPictureService->GetTextureID(sGroupName);
    }

    m_xyCommonOffset.x = 0;
    m_xyCommonOffset.y = 0;
    m_xyCommonOffset = GetIniLongPoint(ini1, name1, ini2, name2, "offset", m_xyCommonOffset);

    // Get images quantity
    auto imgQuantity = 0;
    if (ini1->ReadString(name1, "picture", param, sizeof(param) - 1, ""))
        do
        {
            if (_strnicmp(param, "editsection:", 12) != 0)
                imgQuantity++;
        } while (ini1->ReadStringNext(name1, "picture", param, sizeof(param) - 1));

    m_bRelativeRect = !GetIniLong(ini1, name1, ini2, name2, "bAbsoluteRectangle", 0);

    // Calculate vertex and index quantity
    nVert = imgQuantity * 4;
    nIndx = imgQuantity * 6;
    // Create vertex and index buffers
    if (nVert && nIndx)
    {
        vBuf = m_rs->CreateVertexBuffer(XI_ONETEX_FVF, nVert * sizeof(XI_ONETEX_VERTEX), D3DUSAGE_WRITEONLY);
        iBuf = m_rs->CreateIndexBuffer(nIndx * 2);
        nIndx /= 3;

        // Lock vertex and index buffers and get pointers to this
        auto *pVBuf = static_cast<XI_ONETEX_VERTEX *>(m_rs->LockVertexBuffer(vBuf));
        auto *const pIBuf = static_cast<uint16_t *>(m_rs->LockIndexBuffer(iBuf));

        if (pVBuf != nullptr && pIBuf != nullptr)
        {
            FXYRECT texRect;
            XYRECT scrRect;
            // fill vetex and index buffers of image information
            ini1->ReadString(name1, "picture", param, sizeof(param) - 1, "");
            for (auto i = 0; i < imgQuantity; i++)
            {
                if (_strnicmp(param, "editsection:", 12) != 0)
                {
                    auto dwColor = ARGB(255, 128, 128, 128);
                    char param2[256];
                    const char *pStr = param;
                    n = m_aEditInfo.size();
                    m_aEditInfo.push_back(PicEditInfo());

                    pStr = GetSubStr(pStr, param2, sizeof(param2));
                    pPictureService->GetTexturePos(sGroupName, param2, texRect);
                    m_aEditInfo[n].sName = param2;

                    if (GetMidStr(pStr, param2, sizeof(param2), "col:{", "}"))
                    {
                        int a = ALPHA(dwColor);
                        int r = RED(dwColor);
                        int g = GREEN(dwColor);
                        int b = BLUE(dwColor);
                        GetDataStr(param2, "llll", &a, &r, &g, &b);
                        dwColor = ARGB(a, r, g, b);
                    }
                    m_aEditInfo[n].dwColor = dwColor;

                    scrRect.left = scrRect.top = scrRect.right = scrRect.bottom = 0;
                    if (GetMidStr(pStr, param2, sizeof(param2), "pos:{", "}"))
                        GetDataStr(param2, "llll", &scrRect.left, &scrRect.top, &scrRect.right, &scrRect.bottom);
                    m_aEditInfo[n].nLeft = scrRect.left;
                    m_aEditInfo[n].nTop = scrRect.top;
                    m_aEditInfo[n].nRight = scrRect.right;
                    m_aEditInfo[n].nBottom = scrRect.bottom;
                    if (m_bRelativeRect)
                        GetRelativeRect(scrRect);
                    m_aEditInfo[n].bNative = true;

                    SetBuffers(pVBuf, pIBuf, i, scrRect, texRect, dwColor);
                }
                else
                {
                    if (_stricmp(&param[12], "end") == 0)
                    {
                        n = m_aSections.size() - 1;
                        if (n >= 0)
                            m_aSections[n].nQuantity = m_aEditInfo.size() - m_aSections[n].nStartNum;
                    }
                    else
                    {
                        n = m_aSections.size() - 1;
                        if (n >= 0)
                        {
                            if (m_aSections[n].nQuantity == 0)
                                m_aSections[n].nQuantity = m_aEditInfo.size() - m_aSections[n].nStartNum;
                        }
                        n++;
                        m_aSections.push_back(PicEditSection());
                        m_aSections[n].nStartNum = m_aEditInfo.size();
                        m_aSections[n].sName = &param[12];
                        m_aSections[n].nQuantity = 0;
                    }
                    i--;
                }
                ini1->ReadStringNext(name1, "picture", param, sizeof(param) - 1);
            }
            n = m_aSections.size() - 1;
            if (n >= 0)
            {
                if (m_aSections[n].nQuantity == 0)
                    m_aSections[n].nQuantity = m_aEditInfo.size() - m_aSections[n].nStartNum;
            }
        }

        if (pVBuf != nullptr)
            m_rs->UnLockVertexBuffer(vBuf);
        if (pIBuf != nullptr)
            m_rs->UnLockIndexBuffer(iBuf);
    }
}

void CXI_IMGCOLLECTION::SetBuffers(XI_ONETEX_VERTEX *pVBuf, uint16_t *pIBuf, int rectNum, XYRECT &scrRect,
                                   FXYRECT &texRect, uint32_t dwColor)
{
    // Set pointers to rectangle of get number
    pVBuf += rectNum * 4;
    pIBuf += rectNum * 6;

    // left top point
    pVBuf->pos = CVECTOR(static_cast<float>(scrRect.left), static_cast<float>(scrRect.top), 1.f);
    pVBuf->tu = texRect.left;
    pVBuf->tv = texRect.top;
    pVBuf->color = dwColor;
    pVBuf++;
    // right top point
    pVBuf->pos = CVECTOR(static_cast<float>(scrRect.right), static_cast<float>(scrRect.top), 1.f);
    pVBuf->tu = texRect.right;
    pVBuf->tv = texRect.top;
    pVBuf->color = dwColor;
    pVBuf++;
    // left bottom point
    pVBuf->pos = CVECTOR(static_cast<float>(scrRect.left), static_cast<float>(scrRect.bottom), 1.f);
    pVBuf->tu = texRect.left;
    pVBuf->tv = texRect.bottom;
    pVBuf->color = dwColor;
    pVBuf++;
    // right bottom point
    pVBuf->pos = CVECTOR(static_cast<float>(scrRect.right), static_cast<float>(scrRect.bottom), 1.f);
    pVBuf->tu = texRect.right;
    pVBuf->tv = texRect.bottom;
    pVBuf->color = dwColor;

    const auto idx = rectNum * 4;
    *pIBuf++ = idx;
    *pIBuf++ = idx + 1;
    *pIBuf++ = idx + 2; // one triangle
    *pIBuf++ = idx + 2;
    *pIBuf++ = idx + 1;
    *pIBuf++ = idx + 3; // two triangle
}

void CXI_IMGCOLLECTION::UpdateBuffers()
{
    if (vBuf == -1 || iBuf == -1)
        return;

    auto *const pVBuf = static_cast<XI_ONETEX_VERTEX *>(m_rs->LockVertexBuffer(vBuf));
    auto *const pIBuf = static_cast<uint16_t *>(m_rs->LockIndexBuffer(iBuf));

    if (pVBuf != nullptr && pIBuf != nullptr)
    {
        FXYRECT texRect;
        XYRECT scrRect;

        for (long n = 0; n < m_aEditInfo.size(); n++)
        {
            pPictureService->GetTexturePos(sGroupName, m_aEditInfo[n].sName.c_str(), texRect);

            scrRect.left = m_aEditInfo[n].nLeft;
            scrRect.top = m_aEditInfo[n].nTop;
            scrRect.right = m_aEditInfo[n].nRight;
            scrRect.bottom = m_aEditInfo[n].nBottom;
            if (m_bRelativeRect)
                GetRelativeRect(scrRect);

            SetBuffers(pVBuf, pIBuf, n, scrRect, texRect, m_aEditInfo[n].dwColor);
        }
    }

    if (pVBuf != nullptr)
        m_rs->UnLockVertexBuffer(vBuf);
    if (pIBuf != nullptr)
        m_rs->UnLockIndexBuffer(iBuf);
}

bool CXI_IMGCOLLECTION::IsClick(int buttonID, long xPos, long yPos)
{
    return false;
}

void CXI_IMGCOLLECTION::ChangePosition(XYRECT &rNewPos)
{
    // if (m_aSections.size () == 0) return; // empty collection - created from script
    long n = 0;
    long q = m_aEditInfo.size();

    if (m_nCurSection >= 0 && m_nCurSection < m_aSections.size())
    {
        n = m_aSections[m_nCurSection].nStartNum;
        q = n + m_aSections[m_nCurSection].nQuantity;
    }

    auto rOffset = rNewPos;
    rOffset.left -= m_rect.left;
    rOffset.top -= m_rect.top;
    rOffset.right -= m_rect.right;
    rOffset.bottom -= m_rect.bottom;

    m_xyCommonOffset.x += rOffset.left;
    m_xyCommonOffset.y += rOffset.top;

    // for( long n=m_aSections[m_nCurSection].nStartNum; n<q; n++ )
    for (; n < q; n++)
    {
        m_aEditInfo[n].nLeft += rOffset.left;
        m_aEditInfo[n].nTop += rOffset.top;
        m_aEditInfo[n].nRight += rOffset.right;
        m_aEditInfo[n].nBottom += rOffset.bottom;
    }

    UpdateBuffers();
}

void CXI_IMGCOLLECTION::SaveParametersToIni()
{
    char pcWriteParam[2048];

    auto pIni = fio->OpenIniFile(ptrOwner->m_sDialogFileName.c_str());
    if (!pIni)
    {
        core.Trace("Warning! Can`t open ini file name %s", ptrOwner->m_sDialogFileName.c_str());
        return;
    }

    pIni->DeleteSection(m_nodeName);

    sprintf_s(pcWriteParam, sizeof(pcWriteParam), "%d,%d", m_xyCommonOffset.x, m_xyCommonOffset.y);
    pIni->AddString(m_nodeName, "offset", pcWriteParam);

    long n;
    for (n = 0; n < m_aEditInfo.size(); n++)
        if (m_aEditInfo[n].bNative)
            break;

    if (n < m_aEditInfo.size())
    {
        pIni->AddString(m_nodeName, "groupName", sGroupName);

        // save position
        for (n = 0; n < m_aEditInfo.size(); n++)
        {
            if (!m_aEditInfo[n].bNative)
                continue;
            for (long nGrp = 0; nGrp < m_aSections.size(); nGrp++)
            {
                if (n == m_aSections[nGrp].nStartNum)
                {
                    sprintf_s(pcWriteParam, sizeof(pcWriteParam), "editsection:%s", m_aSections[nGrp].sName.c_str());
                    pIni->AddString(m_nodeName, "picture", pcWriteParam);
                }
                /*else
                  if( n == m_aSections[nGrp].nStartNum + m_aSections[nGrp].nQuantity )
                  {
                    pIni->AddString( m_nodeName, "picture", "editsection:end" );
                  }*/
            }
            sprintf_s(pcWriteParam, sizeof(pcWriteParam), "%s,col:{%d,%d,%d,%d},pos:{%d,%d,%d,%d}",
                      m_aEditInfo[n].sName.c_str(), ALPHA(m_aEditInfo[n].dwColor), RED(m_aEditInfo[n].dwColor),
                      GREEN(m_aEditInfo[n].dwColor), BLUE(m_aEditInfo[n].dwColor), m_aEditInfo[n].nLeft,
                      m_aEditInfo[n].nTop, m_aEditInfo[n].nRight, m_aEditInfo[n].nBottom);
            pIni->AddString(m_nodeName, "picture", pcWriteParam);
        }
    }
}

uint32_t CXI_IMGCOLLECTION::MessageProc(long msgcode, MESSAGE &message)
{
    switch (msgcode)
    {
    case 0: // add icon
    {
        // picture name
        const std::string &param = message.String();
        // picture color
        const auto dwColor = message.Long();
        // picture position
        XYRECT pos;
        pos.left = message.Long();
        pos.top = message.Long();
        pos.right = message.Long();
        pos.bottom = message.Long();
        pos.left += m_xyCommonOffset.x;
        pos.right += m_xyCommonOffset.x;
        pos.top += m_xyCommonOffset.y;
        pos.bottom += m_xyCommonOffset.y;
        AddImage(param.c_str(), dwColor, pos);
    }
    break;

    case 1: // set texture
    {
        const std::string &param = message.String();

        if (!sGroupName || _stricmp(sGroupName, param.c_str()) != 0)
        {
            STORM_DELETE(sGroupName);
            PICTURE_TEXTURE_RELEASE(pPictureService, sGroupName, texl);

            // group name
            const auto len = param.size() + 1;
            sGroupName = new char[len];
            memcpy(sGroupName, param.c_str(), len);
            texl = pPictureService->GetTextureID(sGroupName);
        }
    }
    break;

    case 2: // delete all pictures (leave texture)
    {
        m_aSections.clear();
        m_aEditInfo.clear();
        VERTEX_BUFFER_RELEASE(m_rs, vBuf);
        INDEX_BUFFER_RELEASE(m_rs, iBuf);
    }
    break;

    case 3: // set picture color
    {
        const auto nImgNum = message.Long();
        const uint32_t dwColor = message.Long();
        if (nImgNum >= 0 && nImgNum < m_aEditInfo.size())
        {
            m_aEditInfo[nImgNum].dwColor = dwColor;
            UpdateBuffers();
        }
    }
    break;

    case 4: // change picture
    {
        const auto nImgNum = message.Long();
        const std::string &param = message.String();
        if (nImgNum >= 0 && nImgNum < m_aEditInfo.size())
        {
            m_aEditInfo[nImgNum].sName = param;
            UpdateBuffers();
        }
    }
    break;
    }

    return 0;
}

bool CXI_IMGCOLLECTION::GetInternalNameList(std::vector<std::string> &aStr)
{
    aStr.clear();
    // aStr.Add();
    // aStr[0] = "All";
    aStr.push_back("All");
    for (long n = 0; n < m_aSections.size(); n++)
        aStr.push_back(m_aSections[n].sName);
    return aStr.size() > 0;
}

void CXI_IMGCOLLECTION::SetInternalName(std::string &sName)
{
    m_nCurSection = -1;
    for (long n = 0; n < m_aSections.size(); n++)
    {
        if (m_aSections[n].sName == sName)
        {
            m_nCurSection = n;
            return;
        }
    }
}
