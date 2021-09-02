#pragma once

#include <functional>
#include <variant>

namespace storm
{
// TODO: Make own type for keycodes and use it everywhere (reuse SDL scancodes?)
//! Keyboard key, using virtual key codes (VK_* from WinUser.h) for now
using KeyboardKey = uint32_t;

//! Mouse relative position
struct MousePos
{
    int x = 0;
    int y = 0;
};

//! Mouse keys
enum class MouseKey
{
    Left,
    Right,
    Middle
};

//! Controller axes
enum class ControllerAxis
{
    LeftX = 0,
    //!< Left stick horizontal axis
    LeftY,
    //!< Left stick vertical axis
    RightX,
    //!< Right stick horizontal axis
    RightY,
    //!< Right stick vertical axis
    TriggerLeft,
    //!< Left trigger axis
    TriggerRight //!< Right trigger axis
};

//! Controller buttons
enum class ControllerButton
{
    A,
    B,
    X,
    Y,
    Back,
    Guide,
    Start,
    LeftStick,
    RightStick,
    LeftBumper,
    RightBumper,
    Up,
    Down,
    Left,
    Right
};

//! Current state of controller axis
struct ControllerAxisState
{
    ControllerAxis axis; //!< Axis
    int value;           //!< Current value, from -32768 to 32767 (0 to 32767 for triggers)
};

//! Input event
struct InputEvent
{
    enum Type
    {
        Unknown,
        //!< Invalid event

        KeyboardKeyDown,
        //!< Keyboard key down, data contains KeyboardKey
        KeyboardKeyUp,
        //!< Keyboard key up, data contains KeyboardKey
        KeyboardText,
        //!< Some text was entered, data contains std::string with text in utf-8 encoding

        MouseMove,
        //!< Mouse moved, data contains MousePos with relative mouse movement
        MouseKeyDown,
        //!< Mouse key down, data contains MouseKey
        MouseKeyUp,
        //!< Mouse key up, data contains MouseKey
        MouseWheel,
        //!< Mouse wheel, data contains MousePos with relative wheel movement

        ControllerAxis,
        //!< Controller axis value changed, data contains ControllerAxisState
        ControllerButtonDown,
        //!< Controller button pressed, data contains ControllerButton
        ControllerButtonUp //!< Controller button released, data contains ControllerButton
    };

    //! Event type
    Type type;
    //! Data associated with event
    std::variant<std::string, KeyboardKey, MousePos, MouseKey, ControllerAxisState, ControllerButton> data;
};

//! Abstract input manager that handles all input events
class Input
{
  public:
    using EventHandler = std::function<void(const InputEvent &)>;

    virtual ~Input(){};

    //! Subscribe for events
    //! \param handler event callback
    //! \return subscription id, which should be passed to Unsubscribe()
    virtual int Subscribe(const EventHandler &handler) = 0;

    //! Unsubscribe from events
    //! \param id handler returned by Subscribe()
    virtual void Unsubscribe(int id) = 0;

    //! Current keyboard key state
    //! \return true if key is pressed
    virtual bool KeyboardKeyState(const KeyboardKey &key) const = 0;

    //! Current mouse key state
    //! \return true if key is pressed
    virtual bool MouseKeyState(const MouseKey &key) const = 0;

    //! Platform-specific multiplier to use for scaling
    //! \return unsigned integer representing specific convenient value
    virtual uint32_t GetWheelFactor() const = 0;

    //! Current controller button state
    //! \return true if button is pressed
    virtual bool ControllerButtonState(const ControllerButton &button) const = 0;

    //! Current controller axis value
    //! \return current axis value, from -32768 to 32767 (0 to 32767 for triggers)
    virtual int ControllerAxisValue(const ControllerAxis &axis) const = 0;

    static std::shared_ptr<Input> Create();
};
} // namespace storm
