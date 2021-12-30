//============================================================================================
//    Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//    LGeometry
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#pragma once

#include "LTypes.h"
#include "model.h"

class LGeometry
{
    struct Object
    {
        Object()
            : model(0)
        {
            name = nullptr;
            nameReal = nullptr;
            m = nullptr;
            lBufSize = 0;
        }
        ;
        char *name;     // The path of the .col file
        char *nameReal; // Model name
        MODEL *m;       // Pointer to the model
        entid_t model;  // Model
        long lBufSize;  // Color buffer size for this model
    };

    // --------------------------------------------------------------------------------------------
    // Construction, destruction
    // --------------------------------------------------------------------------------------------
  public:
    LGeometry();
    virtual ~LGeometry();

    // Set path to models
    void SetModelsPath(const char *mPath);
    // Set path for current weather
    void SetLightPath(const char *lPath);
    // Add object
    void AddObject(const char *name, entid_t model);
    // Process data
    bool Process(VDX9RENDER *rs, long numLights);
    // Draw normals
    void DrawNormals(VDX9RENDER *rs);
    // Update colors in buffers
    void UpdateColors(VDX9RENDER *rs);
    // Trace the ray through all models
    float Trace(const CVECTOR &src, const CVECTOR &dst);
    // Save lighting
    bool Save();

    std::vector<Object> object;
    long numObjects;
    long maxObjects;

    std::vector<Vertex> vrt;
    long numVrt;
    long maxVrt;

    std::vector<Triangle> trg;
    long numTrg;
    long maxTrg;

    std::vector<VertexBuffer> vbuffer;
    long numVBuffers;
    long maxVBuffers;

    lighter::Shadow *shadows;

    CVECTOR min, max;
    float radius;

    bool useColor;

    CVECTOR *drawbuf;

    char modelsPath[512];
    char lightPath[512];
};
