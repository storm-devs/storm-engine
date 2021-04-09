// ============================================================================================
// Spirenkov Maxim, 2001
// --------------------------------------------------------------------------------------------
// Sea dogs II
// --------------------------------------------------------------------------------------------
// ModelArray
// --------------------------------------------------------------------------------------------
// Storing model information
// ============================================================================================

#pragma once

#include "model.h"
#include "vmodule_api.h"

class Animation;

class PathTracer;

#define MA_MAX_NAME_LENGTH 256

class ModelArray
{
    class UVSlider : public MODEL::RenderTuner
    {
      public:
        void Set(MODEL *model, VDX9RENDER *rs) override;
        void Restore(MODEL *model, VDX9RENDER *rs) override;

        float u0, v0;
        float us0, vs0;
        float u1, v1;
        float us1, vs1;
    };

    class Relection : public MODEL::RenderTuner
    {
      public:
        void Set(MODEL *model, VDX9RENDER *rs) override;
        void Restore(MODEL *model, VDX9RENDER *rs) override;
        uint32_t tfactor;
    };

    struct Rotator
    {
        float rx, ry, rz;
    };

    struct LocationModel
    {
        entid_t modelrealizer; // Model renderer
        entid_t id;            // Model
        uint32_t hash;         // Hash value for quick search
        union {
            uint32_t flags;

            struct
            {
                uint32_t isVisible : 1;
            };
        };

        UVSlider *slider;
        Rotator *rotator;
        Relection *reflection;
        char name[MA_MAX_NAME_LENGTH]; // Model name
    };

    // --------------------------------------------------------------------------------------------
    // Construction, destruction
    // --------------------------------------------------------------------------------------------
  public:
    ModelArray();
    virtual ~ModelArray();

    // Create model
    long CreateModel(const char *modelName, const char *technique, long level, bool isVisible = true,
                     void *pLights = nullptr);
    // Delete model
    void DeleteModel(long modelIndex);
    // Set animation to the model
    bool SetAnimation(long modelIndex, const char *modelAni);
    // Find model index by name
    long FindModel(const char *modelName);

    // Check if the index is correct
    bool IsValidateIndex(long index) const;
    // Get model name
    const char *GetModelName(long index);

    // Number of models
    long Models() const;
    // Getting model ID by index
    entid_t ID(long modelIndex);
    // Getting a model by index
    MODEL *operator[](long modelIndex);
    // Getting animation by index
    Animation *GetAnimation(long modelIndex);
    // Getting the renderer ID by index
    entid_t RealizerID(long modelIndex);

    // Set slide uv animation to the model
    void SetUVSlide(long modelIndex, float u0, float v0, float u1, float v1);
    // Set a rotation animation to the model
    void SetRotation(long modelIndex, float rx, float ry, float rz);
    // Set the reflection matrix generation mode to the model
    void SetReflection(long modelIndex, float scale);

    // Animate
    void Update(float dltTime);

    void UpdateModelsPath();
    void UpdateTexturesPath();
    void UpdateLightPath();
    void UpdateShadowPath();

    // Check the visibility of two points
    bool VisibleTest(const CVECTOR &p1, const CVECTOR &p2);
    // Trace the ray through the location
    float Trace(const CVECTOR &src, const CVECTOR &dst);
    bool GetCollideTriangle(TRIANGLE &trg) const;
    void Clip(PLANE *p, long numPlanes, CVECTOR &cnt, float rad, bool (*fnc)(const CVECTOR *vtx, long num));

    // --------------------------------------------------------------------------------------------
    // Encapsulation
    // --------------------------------------------------------------------------------------------
  private:
    uint32_t CalcHashString(const char *str);
    static void UpdatePath(char *path);

  private:
    // Location models
    std::vector<LocationModel> model;
    long numModels;
    long maxModels;
    TRIANGLE ctrg;
    bool isHavecTrg;

  public:
    char modelspath[256];
    char texturespath[256];
    char lightpath[256];
    char shadowpath[256];
    char resPath[512];
};

// Check if the index is correct
inline bool ModelArray::IsValidateIndex(long index) const
{
    return index >= 0 && index < numModels;
}

// Get model name
inline const char *ModelArray::GetModelName(long index)
{
    if (index >= 0 && index < numModels)
        return model[index].name;
    return nullptr;
}
