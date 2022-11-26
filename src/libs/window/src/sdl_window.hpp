#pragma once

#include <os_window.hpp>
#include <SDL2/SDL.h>
#include <map>

namespace storm
{
class SDLWindow : public OSWindow
{
  public:
    SDLWindow(int width, int height, int preferred_display, bool fullscreen, bool bordered);
    ~SDLWindow() override;

    void Show() override;
    void Hide() override;
    void Focus() override;

    int Width() const override;
    int Height() const override;
    WindowSize GetWindowSize() const override;
    bool Fullscreen() const override;
    std::string Title() const override;

    void SetFullscreen(bool fullscreen) override;
    void Resize(int width, int height) override;
    void WarpMouseInWindow(int x, int y) override;
    void SetTitle(const std::string &title) override;
    void SetGamma(const uint16_t (&red)[256], const uint16_t (&green)[256], const uint16_t (&blue)[256]) override;

    int Subscribe(const EventHandler &handler) override;
    void Unsubscribe(int id) override;

    void *OSHandle() override;

    SDL_Window *SDLHandle() const;
    void ProcessEvent(const SDL_WindowEvent &evt) const;

  private:
    static int SDLCALL SDLEventHandler(void *userdata, SDL_Event *evt);

    std::unique_ptr<SDL_Window, std::function<void(SDL_Window *)>> window_ = nullptr;
    uint32_t sdlID_;
    bool fullscreen_ = false;
    std::map<int, EventHandler> handlers_;
};
} // namespace storm
