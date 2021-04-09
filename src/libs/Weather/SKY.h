#pragma once

#include "typedef.h"
#include <string>
#include <vector>

class SKY : public Entity
{
  public:
    SKY();
    ~SKY();

    bool Init() override;
    void SetDevice();
    void Realize(uint32_t Delta_Time);
    bool CreateState(ENTITY_STATE_GEN *state_gen);
    bool LoadState(ENTITY_STATE *state);
    uint32_t AttributeChanged(ATTRIBUTES *pAttribute) override;
    uint64_t ProcessMessage(MESSAGE &message) override;

    void ProcessStage(Stage stage, uint32_t delta) override
    {
        switch (stage)
        {
            // case Stage::execute:
            //    Execute(delta); break;
        case Stage::realize:
            Realize(delta);
            break;
            /*case Stage::lost_render:
              LostRender(delta); break;
            case Stage::restore_render:
              RestoreRender(delta); break;*/
        }
    }

    float CalculateAlphaForSun(const CVECTOR &vSunPos, float fSunSize);

  private:
    enum
    {
        FOGVERTEX_FORMAT = D3DFVF_XYZ | D3DFVF_DIFFUSE,
        SKYVERTEX_FORMAT = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX2 | D3DFVF_TEXTUREFORMAT2,
        SKY_NUM_TEXTURES = 5,
        SKY_NUM_VERTEX = 40,

        ENUMSKY_FORCE_DWORD = 0x7FFFFFFF
    };

    struct SKYVERTEX
    {
        CVECTOR pos;
        uint32_t diffuse;
        float tu, tv;
        float tu2, tv2;
    };

    struct FOGVERTEX
    {
        CVECTOR pos;
        uint32_t diffuse;
    };

    // string        sSkyDir;
    std::vector<std::string> aSkyDirArray;
    uint32_t dwSkyColor;
    float fSkySpeedRotate, fSkySize, fAngleY, fSkyAngle;
    std::string sTechSky, sTechSkyBlend, sTechSkyBlendAlpha, sTechSkyFog;

    VDX9RENDER *pRS;
    long TexturesID[SKY_NUM_TEXTURES];
    long TexturesNextID[SKY_NUM_TEXTURES];
    float fTimeFactor;

    long iSkyVertsID, iSkyIndexID;
    long iFogVertsID, iFogIndexID, iFogNumVerts, iFogNumTrgs;

    Entity *pAstronomy;
    Entity *pSunGlow;

    // sky section
    void GenerateSky();
    void LoadTextures();
    void Release();

    // fog section
    void CreateFogSphere();
    void UpdateFogSphere();
    uint32_t CalcFogDiffuse(CVECTOR &vPos);

    void FillSkyDirArray(ATTRIBUTES *pAttribute);
    void GetSkyDirStrings(std::string &sSkyDir, std::string &sSkyDirNext);
    void UpdateTimeFactor();

    uint32_t GetPixelColor(IDirect3DTexture9 *pTex, float fu, float fv) const;
};
