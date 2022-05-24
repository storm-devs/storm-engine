#include "entity_manager.h"

#include <algorithm>
#include <cassert>

#include "v_module_api.h"

void EntityManager::Erase(EntityInternalData &data)
{
    // TODO: shift loop
    for (layer_index_t i = 0; i < max_layers_num; ++i)
    {
        RemoveFromLayer(i, data);
    }

    delete data.ptr;
}

void EntityManager::EraseEntityNoCheck(const entid_t id)
{
    const auto index = static_cast<entid_index_t>(id);
    auto &entData = entities_.first[index];
    entData.deleted = true;

    auto &deletedArr = deletedIndices_.first;
    auto &deletedSize = deletedIndices_.second;

    deletedArr[deletedSize++] = index;
}

hash_t EntityManager::GetClassCode(const entid_t id) const
{
    const auto idx = GetEntityDataIdx(id);
    if (!EntIdxValid(idx))
    {
        return {};
    }

    return entities_.first[idx].hash;
}

void EntityManager::AddToLayer(const layer_index_t index, const entid_t id, const priority_t priority)
{
    assert(index < max_layers_num);
    const auto idx = GetEntityDataIdx(id);
    if (!EntIdxValid(idx))
    {
        return;
    }

    auto& data = entities_.first[idx];

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

    const auto size = std::ssize(priorities);
    assert(size < max_ent_in_layer);

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
        RemoveFromLayer(index, entities_.first[idx]);
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
    assert(size < max_ent_in_layer);

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
    /* FIND VMA */
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

    /* CREATE Entity */
    auto *ptr = static_cast<Entity *>(pClass->CreateClass());
    if (ptr == nullptr)
    {
        throw std::runtime_error("CreateClass returned nullptr");
    }

    /* INIT Entity */
    // set id first
    const auto id = PushEntity(ptr, hash);
    ptr->data_.id = id;
    ptr->AttributesPointer = attr;

    // then init
    if (!ptr->Init())
    {
        // delete if fail
        EraseEntityNoCheck(id);
        return invalid_entity;
    }

    // return entid_t if success
    return id;
}

void EntityManager::EraseEntity(const entid_t id)
{
    const auto idx = GetEntityDataIdx(id);
    if (!EntIdxValid(idx))
    {
        return;
    }

    const auto &data = entities_.first[idx];

    // data ptr deleted
    if (data.ptr == nullptr)
    {
        return;
    }

    // entity id mismatch
    if (data.ptr->GetId() != id)
    {
        return;
    }

    EraseEntityNoCheck(id);
}

void EntityManager::EraseAll()
{
    auto &arr = entities_.first;
    const auto size = entities_.second;
    for (auto it = std::begin(arr); it != std::begin(arr) + size; ++it)
    {
        const auto index = static_cast<entid_index_t>(it->id);
        if (index >= size)
            continue;

        Erase(arr[index]); // release
        arr[index] = {};
    }

    // clear arrays
    entities_.second = 0;
    freeIndices_.second = 0;
}

entptr_t EntityManager::GetEntityPointer(const entid_t id) const
{
    const auto idx = GetEntityDataIdx(id);
    if (!EntIdxValid(idx) || entities_.first[idx].deleted)
    {
        return nullptr;
    }

    return entities_.first[idx].ptr;
}

entity_container_cref EntityManager::GetEntityIds(const layer_type_t type) const
{
    // TODO:: cache
    static std::vector<entid_t> result;
    result.reserve(entities_.first.size()); // TODO: investigate memory consumption
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
    static std::vector<entid_t> result;
    result.reserve(entities_.first.size()); // TODO: investigate memory consumption
    result.clear();

    const auto &arr = entities_.first;
    const auto size = entities_.second;

    for (auto it = std::begin(arr); it != std::begin(arr) + size; ++it)
    {
        if (it->hash == hash && !it->deleted)
        {
            result.push_back(it->id);
        }
    }

    return result;
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
    const auto &arr = entities_.first;
    const auto size = entities_.second;

    for (auto it = std::begin(arr); it != std::begin(arr) + size; ++it)
    {
        if (it->hash == hash && !it->deleted)
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

void EntityManager::PushFreeIndex(entity_index_t index)
{
    auto &stack = freeIndices_.first;
    auto &top = freeIndices_.second;

    assert(top < max_ent_num);

    stack[top++] = index;
}

void EntityManager::NewLifecycle()
{
    auto &arr = entities_.first;
    const auto size = entities_.second;

    auto &deletedArr = deletedIndices_.first;
    volatile auto &deletedSize = deletedIndices_.second;

    // deletedSize will increment while erasing entities. this is safe for std::array iteration
    for (auto it = std::begin(deletedArr); it != std::begin(deletedArr) + deletedSize; ++it)
    {
        const auto index = *it;

        if (index >= size)
            continue;

        if (arr[index].id == 0)
            continue;

        Erase(arr[index]);   // release
        arr[index] = {};      // erase entity data
        PushFreeIndex(index); // push free index to stack instead of shifting
    }
    deletedSize = 0; // clear array of entities to delete
}

size_t EntityManager::GetEntityDataIdx(const entid_t id) const
{
    // index out of range
    const auto index = static_cast<entid_index_t>(id);
    if (index >= entities_.second)
    {
        return invalid_entity_idx_t{};
    }

    // data ptr deleted
    auto &data = entities_.first[index];
    if (data.ptr == nullptr)
    {
        return invalid_entity_idx_t{};
    }

    // entity id mismatch
    const auto entid = data.ptr->GetId();
    if (id != entid)
    {
        return invalid_entity_idx_t{};
    }
    
    return index;
}

entid_t EntityManager::PushEntity(entptr_t ptr, hash_t hash)
{
    auto &arr = entities_.first;
    auto &size = entities_.second;

    assert(size < max_ent_num);

    auto &stack = freeIndices_.first;
    auto &top = freeIndices_.second;

    /* check if free indices available */
    const entid_index_t idx = top != 0 ? stack[--top] : size;

    /* calculate stamp */
    const auto ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());

    /* assemble entity id */
    const auto stamp = static_cast<entid_stamp_t>(ms.count());
    const auto id = static_cast<entid_t>(stamp) << 32 | idx;

    /* push entity data */
    arr[idx] = {{}, {}, {}, ptr, id, hash};
    ++size;

    return id;
}
