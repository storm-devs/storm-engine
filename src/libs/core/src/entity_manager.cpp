#include "entity_manager.h"

#include <algorithm>
#include <cassert>
#include <limits>
#include <ranges>
#include <type_traits>

#include "v_module_api.h"

namespace
{

using invalid_entity_idx_t = std::integral_constant<size_t, std::numeric_limits<size_t>::max()>;

}

void EntityManager::EraseAndFree(EntityInternalData &data)
{
    // TODO: shift loop
    for (layer_index_t i = 0; i < max_layers_num; ++i)
    {
        RemoveFromLayer(i, data);
    }

    delete data.ptr;
}

void EntityManager::MarkDeleted(const entid_t id)
{
    const auto index = static_cast<entid_index_t>(id);
    entities_[index].valid = false;
    deletedIndices_.push(index);
}

hash_t EntityManager::GetClassCode(const entid_t id) const
{
    const auto idx = GetEntityDataIdx(id);
    if (!EntIdxValid(idx))
    {
        return {};
    }

    return entities_[idx].hash;
}

void EntityManager::AddToLayer(const layer_index_t index, const entid_t id, const priority_t priority)
{
    assert(index < max_layers_num);
    const auto idx = GetEntityDataIdx(id);
    if (!EntIdxValid(idx))
    {
        return;
    }

    auto &data = entities_[idx];

    // check if entity is already in layer
    if (data.mask & (1 << index))
    {
        return;
    }

    // set info about layer
    data.mask |= 1 << index;
    data.priorities[index] = priority;

    auto &layer = layers_[index];
    auto &priorities = layer.priorities;
    auto &entity_ids = layer.entity_ids;

    // TODO: investigate if duplicate check is needed
    const auto targetIdx =
        std::distance(std::begin(priorities), std::ranges::upper_bound(priorities, priority));

    priorities.insert(std::begin(priorities) + targetIdx, priority);
    entity_ids.insert(std::begin(entity_ids) + targetIdx, id);
}

void EntityManager::RemoveFromLayer(const layer_index_t index, const entid_t id)
{
    if (const auto idx = GetEntityDataIdx(id); EntIdxValid(idx))
    {
        RemoveFromLayer(index, entities_[idx]);
    }
}

void EntityManager::RemoveFromLayer(const layer_index_t index, EntityInternalData &data)
{
    assert(index < max_layers_num);

    auto &mask = data.mask;
    // check if entity is in layer
    if (!(mask & (1 << index)))
    {
        return;
    }

    const auto priority = data.priorities[index];
    const auto id = data.id;

    auto &layer = layers_[index];
    auto &priorities = layer.priorities;
    auto &entity_ids = layer.entity_ids;

    const auto size = std::ssize(priorities);

    const auto lowerIdx = std::distance(std::begin(priorities), std::ranges::lower_bound(priorities, priority));
    assert(lowerIdx < size);

    // look through this priority only
    for (auto i = lowerIdx; i < size && priorities[i] == priority; ++i)
    {
        if (entity_ids[i] == id)
        {
            priorities.erase(std::begin(priorities) + i);
            entity_ids.erase(std::begin(entity_ids) + i);
            break;
        }
    }

    // clear layer flag
    mask &= ~(1 << index);
}

entid_t EntityManager::CreateEntity(const char *name, ATTRIBUTES *attr)
{
    // Find VMA
    const auto hash = MakeHashValue(name);
    if (hash == 0)
    {
        throw std::runtime_error("null hash");
    }
    VMA *pClass = nullptr;
    for (const auto &c : __STORM_CLASSES_REGISTRY)
    {
        if (c->GetHash() == hash && storm::iEquals(name, c->GetName()))
        {
            pClass = c;
            break;
        }
    }
    if (pClass == nullptr)
    {
        throw std::runtime_error("invalid entity name");
    }

    // Create entity
    auto *ptr = static_cast<Entity *>(pClass->CreateClass());
    if (ptr == nullptr)
    {
        throw std::runtime_error("CreateClass returned nullptr");
    }

    // Init entity
    // set id first
    const auto id = InsertEntity(ptr, hash);
    ptr->data_.id = id;
    ptr->AttributesPointer = attr;

    // add to cache
    cache_.UpdateAdd(hash, id);

    // then init
    if (!ptr->Init())
    {
        // remove from cache
        cache_.UpdateErase(hash, id);

        // delete if fail
        MarkDeleted(id);
        return invalid_entity;
    }
    
    return id;
}

void EntityManager::EraseEntity(const entid_t id)
{
    const auto idx = GetEntityDataIdx(id);
    if (!EntIdxValid(idx))
    {
        return;
    }

    const auto &data = entities_[idx];

    // check if already erased
    if (data.valid == false)
    {
        return;
    }

    // check id mismatch
    if (data.ptr->GetId() != id)
    {
        return;
    }

    // remove from cache
    cache_.UpdateErase(data.hash, id);

    MarkDeleted(id);
}

void EntityManager::EraseAll()
{
    for (auto it = std::begin(entities_); it != std::end(entities_); ++it)
    {
        const auto index = static_cast<entid_index_t>(it->id);
        if (index >= entities_.size())
        {
            __debugbreak();
            // TODO:???
        }

        // release
        EraseAndFree(entities_[index]);
        entities_[index] = {};
    }

    // clear containers
    entities_.clear();
    freeIndices_.clear();
    deletedIndices_.clear();

    // clear cache
    cache_.Clear();
}

