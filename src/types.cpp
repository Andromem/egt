/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "egt/types.h"
#include <cairo.h>
#include <iostream>
#include <map>

#ifdef HAVE_LIBDRM
#include <drm_fourcc.h>
#endif

#ifdef __GNUG__
#include <cstdlib>
#include <memory>
#include <cxxabi.h>
#endif

using namespace std;

namespace egt
{
inline namespace v1
{
std::ostream& operator<<(std::ostream& os, const PixelFormat& format)
{
    static std::map<PixelFormat, std::string> strings;
    if (strings.empty())
    {
#define MAPITEM(p) strings[p] = #p
        MAPITEM(PixelFormat::invalid);
        MAPITEM(PixelFormat::rgb565);
        MAPITEM(PixelFormat::argb8888);
        MAPITEM(PixelFormat::xrgb8888);
        MAPITEM(PixelFormat::yuyv);
        MAPITEM(PixelFormat::nv21);
        MAPITEM(PixelFormat::yuv420);
        MAPITEM(PixelFormat::yvyu);
        MAPITEM(PixelFormat::nv61);
        MAPITEM(PixelFormat::yuy2);
        MAPITEM(PixelFormat::uyvy);
#undef MAPITEM
    }

    os << strings[format];
    return os;
}

namespace detail
{
static const map<PixelFormat, uint32_t> drm_formats =
{
#ifdef HAVE_LIBDRM
    {PixelFormat::rgb565, DRM_FORMAT_RGB565},
    {PixelFormat::argb8888, DRM_FORMAT_ARGB8888},
    {PixelFormat::xrgb8888, DRM_FORMAT_XRGB8888},
    {PixelFormat::yuyv, DRM_FORMAT_YUYV},
    {PixelFormat::yuv420, DRM_FORMAT_YUV420},
    {PixelFormat::nv21, DRM_FORMAT_NV21},
    {PixelFormat::yvyu, DRM_FORMAT_YVYU},
    {PixelFormat::nv61, DRM_FORMAT_NV61},
    {PixelFormat::yuy2, DRM_FORMAT_YUYV},
    {PixelFormat::uyvy, DRM_FORMAT_UYVY},
#else
    {PixelFormat::rgb565, 0},
    {PixelFormat::argb8888, 1},
    {PixelFormat::xrgb8888, 2},
    {PixelFormat::yuyv, 3},
    {PixelFormat::nv21, 4},
    {PixelFormat::yuv420, 5},
    {PixelFormat::yvyu, 6},
    {PixelFormat::nv61, 7},
    {PixelFormat::yuy2, 8},
    {PixelFormat::uyvy, 9},
#endif
};

static const map<PixelFormat, cairo_format_t> cairo_formats =
{
    {PixelFormat::rgb565, CAIRO_FORMAT_RGB16_565},
    {PixelFormat::argb8888, CAIRO_FORMAT_ARGB32},
    {PixelFormat::xrgb8888, CAIRO_FORMAT_RGB24},
};

cairo_format_t cairo_format(PixelFormat format)
{
    auto i = cairo_formats.find(format);
    if (i != cairo_formats.end())
        return i->second;

    return CAIRO_FORMAT_INVALID;
}

uint32_t drm_format(PixelFormat format)
{
    auto i = drm_formats.find(format);
    if (i != drm_formats.end())
        return i->second;

    return 0;
}

PixelFormat egt_format(uint32_t format)
{
    for (auto& i : drm_formats)
        // cppcheck-suppress useStlAlgorithm
        if (i.second == format)
            return i.first;

    return PixelFormat::invalid;
}

PixelFormat egt_format(cairo_format_t format)
{
    for (auto& i : cairo_formats)
        // cppcheck-suppress useStlAlgorithm
        if (i.second == format)
            return i.first;

    return PixelFormat::invalid;
}

std::string gstreamer_format(PixelFormat format)
{
    static const std::map<PixelFormat, std::string> formats =
    {
        {PixelFormat::argb8888, "BGRx"},
        {PixelFormat::xrgb8888, "BGRx"},
        {PixelFormat::rgb565, "RGB16"},
        {PixelFormat::yuv420, "I420"},
        {PixelFormat::yuyv, "YUY2"},
        {PixelFormat::nv21, "NV21"},
        {PixelFormat::yvyu, "YVYU"},
        {PixelFormat::nv61, "NV61"},
        {PixelFormat::yuy2, "YUY2"},
        {PixelFormat::uyvy, "UYVY"},
    };

    const auto i = formats.find(format);
    if (i != formats.end())
        return i->second;

    return {};
}

}

}
}
