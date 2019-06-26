/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/canvas.h"
#include "egt/detail/imagecache.h"
#include "egt/detail/svg.h"
#include <librsvg/rsvg.h>

namespace egt
{
inline namespace v1
{
namespace detail
{

shared_cairo_surface_t load_svg(const std::string& filename,
                                const SizeF& size,
                                const std::string& id)
{
    GError* error = nullptr;
    std::shared_ptr<RsvgHandle> rsvg(rsvg_handle_new_from_file(resolve_file_path(filename).c_str(), &error),
    [](RsvgHandle * r) { g_object_unref(r); });

    if (!error)
        throw std::runtime_error("unable to load file: " + filename);

    RsvgDimensionData dim;
    rsvg_handle_get_dimensions(rsvg.get(), &dim);

    auto s = size;
    if (s.empty())
        s = SizeF(dim.width, dim.height);

    Canvas canvas(s);
    auto cr = canvas.context();

    auto scaled = s / SizeF(dim.width, dim.height);

    /* Scale *before* setting the source surface (1) */
    cairo_scale(cr.get(),
                scaled.width,
                scaled.height);

    /* To avoid getting the edge pixels blended with 0 alpha, which would
     * occur with the default EXTEND_NONE. Use EXTEND_PAD for 1.2 or newer (2)
     */
    cairo_pattern_set_extend(cairo_get_source(cr.get()), CAIRO_EXTEND_PAD);

    /* Replace the destination with the source instead of overlaying */
    cairo_set_operator(cr.get(), CAIRO_OPERATOR_SOURCE);

    if (id.empty())
        rsvg_handle_render_cairo(rsvg.get(), canvas.context().get());
    else
        rsvg_handle_render_cairo_sub(rsvg.get(), canvas.context().get(), id.c_str());

    return canvas.surface();
}

}
}
}
