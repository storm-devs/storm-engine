#pragma once

#include <array>
#include <chrono>
#include <cstdint>
#include <vector>
#include <type_traits>
#include <limits>

#include "entity.h"

class EntityManager final
{
    using entid_index_t = uint32_t;
    using entid_stamp_t = uint32_t;
    using entity_index_t = uint32_t;
    using invalid_entity_idx_t = std::integral_constant<size_t, std::numeric_limits<size_t>::max()>;

    // we are ok with half-precision
    static_assert(sizeof(std::chrono::milliseconds) ==
                  sizeof(entid_stamp_t) * 2);


    struct EntityInternalData
    {
        using layer_mask_t = uint32_t;

        bool deleted;
        layer_mask_t mask;
        priority_t priorities[max_layers_num];
        entptr_t ptr;
        entid_t id;
        hash_t hash;
    };

  public:
      
    struct Layer
    {
        std::vector<priority_t> priorities;
        std::vector<entid_t> entity_ids;

        layer_type_t type;
        bool frozen;
    };

  private:
    /* array of layers */
    std::array<Layer, max_layers_num> layers_ {};

    /* array of entities and actual size */
    std::pair<std::array<EntityInternalData, max_ent_num>, entity_index_t> entities_;

    /* stack for indices of erased entities: static constexpr array and top counter */
    std::pair<std::array<entity_index_t, max_ent_num>, entity_index_t> freeIndices_;

    /* array for indices to delete
     *
     * Background:
     * Entities can be deleted inside destruction of another entity, therefore we shall either
     * loop until there is no entity to delete (O(N^N) at worst) or use a stack for them.
     */
    std::pair<std::array<entity_index_t, max_ent_num>, entity_index_t> deletedIndices_;

    /* actual erase */
    void Erase(EntityInternalData &data);

    /* mark entity to delete */
    void EraseEntityNoCheck(entid_t entity);

    void RemoveFromLayer(layer_index_t index, EntityInternalData &data);

    size_t GetEntityDataIdx(entid_t id) const;

    entid_t GetEntityId(uint32_t hash) const;

    static bool EntIdxValid(size_t idx);

public:

    hash_t GetClassCode(entid_t id) const;
    
    void AddToLayer(layer_index_t index, entid_t id, priority_t priority);

    void RemoveFromLayer(layer_index_t index, entid_t id);

    entid_t CreateEntity(const char *name, ATTRIBUTES *attr = nullptr);

    void EraseEntity(entid_t entity);

    void EraseAll();

    entptr_t GetEntityPointer(entid_t id) const;

    entity_container_cref GetEntityIds(layer_type_t type) const;

    // TODO: hash...
    entity_container_cref GetEntityIds(const char *name) const;

    entity_container_cref GetEntityIds(uint32_t hash) const;

    entity_container_cref GetEntityIds(layer_index_t index) const;

    entid_t GetEntityId(const char *name) const;

    layer_type_t GetLayerType(layer_index_t index) const;

    void SetLayerType(layer_index_t index, layer_type_t type);

    void SetLayerFrozen(layer_index_t index, bool freeze);

    bool IsLayerFrozen(layer_index_t index) const;

    void PushFreeIndex(entity_index_t index);

    /* new lifecycle is started at the end of each frame. it's time to cleanup */
    void NewLifecycle();

    entid_t PushEntity(entptr_t ptr, hash_t hash);
};
