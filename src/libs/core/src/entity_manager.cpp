#include "entity_manager.h"

#include <algorithm>
#include <cassert>
#include <limits>
#include <ranges>
#include <type_traits>

#include "string_compare.hpp"
#include "vma.hpp"

#include <chrono>
#include <functional>

namespace
{

using invalid_entity_idx_t = std::integral_constant<size_t, std::numeric_limits<size_t>::max()>;

}

void EntityManager::EraseAndFree(EntityInternalData &data)
{
    for (layer_index_t i = 0; i < kMaxLayerNum; ++i)
    {
        if (data.mask >> i & 1)
        {
            RemoveFromLayer(i, data);
        }
    }
    delete data.ptr;
}

void EntityManager::MarkDeleted(EntityInternalData &data)
{
    data.state = kExists;
    const auto index = static_cast<entid_index_t>(data.id);
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

void EntityManager::AddToLayer(const layer_index_t index, EntityInternalData &data, const priority_t priority)
{
    assert(index < kMaxLayerNum);

    // set info about layer
    data.mask |= 1 << index;
    data.priorities[index] = priority;

    auto &layer = layers_[index];
    auto &priorities = layer.priorities;
    auto &entity_ids = layer.entity_ids;

    // duplicate assert
    assert(std::ranges::find(entity_ids, data.id) == std::end(entity_ids));

    const auto targetIdx = std::distance(std::begin(priorities), std::ranges::upper_bound(priorities, priority));

    priorities.insert(std::begin(priorities) + targetIdx, priority);
    entity_ids.insert(std::begin(entity_ids) + targetIdx, data.id);
}

void EntityManager::RemoveFromLayer(const layer_index_t index, EntityInternalData &data)
{
    assert(index < kMaxLayerNum);

    auto &mask = data.mask;

    const auto priority = data.priorities[index];
    const auto id = data.id;

    auto &layer = layers_[index];
    auto &priorities = layer.priorities;
    auto &entity_ids = layer.entity_ids;

    const auto ssize = std::ssize(priorities);

    const auto lowerIdx = std::distance(std::begin(priorities), std::ranges::lower_bound(priorities, priority));
    assert(lowerIdx < ssize);

    // look through this priority only
    for (auto i = lowerIdx; i < ssize && priorities[i] == priority; ++i)
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
        const auto index = static_cast<entid_index_t>(id);
        MarkDeleted(entities_[index]);
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

    auto &data = entities_[idx];

    // check if already erased
    if (data.state != kValid)
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
    MarkDeleted(data);
}

void EntityManager::EraseAll()
{
    for (auto it = std::begin(entities_); it != std::end(entities_); ++it)
    {
        it->state = kNotExists;
        delete it->ptr;
    }

    // clear containers
    for (auto &layer : layers_)
    {
        layer.entity_ids.clear();
        layer.priorities.clear();
    }
    entities_.clear();
    freeIndices_.clear();
    deletedIndices_.clear();

    // clear cache
    cache_.Clear();

    previousStamp_ = 0;
}

entptr_t EntityManager::GetEntityPointer(const entid_t id) const
{
    const auto idx = GetEntityDataIdx(id);
    if (!EntIdxValid(idx))
    {
        return nullptr;
    }

    return entities_[idx].ptr;
}

entity_container_cref EntityManager::GetEntityIds(const layer_type_t type) const
{
    // TODO: introduce cache or invalidation-proof iterator?
    static std::vector<entid_t> result;
    result.reserve(entities_.size());
    result.clear();

    for (auto it = std::begin(layers_); it != std::end(layers_); ++it)
    {
        if (it->frozen)
        {
            continue;
        }

        if (it->type == type)
        {
            std::ranges::copy(it->entity_ids, std::back_inserter(result));
        }
    }

    // TODO: Investigate:
    // Some entities are referenced in multiple layers of same type
    // therefore there are getting processed multiple times.
    // This negatively affects any logic that make use of delta time.
    // Removing duplicates would resolve such issues, but
    // 1) it will increase the complexity drastically
    // 2) it's not obvious what duplicate to remove as they might
    // have different priorities inside corresponding layers.
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
            if (it->hash == hash && it->state == kValid)
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
    assert(index < kMaxLayerNum);

    return layers_[index].entity_ids;
}

entid_t EntityManager::GetEntityId(const char *name) const
{
    return GetEntityId(MakeHashValue(name));
}

bool EntityManager::IsEntityValid(entid_t id) const
{
    const auto index = GetEntityDataIdx(id);
    return EntIdxValid(index) && entities_[index].state == kValid;
}

entid_t EntityManager::GetEntityId(const uint32_t hash) const
{
    for (auto it = std::begin(entities_); it != std::end(entities_); ++it)
    {
        if (it->hash == hash && it->state == kValid)
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

entid_t EntityManager::CalculateEntityId(const size_t idx)
{
    // assert for idx exceeding 2^32
    assert(static_cast<size_t>(static_cast<uint32_t>(idx)) == idx);

    const auto stamp = ++previousStamp_;
    const auto id = static_cast<entid_t>(stamp) << 32 | idx;

    return id;
}

layer_type_t EntityManager::GetLayerType(const layer_index_t index) const
{
    assert(index < kMaxLayerNum);

    return layers_[index].type;
}

void EntityManager::SetLayerType(const layer_index_t index, const layer_type_t type)
{
    assert(index < kMaxLayerNum);

    layers_[index].type = type;
}

void EntityManager::SetLayerFrozen(const layer_index_t index, const bool freeze)
{
    assert(index < kMaxLayerNum);

    layers_[index].frozen = freeze;
}

bool EntityManager::IsLayerFrozen(const layer_index_t index) const
{
    assert(index < kMaxLayerNum);

    return layers_[index].frozen;
}

void EntityManager::AddToLayer(layer_index_t layer_idx, entid_t entity_id, priority_t priority)
{

    if (const auto entity_idx = GetEntityDataIdx(entity_id); EntIdxValid(entity_idx))
    {
        if (!(entities_[entity_idx].mask >> layer_idx & 1))
        {
            AddToLayer(layer_idx, entities_[entity_idx], priority);
        }
    }
}

void EntityManager::RemoveFromLayer(layer_index_t layer_idx, entid_t entity_id)
{

    if (const auto entity_idx = GetEntityDataIdx(entity_id); EntIdxValid(entity_idx))
    {
        if (entities_[entity_idx].mask >> layer_idx & 1)
        {
            RemoveFromLayer(layer_idx, entities_[entity_idx]);
        }
    }
}

void EntityManager::NewLifecycle()
{
    // deletedSize will increment while erasing entities. this is safe for std::array iteration
    while (!deletedIndices_.empty())
    {
        const auto entity_idx = deletedIndices_.top();
        assert(entity_idx < entities_.size());
        deletedIndices_.pop();

        auto &data = entities_[entity_idx];
        // release
        EraseAndFree(data);
        // erase entity data
        data = {};
        // push free index to stack instead of shifting
        freeIndices_.push(entity_idx);
    }
}

size_t EntityManager::GetEntityDataIdx(const entid_t id) const
{
    if (id == invalid_entity)
    {
        return invalid_entity_idx_t{};
    }

    // check bounds
    const auto index = static_cast<entid_index_t>(id);
    if (index >= std::size(entities_))
    {
        return invalid_entity_idx_t{};
    }

    // check validity
    auto &data = entities_[index];
    // This check was intentionally removed to workaround some rare corner cases that are not handled properly.
    //
    // Here's an example of such a corner case that leads to crash due to missing sanity checks:
    // Location entity is being destroyed (marked as invalid) from scripts. Therefore in the next frame its
    // destructor is called, invoking Supervisor destructor which in turn destroys (marks as invalid) all the
    // Player entities.
    // Here's how it looks like:
    // + - exists and operates; ! - invalidated (marked as invalid); x - deleted
    //               |  Frame 0 | Frame 1 | Frame 2 | Frame 3 |
    // Location      |     +    |    !    |    x    |    x    |
    // Supervisor    |     +    |    +    |    x    |    x    |
    // Players[s]    |     +    |    +    |    !    |    x    |
    //
    // As it's shown, while Player[s] exists Location is always marked as invalid and doesn't return its pointer.
    // At this point using an existing pointer to Location without check for validity would work (despite the
    // inadmissibility of such); but calling GetEntityPointer legally returns nullptr due to `valid` flag unset.
    // This case is exacerbated by the fact that in this corner case calls to `Player` are done via PostEvent.
    //
    // Although in this case it would be more consistent with introducing sanity checks over the code,
    // such issues are very difficult to catch. And since this workaround doesn't introduce any harm except for
    // encouraging inconsistency code, it makes sense to left it as a lifebuoy for the existing issues.
    // if (data.valid == false)
    // {
    //     return invalid_entity_idx_t{};
    // }
    if (data.state == kNotExists)
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
        const entid_index_t index = std::size(entities_);
        id = CalculateEntityId(index);
        entities_.emplace_back(EntityInternalData{{}, {}, kValid, ptr, id, hash});
    }
    else
    {
        const entid_index_t index = freeIndices_.top();
        id = CalculateEntityId(index);
        freeIndices_.pop();
        entities_[index] = {{}, {}, kValid, ptr, id, hash};
    }

    return id;
}

void EntityManager::ForEachEntity(const std::function<void(entptr_t)> &f)
{
    std::ranges::for_each(entities_, [&](const EntityInternalData &data) {
        if (data.state == kValid)
        {
            if (const auto entity_ptr = GetEntityPointer(data.id))
            {
                f(entity_ptr);
            }
        }
    });
}
