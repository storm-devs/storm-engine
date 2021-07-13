#pragma once

#include "dx9render.h"

class InfoHandler : public Entity
{
    VDX9RENDER *m_rs;

  public:
    InfoHandler();
    ~InfoHandler();
    bool Init() override;
    void Execute(uint32_t delta_time);
    void Realize(uint32_t delta_time) const;
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
    void StringToBufer(char *outStr, int sizeBuf, char *inStr, int copySize) const;
    char *GetCutString(char *pstr, int nOutWidth, float fScale) const;
    bool DoPreOut();

    IDirect3DSurface9 *m_pSurface;
    IDirect3DSurface9 *m_pRenderTarget;

    /*long    m_idVBuf;

    int        m_dwOutputWidth;
    int        m_nOutCenterX, m_nOutCenterY;

    DWORD    m_dwBackFillColor;
    DWORD    m_dwFrontFillColor;

    long    m_idFont;

    struct    STRING_LIST
    {
      int x,y;
      char * strData;
      STRING_LIST    * next;
    } * m_strList;*/
};
