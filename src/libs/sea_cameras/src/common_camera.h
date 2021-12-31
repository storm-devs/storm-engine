#pragma once

#include "entity.h"
#include "sd2_h/vai_objbase.h"
#include "core.h"
#include "v_module_api.h"

class COMMON_CAMERA : public Entity
{
  private:
    bool bActive;
    bool bOn;

    float fPerspective;

    entid_t eidObject;
    VAI_OBJBASE *pAIObj;

  protected:
    ATTRIBUTES *pACharacter;

  public:
    bool FindShip()
    {
        Assert(pACharacter);
        // get entity id from loaded ships
        const auto &entities = EntityManager::GetEntityIdVector("ship");
        for (auto ship : entities)
        {
            auto *pObj = static_cast<VAI_OBJBASE *>(EntityManager::GetEntityPointer(ship));
            if (pObj->GetACharacter() == pACharacter)
            {
                SetEID(pObj->GetModelEID());
                SetAIObj(pObj);
                return true;
            }
        }
        return false;
    }

    MODEL *GetModelPointer() const
    {
        return static_cast<MODEL *>(EntityManager::GetEntityPointer(eidObject));
    }

    void SetAIObj(VAI_OBJBASE *_pAIObj)
    {
        pAIObj = _pAIObj;
    }

    VAI_OBJBASE *GetAIObj() const
    {
        return pAIObj;
    }

    void SetEID(entid_t pEID)
    {
        eidObject = pEID;
    };

    entid_t GetEID() const
    {
        return eidObject;
    };

    virtual void SetCharacter(ATTRIBUTES *_pACharacter)
    {
        pACharacter = _pACharacter;
    };

    void SetPerspective(float _fPerspective)
    {
        fPerspective = _fPerspective;
    };

    float GetPerspective() const
    {
        return fPerspective;
    };

    void SetOn(bool bOnOff)
    {
        bOn = bOnOff;
    };

    void SetActive(bool bNewActive)
    {
        bActive = bNewActive;
    };

    bool isOn() const
    {
        return bOn;
    };

    bool isActive() const
    {
        return bActive;
    };

    virtual void Save(CSaveLoad *pSL) = 0;
    virtual void Load(CSaveLoad *pSL) = 0;

    COMMON_CAMERA()
        : eidObject(0), pAIObj(nullptr), pACharacter(nullptr)
    {
        bOn = false;
        bActive = false;
        fPerspective = 1.285f;
    }
    ;

    ~COMMON_CAMERA() override{};
};
