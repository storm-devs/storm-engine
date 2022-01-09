#pragma once

#include "ships_list.h"

struct SII_VERTEX
{
    CVECTOR pos;
    float tu, tv;
};

class ShipInfoImages
{
  public:
    ShipInfoImages(VDX9RENDER *rs, ATTRIBUTES *pAttr);
    ~ShipInfoImages();

    void Draw();

    void SetVisible(bool bVisible)
    {
        m_bVisible = bVisible;
    }

  protected:
    void Release();
    void Init(ATTRIBUTES *pAttr);
    void CheckAndRecreateBuffers(int32_t nShipQ);
    void UpdateShipList();
    void UpdateShipData(int32_t nShipNum, SHIP_DESCRIBE_LIST::SHIP_DESCR *pSD);
    bool IsEnableShowShipInfo(SHIP_DESCRIBE_LIST::SHIP_DESCR *pSD) const;

    const FRECT &GetUVForRelation(int32_t nRelation) const;
    float GetProgressHull(SHIP_DESCRIBE_LIST::SHIP_DESCR *pSD);
    float GetProgressSail(SHIP_DESCRIBE_LIST::SHIP_DESCR *pSD);
    float GetProgressCrew(SHIP_DESCRIBE_LIST::SHIP_DESCR *pSD);

    void CalculateDirectingVectors(const CVECTOR &pos);
    void WriteSquareVertex(SII_VERTEX *pV, const CVECTOR &center, const CVECTOR &offset, const FPOINT &size,
                           const FRECT &uv, float fProgress) const;

  protected:
    VDX9RENDER *pRS;
    bool m_bVisible;

    int32_t m_idRelationTexture;
    int32_t m_vbRelation;
    int32_t m_ibRelation;

    int32_t m_idProgressTexture;
    int32_t m_vbBackProgress;
    int32_t m_ibBackProgress;
    int32_t m_vbProgress;
    int32_t m_ibProgress;

    int32_t m_nShipQ;
    int32_t m_nCurMaxQuantity;

    CVECTOR m_vRelationOffset;
    FPOINT m_fpRelationSize;
    FRECT m_uvRelation[3];

    CVECTOR m_vProgressBackOffset;
    FPOINT m_fpProgressBackSize;
    FRECT m_uvProgressBack;

    CVECTOR m_vHullOffset;
    FRECT m_uvHull;
    CVECTOR m_vSailOffset;
    FRECT m_uvSail;
    CVECTOR m_vCrewOffset;
    FRECT m_uvCrew;
    FPOINT m_fpProgressSize;

    SII_VERTEX *m_pVBuffProgress;
    SII_VERTEX *m_pVBuffBackProgress;
    SII_VERTEX *m_pVBuffRelation;
    CVECTOR m_vUpDir;
    CVECTOR m_vRightDir;
    float m_fImgScale;
};
