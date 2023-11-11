#include "storm/renderer/image_loader.hpp"

#include <FreeImage.h>

namespace storm::renderer
{

namespace {

class FreeImage final {
  public:
    FreeImage();
    ~FreeImage();
};

FreeImage::FreeImage()
{
    FreeImage_Initialise(false);
}

FreeImage::~FreeImage()
{
    FreeImage_DeInitialise();
}

} // namespace

class ImageLoader::Impl
{
  public:
    Impl();

    std::shared_ptr<FreeImage> freeImage_;

    static std::weak_ptr<FreeImage> FreeImageInstance;

    static std::shared_ptr<FreeImage> getGlobalFreeImageInstance();

    friend ImageLoader;
};

std::weak_ptr<FreeImage> ImageLoader::Impl::FreeImageInstance{};

ImageLoader::ImageLoader()
    : impl_(std::make_unique<Impl>())
{

}

ImageLoader::~ImageLoader() = default;

std::unique_ptr<Image> ImageLoader::LoadImageFromFile(const istring &path)
{
    const auto image_type = FreeImage_GetFileType(path.c_str(), 0);

    if (image_type != FIF_UNKNOWN)
    {
        FIBITMAP *image = FreeImage_Load(image_type, path.c_str(), 0);
        auto result = std::make_unique<Image>();
        result->width = FreeImage_GetWidth(image);
        result->height = FreeImage_GetHeight(image);
        result->pitch = FreeImage_GetPitch(image);
        result->type = FreeImage_GetImageType(image);
        result->bpp = FreeImage_GetBPP(image);
        result->data = FreeImage_GetBits(image);
        return result;
    }
    else {
        return {};
    }
}

void ImageLoader::CopyImageToBuffer(const Image& image, uint8_t *destination)
{
    if (image.bpp == 24) {
        for(size_t y = image.height; y > 0; y--) {
            BYTE *pixel = image.data + (y - 1) * image.pitch;
            for(size_t x = 0; x < image.width; x++) {https://www.piratesahoy.net/threads/build-14-gamma-version-last-update-6th-april-2023.33573/
                destination[0] = pixel[FI_RGBA_BLUE];
                destination[1] = pixel[FI_RGBA_GREEN];
                destination[2] = pixel[FI_RGBA_RED];
                destination[3] = 255;
                destination += 4;
                pixel += 3;
            }
        }
    }
    else {
        for(size_t y = image.height; y > 0; y--) {
            BYTE *pixel = image.data + (y - 1) * image.pitch;
            for(size_t x = 0; x < image.width; x++) {
                destination[0] = pixel[FI_RGBA_BLUE];
                destination[1] = pixel[FI_RGBA_GREEN];
                destination[2] = pixel[FI_RGBA_RED];
                destination[3] = pixel[FI_RGBA_ALPHA];
                destination += 4;
                pixel += 4;
            }
        }
    }
}

ImageLoader::Impl::Impl()
{
    freeImage_ = getGlobalFreeImageInstance();
}

std::shared_ptr<FreeImage> ImageLoader::Impl::getGlobalFreeImageInstance()
{
    auto free_image = FreeImageInstance.lock();
    if (!free_image) {
        free_image = std::make_shared<FreeImage>();
        FreeImageInstance = free_image;
    }
    return free_image;
}

} // namespace storm::renderer
