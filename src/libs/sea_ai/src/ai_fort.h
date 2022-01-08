#pragma once

#include "ai_cannon.h"
#include "ai_ship.h"

#include "ship_lights.h"

// ============================================================================
// Master class AIFort
// Contain AI part for fort, fort cannons, collision part and other
// ============================================================================
class AIShip;
class AIGroup;

class AIFort : public VAI_OBJBASE
{
  public:
    class AI_FORT : public VAI_INNEROBJ
    {
      private:
        class TmpVAI_OBJBASE : public VAI_OBJBASE
        {
          public:
            TmpVAI_OBJBASE()
                : pFort(nullptr)
            {
                mTmp.SetIdentity();
            }

            CMatrix mTmp;
            AI_FORT *pFort;

            CMatrix *GetMatrix() override
            {
                return &GetModel()->mtx;
            };

            MODEL *GetModel() const override
            {
                return pFort->GetModel();
            };

            entid_t GetModelEID() const override
            {
                return pFort->GetModelEID();
            };

            void Save(CSaveLoad *pSL) override{};

            void Load(CSaveLoad *pSL) override{};

            void Fire(const CVECTOR &vPos) override{};

            float Cannon_Trace(int32_t iBallOwner, const CVECTOR &src, const CVECTOR &dst) override
            {
                return 2.0f;
            };

            float Trace(const CVECTOR &vSrc, const CVECTOR &vDst) override
            {
                return 2.0f;
            };

            bool Clip(const PLANE *planes, int32_t nplanes, const CVECTOR &vCenter, float radius,
                      ADD_POLYGON_FUNC addpoly) override
            {
                return false;
            };

            const char *GetCollideMaterialName() override
            {
                return nullptr;
            };

            bool GetCollideTriangle(TRIANGLE &triangle) override
            {
                return false;
            };

            void ProcessStage(Stage, uint32_t) override
            {
            }

            bool Mount(ATTRIBUTES *) override
            {
                return false;
            }

            void SetPos(const CVECTOR &vNewPos) override
            {
            }

            CVECTOR GetPos() const override
            {
                return {};
            }

            CVECTOR GetAng() const override
            {
                return {};
            }

            CVECTOR GetBoxsize() const override
            {
                return {};
            }
        };

        entid_t eidModel;
        entid_t eidBlot;

      public:
        CMatrix mOldMatrix;
        CVECTOR vPos;
        ATTRIBUTES *pFortLabelAP;
        TmpVAI_OBJBASE tmpObject;

        std::vector<AICannon> aCannons;   // fort cannons container
        std::vector<AICannon> aCulverins; // fort culverins container
        std::vector<AICannon> aMortars;   // fort mortars container

        uint32_t dwCannonType, dwCulverinType, dwMortarType;

        MODEL *GetModel() const
        {
            return static_cast<MODEL *>(EntityManager::GetEntityPointer(GetModelEID()));
        }

        void SetModelEID(entid_t _eidModel)
        {
            eidModel = _eidModel;
        }

        entid_t GetModelEID() const
        {
            return eidModel;
        }

        void SetBlotEID(entid_t _eidBlot)
        {
            eidBlot = _eidBlot;
        }

        entid_t GetBlotEID() const
        {
            return eidBlot;
        }

        uint32_t GetAllCannonsNum() const
        {
            return aCannons.size() + aCulverins.size() + aMortars.size();
        }

        uint32_t GetCannonType(uint32_t dwCannonIndex) const
        {
            if (dwCannonIndex < aCannons.size())
                return dwCannonType;
            if (dwCannonIndex < aCannons.size() + aCulverins.size())
                return dwCulverinType;
            return dwMortarType;
        }

        uint32_t GetDamagedCannonsNum()
        {
            uint32_t i, dwDamagedNum = 0;

            for (i = 0; i < aCannons.size(); i++)
                if (aCannons[i].isDamaged())
                    dwDamagedNum++;
            for (i = 0; i < aCulverins.size(); i++)
                if (aCulverins[i].isDamaged())
                    dwDamagedNum++;
            for (i = 0; i < aMortars.size(); i++)
                if (aMortars[i].isDamaged())
                    dwDamagedNum++;

            return dwDamagedNum;
        }

        bool isNormalMode() const
        {
            auto *pAFM = GetACharacter()->FindAClass(GetACharacter(), "Fort.Mode");
            if (pAFM)
                return (pAFM->GetAttributeAsDword() == FORT_NORMAL);
            return false;
        }

        AICannon *GetCannon(uint32_t dwCannonIndex)
        {
            if (dwCannonIndex < aCannons.size())
                return &aCannons[dwCannonIndex];
            if (dwCannonIndex < aCannons.size() + aCulverins.size())
                return &aCulverins[dwCannonIndex - aCannons.size()];
            return &aMortars[dwCannonIndex - (aCannons.size() + aCulverins.size())];
        }

