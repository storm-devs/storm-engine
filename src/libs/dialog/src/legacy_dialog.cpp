#include "legacy_dialog.hpp"

#include <core.h>
#include <v_module_api.h>

#include <array>

CREATE_CLASS(LegacyDialog)

namespace
{

constexpr std::string_view DIALOG_INI_FILE_PATH = "Resource/Ini/dialog.ini";

int32_t LoadFont(const std::string_view &fontName, INIFILE &ini, VDX9RENDER &renderService)
{
    std::array<char, MAX_PATH> string_buffer{};
    ini.ReadString("DIALOG", fontName.data(), string_buffer.data(), string_buffer.size(), "DIALOG0");
    return renderService.LoadFont(string_buffer.data());
}

} // namespace

VDX9RENDER *LegacyDialog::RenderService = nullptr;

LegacyDialog::~LegacyDialog() noexcept
{
}

bool LegacyDialog::Init()
{
    RenderService = static_cast<VDX9RENDER *>(core.GetService("dx9render"));
    Assert(RenderService != nullptr);

    auto ini = fio->OpenIniFile(DIALOG_INI_FILE_PATH.data());

    mainFont_ = LoadFont("mainfont", *ini, *RenderService);
    nameFont_ = LoadFont("namefont", *ini, *RenderService);
    subFont_ = LoadFont("subfont", *ini, *RenderService);

    ini.reset();

    return true;
}

void LegacyDialog::ProcessStage(Stage stage, uint32_t delta)
{
    switch (stage)
    {
    case Stage::realize:
        Realize(delta);
        break;
    }
}

void LegacyDialog::Realize(uint32_t deltaTime)
{
}
