#include "legacy_dialog.hpp"

#include "dialog.hpp"

#include <core.h>
#include <defines.h>
#include <v_module_api.h>

#include <array>

CREATE_CLASS(LegacyDialog)

namespace
{

constexpr std::string_view DIALOG_INI_FILE_PATH = "Resource/Ini/dialog.ini";
const char *DEFAULT_INTERFACE_TEXTURE = "dialog/dialog.tga";

constexpr const uint32_t COLOR_NORMAL = 0xFFFFFFFF;

int32_t LoadFont(const std::string_view &fontName, INIFILE &ini, VDX9RENDER &renderService)
{
    std::array<char, MAX_PATH> string_buffer{};
    ini.ReadString("DIALOG", fontName.data(), string_buffer.data(), string_buffer.size(), "DIALOG0");
    return renderService.LoadFont(string_buffer.data());
}

void FillIndexBuffer(VDX9RENDER &renderService, int32_t indexBuffer, size_t spriteCount)
{
    auto *pI = static_cast<uint16_t *>(renderService.LockIndexBuffer(indexBuffer));
    for (size_t n = 0; n < spriteCount; n++)
    {
        pI[n * 6 + 0] = static_cast<uint16_t>(n * 4 + 0);
        pI[n * 6 + 1] = static_cast<uint16_t>(n * 4 + 2);
        pI[n * 6 + 2] = static_cast<uint16_t>(n * 4 + 1);
        pI[n * 6 + 3] = static_cast<uint16_t>(n * 4 + 1);
        pI[n * 6 + 4] = static_cast<uint16_t>(n * 4 + 2);
        pI[n * 6 + 5] = static_cast<uint16_t>(n * 4 + 3);
    }
    renderService.UnLockIndexBuffer(indexBuffer);
}

constexpr FRECT ScaleUv(FRECT uv)
{
    constexpr float hScale = 1.f / 1024.f;
    constexpr float vScale = 1.f / 256.f;

    return {
        uv.x1 * hScale,
        uv.y1 * vScale,
        uv.x2 * hScale,
        uv.y2 * vScale,
    };
}

struct SpriteInfo
{
    FRECT position{};
    FRECT uv{};
};

constexpr const size_t DIALOG_MAX_LINES = 8;
constexpr const float DIVIDER_HEIGHT = 14;

constexpr std::array SPRITE_DATA = {
    // Head overlay
    SpriteInfo{
        {29, 25, 147, 37},
        ScaleUv({904, 91, 904 + 119, 91 + 12}),
    },
    SpriteInfo{
        .position = {29, 173, 146, 185},
        .uv = ScaleUv({904, 105, 904 + 119, 105 + 11}),
    },
    // General
    SpriteInfo{
        .position = {-39, -39, 169, 216},
        .uv = ScaleUv({0, 0, 208, 255}),
    },
    SpriteInfo{
        .position = {169, -39, 678, 79},
        .uv = ScaleUv({208, 0, 757, 118}),
    },
    SpriteInfo{
        .position = {-39, 451, 678, 518},
        .uv = ScaleUv({209, 189, 1023, 255}),
    },
    // Static vertices 2
    SpriteInfo{
        .position = {29, 25, 147, 37},
        .uv = ScaleUv({904, 91, 1023, 103}),
    },
    SpriteInfo{
        .position = {29, 173, 146, 185},
        .uv = ScaleUv({904, 105, 1023, 116}),
    },
    // Dialog lines
    SpriteInfo{
        .position = {-39, static_cast<float>(479 - 67 + 39 - 26), 639 + 39, static_cast<float>(479 - 67 + 39)},
        .uv = ScaleUv({209, 155, 1023, 186}),
    },
    SpriteInfo{
        .position = {-39, static_cast<float>(479 - 67 + 39 - (26 * (DIALOG_MAX_LINES + 1))), 639 + 39,
                     static_cast<float>(479 - 67 + 39 - (26 * DIALOG_MAX_LINES))},
        .uv = ScaleUv({209, 119, 1023, 156}),
    },
    // Divider
    SpriteInfo{
        .position = {35, static_cast<float>(479 - 67 + 39 - DIVIDER_HEIGHT), 605, static_cast<float>(479 - 67 + 39)},
        .uv = ScaleUv({209, 94, 602, 116}),
    },
};

constexpr auto SPRITE_COUNT = SPRITE_DATA.size() + (DIALOG_MAX_LINES - 1);

} // namespace

VDX9RENDER *LegacyDialog::RenderService = nullptr;

LegacyDialog::~LegacyDialog() noexcept
{
    if (interfaceTexture_)
    {
        RenderService->TextureRelease(interfaceTexture_);
    }
}

