#pragma once

#include <input.hpp>
#include <SDL2/SDL.h>
#include <map>

#include "platform/platform.hpp"

namespace storm
{
class SDLInput : public Input
{
  public:
    SDLInput();
    ~SDLInput() override;

    int Subscribe(const EventHandler &handler) override;
    void Unsubscribe(int id) override;

    bool KeyboardModState(const KeyboardKey &key) const override;
    bool KeyboardKeyState(const KeyboardKey &key) const override;
    bool KeyboardSDLKeyState(const SDL_Scancode &key) const override;
    bool MouseKeyState(const MouseKey &key) const override;

    uint32_t GetWheelFactor() const override;

    bool ControllerButtonState(const ControllerButton &button) const override;
    int ControllerAxisValue(const ControllerAxis &axis) const override;

  private:
    static int SDLCALL SDLEventHandler(void *userdata, SDL_Event *evt);
    void ProcessEvent(const SDL_Event &event);
    void OpenController();

private:
    std::map<int, EventHandler> handlers_;
    const uint8_t *keyStates_;
    std::unique_ptr<SDL_GameController, std::function<void(SDL_GameController *)>> controller_;
    SDL_JoystickID joyID_;
};
} // namespace storm
