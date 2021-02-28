#include "ShipInfoImages.h"
#include "../Utils.h"
#include "ship_base.h"

// define the vertices
#define SPV_FORMAT (D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_TEXTUREFORMAT2)
#define MAX_SHIPINFO_DIST_IN_POW2 1000000.f

ShipInfoImages::ShipInfoImages(VDX9RENDER *rs, ATTRIBUTES *pAttr)
{
    pRS = rs;
    m_bVisible = true;

    m_idRelationTexture = -1;
    m_vbRelation = -1;
    m_ibRelation = -1;

    m_idProgressTexture = -1;
    m_vbProgress = -1;
    m_ibProgress = -1;
    m_vbBackProgress = -1;
    m_ibBackProgress = -1;

    m_nCurMaxQuantity = 0;
    m_nShipQ = 0;
    Init(pAttr);
}

ShipInfoImages::~ShipInfoImages()
{
    Release();
}

void ShipInfoImages::Draw()
{
    if (!m_bVisible)
        return;

    UpdateShipList();
    if (m_nShipQ <= 0)
        return;

    CMatrix matw;
    pRS->SetTransform(D3DTS_WORLD, (D3DXMATRIX *)&matw);

    if (m_idRelationTexture != -1)
    {
        pRS->TextureSet(0, m_idRelationTexture);
        pRS->DrawBuffer(m_vbRelation, sizeof(SII_VERTEX), m_ibRelation, 0, m_nShipQ * 4, 0, m_nShipQ * 2,
                        "battle_shippointer");
    }

    if (m_idProgressTexture != -1)
    {
        pRS->TextureSet(0, m_idProgressTexture);
        pRS->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
        pRS->DrawBuffer(m_vbBackProgress, sizeof(SII_VERTEX), m_ibBackProgress, 0, m_nShipQ * 4, 0, m_nShipQ * 2,
                        "battle_shippointer");
        pRS->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
        pRS->DrawBuffer(m_vbProgress, sizeof(SII_VERTEX), m_ibProgress, 0, m_nShipQ * 3 * 4, 0, m_nShipQ * 3 * 2,
                        "battle_shippointer");
    }
}

void ShipInfoImages::Release()
{
    TEXTURE_RELEASE(pRS, m_idRelationTexture);
    VERTEX_BUFFER_RELEASE(pRS, m_vbRelation);
    INDEX_BUFFER_RELEASE(pRS, m_ibRelation);

    TEXTURE_RELEASE(pRS, m_idProgressTexture);
    VERTEX_BUFFER_RELEASE(pRS, m_vbProgress);
    INDEX_BUFFER_RELEASE(pRS, m_ibProgress);
    VERTEX_BUFFER_RELEASE(pRS, m_vbBackProgress);
    INDEX_BUFFER_RELEASE(pRS, m_ibBackProgress);

    m_nShipQ = 0;
    m_nCurMaxQuantity = 0;
}

