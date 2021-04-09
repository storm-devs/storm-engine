#pragma once

#include "../xdefines.h"
#include "Entity.h"
#include "Matrix.h"
#include "dx9render.h"

class MODEL;
class NODE;
class CMatrix;

class InterfaceBackScene : public Entity
{
    struct LightParam
    {
        bool bUse;

        long indexLight;
        D3DLIGHT9 lightSource;
        D3DLIGHT9 lightOldSource;

        uint32_t dwFlareColor;
        float fMinFlareColor;
        float fMaxFlareColor;

        D3DCOLORVALUE colorMin, colorMax;
        float fColorTimer;
        float fColorPeriod;
        float fAddPeriod;
        float fAddPeriodMax;

        float fRangeMin, fRangeMax;
        float fRangeTimer;
        float fRangePeriod;

        MODEL *pModel;
        entid_t eiModel;

        CVECTOR vLightPos;
        NODE *pLightSrcNode;

        LightParam()
        {
            bUse = false;
            pModel = nullptr;
            pLightSrcNode = nullptr;
            fColorTimer = 0.0f;
        }

        ~LightParam();
        void UpdateParams(float fTime);
    };

  public:
    InterfaceBackScene();
    ~InterfaceBackScene();

    bool Init() override;
    void Execute(uint32_t Delta_Time);
    void Realize(uint32_t Delta_Time);
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
    VDX9RENDER *m_pRS;

    entid_t m_eiModel;
    MODEL *m_pModel;
    entid_t m_eiLocators;
    MODEL *m_pLocators;

    FXYPOINT m_pntOldMouse;
    CVECTOR m_vCamPos;
    CVECTOR m_vCamAng;
    float m_fCamPerspective;

    std::vector<LightParam *> m_aLights;

    struct MenuDescr
    {
        bool bSelectable;
        entid_t eiActive;
        MODEL *pActive;
        entid_t eiPassive;
        MODEL *pPassive;
        std::string sEventName;

        MenuDescr()
        {
            pActive = nullptr;
            pPassive = nullptr;
            bSelectable = false;
        }

        ~MenuDescr();
        void Set(CMatrix *pMtx, const char *pcActiveName, const char *pcPassiveName, const char *pcEvent,
                 const char *pcPathName, const char *pcTechniqueName);
    };

    std::vector<MenuDescr *> m_aMenuDescr;
    long m_nSelectMenuIndex;

    void LoadModel(const char *pcModelName);
    void SetCameraPosition(const char *pcLocatorName);
    void SetShipPosition(const char *pcLocName, ATTRIBUTES *pAChar) const;

    bool FindLocator(const char *pcLocName, CMatrix *pMtx, CVECTOR *pPos, float *pYAng) const;
    void SetLocatorPosition(MODEL *pModel, const char *pcLocName, CVECTOR &pos, NODE *&pNodPtr);

    void ReleaseMenuList();
    void CreateMenuList(long nStartIndex, ATTRIBUTES *pAMenu);
    void ChooseNextMenu();
    void ChoosePrevMenu();
    void SetNewMenu(long nNewSelectIndex);
    void SetMenuSelectableState(long nMenuIndex, bool bSelectable);
    void ExecuteMenu(long nMenuIndex);

    long CheckMousePos(float fX, float fY);

    void InitLight(ATTRIBUTES *pAParam);
    void SetLight();
    void RestoreLight();
    void FlareShow(long idx);

    void InitAniModel(ATTRIBUTES *pAParam);
    void InitStaticModel(ATTRIBUTES *pAParam);

    struct AniModelDescr
    {
        entid_t ei;
        MODEL *pModel;

        bool bUseTFactor;
        uint32_t dwTFactor;

        AniModelDescr()
        {
            pModel = nullptr;
            bUseTFactor = false;
        }

        ~AniModelDescr()
        {
            EntityManager::EraseEntity(ei);
            pModel = nullptr;
        }
    };

    std::vector<AniModelDescr *> m_apAniModel;

    // a fly is a migratory bird!
    struct Particle
    {
        CVECTOR pos;
        float angle;
        float size;
        float alpha;
    };

    struct ParticleEx : public Particle
    {
        uint32_t color;
        float frame;
    };

    struct ParticleFly : public ParticleEx
    {
        float ax, ay;
        float kx, ky;
        float a, k;
    };

    struct LampFlys
    {
        CVECTOR pos;
        float radius;
        long start;
        long num;
    };

    struct Vertex
    {
        CVECTOR pos;
        uint32_t color;
        float u, v;
    };

    std::vector<LampFlys> flys;
    long numFlys;
    long maxFlys;
    std::vector<ParticleFly> fly;
    long numFly;
    long flyTex;
    Vertex buffer[256 * 6];

    CVECTOR m_vFlarePos;
    float m_fFlareSize;
    long m_nFlareTexture;

    void AddLampFlys(CVECTOR &pos);
    void ProcessedFlys(float dltTime);
    void DrawParticles(void *prts, long num, long size, long texture, const char *tech, bool isEx = false,
                       long numU = 0);
};
