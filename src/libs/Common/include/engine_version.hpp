#pragma once

namespace storm
{
enum class ENGINE_VERSION
{
    UNKNOWN,
    SEA_DOGS,
    // 1.0 - Will likely never be supported
    PIRATES_OF_THE_CARIBBEAN,
    // 2.0
    CARIBBEAN_TALES,
    // 2.5
    CITY_OF_ABANDONED_SHIPS,
    // 2.8
    TO_EACH_HIS_OWN,
    LATEST,
    // Current, modified version
};

constexpr bool operator<(const ENGINE_VERSION first, const ENGINE_VERSION second) noexcept
{
    return static_cast<int>(first) < static_cast<int>(second);
}
} // namespace storm
