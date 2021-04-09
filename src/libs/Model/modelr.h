#pragma once

#include "dx9render.h"
#include "geometry.h"
#include "model.h"

class NODER : public NODE
{
    char *sys_modelName;
    char *sys_LightPath;
    char *sys_TexPath;
    char *sys_lmPath;
    bool isReleaed;

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
    ~NODER();
    void Draw();
    float Trace(const CVECTOR &src, const CVECTOR &dst);
    NODER *GetNode(long n);
    NODER *FindNode(const char *cNodeName);
    float Update(CMatrix &mtx, CVECTOR &cnt);
    const char *GetName();
    bool Clip();

    // unlink node
    NODE *Unlink();
    // unlink node to model
    entid_t Unlink2Model();
    // link node to node
    void Link(NODE *node);
    // link model to node
    void Link(entid_t model, bool transform = true);

    void SetTechnique(const char *name);
    const char *GetTechnique();

    void ReleaseGeometry();
    void RestoreGeometry();

    void SetMaxViewDist(float fDist);
};

#define MODEL_ANI_MAXBUFFERS 16

class MODELR : public MODEL
{
    char LightPath[256], lmPath[256];
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
    virtual ~MODELR();
    bool Init();
    void Realize(uint32_t Delta_Time);
    uint64_t ProcessMessage(MESSAGE &message);
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

    virtual NODE *GetNode(long n);
    virtual NODE *FindNode(const char *cNodeName);
    virtual void Update();
    virtual Animation *GetAnimation();

    virtual float Trace(const CVECTOR &src, const CVECTOR &dst);
    virtual const char *GetCollideMaterialName();
    virtual bool GetCollideTriangle(TRIANGLE &triangle);
    virtual bool Clip(const PLANE *planes, long nplanes, const CVECTOR &center, float radius, ADD_POLYGON_FUNC addpoly);

    virtual NODE *GetCollideNode();

  protected:
    bool useBlend;
    uint32_t blendTime, passedTime;
    char blendTechnique[128];
    float alpha1, alpha2;
};