entptr_t EntityManager::GetEntityPointer(const entid_t id) const
{
    const auto idx = GetEntityDataIdx(id);
    if (!EntIdxValid(idx) || !entities_[idx].valid)
    {
        return nullptr;
    }

    return entities_[idx].ptr;
}

entity_container_cref EntityManager::GetEntityIds(const layer_type_t type) const
{
    // TODO:: cache or iterator?
    static std::vector<entid_t> result;
    result.reserve(entities_.size()); // TODO: investigate memory consumption
    result.clear();

    for (auto it = std::begin(layers_); it != std::end(layers_); ++it)
    {
        if (it->frozen)
            continue;

        if (it->type == type)
        {
            for (auto ent_it = std::begin(it->entity_ids); ent_it != std::end(it->entity_ids); ++ent_it)
                result.push_back(*ent_it);
        }
    }

    return result;
}

entity_container_cref EntityManager::GetEntityIds(const char *name) const
{
    return GetEntityIds(MakeHashValue(name));
}

entity_container_cref EntityManager::GetEntityIds(const uint32_t hash) const
{
    static const std::vector<entid_t> null;

    // if contains, it is already valid
    // otherwise let's fill the cache in
    bool empty = !cache_.Contains(hash);
    if (empty)
    {
        for (auto it = std::begin(entities_); it != std::end(entities_); ++it)
        {
            if (it->hash == hash && it->valid)
            {
                cache_.Add(hash, it->id);
                empty = false;
            }
        }
    }

    return empty ? null : cache_.Get(hash);
}

entity_container_cref EntityManager::GetEntityIds(const layer_index_t index) const
{
    assert(index < max_layers_num);

    return layers_[index].entity_ids;
}

entid_t EntityManager::GetEntityId(const char *name) const
{
    return GetEntityId(MakeHashValue(name));
}

entid_t EntityManager::GetEntityId(const uint32_t hash) const
{
    for (auto it = std::begin(entities_); it != std::end(entities_); ++it)
    {
        if (it->hash == hash && it->valid)
        {
            return it->id;
        }
    }

    return invalid_entity;
}

bool EntityManager::EntIdxValid(const size_t idx)
{
    return idx != invalid_entity_idx_t{};
}

entid_t EntityManager::calculate_entity_id(const size_t idx)
{
    // check if idx exceeds 2^32
    assert(static_cast<size_t>(static_cast<uint32_t>(idx)) == idx);

    // calculate stamp
    const auto ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());

    // assemble entity id
    const auto stamp = static_cast<entid_stamp_t>(ms.count());
    const auto id = static_cast<entid_t>(stamp) << 32 | idx;

    return id;
}

layer_type_t EntityManager::GetLayerType(const layer_index_t index) const
{
    assert(index < max_layers_num);

    return layers_[index].type;
}

void EntityManager::SetLayerType(const layer_index_t index, const layer_type_t type)
{
    assert(index < max_layers_num);

    layers_[index].type = type;
}

void EntityManager::SetLayerFrozen(const layer_index_t index, const bool freeze)
{
    assert(index < max_layers_num);

    layers_[index].frozen = freeze;
}

bool EntityManager::IsLayerFrozen(const layer_index_t index) const
{
    assert(index < max_layers_num);

    return layers_[index].frozen;
}

void EntityManager::NewLifecycle()
{
    // deletedSize will increment while erasing entities. this is safe for std::array iteration
    while (!deletedIndices_.empty())
    {
        const auto index = deletedIndices_.top();
        assert(index < entities_.size());
        deletedIndices_.pop();

        // release
        EraseAndFree(entities_[index]);
        // erase entity data
        entities_[index] = {};
        // push free index to stack instead of shifting
        freeIndices_.push(index);
    }
}

size_t EntityManager::GetEntityDataIdx(const entid_t id) const
{
    const auto index = static_cast<entid_index_t>(id);

    // check bounds
    if (index >= std::size(entities_))
    {
        return invalid_entity_idx_t{};
    }

    auto &data = entities_[index];

    // check validity
    if(data.valid == false)
    {
        return invalid_entity_idx_t{};
    }

    // check if already replaced by another entity
    if (data.ptr->GetId() != id)
    {
        return invalid_entity_idx_t{};
    }
        

    return index;
}

entid_t EntityManager::InsertEntity(entptr_t ptr, hash_t hash)
{
    entid_t id;
    if (freeIndices_.empty())
    {
        const entid_index_t index = static_cast<entid_index_t>(std::size(entities_));
        id = calculate_entity_id(index);
        entities_.push_back(EntityInternalData{true, {}, {}, ptr, id, hash});
    }
    else
    {
        const entid_index_t index = freeIndices_.top();
        id = calculate_entity_id(index);
        freeIndices_.pop();
        entities_[index] = {true, {}, {}, ptr, id, hash};
    }

    return id;
}

void EntityManager::ForEachEntity(const std::function<void(entptr_t)> &f)
{
    std::ranges::for_each(entities_, [&](const EntityInternalData& data) {
        if (const auto entity_ptr = GetEntityPointer(data.id))
            {
                f(entity_ptr);
            }
        });
}
