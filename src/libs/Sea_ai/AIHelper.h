#ifndef AIHELPER_HPP
#define AIHELPER_HPP

#include "Character.h"
#include "Island_Base.h"
#include "SD2_H\SaveLoad.h"
#include "Script_Defines.h"
#include "collide.h"
#include "common_defines.h"
#include "dx8render.h"
#include "island_base.h"
#include "templates\DTimer.h"
#include "templates\array.h"
#include "templates\string.h"
#include "vmodule_api.h"

class AIAttributesHolder
{
  protected:
    ATTRIBUTES *pACharacter;

  public:
    virtual void SetACharacter(ATTRIBUTES *pAP)
    {
        pACharacter = pAP;
    };
    virtual ATTRIBUTES *GetACharacter() const
    {
        return pACharacter;
    };
};

class VAI_INNEROBJ;
class AIHelper
{
  public:
    AIHelper();
    ~AIHelper();

    static ATTRIBUTES *pASeaCameras;
    static ISLAND_BASE *pIsland;
    static VDX8RENDER *pRS;
    static COLLIDE *pCollide;

    static float fGravity;

    bool SetDevice();
    bool Init();
    bool Uninit();
    void AddCharacter(ATTRIBUTES *pACharacter, ATTRIBUTES *pAMainCharacter);
    void CalculateRelations();

    bool isFriend(ATTRIBUTES *pA1, ATTRIBUTES *pA2) const;
    bool isEnemy(ATTRIBUTES *pA1, ATTRIBUTES *pA2) const;
    bool isNeutral(ATTRIBUTES *pA1, ATTRIBUTES *pA2) const;

    ATTRIBUTES *GetMainCharacter(ATTRIBUTES *pACharacter);

    static VAI_INNEROBJ *FindAIInnerObj(ATTRIBUTES *pACharacter);

    dword GetRelation(ATTRIBUTES *pA1, ATTRIBUTES *pA2) const;
    dword GetRelationSafe(ATTRIBUTES *pA1, ATTRIBUTES *pA2) const;

    static void Print(float x, float y, float fScale, char *pFormat, ...);
    static void Print3D(CVECTOR vPos, float dy, float fScale, char *pFormat, ...);

    void Save(CSaveLoad *pSL);
    void Load(CSaveLoad *pSL);

  private:
    dword *pRelations, dwRelationSize;
    array<ATTRIBUTES *> aCharacters, aMainCharacters;

    dword *GetRelation(dword x, dword y) const;
    dword FindIndex(ATTRIBUTES *pACharacter) const;
};

extern AIHelper Helper;

#endif