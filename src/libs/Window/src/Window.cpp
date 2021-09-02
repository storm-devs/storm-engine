#include "core.h"
#include "entity.h"
#include "SDLWindow.hpp"
#include "vfile_service.h"
#include "vmodule_api.h"
#include "storm/fs.h"

namespace
{

void HandleWindowEvent(const storm::OSWindow::Event &event)
{
    if (event == storm::OSWindow::Closed)
    {
        core.setRunning(false);
        core.Event("DestroyWindow", nullptr);
    }
    else if (event == storm::OSWindow::FocusGained)
    {
        core.setActive(true);
        core.AppState(true);
    }
    else if (event == storm::OSWindow::FocusLost)
    {
        core.setActive(false);
        core.AppState(false);
    }
}

} // namespace

namespace storm
{

class MainWindow final : public Entity, SDLWindow
{
public:
    bool Init() override
    {
        if (const auto ini = fio->OpenIniFile(fs::ENGINE_INI_FILE_NAME))
        {
            const auto width = ini->GetLong(nullptr, "screen_x", 1024);
            const auto height = ini->GetLong(nullptr, "screen_y", 768);
            const auto full_screen = ini->GetLong(nullptr, "full_screen", false);
            const auto title = AttributesPointer->GetAttribute("title");

            const auto success = SDLWindow::Init(width, height, full_screen, title);
            if(success)
            {
                core.Set_Hwnd(static_cast<HWND>(SDLWindow::OSHandle()));
                SDLWindow::Subscribe(HandleWindowEvent);
                SDLWindow::Show();
            }

            return success;
        }

        spdlog::critical("MainWindow: cannot read configuration");
        return false;
    }

    void ProcessStage(Stage stage, uint32_t delta) override { }
};

CREATE_CLASS(MainWindow);

} // namespace storm
