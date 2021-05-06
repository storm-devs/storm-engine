#pragma once

#include <OSWindow.hpp>
#include <SDL.h>
#include <map>

namespace storm
{
class SDLWindow : public OSWindow
{
  public:
    SDLWindow(int width, int height, bool fullscreen);

    void Show() override;
    void Hide() override;
    void Focus() override;

    int Width() const override;
    int Height() const override;
    bool Fullscreen() const override;
    std::string Title() const override;

    void SetFullscreen(bool fullscreen) override;
    void Resize(int width, int height) override;
    void SetTitle(const std::string &title) override;

    int Subscribe(const EventHandler &handler) override;
    void Unsubscribe(int id) override;

    void *OSHandle() override;

    SDL_Window *SDLHandle();
    void ProcessEvent(const SDL_WindowEvent &evt);

    // Used to track all SDL windows, for event processing
    static std::map<uint32_t, std::weak_ptr<SDLWindow>> windows;

  private:
    std::unique_ptr<SDL_Window, std::function<void(SDL_Window *)>> window_ = nullptr;
    bool fullscreen_ = false;
    std::map<int, EventHandler> handlers_;
};
} // namespace storm