bool LegacyDialog::Init()
{
    RenderService = static_cast<VDX9RENDER *>(core.GetService("dx9render"));
    Assert(RenderService != nullptr);

    LoadIni();

    const char *texture = this->AttributesPointer->GetAttribute("texture");
    if (texture == nullptr)
    {
        texture = DEFAULT_INTERFACE_TEXTURE;
    }
    interfaceTexture_ = RenderService->TextureCreate(texture);

    CreateBackBuffers();

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
    UpdateBackBuffers();

    RenderService->TextureSet(0, interfaceTexture_);
    RenderService->DrawBuffer(backVertexBuffer_, sizeof(XI_TEX_VERTEX), backIndexBuffer_, 0, SPRITE_COUNT * 4, 0,
                              SPRITE_COUNT * 2, "texturedialogfon");
}

uint32_t LegacyDialog::AttributeChanged(ATTRIBUTES *attributes)
{
    if (storm::iEquals(attributes->GetThisName(), "texture"))
    {
        RenderService->TextureRelease(interfaceTexture_);
        interfaceTexture_ = RenderService->TextureCreate(attributes->GetThisAttr());
    }

    return Entity::AttributeChanged(attributes);
}

void LegacyDialog::LoadIni()
{
    auto ini = fio->OpenIniFile(DIALOG_INI_FILE_PATH.data());

    mainFont_ = LoadFont("mainfont", *ini, *RenderService);
    nameFont_ = LoadFont("namefont", *ini, *RenderService);
    subFont_ = LoadFont("subfont", *ini, *RenderService);

    ini.reset();
}

void LegacyDialog::CreateBackBuffers()
{
    constexpr auto vertex_count = SPRITE_COUNT * 4;
    constexpr auto index_count = SPRITE_COUNT * 6;

    backVertexBuffer_ =
        RenderService->CreateVertexBuffer(XI_TEX_FVF, vertex_count * sizeof(XI_TEX_VERTEX), D3DUSAGE_WRITEONLY);

    backIndexBuffer_ = RenderService->CreateIndexBuffer(index_count * sizeof(uint16_t));
    FillIndexBuffer(*RenderService, backIndexBuffer_, SPRITE_COUNT);
}

void LegacyDialog::UpdateBackBuffers()
{
    D3DVIEWPORT9 viewport;
    RenderService->GetViewport(&viewport);
    const auto screenSize = core.GetScreenSize();

    float hScale = static_cast<float>(viewport.Width) / static_cast<float>(screenSize.width);
    float vScale = static_cast<float>(viewport.Height) / static_cast<float>(screenSize.height);

    auto createSpriteMesh = [hScale, vScale](const SpriteInfo &sprite) {
        const std::array mesh = {
            XI_TEX_VERTEX{.pos{hScale * sprite.position.left, vScale * sprite.position.top, 1.f},
                          .rhw = 0.5f,
                          .color = COLOR_NORMAL,
                          .u = sprite.uv.left,
                          .v = sprite.uv.top},
            XI_TEX_VERTEX{.pos{hScale * sprite.position.right, vScale * sprite.position.top, 1.f},
                          .rhw = 0.5f,
                          .color = COLOR_NORMAL,
                          .u = sprite.uv.right,
                          .v = sprite.uv.top},
            XI_TEX_VERTEX{.pos{hScale * sprite.position.left, vScale * sprite.position.bottom, 1.f},
                          .rhw = 0.5f,
                          .color = COLOR_NORMAL,
                          .u = sprite.uv.left,
                          .v = sprite.uv.bottom},
            XI_TEX_VERTEX{.pos{hScale * sprite.position.right, vScale * sprite.position.bottom, 1.f},
                          .rhw = 0.5f,
                          .color = COLOR_NORMAL,
                          .u = sprite.uv.right,
                          .v = sprite.uv.bottom},
        };
        return mesh;
    };

    auto *pV = static_cast<XI_TEX_VERTEX *>(RenderService->LockVertexBuffer(backVertexBuffer_));
    size_t vi = 0;
    for (size_t i = 0; i < 7; ++i)
    {
        const auto &vertices = createSpriteMesh(SPRITE_DATA[i]);
        pV[vi++] = vertices[0];
        pV[vi++] = vertices[1];
        pV[vi++] = vertices[2];
        pV[vi++] = vertices[3];
    }

    for (size_t i = 0; i < DIALOG_MAX_LINES; ++i)
    {
        auto spriteData = SPRITE_DATA[7];
        const auto offset = static_cast<float>(26 * i);
        spriteData.position.top -= offset;
        spriteData.position.bottom -= offset;

        const auto &vertices = createSpriteMesh(spriteData);
        pV[vi++] = vertices[0];
        pV[vi++] = vertices[1];
        pV[vi++] = vertices[2];
        pV[vi++] = vertices[3];
    }
    for (size_t i = 0; i < 3; ++i)
    {
        const auto &vertices = createSpriteMesh(SPRITE_DATA[8 + i]);
        pV[vi++] = vertices[0];
        pV[vi++] = vertices[1];
        pV[vi++] = vertices[2];
        pV[vi++] = vertices[3];
    }
    RenderService->UnLockVertexBuffer(backVertexBuffer_);
}
