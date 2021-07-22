#pragma once

#include "dx9render.h"

class SCRSHOTER : public Entity
{
    VDX9RENDER *rs;

    struct SAVETEXTURES
    {
        char *fileName;
        char *dataString;
        IDirect3DTexture9 *m_pTex;
        SAVETEXTURES *next;
    };

  public:
    SCRSHOTER();
    ~SCRSHOTER() override;
    void SetDevice();
    bool Init() override;
    void Execute(uint32_t Delta_Time);
    void Realize(uint32_t Delta_Time);
    uint64_t ProcessMessage(MESSAGE &message) override;

    void ProcessStage(Stage stage, uint32_t delta) override
    {
        switch (stage)
        {
        case Stage::execute:
            Execute(delta);
            break;
        case Stage::realize:
            Realize(delta);
            break;
            /*case Stage::lost_render:
              LostRender(delta); break;
            case Stage::restore_render:
              RestoreRender(delta); break;*/
        }
    }

  private:
    bool MakeScreenShot();
    IDirect3DTexture9 *FindSaveTexture(const char *fileName) const;
    char *FindSaveData(const char *fileName) const;
    IDirect3DTexture9 *AddSaveTexture(const char *dirName, const char *fileName);
    void DelSaveTexture(const char *fileName);
    IDirect3DTexture9 *GetTexFromSave(char *fileName, char **pDatStr) const;

    IDirect3DTexture9 *m_pScrShotTex;
    SAVETEXTURES *m_list;
};
