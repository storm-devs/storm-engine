#pragma once

#include <Input.hpp>
#include <SDL.h>
#include <map>

namespace storm
{
class SDLInput : public Input
{
  public:
    SDLInput();
    ~SDLInput() override;

    int Subscribe(const EventHandler &handler) override;
    void Unsubscribe(int id) override;

    bool KeyboardKeyState(const KeyboardKey &key) const override;
    bool MouseKeyState(const MouseKey &key) const override;

    bool ControllerButtonState(const ControllerButton &button) const override;
    int ControllerAxisValue(const ControllerAxis &axis) const override;

  private:
    static int SDLEventHandler(void *userdata, SDL_Event *evt);
    void ProcessEvent(const SDL_Event &event);
    void OpenController();

    std::map<int, EventHandler> handlers_;
    const uint8_t *keyStates_;
    std::unique_ptr<SDL_GameController, std::function<void(SDL_GameController *)>> controller_;
    SDL_JoystickID joyID_;
};
} // namespace storm
