//============================================================================================
//    Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//    Grass
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#pragma once

#include "Supervisor.h"
#include "dx9render.h"
#include "grs.h"
#include "vmodule_api.h"

#define MSG_GRASS_LOAD_DATA 40666
#define MSG_GRASS_SET_TEXTURE 41666
#define MSG_GRASS_SET_PARAM 42666

class GEOS;

class Character;

class Grass : public Entity
{

    static inline ID3DXEffect *fx_;
    static inline IDirect3DVertexDeclaration9 *vertexDecl_;
    static inline D3DXHANDLE hgVP_;
    static inline D3DXHANDLE haAngles_;
    static inline D3DXHANDLE haUV_;
    static inline D3DXHANDLE hlDir_;
    static inline D3DXHANDLE hkLitWF_;
    static inline D3DXHANDLE haColor_;
    static inline D3DXHANDLE hlColor_;
    static inline D3DXHANDLE hfDataScale_;
    static inline D3DXHANDLE haSize_;

#pragma pack(push, 1)

    struct Vertex
    {
        float x, y, z;
        uint32_t data;
        uint32_t offset;
        float wx, wz;
        float alpha;
    };

    struct VSConstant
    {
        VSConstant()
        {
            x = y = z = 0.0f;
            w = 1.0f;
        };

        VSConstant(float _x, float _y, float _z, float _w)
        {
            x = _x;
            y = _y;
            z = _z;
            w = _w;
        };
        float x, y, z, w;
    };

#pragma pack(pop)

    struct AngleInfo
    {
        float dx, dz; // Direction
        float cs;     // Cosine of the angle between direction and source
    };

    struct GRSMapElementEx
    {
        float x, y, z; // The position of a blade of grass in the world
        union {
            struct
            {
                uint8_t frame; // Frame
                uint8_t h;     // Height
                uint8_t w;     // Width
                uint8_t ang;   // Angle of rotation
            };

            uint32_t data;
        };
    };

    enum RenderQuality
    {
        rq_full = 0,
        rq_middle = 1,
        rq_low = 2,
        rq_off = 3,
    };

  public:
    struct CharacterPos
    {
        CVECTOR pos;     // Current position
        CVECTOR lastPos; // Inertial position
        Character *chr;  // Character pointer
        long useCounter; // Grass Influence Counter
    };

    // --------------------------------------------------------------------------------------------
    // Construction, destruction
    // --------------------------------------------------------------------------------------------
  public:
    Grass();
    virtual ~Grass();

    // Initialization
    bool Init() override;
    // Work
    void Execute(uint32_t delta_time);
    void Realize(uint32_t delta_time);
    //
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

    // Load data for grass
    bool LoadData(const char *patchName);
    // Set texture
    void SetTexture(const char *texName);

    CharacterPos characters[MAX_CHARACTERS];
    long numCharacters;

    // --------------------------------------------------------------------------------------------
    // Encapsulation
    // --------------------------------------------------------------------------------------------
  private:
    // Render block
    void RenderBlock(const CVECTOR &camPos, const PLANE *plane, long numPlanes, long mx, long mz);
    // Box visibility check
    bool VisibleTest(const PLANE *plane, long numPlanes, const CVECTOR &min, const CVECTOR &max);
    // Render block
    void RenderBlock(GRSMiniMapElement &mme, float kLod);
    // Draw the contents of the buffer
    void DrawBuffer();
    // Get the color
    static long GetColor(CVECTOR color);
    // Vertex declaration
    void CreateVertexDeclaration() const;

  private:
    // Render service
    VDX9RENDER *rs;
    // Buffers
    long vb, ib;
    long numPoints;
    // Texture
    long texture;

    // Minimap
    GRSMiniMapElement *miniMap;
    // Minimap sizes
    long miniX, miniZ;
    // Starting position of the map
    float startX, startZ;
    // Blocks with heights
    GRSMapElementEx *block;
    long numElements;
    // Current parameters for grass turns
    AngleInfo angInfo[16];
    // Current swing phases
    float phase[7];
    // Current light source options
    CVECTOR lDir;   // Direction of the source
    CVECTOR lColor; // Source color
    CVECTOR aColor; // Ambient light color

    long blockChrs[32]; // Indexes of characters processed by the block
    long numBlockChr;   // Number of characters processed by the block

    float lodSelect; // Lod selection range factor (kLod = kLod^lodSelect)
    float winForce;  // Wind speed coefficient 0..1
    CVECTOR winDir;  // Normalized wind direction

    Vertex *vbuffer; // Locked buffer

    RenderQuality quality; // Rendering quality

    float cosPh1, sinPh2, sinPh5, sinPh6, winPow, winF10, kAmpWF, kDirWF, kLitWF;
    float windAng;
    long initForce;

    D3DXVECTOR3 aAngles[16];
    D3DXVECTOR2 aUV[16];

    char textureName[64];

    float m_fDataScale;
    float m_fMaxWidth;
    float m_fMaxHeight;
    float m_fMinVisibleDist;
    float m_fMaxVisibleDist;
    float m_fMinGrassLod;

    // boal grass parameters
    long isGrassLightsOn;
};
