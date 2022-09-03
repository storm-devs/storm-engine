// ============================================================================================
// Spirenkov Maxim, 2001
// --------------------------------------------------------------------------------------------
// Sea dogs II
// --------------------------------------------------------------------------------------------
// ModelArray
// --------------------------------------------------------------------------------------------
// Storing model information
// ============================================================================================

#include "geometry.h"

#include "dx9render.h"
#include "model_array.h"
#include "core.h"
#include "entity.h"
#include "string_compare.hpp"
#include "shared/messages.h"

// ============================================================================================
// Construction, destruction
// ============================================================================================

ModelArray::ModelArray()
{
    numModels = 0;
    maxModels = 0;
    modelspath[0] = 0;
    texturespath[0] = 0;
    lightpath[0] = 0;
    isHavecTrg = false;
}

ModelArray::~ModelArray()
{
    while (numModels)
        DeleteModel(numModels - 1);
}

// Create model
int32_t ModelArray::CreateModel(const char *modelName, const char *technique, int32_t level, bool isVisible, void *pLights)
{
    if (!modelName || !modelName[0])
        return -1;
    // Path to the model
    resPath = modelspath + modelName;
    // Path to textures
    auto *gs = static_cast<VGEOMETRY *>(core.GetService("geometry"));
    if (!gs)
    {
        core.Trace("Can't create geometry service!");
        return -1;
    }
    gs->SetTexturePath(texturespath.c_str());
    // Expanding the array
    if (numModels == maxModels)
    {
        maxModels += 4;
        model.resize(maxModels);
    }
    // Create a model
    entid_t id, idModelRealizer;
    if (!(id = core.CreateEntity("modelr")))
        return -1;
    if (!(idModelRealizer = core.CreateEntity("LocModelRealizer")))
    {
        core.EraseEntity(id);
        return -1;
    }
    core.Send_Message(idModelRealizer, "lip", 1, id, pLights);
    // if(isVisible) core.AddToLayer(realize, idModelRealizer, level);
    core.AddToLayer(REALIZE, idModelRealizer, level);
    core.Send_Message(idModelRealizer, "ll", 2, isVisible);
    auto *m = static_cast<MODEL *>(core.GetEntityPointer(id));
    if (!m)
    {
        gs->SetTexturePath("");
        core.EraseEntity(id);
        core.EraseEntity(idModelRealizer);
        return -1;
    }
    // Loading
    core.Send_Message(id, "ls", MSG_MODEL_SET_LIGHT_PATH, lightpath.c_str());
    core.Send_Message(id, "ls", MSG_MODEL_SET_LIGHT_LMPATH, shadowpath.c_str());
    if (!core.Send_Message(id, "ls", MSG_MODEL_LOAD_GEO, resPath.c_str()))
    {
        gs->SetTexturePath("");
        core.EraseEntity(id);
        core.EraseEntity(idModelRealizer);
        return -1;
    }
    gs->SetTexturePath("");

    // remember
    model[numModels].id = id;
    model[numModels].modelrealizer = idModelRealizer;
    // Save the model name
    if (strlen(modelName) < MA_MAX_NAME_LENGTH)
    {
        strcpy_s(model[numModels].name, modelName);
    }
    else
    {
        core.Trace("Model name %s is very int32_t", maxModels);
        memcpy(model[numModels].name, modelName, MA_MAX_NAME_LENGTH);
        model[numModels].name[MA_MAX_NAME_LENGTH - 1] = 0;
    }
    model[numModels].hash = CalcHashString(model[numModels].name);
    model[numModels].slider = nullptr;
    model[numModels].rotator = nullptr;
    model[numModels].reflection = nullptr;
    model[numModels].flags = 0;
    model[numModels].isVisible = isVisible;
    // Setting the model technique
    /*if(!technique || !technique[0])
    {
      technique = "DLightModel";
    }*/
    if (technique && technique[0])
    {
        for (int32_t i = 0; i < 1024; i++)
        {
            auto *nd = m->GetNode(i);
            if (!nd)
                break;
            nd->SetTechnique(technique);
        }
    }
    // Returning the index
    return numModels++;
}

// Delete model
void ModelArray::DeleteModel(int32_t modelIndex)
{
    Assert(modelIndex >= 0 && modelIndex < numModels);
    // Removing effects
    delete model[modelIndex].slider;
    model[modelIndex].slider = nullptr;
    delete model[modelIndex].rotator;
    model[modelIndex].rotator = nullptr;
    delete model[modelIndex].reflection;
    model[modelIndex].reflection = nullptr;
    // Delete the model
    core.EraseEntity(model[modelIndex].modelrealizer);
    core.EraseEntity(model[modelIndex].id);
    numModels--;
    if (modelIndex != numModels)
        model[modelIndex] = model[numModels];
}

// Set animation to the model
bool ModelArray::SetAnimation(int32_t modelIndex, const char *modelAni)
{
    Assert(modelIndex >= 0 && modelIndex < numModels);
    return core.Send_Message(model[modelIndex].id, "ls", MSG_MODEL_LOAD_ANI, modelAni) != 0;
}

