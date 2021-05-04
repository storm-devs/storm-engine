#pragma once

#include <functional>
#include <variant>

namespace storm
{
// TODO: Make own type for keycodes and use it everywhere (reuse SDL scancodes?)
//! Keyboard key, using virtual key codes (VK_* frim WinUser.h) for now
using KeyboardKey = unsigned int;

//! Mouse relative position
// FIXME: Better type for that?
using MousePos = std::pair<int, int>;

//! Mouse keys
enum class MouseKey
{
    Left,
    Right,
    Middle
};

//! Input event
struct InputEvent
{
    // TODO: Joystick events
    enum Type
    {
        Unknown, //!< Invalid event

        KeyboardKeyDown, //!< Keyboard key down, data contains KeyboardKey
        KeyboardKeyUp,   //!< Keyboard key up, data contains KeyboardKey
        KeyboardText,    //!< Some text was entered, data contains std::string with text in utf-8 encoding

        MouseMove,    //!< Mouse moved, data contains MousePos with relative mouse movement
        MouseKeyDown, //!< Mouse key down, data contains MouseKey
        MouseKeyUp,   //!< Mouse key up, data contains MouseKey
        MouseWheel    //!< Mouse wheel, data contains MousePos with relative wheel movement
    };
    //! Event type
    Type type;
    //! Data associated with event
    std::variant<std::string, KeyboardKey, MousePos, MouseKey> data;
};

//! Abstract input manager that handles all input events
class Input
{
  public:
    using EventHandler = std::function<void(const InputEvent &)>;

    //! Subscribe for events
    //! \param handler event callback
    //! \return subscription id, which should be passed to Unsubscribe()
    virtual int Subscribe(const EventHandler &handler) = 0;

    //! Unsubscribe from events
    //! \param id handler returned by Subscribe()
    virtual void Unsubscribe(int id) = 0;

    //! Process events
    virtual void ProcessEvents() = 0;

    //! Current keyboard key state
    //! \return true if key is pressed
    virtual bool KeyboardKeyState(const KeyboardKey &key) const = 0;

    //! Current mouse key state
    //! \return true if key is pressed
    virtual bool MouseKeyState(const MouseKey &key) const = 0;

    static std::shared_ptr<Input> Create();
};
} // namespace storm
