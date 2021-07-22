#include "vcollide.h"

CREATE_SERVICE(COLL)

entid_t last_trace_eid;

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
LOCAL_COLLIDE *COLL::CreateLocalCollide(EntityManager::layer_index_t idx)
{
    return new LCOLL(idx);
}

//----------------------------------------------------------------------------------
// Ray tracing
//----------------------------------------------------------------------------------
float COLL::Trace(entid_t entity, const CVECTOR &src, const CVECTOR &dst)
{
    auto *cob = static_cast<COLLISION_OBJECT *>(EntityManager::GetEntityPointer(entity));
    if (static_cast<Entity *>(cob) == nullptr)
        return 2.0f;

    last_trace_eid = entity;
    return cob->Trace(src, dst);
}

//----------------------------------------------------------------------------------
// with enclusion list
//----------------------------------------------------------------------------------
float COLL::Trace(EntityManager::LayerIterators its, const CVECTOR &src, const CVECTOR &dst,
                  const entid_t *exclude_list, long entities)
{
    auto best_res = 2.0f;
    for (auto it = its.first; it != its.second; ++it)
    {
        const auto eid = it->second;

        long e;
        for (e = 0; e < entities; e++)
            if (eid == exclude_list[e])
                break;

        if (e == entities)
        {
            auto *cob = static_cast<COLLISION_OBJECT *>(EntityManager::GetEntityPointer(eid));
            if (cob != nullptr)
            {
                const auto res = cob->Trace(src, dst);
                if (res < best_res)
                {
                    best_res = res;
                    last_trace_eid = eid;
                }
            }
        }
    }

    return best_res;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool COLL::Clip(EntityManager::LayerIterators its, const PLANE *planes, long nplanes, const CVECTOR &center,
                float radius, ADD_POLYGON_FUNC addpoly, const entid_t *exclude_list, long entities)
{
    auto retval = false;

    for (auto it = its.first; it != its.second; ++it)
    {
        const auto eid = it->second;

        long e;
        for (e = 0; e < entities; e++)
            if (eid == exclude_list[e])
                break;

        if (e == entities)
        {
            auto *cob = static_cast<COLLISION_OBJECT *>(EntityManager::GetEntityPointer(eid));
            if (cob != nullptr)
            {
                last_trace_eid = eid;
                if (cob->Clip(planes, nplanes, center, radius, addpoly) == true)
                    retval = true;
            }
        }
    }

    return retval;
}

//----------------------------------------------------------------------------------
// get last trace entity id
//----------------------------------------------------------------------------------
entid_t COLL::GetObjectID()
{
    return last_trace_eid;
}