        AI_FORT(ATTRIBUTES *_pFortLabelAP)
        {
            SetObjType(AIOBJ_FORT);

            tmpObject.pFort = this;

            pFortLabelAP = _pFortLabelAP;

            vPos.x = pFortLabelAP->GetAttributeAsFloat("x");
            vPos.y = 0.0f; // pFortLabelAP->GetAttributeAsFloat("y");
            vPos.z = pFortLabelAP->GetAttributeAsFloat("z");
        }

        CVECTOR GetPos() const override
        {
            return vPos;
        };
        CVECTOR GetAttackPoint(VAI_INNEROBJ *) override;

        CVECTOR GetFollowPoint(VAI_INNEROBJ *) override
        {
            return 0.0f;
        };

        CVECTOR GetAbordagePoint(VAI_INNEROBJ *) override
        {
            return 0.0f;
        };

        float GetMaxFireDistance() override
        {
            return 0.0f;
        };

        float GetMinFireDistance() override
        {
            return 0.0f;
        };

        void Save(CSaveLoad *pSL);
        void Load(CSaveLoad *pSL, entid_t eid);

        CVECTOR GetAng() const override
        {
            return {};
        }

        CVECTOR GetBoxsize() const override
        {
            return {};
        }

        void SetPos(const CVECTOR &vNewPos) override{};

        void SetAngleY(float fAngleY) override{};
    };

    uint32_t GetNumForts() const
    {
        return aForts.size();
    }

    AI_FORT *FindFort(ATTRIBUTES *pACharacter);

    AI_FORT *GetFort(uint32_t k)
    {
        Assert(k < aForts.size());
        return aForts[k];
    }

  private:
    CMatrix mtxFort;

    DTimer dtFiredTimer;
    AI_FORT *pLastTraceFort;
    std::vector<AI_FORT *> aForts; // fort container

    float fMinCannonDamageDistance;

    // Ships lights
    IShipLights *pShipsLights;

    void AddFortHit(int32_t iCharacterIndex, CVECTOR &vHitPos);
    float GetSpeedV0(uint32_t dwFortIndex);
    bool ScanFortForCannons(AI_FORT *pFort, char *pModelsDir, char *pLocatorsName) const;
    bool AddFort(ATTRIBUTES *pIslandAP, ATTRIBUTES *pFortAP, ATTRIBUTES *pFortCharacter, entid_t eidModel,
                 entid_t eidBlot);
    AI_FORT *FindFort(entid_t eidModel);

  public:
    static AIFort *pAIFort;

    AIFort();
    ~AIFort() override;

    // AI section
    bool isAttack(AIGroup *pGroup);
    bool isDead(); // is group dead?
    float GetPower();

    // inherit functions Entity
    bool Init() override;
    void SetDevice();

    void Realize(uint32_t Delta_Time);
    void Execute(uint32_t Delta_Time);

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

    bool CreateState(ENTITY_STATE_GEN *state_gen);
    bool LoadState(ENTITY_STATE *state);

    uint64_t ProcessMessage(MESSAGE &message) override;

    uint32_t AttributeChanged(ATTRIBUTES *pAttribute) override;

    // inherit functions COLLISION_OBJECT
    float Trace(const CVECTOR &vSrc, const CVECTOR &vDst) override;

    bool Clip(const PLANE *planes, int32_t nplanes, const CVECTOR &vCenter, float radius,
              ADD_POLYGON_FUNC addpoly) override
    {
        return false;
    };

    const char *GetCollideMaterialName() override
    {
        return nullptr;
    };

    bool GetCollideTriangle(TRIANGLE &triangle) override
    {
        return false;
    };

    // inherit functions CANNON_TRACE_BASE
    float Cannon_Trace(int32_t iBallOwner, const CVECTOR &vSrc, const CVECTOR &vDst) override;

    // inherit functions VAI_OBJBASE
    ATTRIBUTES *GetACharacter() override;

    CMatrix *GetMatrix() override
    {
        return &mtxFort;
    };

    MODEL *GetModel() const override
    {
        return nullptr;
    };

    entid_t GetModelEID() const override
    {
        return {};
    };

    CVECTOR GetPos() const override
    {
        return CVECTOR(0.0f, 0.0f, 0.0f);
    };

    CVECTOR GetAng() const override
    {
        return CVECTOR(0.0f, 0.0f, 0.0f);
    };

    bool Mount(ATTRIBUTES *pAttribute) override;

    void Save(CSaveLoad *pSL) override;
    void Load(CSaveLoad *pSL) override;

    void Fire(const CVECTOR &vPos) override;

    void SetPos(const CVECTOR &vNewPos) override{};

    CVECTOR GetBoxsize() const override
    {
        return {};
    }
};
