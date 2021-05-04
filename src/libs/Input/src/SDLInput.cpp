#include "SDLInput.hpp"

#include <SDL.h>
#include <SDL_system.h>
#include <SDL_video.h>
#include <Windows.h>
#include <map>

namespace storm
{
namespace
{
// TODO: Use scancodes everywhere instead of Windows virtual key
static const std::map<unsigned int, unsigned int> VK_TO_SDL_MAP{
    {VK_CANCEL, SDL_SCANCODE_CANCEL},
    {VK_BACK, SDL_SCANCODE_BACKSPACE},
    {VK_TAB, SDL_SCANCODE_TAB},
    {VK_CLEAR, SDL_SCANCODE_CLEAR},
    {VK_RETURN, SDL_SCANCODE_RETURN},
    {VK_SHIFT, SDL_SCANCODE_LSHIFT},
    {VK_CONTROL, SDL_SCANCODE_LCTRL},
    {VK_MENU, SDL_SCANCODE_MENU},
    {VK_PAUSE, SDL_SCANCODE_PAUSE},
    {VK_CAPITAL, SDL_SCANCODE_CAPSLOCK},
    {VK_ESCAPE, SDL_SCANCODE_ESCAPE},
    {VK_SPACE, SDL_SCANCODE_SPACE},
    {VK_PRIOR, SDL_SCANCODE_PAGEUP},
    {VK_NEXT, SDL_SCANCODE_PAGEDOWN},
    {VK_END, SDL_SCANCODE_END},
    {VK_HOME, SDL_SCANCODE_HOME},
    {VK_LEFT, SDL_SCANCODE_LEFT},
    {VK_UP, SDL_SCANCODE_UP},
    {VK_RIGHT, SDL_SCANCODE_RIGHT},
    {VK_DOWN, SDL_SCANCODE_DOWN},
    {VK_SELECT, SDL_SCANCODE_SELECT},
    {VK_EXECUTE, SDL_SCANCODE_EXECUTE},
    {VK_SNAPSHOT, SDL_SCANCODE_PRINTSCREEN},
    {VK_INSERT, SDL_SCANCODE_INSERT},
    {VK_DELETE, SDL_SCANCODE_DELETE},
    {VK_HELP, SDL_SCANCODE_HELP},
    {'0', SDL_SCANCODE_0},
    {'1', SDL_SCANCODE_1},
    {'2', SDL_SCANCODE_2},
    {'3', SDL_SCANCODE_3},
    {'4', SDL_SCANCODE_4},
    {'5', SDL_SCANCODE_5},
    {'6', SDL_SCANCODE_6},
    {'7', SDL_SCANCODE_7},
    {'8', SDL_SCANCODE_8},
    {'9', SDL_SCANCODE_9},
    {'A', SDL_SCANCODE_A},
    {'B', SDL_SCANCODE_B},
    {'C', SDL_SCANCODE_C},
    {'D', SDL_SCANCODE_D},
    {'E', SDL_SCANCODE_E},
    {'F', SDL_SCANCODE_F},
    {'G', SDL_SCANCODE_G},
    {'H', SDL_SCANCODE_H},
    {'I', SDL_SCANCODE_I},
    {'J', SDL_SCANCODE_J},
    {'K', SDL_SCANCODE_K},
    {'L', SDL_SCANCODE_L},
    {'M', SDL_SCANCODE_M},
    {'N', SDL_SCANCODE_N},
    {'O', SDL_SCANCODE_O},
    {'P', SDL_SCANCODE_P},
    {'Q', SDL_SCANCODE_Q},
    {'R', SDL_SCANCODE_R},
    {'S', SDL_SCANCODE_S},
    {'T', SDL_SCANCODE_T},
    {'U', SDL_SCANCODE_U},
    {'V', SDL_SCANCODE_V},
    {'W', SDL_SCANCODE_W},
    {'X', SDL_SCANCODE_X},
    {'Y', SDL_SCANCODE_Y},
    {'Z', SDL_SCANCODE_Z},
    {VK_LWIN, SDL_SCANCODE_LGUI},
    {VK_RWIN, SDL_SCANCODE_RGUI},
    {VK_APPS, SDL_SCANCODE_MENU},
    {VK_SLEEP, SDL_SCANCODE_SLEEP},
    {VK_NUMPAD0, SDL_SCANCODE_KP_0},
    {VK_NUMPAD1, SDL_SCANCODE_KP_1},
    {VK_NUMPAD2, SDL_SCANCODE_KP_2},
    {VK_NUMPAD3, SDL_SCANCODE_KP_3},
    {VK_NUMPAD4, SDL_SCANCODE_KP_4},
    {VK_NUMPAD5, SDL_SCANCODE_KP_5},
    {VK_NUMPAD6, SDL_SCANCODE_KP_6},
    {VK_NUMPAD7, SDL_SCANCODE_KP_7},
    {VK_NUMPAD8, SDL_SCANCODE_KP_8},
    {VK_NUMPAD9, SDL_SCANCODE_KP_9},
    {VK_MULTIPLY, SDL_SCANCODE_KP_MULTIPLY},
    {VK_ADD, SDL_SCANCODE_KP_PLUS},
    {VK_SUBTRACT, SDL_SCANCODE_KP_MINUS},
    {VK_DECIMAL, SDL_SCANCODE_KP_COMMA},
    {VK_DIVIDE, SDL_SCANCODE_KP_DIVIDE},
    {VK_F1, SDL_SCANCODE_F1},
    {VK_F2, SDL_SCANCODE_F2},
    {VK_F3, SDL_SCANCODE_F3},
    {VK_F4, SDL_SCANCODE_F4},
    {VK_F5, SDL_SCANCODE_F5},
    {VK_F6, SDL_SCANCODE_F6},
    {VK_F7, SDL_SCANCODE_F7},
    {VK_F8, SDL_SCANCODE_F8},
    {VK_F9, SDL_SCANCODE_F9},
    {VK_F10, SDL_SCANCODE_F10},
    {VK_F11, SDL_SCANCODE_F11},
    {VK_F12, SDL_SCANCODE_F12},
    {VK_F13, SDL_SCANCODE_F13},
    {VK_F14, SDL_SCANCODE_F14},
    {VK_F15, SDL_SCANCODE_F15},
    {VK_F16, SDL_SCANCODE_F16},
    {VK_F17, SDL_SCANCODE_F17},
    {VK_F18, SDL_SCANCODE_F18},
    {VK_F19, SDL_SCANCODE_F19},
    {VK_F20, SDL_SCANCODE_F20},
    {VK_F21, SDL_SCANCODE_F21},
    {VK_F22, SDL_SCANCODE_F22},
    {VK_F23, SDL_SCANCODE_F23},
    {VK_F24, SDL_SCANCODE_F24},
    {VK_NUMLOCK, SDL_SCANCODE_NUMLOCKCLEAR},
    {VK_SCROLL, SDL_SCANCODE_SCROLLLOCK},
    {VK_LSHIFT, SDL_SCANCODE_LSHIFT},
    {VK_RSHIFT, SDL_SCANCODE_RSHIFT},
    {VK_LCONTROL, SDL_SCANCODE_LCTRL},
    {VK_RCONTROL, SDL_SCANCODE_RCTRL},
    {VK_LMENU, SDL_SCANCODE_MENU},
    {VK_RMENU, SDL_SCANCODE_MENU},
    {VK_OEM_1, SDL_SCANCODE_SEMICOLON},
    {VK_OEM_PLUS, SDL_SCANCODE_EQUALS},
    {VK_OEM_COMMA, SDL_SCANCODE_COMMA},
    {VK_OEM_MINUS, SDL_SCANCODE_MINUS},
    {VK_OEM_PERIOD, SDL_SCANCODE_PERIOD},
    {VK_OEM_2, SDL_SCANCODE_SLASH},
    {VK_OEM_3, SDL_SCANCODE_GRAVE},
    {VK_OEM_4, SDL_SCANCODE_LEFTBRACKET},
    {VK_OEM_5, SDL_SCANCODE_BACKSLASH},
    {VK_OEM_6, SDL_SCANCODE_RIGHTBRACKET},
    {VK_OEM_7, SDL_SCANCODE_APOSTROPHE},
    {VK_VOLUME_MUTE, SDL_SCANCODE_MUTE},
    {VK_VOLUME_DOWN, SDL_SCANCODE_VOLUMEDOWN},
    {VK_VOLUME_UP, SDL_SCANCODE_VOLUMEUP},
};

static const std::map<unsigned int, unsigned int> SDL_TO_VK_MAP{
    {SDL_SCANCODE_CANCEL, VK_CANCEL},
    {SDL_SCANCODE_BACKSPACE, VK_BACK},
    {SDL_SCANCODE_TAB, VK_TAB},
    {SDL_SCANCODE_CLEAR, VK_CLEAR},
    {SDL_SCANCODE_RETURN, VK_RETURN},
    {SDL_SCANCODE_LSHIFT, VK_SHIFT},
    {SDL_SCANCODE_LCTRL, VK_CONTROL},
    {SDL_SCANCODE_MENU, VK_MENU},
    {SDL_SCANCODE_PAUSE, VK_PAUSE},
    {SDL_SCANCODE_CAPSLOCK, VK_CAPITAL},
    {SDL_SCANCODE_ESCAPE, VK_ESCAPE},
    {SDL_SCANCODE_SPACE, VK_SPACE},
    {SDL_SCANCODE_PAGEUP, VK_PRIOR},
    {SDL_SCANCODE_PAGEDOWN, VK_NEXT},
    {SDL_SCANCODE_END, VK_END},
    {SDL_SCANCODE_HOME, VK_HOME},
    {SDL_SCANCODE_LEFT, VK_LEFT},
    {SDL_SCANCODE_UP, VK_UP},
    {SDL_SCANCODE_RIGHT, VK_RIGHT},
    {SDL_SCANCODE_DOWN, VK_DOWN},
    {SDL_SCANCODE_SELECT, VK_SELECT},
    {SDL_SCANCODE_EXECUTE, VK_EXECUTE},
    {SDL_SCANCODE_PRINTSCREEN, VK_SNAPSHOT},
    {SDL_SCANCODE_INSERT, VK_INSERT},
    {SDL_SCANCODE_DELETE, VK_DELETE},
    {SDL_SCANCODE_HELP, VK_HELP},
    {SDL_SCANCODE_0, '0'},
    {SDL_SCANCODE_1, '1'},
    {SDL_SCANCODE_2, '2'},
    {SDL_SCANCODE_3, '3'},
    {SDL_SCANCODE_4, '4'},
    {SDL_SCANCODE_5, '5'},
    {SDL_SCANCODE_6, '6'},
    {SDL_SCANCODE_7, '7'},
    {SDL_SCANCODE_8, '8'},
    {SDL_SCANCODE_9, '9'},
    {SDL_SCANCODE_A, 'A'},
    {SDL_SCANCODE_B, 'B'},
    {SDL_SCANCODE_C, 'C'},
    {SDL_SCANCODE_D, 'D'},
    {SDL_SCANCODE_E, 'E'},
    {SDL_SCANCODE_F, 'F'},
    {SDL_SCANCODE_G, 'G'},
    {SDL_SCANCODE_H, 'H'},
    {SDL_SCANCODE_I, 'I'},
    {SDL_SCANCODE_J, 'J'},
    {SDL_SCANCODE_K, 'K'},
    {SDL_SCANCODE_L, 'L'},
    {SDL_SCANCODE_M, 'M'},
    {SDL_SCANCODE_N, 'N'},
    {SDL_SCANCODE_O, 'O'},
    {SDL_SCANCODE_P, 'P'},
    {SDL_SCANCODE_Q, 'Q'},
    {SDL_SCANCODE_R, 'R'},
    {SDL_SCANCODE_S, 'S'},
    {SDL_SCANCODE_T, 'T'},
    {SDL_SCANCODE_U, 'U'},
    {SDL_SCANCODE_V, 'V'},
    {SDL_SCANCODE_W, 'W'},
    {SDL_SCANCODE_X, 'X'},
    {SDL_SCANCODE_Y, 'Y'},
    {SDL_SCANCODE_Z, 'Z'},
    {SDL_SCANCODE_LGUI, VK_LWIN},
    {SDL_SCANCODE_RGUI, VK_RWIN},
    {SDL_SCANCODE_MENU, VK_APPS},
    {SDL_SCANCODE_SLEEP, VK_SLEEP},
    {SDL_SCANCODE_KP_0, VK_NUMPAD0},
    {SDL_SCANCODE_KP_1, VK_NUMPAD1},
    {SDL_SCANCODE_KP_2, VK_NUMPAD2},
    {SDL_SCANCODE_KP_3, VK_NUMPAD3},
    {SDL_SCANCODE_KP_4, VK_NUMPAD4},
    {SDL_SCANCODE_KP_5, VK_NUMPAD5},
    {SDL_SCANCODE_KP_6, VK_NUMPAD6},
    {SDL_SCANCODE_KP_7, VK_NUMPAD7},
    {SDL_SCANCODE_KP_8, VK_NUMPAD8},
    {SDL_SCANCODE_KP_9, VK_NUMPAD9},
    {SDL_SCANCODE_KP_MULTIPLY, VK_MULTIPLY},
    {SDL_SCANCODE_KP_PLUS, VK_ADD},
    {SDL_SCANCODE_KP_MINUS, VK_SUBTRACT},
    {SDL_SCANCODE_KP_COMMA, VK_DECIMAL},
    {SDL_SCANCODE_KP_DIVIDE, VK_DIVIDE},
    {SDL_SCANCODE_F1, VK_F1},
    {SDL_SCANCODE_F2, VK_F2},
    {SDL_SCANCODE_F3, VK_F3},
    {SDL_SCANCODE_F4, VK_F4},
    {SDL_SCANCODE_F5, VK_F5},
    {SDL_SCANCODE_F6, VK_F6},
    {SDL_SCANCODE_F7, VK_F7},
    {SDL_SCANCODE_F8, VK_F8},
    {SDL_SCANCODE_F9, VK_F9},
    {SDL_SCANCODE_F10, VK_F10},
    {SDL_SCANCODE_F11, VK_F11},
    {SDL_SCANCODE_F12, VK_F12},
    {SDL_SCANCODE_F13, VK_F13},
    {SDL_SCANCODE_F14, VK_F14},
    {SDL_SCANCODE_F15, VK_F15},
    {SDL_SCANCODE_F16, VK_F16},
    {SDL_SCANCODE_F17, VK_F17},
    {SDL_SCANCODE_F18, VK_F18},
    {SDL_SCANCODE_F19, VK_F19},
    {SDL_SCANCODE_F20, VK_F20},
    {SDL_SCANCODE_F21, VK_F21},
    {SDL_SCANCODE_F22, VK_F22},
    {SDL_SCANCODE_F23, VK_F23},
    {SDL_SCANCODE_F24, VK_F24},
    {SDL_SCANCODE_NUMLOCKCLEAR, VK_NUMLOCK},
    {SDL_SCANCODE_SCROLLLOCK, VK_SCROLL},
    {SDL_SCANCODE_LSHIFT, VK_LSHIFT},
    {SDL_SCANCODE_RSHIFT, VK_RSHIFT},
    {SDL_SCANCODE_LCTRL, VK_LCONTROL},
    {SDL_SCANCODE_RCTRL, VK_RCONTROL},
    {SDL_SCANCODE_MENU, VK_LMENU},
    {SDL_SCANCODE_MENU, VK_RMENU},
    {SDL_SCANCODE_SEMICOLON, VK_OEM_1},
    {SDL_SCANCODE_EQUALS, VK_OEM_PLUS},
    {SDL_SCANCODE_COMMA, VK_OEM_COMMA},
    {SDL_SCANCODE_MINUS, VK_OEM_MINUS},
    {SDL_SCANCODE_PERIOD, VK_OEM_PERIOD},
    {SDL_SCANCODE_SLASH, VK_OEM_2},
    {SDL_SCANCODE_GRAVE, VK_OEM_3},
    {SDL_SCANCODE_LEFTBRACKET, VK_OEM_4},
    {SDL_SCANCODE_BACKSLASH, VK_OEM_5},
    {SDL_SCANCODE_RIGHTBRACKET, VK_OEM_6},
    {SDL_SCANCODE_APOSTROPHE, VK_OEM_7},
    {SDL_SCANCODE_MUTE, VK_VOLUME_MUTE},
    {SDL_SCANCODE_VOLUMEDOWN, VK_VOLUME_DOWN},
    {SDL_SCANCODE_VOLUMEUP, VK_VOLUME_UP},
};

inline KeyboardKey sdlToKey(unsigned int code)
{
    auto it = SDL_TO_VK_MAP.find(code);
    if (it == SDL_TO_VK_MAP.end())
        return 0;
    return it->second;
}

inline unsigned int keyToSDL(KeyboardKey key)
{
    auto it = VK_TO_SDL_MAP.find(key);
    if (it == VK_TO_SDL_MAP.end())
        return 0;
    return it->second;
}
} // namespace

SDLInput::SDLInput() : m_keyStates(nullptr)
{
    m_keyStates = SDL_GetKeyboardState(nullptr);
    SDL_SetRelativeMouseMode(SDL_TRUE);
}

int SDLInput::Subscribe(const EventHandler &handler)
{
    int id = 1;
    if (!m_handlers.empty())
        id = (--m_handlers.end())->first + 1;
    m_handlers[id] = handler;
    return id;
}

void SDLInput::Unsubscribe(int id)
{
    auto it = m_handlers.find(id);
    if (it != m_handlers.end())
        m_handlers.erase(it);
}

void SDLInput::ProcessEvents()
{
    SDL_Event event;

    SDL_PumpEvents();
    while (SDL_PeepEvents(&event, 1, SDL_GETEVENT, SDL_KEYDOWN, SDL_MOUSEWHEEL) > 0)
    {
        InputEvent out;
        out.type = InputEvent::Unknown;

        if (event.type == SDL_MOUSEMOTION)
        {
            out.type = InputEvent::MouseMove;
            out.data = std::make_pair(event.motion.xrel, event.motion.yrel);
        }
        else if (event.type == SDL_MOUSEWHEEL)
        {
            out.type = InputEvent::MouseWheel;
            out.data = std::make_pair(event.wheel.x, event.wheel.y);
        }
        else if (event.type == SDL_KEYDOWN)
        {
            out.type = InputEvent::KeyboardKeyDown;
            out.data = sdlToKey(event.key.keysym.scancode);
        }
        else if (event.type == SDL_KEYUP)
        {
            out.type = InputEvent::KeyboardKeyUp;
            out.data = sdlToKey(event.key.keysym.scancode);
        }
        else if (event.type == SDL_TEXTINPUT)
        {
            out.type = InputEvent::KeyboardText;
            out.data = std::string(event.text.text);
        }

        if (out.type != InputEvent::Unknown)
        {
            for (const auto &handler : m_handlers)
                handler.second(out);
        }
    }
}

bool SDLInput::KeyboardKeyState(const KeyboardKey &key) const
{
    return m_keyStates[keyToSDL(key)] != 0;
}

bool SDLInput::MouseKeyState(const MouseKey &key) const
{
    uint32_t btn = SDL_GetMouseState(nullptr, nullptr);
    if (key == MouseKey::Left)
        return btn & SDL_BUTTON_LMASK ? true : false;
    else if (key == MouseKey::Right)
        return btn & SDL_BUTTON_RMASK ? true : false;
    else if (key == MouseKey::Middle)
        return btn & SDL_BUTTON_MMASK ? true : false;

    return false;
}

std::shared_ptr<Input> Input::Create()
{
    return std::make_shared<SDLInput>();
}
} // namespace storm
