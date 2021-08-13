#pragma once

#include "dx9render.h"

class SCRSHOTER : public Entity
{
    struct SAVETEXTURES
    {
        char *fileName;
        char *dataString;
        long textureId = -1;
        SAVETEXTURES *next;
    };

  public:
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
    long FindSaveTexture(const char *fileName) const;
    char *FindSaveData(const char *fileName) const;
    long AddSaveTexture(const char *dirName, const char *fileName);
    void DelSaveTexture(const char *fileName);
    long GetTexFromSave(char *fileName, char **pDatStr) const;
    
    VDX9RENDER *rs = nullptr;
    IDirect3DTexture9 *texture_ = nullptr;
    long textureIndex_ = -1;
    SAVETEXTURES *m_list = nullptr;
};
