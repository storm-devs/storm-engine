#pragma once

#include "dx9render.h"

#include <string_view>

struct FLOAT_RECT
{
    float x1, y1, x2, y2;
};

struct FONT_SYMBOL
{
    FLOAT_RECT Pos;
    FLOAT_RECT Tuv;
};

class FONT final
{
  public:
    FONT(VDX9RENDER &renderer, IDirect3DDevice9 &device);
    ~FONT();
    bool Init(const char *font_name, const char *iniName);
    void TempUnload();
    void RepeatInit();

    struct FONT_PRINT_OVERRIDES
    {
        std::optional<float> scale;
        std::optional<uint32_t> color;
        std::optional<bool> shadow;
    };

    int32_t Print(int32_t x, int32_t y, char *Text, const FONT_PRINT_OVERRIDES &overrides = {});

    [[nodiscard]] int32_t GetStringWidth(const std::string_view &Text, std::optional<float> scale_override = {}) const;

    [[nodiscard]] int32_t GetHeight() const
    {
        return height_;
    }

  private:
    int32_t UpdateVertexBuffer(int32_t x, int32_t y, char *data_PTR, int utf8length, float scale, uint32_t color);

    std::vector<FONT_SYMBOL> charDescriptors_{};

    std::string techniqueName_{};
    std::string textureName_{};

    VDX9RENDER &renderService_;
    IDirect3DDevice9 &device_;
    IDirect3DVertexBuffer9 *vertexBuffer_ = nullptr;

    float scale_ = 1;
    float horizontalAspectRatio_ = 1;
    float verticalAspectRatio_ = 1;

    uint32_t color_ = 0xffffffff;

    int32_t textureHandle_ = -1;
    int32_t height_ = 0;
    int32_t textureSizeX_ = 1;
    int32_t textureSizeY_ = 1;
    int32_t symbolInterval_ = 0;

    int32_t shadowOffsetX_ = 2;
    int32_t shadowOffsetY_ = 2;

    int32_t spacebarWidth_ = 8;

    bool drawShadows_ = false;

    bool initialized_ = false;
};