void ShipInfoImages::Init(ATTRIBUTES *pAttr)
{
    // set deafault values
    CheckAndRecreateBuffers(30);

    // params for relation image
    m_idRelationTexture = BIUtils::GetTextureFromAttr(pRS, pAttr, "RelationTexture");
    m_vRelationOffset = CVECTOR(0.f, 10.f, 0.f);
    BIUtils::ReadVectorFormAttr(pAttr, "RelationOffset", m_vRelationOffset, m_vRelationOffset);
    BIUtils::ReadPosFromAttr(pAttr, "RelationSize", m_fpRelationSize.x, m_fpRelationSize.y, 1.f, 1.f);
    FULLRECT(m_uvRelation[0]);
    BIUtils::ReadRectFromAttr(pAttr, "RelationUV1", m_uvRelation[0], m_uvRelation[0]);
    FULLRECT(m_uvRelation[1]);
    BIUtils::ReadRectFromAttr(pAttr, "RelationUV2", m_uvRelation[1], m_uvRelation[1]);
    FULLRECT(m_uvRelation[2]);
    BIUtils::ReadRectFromAttr(pAttr, "RelationUV3", m_uvRelation[2], m_uvRelation[2]);

    // params for progress background image
    m_idProgressTexture = BIUtils::GetTextureFromAttr(pRS, pAttr, "ProgressTexture");
    m_vProgressBackOffset = CVECTOR(0.f, 11.25f, 0.f);
    BIUtils::ReadVectorFormAttr(pAttr, "ProgressBackOffset", m_vProgressBackOffset, m_vProgressBackOffset);
    BIUtils::ReadPosFromAttr(pAttr, "ProgressBackSize", m_fpProgressBackSize.x, m_fpProgressBackSize.y, 2.f, 0.3f);
    FULLRECT(m_uvProgressBack);
    BIUtils::ReadRectFromAttr(pAttr, "ProgressBackUV", m_uvProgressBack, m_uvProgressBack);

    // params for hull progress image
    m_vHullOffset = CVECTOR(0.f, 10.75f, 0.f);
    BIUtils::ReadVectorFormAttr(pAttr, "HullOffset", m_vHullOffset, m_vHullOffset);
    FULLRECT(m_uvHull);
    BIUtils::ReadRectFromAttr(pAttr, "HullUV", m_uvHull, m_uvHull);

    // params for sail progress image
    m_vSailOffset = CVECTOR(0.f, 11.25f, 0.f);
    BIUtils::ReadVectorFormAttr(pAttr, "SailOffset", m_vSailOffset, m_vSailOffset);
    FULLRECT(m_uvSail);
    BIUtils::ReadRectFromAttr(pAttr, "SailUV", m_uvSail, m_uvSail);

    // params for crew progress image
    m_vCrewOffset = CVECTOR(0.f, 11.75f, 0.f);
    BIUtils::ReadVectorFormAttr(pAttr, "CrewOffset", m_vCrewOffset, m_vCrewOffset);
    FULLRECT(m_uvCrew);
    BIUtils::ReadRectFromAttr(pAttr, "CrewUV", m_uvCrew, m_uvCrew);

    // size of progress images
    BIUtils::ReadPosFromAttr(pAttr, "ProgressSize", m_fpProgressSize.x, m_fpProgressSize.y, 2.f, 0.5f);
}

void ShipInfoImages::CheckAndRecreateBuffers(long nShipQ)
{
    if (nShipQ > m_nCurMaxQuantity)
    {
        // recreate buffers
        m_nCurMaxQuantity = nShipQ;

        // delete old buffers
        VERTEX_BUFFER_RELEASE(pRS, m_vbRelation);
        INDEX_BUFFER_RELEASE(pRS, m_ibRelation);
        VERTEX_BUFFER_RELEASE(pRS, m_vbProgress);
        INDEX_BUFFER_RELEASE(pRS, m_ibProgress);
        VERTEX_BUFFER_RELEASE(pRS, m_vbBackProgress);
        INDEX_BUFFER_RELEASE(pRS, m_ibBackProgress);

        // create new buffers
        m_vbRelation = pRS->CreateVertexBuffer(SPV_FORMAT, nShipQ * 4 * sizeof(SII_VERTEX), D3DUSAGE_WRITEONLY);
        m_ibRelation = pRS->CreateIndexBuffer(nShipQ * 6 * sizeof(uint16_t));
        m_vbProgress = pRS->CreateVertexBuffer(SPV_FORMAT, nShipQ * 3 * 4 * sizeof(SII_VERTEX), D3DUSAGE_WRITEONLY);
        m_ibProgress = pRS->CreateIndexBuffer(nShipQ * 3 * 6 * sizeof(uint16_t));
        m_vbBackProgress = pRS->CreateVertexBuffer(SPV_FORMAT, nShipQ * 4 * sizeof(SII_VERTEX), D3DUSAGE_WRITEONLY);
        m_ibBackProgress = pRS->CreateIndexBuffer(nShipQ * 6 * sizeof(uint16_t));

        // fill there index buffers
        long n;
        auto *pIB = static_cast<uint16_t *>(pRS->LockIndexBuffer(m_ibRelation));
        if (pIB)
        {
            for (n = 0; n < nShipQ; n++)
            {
                pIB[n * 6 + 0] = static_cast<uint16_t>(n * 4 + 0);
                pIB[n * 6 + 1] = static_cast<uint16_t>(n * 4 + 1);
                pIB[n * 6 + 2] = static_cast<uint16_t>(n * 4 + 2);
                pIB[n * 6 + 3] = static_cast<uint16_t>(n * 4 + 1);
                pIB[n * 6 + 4] = static_cast<uint16_t>(n * 4 + 3);
                pIB[n * 6 + 5] = static_cast<uint16_t>(n * 4 + 2);
            }
            pRS->UnLockIndexBuffer(m_ibRelation);
        }
        pIB = static_cast<uint16_t *>(pRS->LockIndexBuffer(m_ibProgress));
        if (pIB)
        {
            for (n = 0; n < nShipQ * 3; n++)
            {
                pIB[n * 6 + 0] = static_cast<uint16_t>(n * 4 + 0);
                pIB[n * 6 + 1] = static_cast<uint16_t>(n * 4 + 1);
                pIB[n * 6 + 2] = static_cast<uint16_t>(n * 4 + 2);
                pIB[n * 6 + 3] = static_cast<uint16_t>(n * 4 + 1);
                pIB[n * 6 + 4] = static_cast<uint16_t>(n * 4 + 3);
                pIB[n * 6 + 5] = static_cast<uint16_t>(n * 4 + 2);
            }
            pRS->UnLockIndexBuffer(m_ibProgress);
        }
        pIB = static_cast<uint16_t *>(pRS->LockIndexBuffer(m_ibBackProgress));
        if (pIB)
        {
            for (n = 0; n < nShipQ; n++)
            {
                pIB[n * 6 + 0] = static_cast<uint16_t>(n * 4 + 0);
                pIB[n * 6 + 1] = static_cast<uint16_t>(n * 4 + 1);
                pIB[n * 6 + 2] = static_cast<uint16_t>(n * 4 + 2);
                pIB[n * 6 + 3] = static_cast<uint16_t>(n * 4 + 1);
                pIB[n * 6 + 4] = static_cast<uint16_t>(n * 4 + 3);
                pIB[n * 6 + 5] = static_cast<uint16_t>(n * 4 + 2);
            }
            pRS->UnLockIndexBuffer(m_ibBackProgress);
        }
    }
}

