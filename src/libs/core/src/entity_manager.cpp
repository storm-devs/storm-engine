#include "entity_manager.h"

#include <algorithm>
#include <cassert>

#include "v_module_api.h"

std::array<EntityManager::Layer, max_layers_num> EntityManager::layers_;

/* static array of entities and actual size */
std::pair<std::array<EntityManager::EntityInternalData, max_ent_num>, EntityManager::entity_index_t>
    EntityManager::entities_;

/* stack for indices of erased entities: static constexpr array and top counter */
std::pair<std::array<EntityManager::entity_index_t, max_ent_num>, EntityManager::entity_index_t>
    EntityManager::freeIndices_;

/* static array for indices to delete */
/* RATIONALE:
 * Entities can be deleted inside destruction of another entity, therefore we shall either
 * loop until there is no entity to delete (O(N^N) at worst) or use a stack for them.
 */
std::pair<std::array<EntityManager::entity_index_t, max_ent_num>, EntityManager::entity_index_t>
    EntityManager::deletedIndices_;

void EntityManager::_erase(EntityInternalData &data)
{
    auto &mask = data.mask;

    // remove from layers
    for (unsigned i = 0; i < sizeof(mask) * 8; ++i)
    {
        RemoveFromLayer(i, data);
    }

    delete data.ptr;
}

void EntityManager::_EraseEntityNoCheck(const entid_t entity)
{
    const auto index = static_cast<entid_index_t>(entity);
    auto &entData = entities_.first[index];
    entData.deleted = true;

    auto &deletedArr = deletedIndices_.first;
    auto &deletedSize = deletedIndices_.second;

    deletedArr[deletedSize++] = index;
}

EntityManager::hash_t EntityManager::GetClassCode(const entid_t id)
{
    const auto &entData = GetEntityData(id);
    return entData.hash;
}

void EntityManager::AddToLayer(const layer_index_t index, const entid_t id, const priority_t priority)
{
    assert(index < max_layers_num); // valid index
    // assert(entities_.first[static_cast<entid_index_t>(id)].mask & (1 << index)); // mask shall be set

    auto &entData = GetEntityData(id);
    // check if entity is already in layer
    if (entData.mask & (1 << index))
        return;

    // set info about layer
    entData.mask |= 1 << index;
    entData.priorities[index] = priority;

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
    return RemoveFromLayer(index, GetEntityData(id));
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
        _EraseEntityNoCheck(id);
        return invalid_entity;
    }

    // return entid_t if success
    return id;
}

void EntityManager::EraseEntity(const entid_t entity)
{
    const auto &entData = GetEntityData(entity);
    if (entData.ptr == nullptr)
        return;

    // validate
    if (entData.ptr->GetId() != entity)
        return; /* already replaced by another entity */

    _EraseEntityNoCheck(entity);
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

        _erase(arr[index]); // release
        arr[index] = {};
    }

    // clear arrays
    entities_.second = 0;
    freeIndices_.second = 0;
}

entptr_t EntityManager::GetEntityPointer(const entid_t entity)
{
    const auto &entData = GetEntityData(entity);
    return entData.deleted ? nullptr : entData.ptr;
}

EntityManager::EntityVector EntityManager::GetEntityIdVector(const Layer::Type type)
{
    std::vector<entid_t> result;
    // should be vector of layer iterators actually... special iterator would be cool here too.
    result.reserve(entities_.first.size()); // TODO: investigate memory consumption

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

EntityManager::EntityVector EntityManager::GetEntityIdVector(const char *name)
{
    return GetEntityIdVector(MakeHashValue(name));
}

EntityManager::EntityVector EntityManager::GetEntityIdVector(const uint32_t hash)
{
    std::vector<entid_t> result;
    result.reserve(max_ent_num); // TODO: investigate memory consumption

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

EntityManager::LayerIterators EntityManager::GetEntityIdIterators(const layer_index_t index)
{
    assert(index < max_layers_num);

    auto &layer = layers_[index];

    return std::pair{std::begin(layer.entity_ids), std::end(layer.entity_ids)};
}

EntityManager::AllEntIterators EntityManager::GetEntityIdIterators()
{
    return std::pair{std::begin(entities_.first), std::begin(entities_.first) + entities_.second};
}

entid_t EntityManager::GetEntityId(const char *name)
{
    return GetEntityId(MakeHashValue(name));
}

entid_t EntityManager::GetEntityId(const uint32_t hash)
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

EntityManager::Layer::Type EntityManager::GetLayerType(const layer_index_t index)
{
    assert(index < max_layers_num);

    return layers_[index].type;
}

void EntityManager::SetLayerType(const layer_index_t index, const Layer::Type type)
{
    assert(index < max_layers_num);

    layers_[index].type = type;
}

void EntityManager::SetLayerFrozen(const layer_index_t index, const bool freeze)
{
    assert(index < max_layers_num);

    layers_[index].frozen = freeze;
}

bool EntityManager::IsLayerFrozen(const layer_index_t index)
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

        _erase(arr[index]);   // release
        arr[index] = {};      // erase entity data
        PushFreeIndex(index); // push free index to stack instead of shifting
    }
    deletedSize = 0; // clear array of entities to delete
}

EntityManager::EntityInternalData & EntityManager::GetEntityData(const entid_t entity)
{
    static EntityInternalData null{.deleted = true};

    const auto index = static_cast<entid_index_t>(entity);

    if (index >= entities_.second) /* calm down it's ok... */
        return null;

    auto &data = entities_.first[index];

    if (data.ptr == nullptr)
        return null; /* THIS IS ALSO FUCKING OK */

    /* check if valid */
    const auto entid = data.ptr->GetId();
    if (entity != entid)
    {
        return null;
    }

    return data;
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
