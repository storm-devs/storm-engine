#pragma once
#include "attributes.h" // TODO: REMOVE
#include <cstdint>

/* typedefs */
class Entity;
class MESSAGE;
using entptr_t = Entity *;
using entid_t = uint64_t;

constexpr auto max_ent_in_layer = 1024;
constexpr auto max_layers_num = 32;
constexpr auto max_ent_num = max_ent_in_layer * max_layers_num;
constexpr entid_t invalid_entity = 0;

/* Entity base class */
class Entity
{
    friend class EntityManager;

  public:
    struct EntitySelfData
    {
        // const char* name;
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

    /*[[nodiscard]]
    auto GetName() const
    {
      return data_.name;
    }*/

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