void ShipInfoImages::UpdateShipList()
{
    long n;
    SHIP_DESCRIBE_LIST::SHIP_DESCR *pSD;

    // calculate ship quantity
    m_nShipQ = 0;
    for (pSD = g_ShipList.GetShipRoot(); pSD; pSD = pSD->next)
        if (IsEnableShowShipInfo(pSD))
            m_nShipQ++;
    if (m_nShipQ <= 0)
        return;

    // check buffers and recreate if needed
    CheckAndRecreateBuffers(m_nShipQ);

    // lock buffers
    m_pVBuffRelation = static_cast<SII_VERTEX *>(pRS->LockVertexBuffer(m_vbRelation));
    if (!m_pVBuffRelation)
    {
        m_nShipQ = 0;
        return;
    }
    m_pVBuffProgress = static_cast<SII_VERTEX *>(pRS->LockVertexBuffer(m_vbProgress));
    if (!m_pVBuffProgress)
    {
        pRS->UnLockVertexBuffer(m_vbRelation);
        m_nShipQ = 0;
        return;
    }
    m_pVBuffBackProgress = static_cast<SII_VERTEX *>(pRS->LockVertexBuffer(m_vbBackProgress));
    if (!m_pVBuffBackProgress)
    {
        pRS->UnLockVertexBuffer(m_vbRelation);
        pRS->UnLockVertexBuffer(m_vbProgress);
        m_nShipQ = 0;
        return;
    }

    // update for real ships
    n = 0;
    for (pSD = g_ShipList.GetShipRoot(); pSD; pSD = pSD->next)
    {
        if (!IsEnableShowShipInfo(pSD))
            continue;
        UpdateShipData(n, pSD);
        n++;
    }

    // unlock buffers
    pRS->UnLockVertexBuffer(m_vbRelation);
    pRS->UnLockVertexBuffer(m_vbProgress);
    pRS->UnLockVertexBuffer(m_vbBackProgress);
}

void ShipInfoImages::UpdateShipData(long nShipNum, SHIP_DESCRIBE_LIST::SHIP_DESCR *pSD)
{
    if (!pSD || nShipNum >= m_nCurMaxQuantity)
        return;
    auto shippos = pSD->pShip->GetPos();
    shippos.y += static_cast<SHIP_BASE *>(pSD->pShip)->State.vRealBoxSize.y + 5.f;

    CalculateDirectingVectors(shippos);

    // write relation square
    WriteSquareVertex(&m_pVBuffRelation[nShipNum * 4], shippos, m_vRelationOffset, m_fpRelationSize,
                      GetUVForRelation(pSD->relation), 1.f);

    // write progress back square
    WriteSquareVertex(&m_pVBuffBackProgress[nShipNum * 4], shippos, m_vProgressBackOffset, m_fpProgressBackSize,
                      m_uvProgressBack, 1.f);

    // write progress squares
    //---------------------------------
    // write hull
    WriteSquareVertex(&m_pVBuffProgress[nShipNum * 3 * 4], shippos, m_vHullOffset, m_fpProgressSize, m_uvHull,
                      GetProgressHull(pSD));
    // write sail
    WriteSquareVertex(&m_pVBuffProgress[nShipNum * 3 * 4 + 4], shippos, m_vSailOffset, m_fpProgressSize, m_uvSail,
                      GetProgressSail(pSD));
    // write crew
    WriteSquareVertex(&m_pVBuffProgress[nShipNum * 3 * 4 + 8], shippos, m_vCrewOffset, m_fpProgressSize, m_uvCrew,
                      GetProgressCrew(pSD));
}

