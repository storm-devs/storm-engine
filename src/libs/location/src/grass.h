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

#include "supervisor.h"
#include "dx9render.h"
#include "grs.h"
#include "vma.hpp"

#define MSG_GRASS_LOAD_DATA 40666
#define MSG_GRASS_SET_TEXTURE 41666
#define MSG_GRASS_SET_PARAM 42666

class GEOS;

class Character;

class Grass : public Entity
{
#ifdef _WIN32 // Effects
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
#else
    static inline IDirect3DVertexDeclaration9 *vertexDecl_;
#endif

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
        int32_t useCounter; // Grass Influence Counter
    };

    // --------------------------------------------------------------------------------------------
    // Construction, destruction
    // --------------------------------------------------------------------------------------------
  public:
    Grass();
    ~Grass() override;

    // Initialization
    bool Init() override;
    // Work
    void Execute(uint32_t delta_time);
    void Realize(uint32_t delta_time);
    //
    uint64_t ProcessMessage(MESSAGE &message) override;

    void RestoreRender();

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
        case Stage::restore_render:
            RestoreRender();
            break;
        }
    }

    // Load data for grass
    bool LoadData(const char *patchName);
    // Set texture
    void SetTexture(const char *texName);

    std::vector<CharacterPos> characters;

    // --------------------------------------------------------------------------------------------
    // Encapsulation
    // --------------------------------------------------------------------------------------------
  private:
    // Render block
    void RenderBlock(const CVECTOR &camPos, const PLANE *plane, int32_t numPlanes, int32_t mx, int32_t mz);
    // Box visibility check
    bool VisibleTest(const PLANE *plane, int32_t numPlanes, const CVECTOR &min, const CVECTOR &max);
    // Render block
    void RenderBlock(GRSMiniMapElement &mme, float kLod);
    // Draw the contents of the buffer
    void DrawBuffer();
    // Get the color
    static int32_t GetColor(CVECTOR color);
    // Vertex declaration
    void CreateVertexDeclaration() const;

  private:
    // Render service
    VDX9RENDER *rs;
    // Buffers
    int32_t vb, ib;
    int32_t numPoints;
    // Texture
    int32_t texture;

    // Minimap
    GRSMiniMapElement *miniMap;
    // Full non-empty cached minimap
    std::vector<std::pair<int32_t, int32_t>> cachedMiniMap;
    // Minimap sizes
    int32_t miniX, miniZ;
    // Starting position of the map
    float startX, startZ;
    // Blocks with heights
    GRSMapElementEx *block;
    int32_t numElements;
    // Current parameters for grass turns
    AngleInfo angInfo[16];
    // Current swing phases
    float phase[7];
    // Current light source options
    CVECTOR lDir;   // Direction of the source
    CVECTOR lColor; // Source color
    CVECTOR aColor; // Ambient light color

    std::vector<int32_t> blockChrs; // Indexes of characters processed by the block

    float lodSelect; // Lod selection range factor (kLod = kLod^lodSelect)
    float winForce;  // Wind speed coefficient 0..1
    CVECTOR winDir;  // Normalized wind direction

    Vertex *vbuffer; // Locked buffer

    RenderQuality quality; // Rendering quality

    float cosPh1, sinPh2, sinPh5, sinPh6, winPow, winF10, kAmpWF, kDirWF, kLitWF;
    float windAng;
    int32_t initForce;

#ifdef _WIN32 // Effects
    D3DXVECTOR3 aAngles[16];
    D3DXVECTOR2 aUV[16];
#else
    VSConstant consts[42];
#endif

    char textureName[64];

    float m_fDataScale;
    float m_fMaxWidth;
    float m_fMaxHeight;
    float m_fMinVisibleDist;
    float m_fMaxVisibleDist;
    float m_fMinGrassLod;

    // boal grass parameters
    int32_t isGrassLightsOn;
};