// Find model index by name
int32_t ModelArray::FindModel(const char *modelName)
{
    if (!modelName)
        return -1;
    // Convert to internal names
    char buf[MA_MAX_NAME_LENGTH];
    if (strlen(modelName) < MA_MAX_NAME_LENGTH)
    {
        strcpy_s(buf, modelName);
    }
    else
    {
        memcpy(buf, modelName, MA_MAX_NAME_LENGTH);
        buf[MA_MAX_NAME_LENGTH - 1] = 0;
    }
    // Looking for hash value
    const auto hash = CalcHashString(buf);
    // looking for a model
    for (int32_t i = 0; i < numModels; i++)
    {
        if (model[i].hash == hash)
        {
            if (storm::iEquals(model[i].name, buf))
            {
                return i;
            }
        }
    }
    return -1;
}

// Number of models
int32_t ModelArray::Models() const
{
    return numModels;
}

// Getting model ID by index
entid_t ModelArray::ID(int32_t modelIndex)
{
    Assert(modelIndex >= 0 && modelIndex < numModels);
    return model[modelIndex].id;
}

entid_t ModelArray::RealizerID(int32_t modelIndex)
{
    Assert(modelIndex >= 0 && modelIndex < numModels);
    return model[modelIndex].modelrealizer;
}

// Getting a model by index
MODEL *ModelArray::operator[](int32_t modelIndex)
{
    Assert(modelIndex >= 0 && modelIndex < numModels);
    return static_cast<MODEL *>(core.GetEntityPointer(model[modelIndex].id));
}

// Getting animation by index
Animation *ModelArray::GetAnimation(int32_t modelIndex)
{
    Assert(modelIndex >= 0 && modelIndex < numModels);
    auto *m = static_cast<MODEL *>(core.GetEntityPointer(model[modelIndex].id));
    if (!m)
        return nullptr;
    return m->GetAnimation();
}

// Set slide uv animation to the model
void ModelArray::SetUVSlide(int32_t modelIndex, float u0, float v0, float u1, float v1)
{
    Assert(modelIndex >= 0 && modelIndex < numModels);
    if (!model[modelIndex].slider)
        model[modelIndex].slider = new UVSlider;
    auto *const sl = model[modelIndex].slider;
    sl->u0 = sl->v0 = 0.0f;
    sl->u1 = sl->v1 = 0.0f;
    sl->us0 = u0;
    sl->vs0 = v0;
    sl->us1 = u1;
    sl->vs1 = v1;
    auto *mdl = (*this)[modelIndex];
    if (mdl)
        mdl->SetRenderTuner(sl);
    else
        core.Trace("Location: Can't get model pointer for set RenderTuner");
}

// Set a rotation animation to the model
void ModelArray::SetRotation(int32_t modelIndex, float rx, float ry, float rz)
{
    Assert(modelIndex >= 0 && modelIndex < numModels);
    if (!model[modelIndex].rotator)
        model[modelIndex].rotator = new Rotator;
    model[modelIndex].rotator->rx = rx;
    model[modelIndex].rotator->ry = ry;
    model[modelIndex].rotator->rz = rz;
}

// Set the reflection matrix generation mode to the model
void ModelArray::SetReflection(int32_t modelIndex, float scale)
{
    Assert(modelIndex >= 0 && modelIndex < numModels);
    if (!model[modelIndex].reflection)
        model[modelIndex].reflection = new Relection();
    if (scale < 0.0f)
        scale = 0.0f;
    if (scale > 1.0f)
        scale = 1.0f;
    const auto alpha = static_cast<uint32_t>(scale * 255.0f);
    model[modelIndex].reflection->tfactor = (alpha << 24) | 0x00ffffff;
    auto *mdl = (*this)[modelIndex];
    if (mdl)
        mdl->SetRenderTuner(model[modelIndex].reflection);
    else
        core.Trace("Location: Can't get model pointer for set RenderTuner");
}

// Animate texture coordinates
void ModelArray::Update(float dltTime)
{
    for (int32_t i = 0; i < numModels; i++)
    {
        if (model[i].slider)
        {
            auto *sl = model[i].slider;
            sl->u0 += dltTime * sl->us0;
            sl->v0 += dltTime * sl->vs0;
            sl->u1 += dltTime * sl->us1;
            sl->v1 += dltTime * sl->vs1;
            if (sl->u0 > 1.0f)
                sl->u0 -= 1.0f;
            if (sl->v0 > 1.0f)
                sl->v0 -= 1.0f;
            if (sl->u1 > 1.0f)
                sl->u1 -= 1.0f;
            if (sl->v1 > 1.0f)
                sl->v1 -= 1.0f;
            if (sl->u0 < -1.0f)
                sl->u0 += 1.0f;
            if (sl->v0 < -1.0f)
                sl->v0 += 1.0f;
            if (sl->u1 < -1.0f)
                sl->u1 += 1.0f;
            if (sl->v1 < -1.0f)
                sl->v1 += 1.0f;
        }
        if (model[i].rotator)
        {
            CMatrix mtr(model[i].rotator->rx * dltTime, model[i].rotator->ry * dltTime, model[i].rotator->rz * dltTime);
            auto *mdl = static_cast<MODEL *>(core.GetEntityPointer(model[i].id));
            if (mdl)
                mdl->mtx = CMatrix(mtr, mdl->mtx);
        }
    }
}

