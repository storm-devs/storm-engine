#include "scrshoter.h"
#include "../xdefines.h"

#include "core.h"

#define SS_TEXTURE_WIDTH 128
#define SS_TEXTURE_HEIGHT 128
#define SS_TEXTURE_FONECOLOR 0xFF000000

uint32_t GetA8R8G8B8_FromFMT(void *p, uint32_t fmt)
{
    uint32_t retVal;

    if (fmt == D3DFMT_R5G6B5)
    {
        retVal = 0xFF000000 | (static_cast<uint32_t>(*static_cast<uint16_t *>(p) & 0xF800) << 8) |
                 (static_cast<uint32_t>(*static_cast<uint16_t *>(p) & 0x7E0) << 5) |
                 (static_cast<uint32_t>(*static_cast<uint16_t *>(p) & 0x1F) << 3);
    }
    else
    {
        retVal = 0xFF000000 | (*static_cast<uint32_t *>(p));
    }

    return retVal;
}

SCRSHOTER::~SCRSHOTER()
{
    if (textureIndex_ != -1 && rs != nullptr)
        rs->TextureRelease(textureIndex_);
    texture_ = nullptr;
    SAVETEXTURES *pst;
    while (m_list)
    {
        pst = m_list;
        m_list = pst->next;
        delete pst->fileName;
        if (pst->textureId != -1 && rs != nullptr)
            rs->TextureRelease(pst->textureId);
        delete pst;
    }
}

bool SCRSHOTER::Init()
{
    // GUARD(SCRSHOTER::Init())
    SetDevice();
    // UNGUARD
    return true;
}

void SCRSHOTER::SetDevice()
{
    // get render service
    rs = static_cast<VDX9RENDER *>(core.CreateService("dx9render"));
    if (!rs)
        throw std::runtime_error("No service: dx9render");
}

void SCRSHOTER::Execute(uint32_t Delta_Time)
{
}

void SCRSHOTER::Realize(uint32_t Delta_Time)
{
    if (texture_ == nullptr)
        if (!MakeScreenShot())
        {
            core.Trace("ERROR!!! screen shot create error");
            core.Event("makescrshot", nullptr);
        }
        else
            core.Event("makescrshot", nullptr);
}

