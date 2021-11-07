#pragma once
#ifdef STORM_WATERMARK_FILE
#include STORM_WATERMARK_FILE
#define STORM_STRINGIFY_EXPAND_(x) #x
#define STORM_STRINGIFY_(x) STORM_STRINGIFY_EXPAND_(x)
#define STORM_BUILD_WATERMARK_STRING STORM_STRINGIFY_(STORM_BUILD_WATERMARK)
#else
#error "Watermark file is not present. Check build configuration"
#endif
