#pragma once

#include "dx9render.h"
#include <defines.h>

class HELPCHOOSER : public Entity
{
    VDX9RENDER *rs;

  public:
    HELPCHOOSER();
    ~HELPCHOOSER() override;
    void SetDevice();
    bool Init() override;
    void Execute(uint32_t Delta_Time);
    void Realize(uint32_t Delta_Time) const;
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

  protected:
    void AllRelease();
    bool RunChooser(const char *ChooserGroup);
    void SetRectangle(int32_t newRectNum);
    int32_t GetRectangleLeft() const;
    int32_t GetRectangleRight() const;
    int32_t GetRectangleUp() const;
    int32_t GetRectangleDown() const;
    bool MouseMove();
    int32_t GetRectangleFromPos(float x, float y) const;

  protected:
    int32_t m_idMouseTexture;
    int32_t m_idPicTexture;
    int32_t m_idBackTexture;
    int32_t m_idVBuf;

    float m_fScreenWidth;
    float m_fScreenHeight;
    FRECT *m_pRectList;
    char **m_psRectName;
    int32_t m_nRectQ;
    int32_t m_nCurRect;

    // mouse data
    float m_fCurMouseX;
    float m_fCurMouseY;
    int m_nMouseCornerX;
    int m_nMouseCornerY;
    int m_nMouseWidth;
    int m_nMouseHeight;
};
