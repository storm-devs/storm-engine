#pragma once

#include "defines.h"
#include "dx9render.h"

class ActivePerkShower : public Entity
{
    VDX9RENDER *rs;

  public:
    ActivePerkShower();
    ~ActivePerkShower();
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
    void ReleaseAll();

    long m_idVBuf;
    long m_idIBuf;
    void FillVIBuffers();
    void FillRectData(void *vbuf, const FRECT &rectPos, const FRECT &rectTex);

    size_t m_nTextureQ;

    struct _TEXTURE_DESCR
    {
        long m_idTexture;
        long m_nCol;
        long m_nRow;

        long m_nPicsQ;
        long m_nVertStart;
        long m_nIndxStart;
    } * m_pTexDescr;

    bool CreateTextures(ATTRIBUTES *pATextureRoot);
    FRECT GetTextureRect(int textIdx, int picIdx) const;

    int m_nIconWidth;
    int m_nIconHeight;
    int m_nSpaceHorz;
    int m_nSpaceVert;

    int m_nShowPlaceQ;
    typedef FRECT _SHOW_PLACE;
    _SHOW_PLACE *m_pShowPlaces;
    bool CreateShowPlaces(ATTRIBUTES *pAPlacesRoot);
    void RefreshShowPlaces(ATTRIBUTES *pAPlacesRoot);

    size_t m_nIShowQ;

    struct _PICTURE_DESCR
    {
        long m_nPicNum;
        long m_nPicTexIdx;
    } * m_pIconsList;

    bool InitIconsList(ATTRIBUTES *pAIconsRoot);
    void AddIconToList(ATTRIBUTES *pAIconDescr);
    void DelIconFromList(ATTRIBUTES *pAIconDescr);

    bool InitCommonBuffers();
};
