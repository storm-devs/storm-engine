#pragma once

#include <functional>
#include <memory>
#include <string>

namespace storm
{
//! Abstract window
class OSWindow
{
  public:
    // TODO: More events?
    enum Event
    {
        Unknown,
        //!< Invalid event

        FocusGained,
        //!< Window gained focus
        FocusLost,
        //!< Window lost focus
        Closed //!< Window was closed by user
    };

    using EventHandler = std::function<void(const Event &)>;

    virtual ~OSWindow(){};

    //! Show window
    virtual void Show() = 0;
    //! Hide window
    virtual void Hide() = 0;
    //! Make window focused
    virtual void Focus() = 0;

    //! Current width of window
    virtual int Width() const = 0;
    //! Current height of window
    virtual int Height() const = 0;
    //! Is window fullscreen
    virtual bool Fullscreen() const = 0;
    //! Window title
    virtual std::string Title() const = 0;

    //! Toggle fullscreen
    virtual void SetFullscreen(bool fullscreen) = 0;
    //! Resize window
    virtual void Resize(int width, int height) = 0;
    //! Set window title
    virtual void SetTitle(const std::string &title) = 0;

    //! Subscribe for events
    //! \param handler event callback
    //! \return subscription id, which should be passed to unsubscribe()
    virtual int Subscribe(const EventHandler &handler) = 0;
    //! Unsubscribe from events
    //! \param id handler returned by subscribe()
    virtual void Unsubscribe(int id) = 0;

    //! Os-depended window handler (i.e. HWND on Windows)
    virtual void *OSHandle() = 0;

    //! Create new window
    static std::shared_ptr<OSWindow> Create(int width, int height, bool fullscreen);
};
} // namespace storm