void ModelArray::UpdateModelsPath()
{
    UpdatePath(modelspath);
    modelspath += "\\";
}

void ModelArray::UpdateTexturesPath()
{
    UpdatePath(texturespath);
    texturespath += "\\";
}

void ModelArray::UpdateLightPath()
{
    UpdatePath(lightpath);
}

void ModelArray::UpdateShadowPath()
{
    UpdatePath(shadowpath);
};

uint32_t ModelArray::CalcHashString(const char *str)
{
    uint32_t hval = 0;
    while (*str != '\0')
    {
        auto c = *str++;
        if (c >= 'A' && c <= 'Z')
            c += 'a' - 'A';
        hval = (hval << 4) + static_cast<uint32_t>(c);
        const auto g = hval & (static_cast<uint32_t>(0xf) << (32 - 4));
        if (g != 0)
        {
            hval ^= g >> (32 - 8);
            hval ^= g;
        }
    }
    return hval;
}

void ModelArray::UVSlider::Set(MODEL *model, VDX9RENDER *rs)
{
    static CMatrix mtx;
    mtx.m[2][0] = u0;
    mtx.m[2][1] = v0;
    rs->SetTransform(D3DTS_TEXTURE0, mtx);
    mtx.m[2][0] = u1;
    mtx.m[2][1] = v1;
    rs->SetTransform(D3DTS_TEXTURE1, mtx);
    rs->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
    rs->SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
}

void ModelArray::UVSlider::Restore(MODEL *model, VDX9RENDER *rs)
{
    static CMatrix mtx;
    rs->SetTransform(D3DTS_TEXTURE0, mtx);
    rs->SetTransform(D3DTS_TEXTURE1, mtx);
    rs->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
    rs->SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
}

void ModelArray::Relection::Set(MODEL *model, VDX9RENDER *rs)
{
    CMatrix mtx;
    rs->GetTransform(D3DTS_VIEW, mtx);
    mtx.Transposition();
    mtx.Pos() = 0.0f;
    rs->SetTransform(D3DTS_TEXTURE1, mtx);
    rs->SetRenderState(D3DRS_TEXTUREFACTOR, tfactor);
}

void ModelArray::Relection::Restore(MODEL *model, VDX9RENDER *rs)
{
    const CMatrix mtx;
    rs->SetTransform(D3DTS_TEXTURE1, mtx);
}

void ModelArray::UpdatePath(std::string &path)
{
    int32_t j = 0;
    for (int32_t i = 0; path[i]; i++)
    {
        if (path[i] == '\\')
        {
            if (j > 0)
            {
                if (path[j - 1] != '\\')
                    path[i] = path[j++];
            }
        }
        else
            path[i] = path[j++];
    }
    if (--j >= 0 && path[j] == '\\')
        path.erase(j);
}

// Check the visibility of two points
bool ModelArray::VisibleTest(const CVECTOR &p1, const CVECTOR &p2)
{
    for (int32_t i = 0; i < numModels; i++)
    {
        if (model[i].isVisible)
        {
            auto *mdl = static_cast<MODEL *>(core.GetEntityPointer(model[i].id));
            if (mdl->Trace(p1, p2) < 1.0f)
                return false;
        }
    }
    return true;
}

// Trace the ray through the location
float ModelArray::Trace(const CVECTOR &src, const CVECTOR &dst)
{
    isHavecTrg = false;
    auto k = 2.0f;
    for (int32_t i = 0; i < numModels; i++)
    {
        if (model[i].isVisible)
        {
            auto *mdl = static_cast<MODEL *>(core.GetEntityPointer(model[i].id));
            const auto km = mdl->Trace(src, dst);
            if (k > km)
            {
                k = km;
                isHavecTrg = mdl->GetCollideTriangle(ctrg);
            }
        }
    }
    return k;
}

bool ModelArray::GetCollideTriangle(TRIANGLE &trg) const
{
    trg = ctrg;
    return isHavecTrg;
}

void ModelArray::Clip(PLANE *p, int32_t numPlanes, CVECTOR &cnt, float rad, bool (*fnc)(const CVECTOR *vtx, int32_t num))
{
    for (int32_t i = 0; i < numModels; i++)
    {
        if (model[i].isVisible)
        {
            auto *mdl = static_cast<MODEL *>(core.GetEntityPointer(model[i].id));
            mdl->Clip(p, numPlanes, cnt, rad, fnc);
        }
    }
}
