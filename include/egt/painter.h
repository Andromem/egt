/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_PAINTER_H
#define EGT_PAINTER_H

#include <cairo.h>
#include <egt/color.h>
#include <egt/font.h>
#include <egt/geometry.h>
#include <egt/types.h>
#include <egt/utils.h>
#include <string>

namespace egt
{
inline namespace v1
{

class Image;

/**
 * @brief Drawing interface for 2D graphics.
 *
 * This is the interface for all 2D drawing primitives.
 */
class Painter : public detail::noncopyable
{
public:

    /**
     * Scoped save() and restore() for a Painter.
     *
     * You are encouraged to use this instead of manually calling
     * Painter::save() and Painter::restore().
     */
    struct AutoSaveRestore
    {
        explicit AutoSaveRestore(Painter& painter)
            : m_painter(painter)
        {
            m_painter.save();
        }

        ~AutoSaveRestore()
        {
            m_painter.restore();
        }

        Painter& m_painter;
    };

    /**
     * @todo Painter needs to come from the Screen. This constructor should
     * be hidden and you should have to get a custom version of it from the
     * Screen for drawing. A default software/GPU painter can be created,
     * otherwise for something like X11 we should be using X11 to paint.
     *
     * Or maybe we leave this as it is and put the drawing abstraction under
     * cairo...
     */
    explicit Painter(shared_cairo_t cr);

    /**
     * Save the state of the current context.
     *
     * @see AutoSaveRestore
     */
    virtual void save();

    /**
     * Restore the previous saved state of the current context.
     *
     * @see AutoSaveRestore
     */
    virtual void restore();

    /**
     * Set the current color.
     */
    virtual Painter& set(const Color& color);

    /**
     * Set the active font.
     */
    virtual Painter& set(const Font& font);

    /**
     * Set the current line width.
     *
     * @param[in] width Line width.
     */
    virtual Painter& set_line_width(float width);

    /**
     * Move to a point.
     *
     * @param[in] point The point.
     */
    template<class T>
    Painter& draw(const PointType<T, compatible::normal>& point)
    {
        cairo_move_to(m_cr.get(), point.x, point.y);

        return *this;
    }

    /**
     * Create a line from the start point to the end point.
     *
     * @param[in] start The point.
     * @param[in] end The point.
     */
    template<class T>
    Painter& draw(const T& start, const T& end)
    {
        cairo_move_to(m_cr.get(), start.x, start.y);
        cairo_line_to(m_cr.get(), end.x, end.y);

        return *this;
    }

    /**
     * Create a rectangle.
     *
     * @param[in] rect The rectangle.
     */
    template<class T>
    Painter& draw(const RectType<T>& rect)
    {
        if (rect.empty())
            return *this;

        cairo_rectangle(m_cr.get(),
                        rect.x,
                        rect.y,
                        rect.w,
                        rect.h);

        return *this;
    }

    /**
     * Create an arc.
     *
     * @param[in] arc The arc.
     */
    template<class T>
    Painter& draw(const ArcType<T>& arc)
    {
        if (arc.empty())
            return *this;

        cairo_arc(m_cr.get(), arc.center.x, arc.center.y,
                  arc.radius, arc.angle1, arc.angle2);

        return *this;
    }

    /**
     * Draw an image surface at the specified point.
     */
    virtual Painter& draw(const Image& image);

    /**
     * @param[in] rect The source rect to copy.
     * @param[in] image The image surface to draw.
     */
    virtual Painter& draw(const Rect& rect,
                          const Image& image);

    /**
     * Draw text inside the specified rectangle.
     */
    virtual Painter& draw(const std::string& str, bool difference = false);

    virtual Painter& clip();

    virtual Painter& fill();

    virtual Painter& paint();

    virtual Painter& stroke();

    virtual Size text_size(const std::string& text);

    virtual Size font_size(const std::string& text);

    /**
     * Get the current underlying context the painter is using.
     */
    inline shared_cairo_t context() const
    {
        return m_cr;
    }

    /**
     * Get a Size from a surface.
     */
    static inline Size surface_to_size(shared_cairo_surface_t surface)
    {
        return Size(cairo_image_surface_get_width(surface.get()),
                    cairo_image_surface_get_height(surface.get()));
    }

    virtual ~Painter() = default;

protected:

    /**
     * Cairo context.
     */
    shared_cairo_t m_cr;

private:

    Painter() = delete;
};

}
}

#endif
