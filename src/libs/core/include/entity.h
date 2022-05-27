#pragma once

#include <cstdint>

#include "attributes.h" // TODO: REMOVE

/* typedefs */
class Entity;
class MESSAGE;
using entptr_t = Entity *;
using entid_t = uint64_t;
using layer_index_t = uint8_t;
using priority_t = uint32_t;
using hash_t = uint32_t;
using entity_container_cref = const std::vector<entid_t>&;

enum class layer_type_t : uint8_t
{
    common,
    execute,
    realize
};

constexpr entid_t invalid_entity = {};

/* Entity base class */
class Entity
{
    friend class EntityManager;

  public:
    struct EntitySelfData
    {
        entid_t id;
    };

    enum class Stage : uint_fast8_t
    {
        execute,
        realize,
        lost_render,
        restore_render
    };

    ATTRIBUTES *AttributesPointer = nullptr; // TODO: CHANGE!

    [[nodiscard]] auto GetId() const
    {
        return data_.id;
    }

    Entity() = default;
    Entity(const Entity &) = delete;
    Entity(Entity &&) = delete;
    Entity &operator=(const Entity &) = delete;
    virtual ~Entity() = default;

    virtual bool Init() = 0;
    virtual void ProcessStage(Stage stage, uint32_t delta = 0) = 0;

    virtual uint64_t ProcessMessage(MESSAGE &msg)
    {
        return {};
    }

    virtual uint32_t AttributeChanged(ATTRIBUTES *)
    {
        return {};
    }

  private:
    EntitySelfData data_{};
};