bool ShipInfoImages::IsEnableShowShipInfo(SHIP_DESCRIBE_LIST::SHIP_DESCR *pSD) const
{
    if (!pSD)
        return false;
    if (pSD->isDead)
        return false;

    CVECTOR vpos, vang;
    float fpersp;
    pRS->GetCamera(vpos, vang, fpersp);
    if (~(pSD->pShip->GetPos() - vpos) > MAX_SHIPINFO_DIST_IN_POW2)
        return false;

    return true;
}

const FRECT &ShipInfoImages::GetUVForRelation(long nRelation) const
{
    switch (nRelation)
    {
    case 1:
        return m_uvRelation[0];
        break; // friend
    case 0:
        return m_uvRelation[1];
        break; // enemy
    }
    return m_uvRelation[2]; // neutral
}

float ShipInfoImages::GetProgressHull(SHIP_DESCRIBE_LIST::SHIP_DESCR *pSD)
{
    if (!pSD || !pSD->pAttr)
        return 0.f;
    if (pSD->maxHP <= 0)
        return 0.f;
    auto f = pSD->pAttr->GetAttributeAsFloat("HP", 0.f) / static_cast<float>(pSD->maxHP);
    if (f < 0.f)
        f = 0.f;
    if (f > 1.f)
        f = 1.f;
    return f;
}

float ShipInfoImages::GetProgressSail(SHIP_DESCRIBE_LIST::SHIP_DESCR *pSD)
{
    if (!pSD || !pSD->pAttr)
        return 0.f;
    if (pSD->maxSP <= 0)
        return 0.f;
    auto f = pSD->pAttr->GetAttributeAsFloat("SP", 0.f) / static_cast<float>(pSD->maxSP);
    if (f < 0.f)
        f = 0.f;
    if (f > 1.f)
        f = 1.f;
    return f;
}

float ShipInfoImages::GetProgressCrew(SHIP_DESCRIBE_LIST::SHIP_DESCR *pSD)
{
    if (!pSD || !pSD->pAttr)
        return 0.f;
    if (pSD->maxCrew <= 0)
        return 0.f;
    auto *pA = pSD->pAttr->GetAttributeClass("crew");
    if (!pA)
        return 0.f;
    auto f = pA->GetAttributeAsFloat("quantity", 0.f) / static_cast<float>(pSD->maxCrew);
    if (f < 0.f)
        f = 0.f;
    if (f > 1.f)
        f = 1.f;
    return f;
}

void ShipInfoImages::CalculateDirectingVectors(const CVECTOR &pos)
{
    CVECTOR campos, camang;
    float camper;
    pRS->GetCamera(campos, camang, camper);

    m_fImgScale = camper * .05f * sqrtf(~(campos - pos));
    if (m_fImgScale < 1.f)
        m_fImgScale = 1.f;

    m_vRightDir = m_fImgScale * .5f * !((campos - pos) ^ CVECTOR(0.f, 1.f, 0.f));
    m_vUpDir = m_fImgScale * CVECTOR(0.f, 1.f, 0.f);
}

void ShipInfoImages::WriteSquareVertex(SII_VERTEX *pV, const CVECTOR &center, const CVECTOR &offset, const FPOINT &size,
                                       const FRECT &uv, float fProgress) const
{
    const CVECTOR vVert = size.y * m_vUpDir;

    // left side
    //--------------
    CVECTOR vc = center + m_fImgScale * offset - 0.5f * size.x * m_vRightDir + 0.5f * vVert;
    // top
    pV[0].pos = vc;
    pV[0].tu = uv.left;
    pV[0].tv = uv.top;
    // bottom
    pV[1].pos = vc - vVert;
    pV[1].tu = uv.left;
    pV[1].tv = uv.bottom;

    // right side
    //--------------
    // top
    vc += fProgress * size.x * m_vRightDir;
    pV[2].pos = vc;
    pV[2].tu = uv.right;
    pV[2].tv = uv.top;
    // bottom
    pV[3].pos = vc - vVert;
    pV[3].tu = uv.right;
    pV[3].tv = uv.bottom;
}
