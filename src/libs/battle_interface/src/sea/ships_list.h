#pragma once

#include "bi_defines.h"
#include "vai_objbase.h"

class SHIP_DESCRIBE_LIST
{
  public:
    struct SHIP_DESCR
    {
        bool isMyShip;
        bool isTransferableShip;
        bool isDead;
        int32_t relation;
        int32_t characterIndex;
        ATTRIBUTES *pAttr; // ship attributes
        int32_t maxHP;
        int32_t maxSP;
        int32_t maxCrew;
        int32_t pictureNum;
        int32_t selectPictureNum;
        int32_t textureNum;
        VAI_OBJBASE *pShip;
        uint32_t dwShipColor;

        SHIP_DESCR *next;
    };

  public:
    SHIP_DESCRIBE_LIST();
    ~SHIP_DESCRIBE_LIST();

    void Add(int32_t mainChrIndex, int32_t chIdx, ATTRIBUTES *pChAttr, ATTRIBUTES *pShipAttr, bool bMyShip,
             bool bTransferableShip, int32_t relation, uint32_t dwShipColor);

    SHIP_DESCR *GetMainCharacterShip() const
    {
        return mainCharacter;
    }

    ATTRIBUTES *GetMainCharacterShipAttr() const
    {
        return pMainShipAttr;
    }

    void Release(int32_t charIdx);
    void ShipSink(int32_t charIdx);
    void ReleaseAll();
    SHIP_DESCR *FindShip(int32_t idxCharacter) const;

    SHIP_DESCR *GetShipRoot() const
    {
        return root;
    }

    void Refresh();

  protected:
    SHIP_DESCR *root;
    SHIP_DESCR *mainCharacter;
    ATTRIBUTES *pMainShipAttr;
};

extern SHIP_DESCRIBE_LIST g_ShipList;
