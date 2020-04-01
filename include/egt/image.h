/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_IMAGE_H
#define EGT_IMAGE_H

/**
 * @file
 * @brief Working with images.
 */

#include <cairo.h>
#include <egt/detail/meta.h>
#include <egt/geometry.h>
#include <egt/painter.h>
#include <egt/serialize.h>
#include <map>
#include <string>

namespace egt
{
inline namespace v1
{

/**
 * Raster image resource used for drawing or displaying.
 *
 * This class by default shares the internal surface pointer with anything
 * else using the surface.  To force this class to keep its own copy, call
 * the copy() function.
 *
 * @ingroup media
 */
class EGT_API Image
{
public:

    /**
     * Construct a raster image from a URI with an optional scale.
     *
     * @code{.cpp}
     * Image image("file:image.png");
     * @endcode
     *
     * @param uri Resource path. @see @ref resources
     * @param scale Scale of the image, with 1.0 being 100%.
     */
    // cppcheck-suppress noExplicitConstructor
    // NOLINTNEXTLINE(hicpp-explicit-conversions, google-explicit-constructor)
    Image(const std::string& uri = {}, float scale = 1.0);

    /**
     * Construct a raster image from a URI with an optional scale.
     *
     * @param uri Resource path. @see @ref resources
     * @param hscale Horizontal scale of the image, with 1.0 being 100%.
     * @param vscale Vertical scale of the image, with 1.0 being 100%.
     */
    Image(const std::string& uri, float hscale, float vscale);

    /**
     * Construct a raster image from memory.
     *
     * @param data Pointer to the data.
     * @param len Length of the data.
     */
    Image(const unsigned char* data, size_t len);

    /**
     * Load a new source image with an optional scale.
     *
     * @param uri Resource path. @see @ref resources
     * @param hscale Horizontal scale of the image, with 1.0 being 100%.
     * @param vscale Vertical scale of the image, with 1.0 being 100%.
     */
    void load(const std::string& uri, float hscale = 1.0, float vscale = 1.0);

    /**
     * @param surface A pre-existing surface.
     *
     * This take a reference to the passed in image.
     *
     * @todo This is a broken API.  It's different from the constructor below.
     * Need consistency.
     */
    // cppcheck-suppress noExplicitConstructor
    // NOLINTNEXTLINE(hicpp-explicit-conversions, google-explicit-constructor)
    Image(shared_cairo_surface_t surface);

    /**
     * @param surface A pre-existing surface.
     *
     * This will not own the passed in pointer or surface, and instead will make
     * a copy of the surface.
     */
    // cppcheck-suppress noExplicitConstructor
    // NOLINTNEXTLINE(hicpp-explicit-conversions, google-explicit-constructor)
    Image(cairo_surface_t* surface);

    /**
     * Scale the image.
     *
     * Change the scale of the image, similar to calling Image::resize().
     *
     * This scales relative to the original size of the image.  Not the result
     * of any subsequent Image::resize() or Image::scale() call.
     *
     * @param hscale Horizontal scale of the image, with 1.0 being 100%.
     * @param vscale Vertical scale of the image, with 1.0 being 100%.
     * @param approximate Approximate the scale to increase image cache
     *            hit efficiency.
     */
    void scale(float hscale, float vscale, bool approximate = false);

    /**
     * Scale the image.
     *
     * @see scale()
     */
    inline void scale(float scale, bool approximate = false)
    {
        this->scale(scale, scale, approximate);
    }

    /**
     * Resize the image to the specified absolute size.
     *
     * @param size The new size of the image.
     */
    void resize(const Size& size)
    {
        if (this->size() != size)
        {
            float hs = static_cast<float>(size.width()) / static_cast<float>(m_orig_size.width());
            float vs = static_cast<float>(size.height()) / static_cast<float>(m_orig_size.height());
            scale(hs, vs);
        }
    }

    /**
     * Get the horizontal scale value.
     */
    float hscale() const { return m_hscale; }

    /**
     * Get the vertical scale value.
     */
    float vscale() const { return m_vscale; }

    /**
     * Get the absolute size of the image.
     */
    Size size() const
    {
        if (empty())
            return {};

        return {cairo_image_surface_get_width(surface().get()),
                cairo_image_surface_get_height(surface().get())};
    }

    DefaultDim width() const
    {
        return size().width();
    }

    DefaultDim height() const
    {
        return size().height();
    }

    /**
     * Returns true if no internal surface is set.
     */
    bool empty() const
    {
        return !surface();
    }

    /**
     * Get a reference to the internal image surface.
     */
    shared_cairo_surface_t surface() const
    {
        if (m_surface_local.get())
            return m_surface_local;
        return m_surface;
    }

    /**
     * Get the original size of the image before any Image::resize() or
     * Image::scale() calls.
     */
    Size size_orig() const { return m_orig_size; }

    /**
     * This function must be called any time the surface is going to be
     * modified.  It's safe to call this function when not necessary, and in
     * any event it will ensure this Image contains a unique copy of the
     * surface.
     *
     * @todo Tricky API.
     */
    void copy();

    std::string uri() const
    {
        return m_uri;
    }

    Image crop(const RectF& rect);

    Image crop(const Rect& rect)
    {
        return crop(RectF(rect.x(), rect.y(), rect.width(), rect.height()));
    }

    /**
    * Serialize to the specified serializer.
    */
    void serialize(const std::string& name, Serializer& serializer) const;

    /**
     * Deserialized property.
     */
    void deserialize(const std::string& name, const std::string& value,
                     const Serializer::Attributes& attrs);

protected:

    /// If a URI was used, the URI.
    std::string m_uri;

    /// Horizontal scale value, compared to original.
    float m_hscale{1.0};

    /// Vertical scale value, compared to original.
    float m_vscale{1.0};

    /// Shared surface pointer.
    shared_cairo_surface_t m_surface;

    /// Local surface pointer.
    shared_cairo_surface_t m_surface_local;

    /// Original image size.
    Size m_orig_size;
};

static_assert(detail::rule_of_5<Image>(), "must fulfill rule of 5");

}
}

#endif
