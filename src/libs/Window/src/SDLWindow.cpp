#include "SDLWindow.hpp"

#include <map>

#include <SDL_syswm.h>

namespace storm
{
SDLWindow::SDLWindow()
{
    SDL_AddEventWatch(&SDLEventHandler, this);
}

SDLWindow::~SDLWindow()
{
    SDL_DelEventWatch(&SDLEventHandler, this);
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

bool SDLWindow::Init(int width, int height, bool fullscreen, const char * title)
{
    window_ = std::unique_ptr<SDL_Window, std::function<void(SDL_Window *)>>(
        SDL_CreateWindow("", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height,
                         (fullscreen ? SDL_WINDOW_FULLSCREEN : 0) | SDL_WINDOW_HIDDEN),
        [](SDL_Window *w) { SDL_DestroyWindow(w); });

    sdlID_ = SDL_GetWindowID(window_.get());

    fullscreen_ = fullscreen;

    if (title != nullptr)
    {
        SetTitle(title);
    }

    return true;
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

int SDLWindow::SDLEventHandler(void *userdata, SDL_Event *evt)
{
    auto w = static_cast<SDLWindow *>(userdata);

    if ((evt->type != SDL_WINDOWEVENT) || (evt->window.windowID != w->sdlID_))
        return 0;

    w->ProcessEvent(evt->window);

    return 0;
}
} // namespace storm
