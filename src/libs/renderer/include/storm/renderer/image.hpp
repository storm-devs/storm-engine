#pragma once

namespace storm::renderer
{

class Image {
  public:
    size_t width{};
    size_t height{};
    size_t pitch{};
    uint32_t type{};
    uint32_t bpp{};
    uint8_t *data{};
};

} // namespace storm::renderer
