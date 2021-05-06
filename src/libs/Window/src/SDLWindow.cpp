#include "SDLWindow.hpp"

#include <SDL_syswm.h>
#include <map>

namespace storm
{

std::map<uint32_t, std::weak_ptr<SDLWindow>> SDLWindow::windows;

SDLWindow::SDLWindow(int width, int height, bool fullscreen) : fullscreen_(fullscreen)
{
    window_ = std::unique_ptr<SDL_Window, std::function<void(SDL_Window *)>>(
        SDL_CreateWindow("", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height,
                         (fullscreen ? SDL_WINDOW_FULLSCREEN : 0) | SDL_WINDOW_HIDDEN),
        [](SDL_Window *w) { SDL_DestroyWindow(w); });
}

void SDLWindow::Show()
{
    SDL_ShowWindow(window_.get());
}

void SDLWindow::Hide()
{
    SDL_HideWindow(window_.get());
}

void SDLWindow::Focus()
{
    SDL_RaiseWindow(window_.get());
}

int SDLWindow::Width() const
{
    int w, h;
    SDL_GetWindowSize(window_.get(), &w, &h);
    return w;
}

int SDLWindow::Height() const
{
    int w, h;
    SDL_GetWindowSize(window_.get(), &w, &h);
    return h;
}

bool SDLWindow::Fullscreen() const
{
    return fullscreen_;
}

std::string SDLWindow::Title() const
{
    return std::string(SDL_GetWindowTitle(window_.get()));
}

void SDLWindow::SetFullscreen(bool fullscreen)
{
    fullscreen_ = fullscreen;
    SDL_SetWindowFullscreen(window_.get(), fullscreen_ ? SDL_WINDOW_FULLSCREEN : 0);
}

void SDLWindow::Resize(int width, int height)
{
    SDL_SetWindowSize(window_.get(), width, height);
}

void SDLWindow::SetTitle(const std::string &title)
{
    SDL_SetWindowTitle(window_.get(), title.c_str());
}

int SDLWindow::Subscribe(const EventHandler &handler)
{
    int id = 1;
    if (!handlers_.empty())
        id = (--handlers_.end())->first + 1;
    handlers_[id] = handler;
    return id;
}

void SDLWindow::Unsubscribe(int id)
{
    auto it = handlers_.find(id);
    if (it != handlers_.end())
        handlers_.erase(it);
}

// TODO: X/Wayland/MacOS
void *SDLWindow::OSHandle()
{
    if (!window_)
        return nullptr;

    SDL_SysWMinfo info;
    SDL_VERSION(&info.version);
    SDL_GetWindowWMInfo(window_.get(), &info);
    return info.info.win.window;
}

SDL_Window *SDLWindow::SDLHandle()
{
    return window_.get();
}

void SDLWindow::ProcessEvent(const SDL_WindowEvent &evt)
{
    Event winEvent;
    switch (evt.event)
    {
    case SDL_WINDOWEVENT_FOCUS_GAINED:
        winEvent = FocusGained;
        break;

    case SDL_WINDOWEVENT_FOCUS_LOST:
        winEvent = FocusLost;
        break;

    case SDL_WINDOWEVENT_CLOSE:
        winEvent = Closed;
        break;

    default:
        return;
    }

    for (auto handler : handlers_)
        handler.second(winEvent);
}

std::shared_ptr<OSWindow> OSWindow::Create(int width, int height, bool fullscreen)
{
    auto remove = [](SDLWindow *w) {
        auto it = SDLWindow::windows.find(SDL_GetWindowID(w->SDLHandle()));
        if (it != SDLWindow::windows.end())
            SDLWindow::windows.erase(it);
        delete w;
    };
    std::shared_ptr<SDLWindow> w = std::shared_ptr<SDLWindow>(new SDLWindow(width, height, fullscreen), remove);
    SDLWindow::windows[SDL_GetWindowID(w->SDLHandle())] = w;
    return w;
}

void OSWindow::ProcessEvents()
{
    SDL_PumpEvents();

    SDL_Event event;
    while (SDL_PeepEvents(&event, 1, SDL_GETEVENT, SDL_WINDOWEVENT, SDL_WINDOWEVENT) > 0)
    {
        auto winIt = SDLWindow::windows.find(event.window.windowID);

        if (winIt == SDLWindow::windows.end())
            continue;

        std::shared_ptr<SDLWindow> w = winIt->second.lock();

        if (!w)
            continue;

        w->ProcessEvent(event.window);
    }
}
} // namespace storm
