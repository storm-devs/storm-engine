#pragma once

#include <string>

#include "dx9render.h"
#include "geometry.h"
#include "model.h"

class NODER : public NODE
{
    std::string sys_modelName_base;
    std::string sys_modelName_full;
    std::string sys_LightPath;
    std::string sys_TexPath;
    std::string sys_lmPath;
    bool isReleased;

    static long depth, node;
    uintptr_t idGeoGroup; // id of "geometry" string
    char technique[256], name[256];

    // local radius and center of geometry
    float geo_radius;
    CVECTOR geo_center;

    float max_view_dist;
    float distance_blend;

  public:
    // local radius and center of whole node with children
    float radius;
    CVECTOR center;

    static VGEOMETRY *gs;
    static VDX9RENDER *rs;

    bool Init(const char *lightPath, const char *pname, const char *oname, const CMatrix &m, const CMatrix &globm,
              NODER *par, const char *lmPath) override;
    NODER();
    ~NODER() override;
    void Draw();
    float Trace(const CVECTOR &src, const CVECTOR &dst) override;
    NODER *GetNode(long n);
    NODER *FindNode(const char *cNodeName);
    float Update(CMatrix &mtx, CVECTOR &cnt);
    const char *GetName() override;
    bool Clip();

    // unlink node
    NODE *Unlink() override;
    // unlink node to model
    entid_t Unlink2Model() override;
    // link node to node
    void Link(NODE *node) override;
    // link model to node
    void Link(entid_t model, bool transform = true) override;

    void SetTechnique(const char *name) override;
    const char *GetTechnique() override;
    
    // replace only this node model without touching anything else
    void SubstituteGeometry(const std::string &new_model) override;

    void ReleaseGeometry();
    void RestoreGeometry();

    void SetMaxViewDist(float fDist);

};

#define MODEL_ANI_MAXBUFFERS 16

class MODELR : public MODEL
{
    std::string LightPath, lmPath;

    struct ANIBUFFER
    {
        FVF_VERTEX *v;
        long num;
        long vb;
    };

    VDX9RENDER *rs;
    VGEOMETRY *GeometyService;
    Animation *ani;
    float aniPos[ANI_MAX_ACTIONS];

    bool bSetupFog;
    bool bFogEnable;
    float fFogDensity;

    long nAniVerts;

    ANIBUFFER aniVerts[MODEL_ANI_MAXBUFFERS];
    void AniRender();
    NODE *colideNode;
    void FindPlanes(const CMatrix &view, const CMatrix &proj);
    IDirect3DVertexBuffer9 *d3dDestVB;

    unsigned short *idxBuff;

  public:
    NODER *root;

    MODELR();
    ~MODELR() override;
    bool Init() override;
    void Realize(uint32_t Delta_Time);
    uint64_t ProcessMessage(MESSAGE &message) override;
    void LostRender();
    void RestoreRender();

    void ProcessStage(Stage stage, uint32_t delta) override
    {
        switch (stage)
        {
            // case Stage::execute:
            //    Execute(delta); break;
        case Stage::realize:
            Realize(delta);
            break;
        case Stage::lost_render:
            LostRender();
            break;
        case Stage::restore_render:
            RestoreRender();
            break;
        }
    }

    NODE *GetNode(long n) override;
    NODE *FindNode(const char *cNodeName) override;
    void Update() override;
    Animation *GetAnimation() override;

    float Trace(const CVECTOR &src, const CVECTOR &dst) override;
    const char *GetCollideMaterialName() override;
    bool GetCollideTriangle(TRIANGLE &triangle) override;
    bool Clip(const PLANE *planes, long nplanes, const CVECTOR &center, float radius,
              ADD_POLYGON_FUNC addpoly) override;

    NODE *GetCollideNode() override;

  protected:
    bool useBlend;
    uint32_t blendTime, passedTime;
    std::string blendTechnique;
    float alpha1, alpha2;
};
