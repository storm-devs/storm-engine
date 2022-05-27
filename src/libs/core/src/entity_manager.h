#pragma once

#include <array>
#include <chrono>
#include <cstdint>
#include <vector>

#include <plf_stack.h>

#include "entity.h"
#include "entity_container_cache.h"

class EntityManager final
{
    constexpr static size_t max_layers_num = 32; // cannot exceed 32

    using entid_index_t = uint32_t;
    using entid_stamp_t = uint32_t;
    using entity_index_t = uint32_t;

    struct EntityInternalData
    {
        using layer_mask_t = uint32_t;

        bool valid;
        layer_mask_t mask;
        priority_t priorities[max_layers_num];
        entptr_t ptr;
        entid_t id;
        hash_t hash;
    };

    struct Layer
    {
        std::vector<priority_t> priorities;
        std::vector<entid_t> entity_ids;

        layer_type_t type;
        bool frozen;
    };

  public:
    hash_t GetClassCode(entid_t id) const;
    entptr_t GetEntityPointer(entid_t id) const;
    entity_container_cref GetEntityIds(layer_type_t type) const;
    entity_container_cref GetEntityIds(const char *name) const;
    entity_container_cref GetEntityIds(uint32_t hash) const;
    entity_container_cref GetEntityIds(layer_index_t index) const;
    entid_t GetEntityId(const char *name) const;
    layer_type_t GetLayerType(layer_index_t index) const;
    bool IsLayerFrozen(layer_index_t index) const;

    void AddToLayer(layer_index_t index, entid_t id, priority_t priority);
    void RemoveFromLayer(layer_index_t index, entid_t id);
    entid_t CreateEntity(const char *name, ATTRIBUTES *attr = nullptr);
    void EraseEntity(entid_t id);
    void EraseAll();
    void SetLayerType(layer_index_t index, layer_type_t type);
    void SetLayerFrozen(layer_index_t index, bool freeze);
    void NewLifecycle();
    void ForEachEntity(const std::function<void(entptr_t)> &f);

  private:
    static bool EntIdxValid(size_t idx);
    static entid_t calculate_entity_id(size_t idx);

    size_t GetEntityDataIdx(entid_t id) const;
    entid_t GetEntityId(uint32_t hash) const;

    void EraseAndFree(EntityInternalData &data);
    void MarkDeleted(entid_t id);
    void RemoveFromLayer(layer_index_t index, EntityInternalData &data);
    entid_t InsertEntity(entptr_t ptr, hash_t hash);

    mutable EntityContainerCache cache_;

    // array of layers
    std::array<Layer, max_layers_num> layers_{};

    // array of all entities
    std::vector<EntityInternalData> entities_;

    // stack of erased entities
    plf::stack<entity_index_t> freeIndices_;

    // stack of entities for deferred delete
    plf::stack<entity_index_t> deletedIndices_;
};
