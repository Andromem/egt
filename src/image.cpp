/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/image.h"
#include "egt/imagecache.h"
#include "egt/canvas.h"

using namespace std;

namespace egt
{

Image::Image(const std::string& filename,
             double hscale, double vscale)
    : m_filename(filename)
{
    if (!filename.empty())
    {
        m_surface = detail::image_cache().get(filename, hscale, vscale);
        assert(cairo_surface_status(m_surface.get()) == CAIRO_STATUS_SUCCESS);

        m_orig_size = Size(cairo_image_surface_get_width(m_surface.get()),
                           cairo_image_surface_get_height(m_surface.get()));
    }
}

Image::Image(shared_cairo_surface_t surface)
    : m_surface(surface)
{
    assert(cairo_surface_status(m_surface.get()) == CAIRO_STATUS_SUCCESS);

    m_orig_size = Size(cairo_image_surface_get_width(m_surface.get()),
                       cairo_image_surface_get_height(m_surface.get()));
}

Image::Image(cairo_surface_t* surface)
    : m_surface(cairo_image_surface_create(cairo_image_surface_get_format(surface),
                                           cairo_image_surface_get_width(surface),
                                           cairo_image_surface_get_height(surface)),
                cairo_surface_destroy)
{
    assert(cairo_surface_status(m_surface.get()) == CAIRO_STATUS_SUCCESS);

    m_orig_size = Size(cairo_image_surface_get_width(m_surface.get()),
                       cairo_image_surface_get_height(m_surface.get()));
}

void Image::scale(double hscale, double vscale, bool approximate)
{
    if (m_filename.empty())
        return;

    if (m_hscale != hscale || m_vscale != vscale)
    {
        m_surface = detail::image_cache().get(m_filename, hscale, vscale, approximate);
        m_hscale = hscale;
        m_vscale = vscale;
    }
}

void Image::copy()
{
    if (!m_surface_local.get())
    {
        auto canvas = Canvas(surface());
        m_surface_local = canvas.surface();
    }
}

Image::~Image()
{}

}
