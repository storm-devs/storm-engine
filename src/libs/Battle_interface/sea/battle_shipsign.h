#pragma once

#include "../bi_defines.h"
#include "ships_list.h"
#include <string>
#include <vector>

#define MAX_SHIP_QUANTITY 8

class BIShipCommandList;

class BIShipIcon
{
  public:
    enum CommandType
    {
        Command_confirm,
        Command_left,
        Command_right,
        Command_up,
        Command_down,
        Command_cancel,

        Command_ForceDword = 0xffffffff
    };

    BIShipIcon(BIShipIcon &&) = delete;
    BIShipIcon(const BIShipIcon &) = delete;
    BIShipIcon(entid_t BIEntityID, VDX9RENDER *pRS);
    ~BIShipIcon();

    void Draw();
    void Init(ATTRIBUTES *pRoot, ATTRIBUTES *pA);

    size_t AddTexture(const char *pcTextureName, long nCols, long nRows) const;

    void Recollect();
    void SetUpdate()
    {
        m_bMakeUpdate = true;
    }
    bool IsActive() const;
    void SetActive(bool bActive);
    void MakeControl();
    void ExecuteCommand(CommandType command);

  protected:
    void Release();
    long CalculateShipQuantity();
    void UpdateBuffers(long nShipQ);
    void FillIndexBuffer() const;
    void FillVertexBuffer();
    long WriteSquareToVBuff(BI_COLOR_VERTEX *pv, const FRECT &uv, uint32_t color, const BIFPOINT &center,
                            const FPOINT &size);
    long WriteSquareToVBuffWithProgress(BI_COLOR_VERTEX *pv, const FRECT &uv, uint32_t color, const BIFPOINT &center,
                                        const FPOINT &size, float fClampUp, float fClampDown, float fClampLeft,
                                        float fClampRight);
    void UpdateCommandList();

    long GetCurrentCommandTopLine();
    long GetCurrentCommandCharacterIndex();
    long GetCurrentCommandMode() const;

    ATTRIBUTES *GetSailorQuantityAttribute(SHIP_DESCRIBE_LIST::SHIP_DESCR *pSD);
    float GetProgressShipHP(long nShipNum) const;
    float GetProgressShipSP(long nShipNum);
    float GetProgressShipClass(long nShipNum);
    void GetShipUVFromPictureIndex(long nPicIndex, FRECT &rUV);

    long GetShipClass(long nCharIdx);

    VDX9RENDER *m_pRS;

    long m_nVBufID;
    long m_nIBufID;

    long m_nMaxSquareQ;

    long m_nBackTextureID;
    long m_nBackSquareQ;
    uint32_t m_dwBackColor;
    FRECT m_rBackUV;
    BIFPOINT m_pntBackOffset;
    FPOINT m_pntBackIconSize;

    long m_nShipStateTextureID;
    long m_nShipStateSquareQ;
    uint32_t m_dwShipStateColor;
    FRECT m_rShipHPUV;
    BIFPOINT m_pntShipHPOffset;
    FPOINT m_pntShipHPIconSize;
    FRECT m_rShipSPUV;
    BIFPOINT m_pntShipSPOffset;
    FPOINT m_pntShipSPIconSize;

    long m_nShipClassTextureID;
    long m_nShipClassSquareQ;
    uint32_t m_dwShipClassColor;
    FRECT m_rShipClassUV;
    BIFPOINT m_pntShipClassOffset;
    FPOINT m_pntShipClassIconSize;
    std::vector<float> m_aClassProgress;

    long m_nShipTextureID;
    long m_nShipSquareQ;
    uint32_t m_dwShipColor;
    // FRECT m_rShipUV;
    BIFPOINT m_pntShipOffset;
    FPOINT m_pntShipIconSize;

    struct ShipDescr
    {
        long nCharacterIndex;
        FPOINT pntPos; // center
        ATTRIBUTES *pASailorQuantity;
        ATTRIBUTES *pAShip;
        long nMaxHP;
        long nMaxSP;
        FRECT rUV;
        std::string sShipName;
        long nShipClass;
    } m_Ship[MAX_SHIP_QUANTITY];

    long m_nShipQuantity;
    long m_nCurrentShipIndex;
    long m_nCommandListVerticalOffset;

    long m_idSailorFont;
    uint32_t m_dwSailorFontColor;
    float m_fSailorFontScale;
    POINT m_SailorFontOffset;

    long m_idShipNameFont;
    uint32_t m_dwShipNameFontColor;
    float m_fShipNameFontScale;
    POINT m_ShipNameFontOffset;

    bool m_bMakeUpdate;

    ATTRIBUTES *m_pARoot;

    BIShipCommandList *m_pCommandList;
    entid_t m_idHostEntity;
    long m_nCommandMode;
};
