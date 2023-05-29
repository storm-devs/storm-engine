#pragma once

#include <istring.hpp>

#include "image.hpp"

#include <memory>

namespace storm::renderer
{

class ImageLoader final
{
  public:
    ImageLoader();
    ~ImageLoader();

    std::unique_ptr<Image> LoadImageFromFile(const istring &path);

    static void CopyImageToBuffer(const Image& image, uint8_t *destination);

  private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace storm::renderer