bool SCRSHOTER::MakeScreenShot()
{
    int n;
    D3DLOCKED_RECT inRect, outRect;

    auto hr = D3D_OK;

    // make the video card draw all unfinished tasks
    hr = rs->EndScene();
    if (hr != D3D_OK)
    {
        core.Trace("ERROR!!! Can`t EndScene");
        return false;
    }
    hr = rs->BeginScene();
    if (hr != D3D_OK)
    {
        core.Trace("ERROR!!! Can`t BeginScene");
        return false;
    }

    // delete the old screen shot
    if (textureIndex_ != -1 && rs != nullptr)
    {
        rs->TextureRelease(textureIndex_);
    }
    texture_ = nullptr;
    textureIndex_ = -1;

    // get data of the old render surface
    D3DSURFACE_DESC desc;
    IDirect3DSurface9 *pOldRenderTarg = nullptr;
    hr = rs->GetRenderTarget(&pOldRenderTarg);
    if (hr == D3D_OK)
        hr = pOldRenderTarg->GetDesc(&desc);

    // get a copy of the render buffer
    IDirect3DSurface9 *pRenderTarg = nullptr;
    if (hr == D3D_OK)
        hr = rs->CreateOffscreenPlainSurface(desc.Width, desc.Height, desc.Format, &pRenderTarg);
    if (hr == D3D_OK)
        hr = rs->GetRenderTargetData(pOldRenderTarg, pRenderTarg);
    pOldRenderTarg->Release();

    // create a new screen shot
    if (hr == D3D_OK)
    {
        textureIndex_ =
            rs->TextureCreate(SS_TEXTURE_WIDTH, SS_TEXTURE_HEIGHT, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED);
        texture_ = static_cast<IDirect3DTexture9 *>(rs->GetTextureFromID(textureIndex_));
    }

    // get a buffer for a copy of the rendering surface
    void *pIn = nullptr;
    if (hr == D3D_OK)
        hr = pRenderTarg->LockRect(&inRect, nullptr, 0);
    if (hr == D3D_OK)
        pIn = inRect.pBits;
    // get a buffer for the texture
    void *pOut = nullptr;
    if (hr == D3D_OK)
        hr = texture_->LockRect(0, &outRect, nullptr, 0);
    if (hr == D3D_OK)
        pOut = outRect.pBits;

    // fill this texture from a copy of our render buffer
    if (hr == D3D_OK)
    {
        for (int vi = 0; vi < SS_TEXTURE_HEIGHT; vi++)
        {
            const size_t vertical_offset = vi * desc.Height / SS_TEXTURE_HEIGHT;
            auto *const pInPxl = static_cast<uint8_t *>(pIn) + inRect.Pitch * vertical_offset;
            auto *pOutPxl = (uint32_t *)(static_cast<uint8_t *>(pOut) + outRect.Pitch * vi);
            for (int hi = 0; hi < SS_TEXTURE_WIDTH; hi++)
            {
                const size_t horizontal_offset = (hi * desc.Width / SS_TEXTURE_WIDTH) * (inRect.Pitch / desc.Width);
                pOutPxl[hi] = GetA8R8G8B8_FromFMT(&pInPxl[horizontal_offset], desc.Format);
            }
        }
    }

    // close opened buffers
    if (pIn != nullptr)
        pRenderTarg->UnlockRect();

    // close opened buffers
    if (pOut != nullptr)
        texture_->UnlockRect(0);

    // Delete unnecessary screen copy
    pRenderTarg->Release();

    // Add a texture with a frame to the shot
    const int nTextureID = rs->TextureCreate("interfaces\\EmptyBorder.tga");
    if (nTextureID >= 0)
    {
        IDirect3DTexture9 *pScrShotTex = nullptr;
        if (D3D_OK == rs->CreateTexture(SS_TEXTURE_WIDTH, SS_TEXTURE_HEIGHT, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8,
                                        D3DPOOL_DEFAULT, &pScrShotTex))
        {
            const uint32_t BI_SCRSHOTER_VERTEX_FORMAT = (D3DFVF_XYZRHW | D3DFVF_TEX1 | D3DFVF_TEXTUREFORMAT2);
            struct BI_SCRSHOTER_VERTEX
            {
                CVECTOR pos;
                float w;
                float tu, tv;
            } vert[4];
            for (auto i = 0; i < 4; i++)
            {
                vert[i].pos.z = 1.f;
                vert[i].w = 0.5f;
            }
            vert[1].pos.x = vert[0].pos.x = 0;
            vert[2].pos.y = vert[0].pos.y = 0;
            vert[3].pos.x = vert[2].pos.x = SS_TEXTURE_WIDTH;
            vert[3].pos.y = vert[1].pos.y = SS_TEXTURE_HEIGHT;
            vert[1].tu = vert[0].tu = 0;
            vert[2].tv = vert[0].tv = 0;
            vert[3].tu = vert[2].tu = 1.f;
            vert[3].tv = vert[1].tv = 1.f;

            pOldRenderTarg = nullptr;
            pRenderTarg = nullptr;
            if (rs->GetRenderTarget(&pOldRenderTarg) == S_OK)
            {
                IDirect3DSurface9 *pStencil = nullptr;
                rs->GetDepthStencilSurface(&pStencil);
                pScrShotTex->GetSurfaceLevel(0, &pRenderTarg);
                if (rs->SetRenderTarget(pRenderTarg, nullptr) == S_OK)
                {
                    rs->TextureSet(0, textureIndex_);
                    rs->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, BI_SCRSHOTER_VERTEX_FORMAT, 2, vert,
                                        sizeof(BI_SCRSHOTER_VERTEX), "battle_icons");
                    rs->TextureSet(0, nTextureID);
                    rs->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, BI_SCRSHOTER_VERTEX_FORMAT, 2, vert,
                                        sizeof(BI_SCRSHOTER_VERTEX), "battle_icons");
                    rs->SetRenderTarget(pOldRenderTarg, pStencil);
                }
                if (pRenderTarg)
                    pRenderTarg->Release();
                if (pStencil)
                    pStencil->Release();
                if (pOldRenderTarg)
                    pOldRenderTarg->Release();
            }
            IDirect3DSurface9 *pSurf1 = nullptr, *pSurf2 = nullptr;
            rs->GetSurfaceLevel(texture_, 0, &pSurf1);
            rs->GetSurfaceLevel(pScrShotTex, 0, &pSurf2);
            // rs->UpdateSurface(pSurf2,null,0,pSurf1,null);
            hr = D3DXLoadSurfaceFromSurface(pSurf1, nullptr, nullptr, pSurf2, nullptr, nullptr, D3DX_DEFAULT, 0);
            if (pSurf1)
                rs->Release(pSurf1);
            if (pSurf2)
                rs->Release(pSurf2);
            rs->Release(pScrShotTex);
        }

        rs->TextureRelease(nTextureID);
    }

    return hr == D3D_OK;
}

uint64_t SCRSHOTER::ProcessMessage(MESSAGE &message)
{
    switch (message.Long())
    {
    case MSG_SCRSHOT_MAKE:
        return textureIndex_;
        break;
    case MSG_SCRSHOT_READ: {
        VDATA *pvdat;

        const std::string &param = message.String();
        const std::string &param2 = message.String();
        pvdat = message.ScriptVariablePointer();

        long pRetTex = AddSaveTexture(param.c_str(), param2.c_str());
        auto *const strDat = FindSaveData(param2.c_str());
        if (pvdat)
            if (!strDat)
            {
                pvdat->Set("\0");
            }
            else
            {
                pvdat->Set(strDat);
            }
        return pRetTex;
    }
    break;
    case MSG_SCRSHOT_RELEASE: {
        const std::string &param = message.String();
        DelSaveTexture(param.c_str());
    }
    break;
    }
    return 0;
}

