#pragma once

#include "ships_list.h"

struct SII_VERTEX
{
    Vector pos;
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
    void CheckAndRecreateBuffers(long nShipQ);
    void UpdateShipList();
    void UpdateShipData(long nShipNum, SHIP_DESCRIBE_LIST::SHIP_DESCR *pSD);
    bool IsEnableShowShipInfo(SHIP_DESCRIBE_LIST::SHIP_DESCR *pSD) const;

    const FRECT &GetUVForRelation(long nRelation) const;
    float GetProgressHull(SHIP_DESCRIBE_LIST::SHIP_DESCR *pSD);
    float GetProgressSail(SHIP_DESCRIBE_LIST::SHIP_DESCR *pSD);
    float GetProgressCrew(SHIP_DESCRIBE_LIST::SHIP_DESCR *pSD);

    void CalculateDirectingVectors(const Vector &pos);
    void WriteSquareVertex(SII_VERTEX *pV, const Vector &center, const Vector &offset, const FPOINT &size,
                           const FRECT &uv, float fProgress) const;

  protected:
    VDX9RENDER *pRS;
    bool m_bVisible;

    long m_idRelationTexture;
    long m_vbRelation;
    long m_ibRelation;

    long m_idProgressTexture;
    long m_vbBackProgress;
    long m_ibBackProgress;
    long m_vbProgress;
    long m_ibProgress;

    long m_nShipQ;
    long m_nCurMaxQuantity;

    Vector m_vRelationOffset;
    FPOINT m_fpRelationSize;
    FRECT m_uvRelation[3];

    Vector m_vProgressBackOffset;
    FPOINT m_fpProgressBackSize;
    FRECT m_uvProgressBack;

    Vector m_vHullOffset;
    FRECT m_uvHull;
    Vector m_vSailOffset;
    FRECT m_uvSail;
    Vector m_vCrewOffset;
    FRECT m_uvCrew;
    FPOINT m_fpProgressSize;

    SII_VERTEX *m_pVBuffProgress;
    SII_VERTEX *m_pVBuffBackProgress;
    SII_VERTEX *m_pVBuffRelation;
    Vector m_vUpDir;
    Vector m_vRightDir;
    float m_fImgScale;
};
