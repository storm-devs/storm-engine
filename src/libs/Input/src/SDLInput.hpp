#pragma once

#include <Input.hpp>
#include <map>

namespace storm
{
class SDLInput : public Input
{
  public:
    SDLInput();

    int Subscribe(const EventHandler &handler) override;
    void Unsubscribe(int id) override;

    void ProcessEvents() override;

    bool KeyboardKeyState(const KeyboardKey &key) const override;
    bool MouseKeyState(const MouseKey &key) const override;

  private:
    std::map<int, EventHandler> handlers_;
    const uint8_t *keyStates_;
};
} // namespace storm