long SCRSHOTER::FindSaveTexture(const char *fileName) const
{
    if (!fileName)
        return -1;
    auto *ps = m_list;
    while (ps)
    {
        if (ps->fileName && _stricmp(fileName, ps->fileName) == 0)
            return ps->textureId;
        ps = ps->next;
    }
    return -1;
}

char *SCRSHOTER::FindSaveData(const char *fileName) const
{
    if (!fileName)
        return nullptr;
    SAVETEXTURES *ps = m_list;
    while (ps)
    {
        if (ps->fileName && _stricmp(fileName, ps->fileName) == 0)
            return ps->dataString;
        ps = ps->next;
    }
    return nullptr;
}

long SCRSHOTER::AddSaveTexture(const char *dirName, const char *fileName)
{
    if (fileName == nullptr)
        return -1;
    long rval = FindSaveTexture(fileName);
    if (rval != -1)
        return rval;
    if (_stricmp(fileName, "newsave") == 0)
        return textureIndex_;
    auto *ps = new SAVETEXTURES;
    if (ps == nullptr)
    {
        throw std::runtime_error("Allocate memory error");
    }
    ps->dataString = nullptr;
    ps->next = m_list;
    m_list = ps;
    const auto len = strlen(fileName) + 1;
    m_list->fileName = new char[len];
    if (m_list->fileName == nullptr)
    {
        throw std::runtime_error("Allocate memory error");
    }
    memcpy(m_list->fileName, fileName, len);
    char param[1024];
    if (dirName == nullptr || dirName[0] == 0)
        sprintf_s(param, "%s", fileName);
    else
        sprintf_s(param, "%s\\%s", dirName, fileName);
    m_list->textureId = GetTexFromSave(param, &ps->dataString);
    return m_list->textureId;
}

void SCRSHOTER::DelSaveTexture(const char *fileName)
{
    if (!fileName)
        return;
    SAVETEXTURES *oldps = nullptr;
    SAVETEXTURES *ps = m_list;
    while (ps)
    {
        if (ps->fileName && _stricmp(fileName, ps->fileName) == 0)
        {
            if (oldps)
                oldps->next = ps->next;
            else
                m_list = ps->next;
            delete ps->fileName;
            delete ps->dataString;
            if (ps->textureId != -1 && rs != nullptr)
                rs->TextureRelease(ps->textureId);
            delete ps;
            return;
        }
        oldps = ps;
        ps = ps->next;
    }
}

long SCRSHOTER::GetTexFromSave(char *fileName, char **pDatStr) const
{
    HRESULT hr = D3D_OK;
    D3DLOCKED_RECT outRect;
    long textureId = -1;
    *pDatStr = nullptr; //~!~

    long datSize = 0;
    char *pdat = nullptr;
    pdat = static_cast<char *>(core.GetSaveData(fileName, datSize));
    long startIdx = 0;
    long texSize = 0;
    if (pdat != nullptr && datSize > sizeof(SAVE_DATA_HANDLE))
    {
        startIdx = ((SAVE_DATA_HANDLE *)pdat)->StringDataSize + sizeof(SAVE_DATA_HANDLE);
        texSize = ((SAVE_DATA_HANDLE *)pdat)->SurfaceDataSize;
    }
    if (datSize > sizeof(SAVE_DATA_HANDLE) && datSize == startIdx + texSize &&
        texSize == 4 * SS_TEXTURE_WIDTH * SS_TEXTURE_HEIGHT)
    {
        textureId = rs->TextureCreate(SS_TEXTURE_WIDTH, SS_TEXTURE_HEIGHT, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED);
        auto *pt = static_cast<IDirect3DTexture9 *>(rs->GetTextureFromID(textureId));
        hr = pt->LockRect(0, &outRect, nullptr, 0);
        if (hr == D3D_OK)
        {
            memcpy(outRect.pBits, &pdat[startIdx], texSize);
            pt->UnlockRect(0);
        }
        char *stringData = &pdat[sizeof(SAVE_DATA_HANDLE)];
        if (!utf8::IsValidUtf8(stringData))
        {
            utf8::FixInvalidUtf8(stringData);
        }
        const int strLen = ((SAVE_DATA_HANDLE *)pdat)->StringDataSize;
        *pDatStr = new char[strLen + 1];
        if (!*pDatStr)
        {
            throw std::runtime_error("allocate memory error");
        }
        strncpy_s(*pDatStr, strLen + 1, stringData, strLen);
        (*pDatStr)[strLen] = 0;
    }
    /*    else
      {
        int idx=0;
        for(int i=0; i<SS_TEXTURE_HEIGHT; i++)
          for(int j=0; j<SS_TEXTURE_WIDTH; j++)
          {
            ((DWORD*)outRect.pBits)[idx] = 0xFF000000;
            idx++;
          }
      }*/
    delete pdat;

    if (hr == D3D_OK)
    {
        return textureId;
    }
    if (*pDatStr)
        delete (*pDatStr);
    *pDatStr = nullptr;
    if (textureId && rs)
        rs->TextureRelease(textureId);
    return -1;
}
