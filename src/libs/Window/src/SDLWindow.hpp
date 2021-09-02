#pragma once

#include <OSWindow.hpp>

#include <map>

#include <SDL.h>

namespace storm
{
class SDLWindow : public OSWindow
{
  public:
    SDLWindow();
    ~SDLWindow() override;

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
    
    bool Init(int width, int height, bool fullscreen, const char * title);

    SDL_Window *SDLHandle();
    void ProcessEvent(const SDL_WindowEvent &evt);

  private:
    static int SDLEventHandler(void *userdata, SDL_Event *evt);

    std::unique_ptr<SDL_Window, std::function<void(SDL_Window *)>> window_;
    uint32_t sdlID_{};
    bool fullscreen_{};
    std::map<int, EventHandler> handlers_;
};
} // namespace storm
