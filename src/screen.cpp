/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "egt/color.h"
#include "egt/palette.h"
#include "egt/screen.h"
#include "egt/utils.h"
#include <cassert>
#ifdef HAVE_LIBDRM
#include <drm_fourcc.h>
#endif
#include <map>

using namespace std;

namespace egt
{
inline namespace v1
{
std::ostream& operator<<(std::ostream& os, const pixel_format& format)
{
    static std::map<pixel_format, std::string> strings;
    if (strings.empty())
    {
#define MAPITEM(p) strings[p] = #p
        MAPITEM(pixel_format::invalid);
        MAPITEM(pixel_format::rgb565);
        MAPITEM(pixel_format::argb8888);
        MAPITEM(pixel_format::yuyv);
        MAPITEM(pixel_format::nv21);
        MAPITEM(pixel_format::yuv420);
#undef MAPITEM
    }

    os << strings[format];
    return os;
}

namespace detail
{
static const map<pixel_format, uint32_t> drm_formats =
{
#ifdef HAVE_LIBDRM
    {pixel_format::rgb565, DRM_FORMAT_RGB565},
    {pixel_format::argb8888, DRM_FORMAT_ARGB8888},
    {pixel_format::xrgb8888, DRM_FORMAT_XRGB8888},
    {pixel_format::yuyv, DRM_FORMAT_YUYV},
    {pixel_format::yuv420, DRM_FORMAT_YUV420},
#else
    {pixel_format::rgb565, 0},
    {pixel_format::argb8888, 1},
    {pixel_format::xrgb8888, 2},
    {pixel_format::yuyv, 3},
    {pixel_format::yuv420, 4},
#endif
};

static const map<pixel_format, cairo_format_t> cairo_formats =
{
    {pixel_format::rgb565, CAIRO_FORMAT_RGB16_565},
    {pixel_format::argb8888, CAIRO_FORMAT_ARGB32},
    {pixel_format::xrgb8888, CAIRO_FORMAT_RGB24},
};

cairo_format_t cairo_format(pixel_format format)
{
    auto i = cairo_formats.find(format);
    if (i != cairo_formats.end())
        return i->second;

    return CAIRO_FORMAT_INVALID;
}

uint32_t drm_format(pixel_format format)
{
    auto i = drm_formats.find(format);
    if (i != drm_formats.end())
        return i->second;

    return 0;
}

pixel_format egt_format(uint32_t format)
{
    for (auto& i : drm_formats)
        // cppcheck-suppress useStlAlgorithm
        if (i.second == format)
            return i.first;

    return pixel_format::invalid;
}

pixel_format egt_format(cairo_format_t format)
{
    for (auto& i : cairo_formats)
        // cppcheck-suppress useStlAlgorithm
        if (i.second == format)
            return i.first;

    return pixel_format::invalid;
}
}

static Screen* the_screen = nullptr;

Screen*& main_screen()
{
    return the_screen;
}

Screen::Screen()
{
}

void Screen::flip(const damage_array& damage)
{
    static int envset = -1;
    if (envset < 0)
        envset = !!getenv("EGT_GREENSCREEN");

    if (!damage.empty() && index() < m_buffers.size())
    {
        damage_array olddamage = m_buffers[index()].damage;

        // save the damage to all buffers
        for (auto& b : m_buffers)
            for (const auto& d : damage)
                b.add_damage(d);

        DisplayBuffer& buffer = m_buffers[index()];

        if (envset)
            copy_to_buffer_greenscreen(buffer, olddamage);
        else
            copy_to_buffer(buffer);

        // delete all damage from current buffer
        buffer.damage.clear();
        schedule_flip();
    }
}

/**
 * @todo greenscreen is broken - does not cover all cases and getting it to
 * work with flipping is difficult.  Should consider going to a single
 * buffer for greenscreen.
 */
void Screen::copy_to_buffer_greenscreen(DisplayBuffer& buffer,
                                        const damage_array& olddamage)
{
    cairo_set_source_surface(buffer.cr.get(), m_surface.get(), 0, 0);
    cairo_set_operator(buffer.cr.get(), CAIRO_OPERATOR_SOURCE);

    for (const auto& d : buffer.damage)
        cairo_rectangle(buffer.cr.get(), d.x, d.y, d.w, d.h);

    cairo_fill(buffer.cr.get());

    Color color = Palette::green;
    cairo_set_source_rgb(buffer.cr.get(), color.redf(),
                         color.greenf(), color.bluef());
    cairo_set_line_width(buffer.cr.get(), 4.0);

    for (const auto& d : buffer.damage)
        if (find(olddamage.begin(), olddamage.end(), d) != olddamage.end())
            cairo_rectangle(buffer.cr.get(), d.x, d.y, d.w, d.h);

    cairo_stroke(buffer.cr.get());

    cairo_surface_flush(buffer.surface.get());
}

void Screen::copy_to_buffer(DisplayBuffer& buffer)
{
    cairo_set_source_surface(buffer.cr.get(), m_surface.get(), 0, 0);
    cairo_set_operator(buffer.cr.get(), CAIRO_OPERATOR_SOURCE);

    for (const auto& rect : buffer.damage)
        cairo_rectangle(buffer.cr.get(), rect.x, rect.y, rect.w, rect.h);

    cairo_fill(buffer.cr.get());
    cairo_surface_flush(buffer.surface.get());
}

Screen::~Screen()
{
}

void Screen::damage_algorithm(Screen::damage_array& damage, const Rect& rect)
{
    if (rect.empty())
        return;

    // work backwards for a stronger hit chance for existing rectangles
    for (auto i = damage.rbegin(); i != damage.rend(); ++i)
    {
        // exact rectangle already exists; done
        if (*i == rect)
            return;

        // if this rectangle intersects an existing rectangle, then merge
        // the rectangles and re-add the super rectangle
        if (Rect::intersect(*i, rect))
        {
            Rect super(Rect::merge(*i, rect));
            damage.erase(std::next(i).base());
            Screen::damage_algorithm(damage, super);
            return;
        }
    }

    // if we get here, no intersect found so add it
    damage.emplace_back(rect);
}

void Screen::init(void** ptr, uint32_t count, int w, int h, pixel_format format)
{
    m_size = Size(w, h);

    cairo_format_t f = detail::cairo_format(format);
    if (f == CAIRO_FORMAT_INVALID)
        f = CAIRO_FORMAT_ARGB32;

    m_buffers.clear();

    for (uint32_t x = 0; x < count; x++)
    {
        DisplayBuffer buffer;
        buffer.surface = shared_cairo_surface_t(
                             cairo_image_surface_create_for_data((unsigned char*)ptr[x],
                                     f,
                                     w, h,
                                     cairo_format_stride_for_width(f, w)),
                             cairo_surface_destroy);
        assert(buffer.surface);

        buffer.cr = shared_cairo_t(cairo_create(buffer.surface.get()), cairo_destroy);
        assert(buffer.cr);

        buffer.damage.emplace_back(0, 0, w, h);

        m_buffers.emplace_back(buffer);
    }

    m_surface = shared_cairo_surface_t(cairo_image_surface_create(f, w, h),
                                       cairo_surface_destroy);

    assert(m_surface.get());

    m_cr = shared_cairo_t(cairo_create(m_surface.get()), cairo_destroy);
    assert(m_cr);

    cairo_font_options_t* cfo = cairo_font_options_create();
    cairo_font_options_set_antialias(cfo, CAIRO_ANTIALIAS_FAST);
    cairo_font_options_set_hint_style(cfo, CAIRO_HINT_STYLE_SLIGHT);
    cairo_set_font_options(m_cr.get(), cfo);
    cairo_font_options_destroy(cfo);
    cairo_set_antialias(m_cr.get(), CAIRO_ANTIALIAS_FAST);

    if (!the_screen)
        the_screen = this;
}

}
}
