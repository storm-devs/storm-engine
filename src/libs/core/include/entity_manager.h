#pragma once

#include <array>
#include <chrono>
#include <cstdint>
#include <vector>

#include "entity.h"
#include "shared/layers.h"

class EntityManager final
{
    using entid_index_t = uint32_t;
    using entid_stamp_t = uint32_t;
    using entity_index_t = uint32_t;
    using priority_t = uint32_t;
    using hash_t = uint32_t;

  public:

    using layer_index_t = uint8_t;

    // TODO: this is a workaround. i need to implement specific iterator returning only id
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
      
    struct Layer
    {
        enum Type : uint8_t
        {
            common,
            execute,
            realize
        };

        std::vector<priority_t> priorities;
        std::vector<entid_t> entity_ids;

        Type type;
        bool frozen;
    };

  private:
    /* static array of layers */
    static std::array<Layer, max_layers_num> layers_;

    /* static array of entities and actual size */
    static std::pair<std::array<EntityInternalData, max_ent_num>, entity_index_t> entities_;

    /* stack for indices of erased entities: static constexpr array and top counter */
    static std::pair<std::array<entity_index_t, max_ent_num>, entity_index_t> freeIndices_;

    /* static array for indices to delete */
    /* RATIONALE:
     * Entities can be deleted inside destruction of another entity, therefore we shall either
     * loop until there is no entity to delete (O(N^N) at worst) or use a stack for them.
     */
    static std::pair<std::array<entity_index_t, max_ent_num>, entity_index_t> deletedIndices_;

    /* actual erase */
    static void _erase(EntityInternalData &data);

    /* mark entity to delete */
    static void _EraseEntityNoCheck(entid_t entity);

public:
    using EntityVector = const std::vector<entid_t>;
    using LayerIterators = std::pair<decltype(Layer::entity_ids)::const_iterator, decltype(Layer::entity_ids)::const_iterator>;
    using AllEntIterators =
        std::pair<decltype(entities_.first)::const_iterator, decltype(entities_.first)::const_iterator>;

    /* fully static constexpr class */
    EntityManager() = delete;

    static hash_t GetClassCode(entid_t id);

    /* layer functioning is fully static constexpr */
    static void AddToLayer(layer_index_t index, entid_t id, priority_t priority);

    static void RemoveFromLayer(layer_index_t index, entid_t id);

    static void RemoveFromLayer(layer_index_t index, EntityInternalData &data);

    static entid_t CreateEntity(const char *name, ATTRIBUTES *attr = nullptr);

    static void EraseEntity(entid_t entity);

    static void EraseAll();

    static entptr_t GetEntityPointer(entid_t entity);

    static EntityVector GetEntityIdVector(Layer::Type type);

    // TODO: hash...
    static EntityVector GetEntityIdVector(const char *name);

    static EntityVector GetEntityIdVector(uint32_t hash);

    static LayerIterators GetEntityIdIterators(layer_index_t index);

    /* this is (temporary) workaround solution */
    static AllEntIterators GetEntityIdIterators();

    static entid_t GetEntityId(const char *name);

    static entid_t GetEntityId(uint32_t hash);

    static Layer::Type GetLayerType(layer_index_t index);

    static void SetLayerType(layer_index_t index, Layer::Type type);

    static void SetLayerFrozen(layer_index_t index, bool freeze);

    static bool IsLayerFrozen(layer_index_t index);

    static void PushFreeIndex(entity_index_t index);

    /* new lifecycle is started at the end of each frame. it's time to cleanup */
    static void NewLifecycle();

    static EntityInternalData &GetEntityData(entid_t entity);

    static entid_t PushEntity(entptr_t ptr, hash_t hash);

    /* static constexpr asserts */
    static_assert(sizeof(std::chrono::milliseconds) == sizeof(entid_stamp_t) * 2); /* we are ok with half-precision */
};
