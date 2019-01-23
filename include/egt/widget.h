/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_WIDGET_H
#define EGT_WIDGET_H

/**
 * @file
 * @brief Base class Widget definition.
 */

#include <cairo.h>
#include <cassert>
#include <cstdint>
#include <egt/bitmask.h>
#include <egt/font.h>
#include <egt/geometry.h>
#include <egt/input.h>
#include <egt/object.h>
#include <egt/palette.h>
#include <egt/screen.h>
#include <egt/theme.h>
#include <egt/utils.h>
#include <iosfwd>
#include <memory>
#include <string>
#include <vector>

namespace egt
{
inline namespace v1
{
class Painter;

/**
 * Flags used for various widget properties.
 */
enum class widgetmask : uint32_t
{
    NONE = 0,

    /**
     * Do not draw the background color.
     *
     * The background color is usually drawn by default. A background will
     * not be drawn if this flag is set. If something else is not drawn,
     * instead (like a child widget), this can result in unintended side
     * effects. Not drawing a background is an optimization to reduce
     * unecessary drawing.
     */
    NO_BACKGROUND = (1 << 0),

    /**
     * This is an overlay plane window.
     */
    PLANE_WINDOW = (1 << 1),

    /**
     * This is a window widget.
     */
    WINDOW = (1 << 4),

    /**
     * This is a frame.
     */
    FRAME = (1 << 5),

    /**
     * Grab elated mouse events.
     *
     * For example, if a button is pressed with the eventid::MOUSE_DOWN
     * event, make sure the button gets subsequent mouse events, including
     * the eventid::MOUSE_UP event.
     */
    GRAB_MOUSE = (1 << 6),

    /**
     * Enable transparent background.
     */
    TRANSPARENT_BACKGROUND = (1 << 7),

    /**
     * Default window flags.
     */
    WINDOW_DEFAULT = WINDOW,
};

ENABLE_BITMASK_OPERATORS(widgetmask)

/**
 * Alignment flags.
 */
enum class alignmask : uint32_t
{
    /** No alignment. */
    NONE = 0,
    /**
     * Center alignment is a weak alignment both horizontal and
     * vertical. To break one of those dimensions to another
     * alignment, specify it in addiiton to CENTER.  If both
     * are broken, CENTER has no effect.
     */
    CENTER = (1 << 0),
    /** Horizontal alignment. */
    LEFT = (1 << 1),
    /** Horizontal alignment. */
    RIGHT = (1 << 2),
    /** Vertical alignment. */
    TOP = (1 << 3),
    /** Vertical alignment. */
    BOTTOM = (1 << 4),
    /** Expand only horizontally. */
    EXPAND_HORIZONTAL = (1 << 5),
    /** Expand only vertically. */
    EXPAND_VERTICAL = (1 << 6),
    /** Don't align, expand. */
    EXPAND = EXPAND_HORIZONTAL | EXPAND_VERTICAL,
};

ENABLE_BITMASK_OPERATORS(alignmask)

/**
 * Generic orientation flags.
 */
enum class orientation
{
    HORIZONTAL,
    VERTICAL,
};

class Frame;

/**
 * Base widget class.
 *
 * This is the base class for all widgets. A Widget is a thing, with a basic
 * set of properties.  In this case, it has a bounding rectangle,
 * some flags, and some states - and these properties can be manipulated. A
 * Widget can handle events, draw itself, and more.  However, the specifics
 * of what it means to handle an event or draw the widget is implemented in
 * classes that are derived from this one, like a Button or a Label.
 */
class Widget : public detail::Object
{
public:

    /**
     * Given an item size, and a bounding box, and an alignment parameter,
     * return the rectangle the item box should be move/resized to.
     */
    static Rect align_algorithm(const Size& size, const Rect& bounding,
                                alignmask align, int margin = 0);

    /**
     * This is used for aligning two rectangles in a single widget.  For example,
     * a widget that has both text and an image.  The first rectangle is aligned
     * to the main rectangle.  The second rectangle is aligned relative to the
     * first rectangle.
     */
    static void double_align(const Rect& main,
                             const Size& fsize, alignmask first_align, Rect& first,
                             const Size& ssize, alignmask second_align, Rect& second,
                             int margin = 0);

    /**
     * Construct a widget.
     *
     * @param[in] rect Initial rectangle of the Widget.
     * @param[in] flags Widget flags.
     */
    Widget(const Rect& rect = Rect(),
           widgetmask flags = widgetmask::NONE) noexcept;

    /**
     * Construct a widget.
     *
     * @param[in] parent Parent Frame of the widget.
     * @param[in] rect Initial rectangle of the Widget.
     * @param[in] flags Widget flags.
     */
    Widget(Frame& parent, const Rect& rect = Rect(),
           widgetmask flags = widgetmask::NONE) noexcept;

    /**
     * Draw the widget.
     *
     * @param[in] painter Instance of the Painter for the screen.
     * @param[in] rect The rectangle to draw.
     *
     * To change how a widget is drawn, this function can be overloaded and
     * changed.
     *
     * To optimize drawing, a widget may use the @b rect parameter to limit
     * what needs to be redrawn, which may be smaler than the widget's box(),
     * but will never be outside of the widget's box().
     *
     * Painter will always be supplied in a default state to this function,
     * so there is no need to do any cleanup or state saving inside this
     * draw() function.
     *
     * @warning Normally this should not be called directly and instead the
     * event loop will call this function with an already established
     * Painter when the widget needs to be redrawn.
     *
     */
    virtual void draw(Painter& painter, const Rect& rect) = 0;

    /**
     * Handle an event.
     * Override this function in a derived class to handle events.
     *
     * Only the event ID is passed to this function. To get data associated
     * with the event, you have to call other functions.
     *
     * The default implementation in the Widget class, will dispatch the
     * event to any third party handleres that have been registered. What
     * this means is if you expect other handlers to receive the events
     * then this must be called from derived classes.  Or, manually call
     * Widget::invoke_handlers().
     *
     * @return A return value of non-zero stops the propagation of the event.
     */
    virtual int handle(eventid event);

    /**
     * Resize the widget.
     * Changes the width and height of the widget.
     *
     * @param[in] s The new size of the Widget.
     * @note This will cause a redraw of the widget.
     */
    virtual void resize(const Size& s);

    /**
     * Change the width of the widget.
     * @param[in] w The new width of the widget.
     */
    inline void set_width(int w) { resize(Size(w, h())); }

    /**
     * Change the height of the widget.
     * @param[in] h The new height of the widget.
     */
    void set_height(int h) { resize(Size(w(), h)); }

    /**
     * Move the widget.
     * Changes the x and y position of the widget.
     *
     * @param[in] point The new origin point for the widget relative to its parent.
     * @note This will cause a redraw of the widget.
     */
    virtual void move(const Point& point);

    /**
     * @param[in] x The new origin X value for the widget relative to its parent.
     */
    inline void set_x(int x) { move(Point(x, y())); }

    /**
     * @param[in] y The new origin Y value for the widget relative to its parent.
     */
    inline void set_y(int y) { move(Point(x(), y)); }

    /**
     * Move the widget to the specified center point.
     * Changes the x and y position of the widget relative to the center point.
     *
     * @note This will cause a redraw of the widget.
     */
    virtual void move_to_center(const Point& point);

    /**
     * Change the box of the widget.
     * This is the same as calling move() and resize() at the same time.
     */
    virtual void set_box(const Rect& rect);

    /**
     * Hide the widget.
     *
     * A widget that is not visible will receive no draw() calls.
     *
     * @note This changes the visible() property of the widget.
     */
    virtual void hide();

    /**
     * Show the widget.
     *
     * @note This changes the visible() property of the widget.
     */
    virtual void show();

    /**
     * Return true if the widget is visible.
     */
    virtual bool visible() const;

    /**
     * Return true if the widget is active.
     *
     * The meaning of active is largely up to the derived implementation.
     */
    virtual bool active() const;

    /**
     * Set the active property of the widget.
     */
    virtual void set_active(bool value);

    /**
     * Return the disabled status of the widget.
     *
     * When a widget is disabled, it does not receive events. Also, the
     * color scheme may change when a widget is disabled.
     */
    virtual bool disabled() const;

    /**
     * Set the disabled status of the widget to true.
     */
    virtual void disable();

    /**
     * Set the enable status of the widget to true.
     */
    virtual void enable();

    /**
     * Damage the box() of the widget.
     *
     * @note This is just a utility wrapper around damage(box()) in most cases.
     */
    virtual void damage();

    /**
     * Mark the specified rect as a damaged area.
     *
     * This call will propagate to a top level parent frame.
     *
     * @param rect The rectangle to save for damage.
     */
    virtual void damage(const Rect& rect);

    /**
     * Bounding box for the widget.
     */
    virtual const Rect& box() const;

    /**
     * Get the size of the widget.
     */
    virtual Size size() const;

    /**
     * Get the origin point of the widget.
     */
    virtual Point point() const;

    /**
     * @{
     * Bounding box dimensions.
     */
    inline int w() const { return m_box.w; }
    inline int h() const { return m_box.h; }
    inline int x() const { return m_box.x; }
    inline int y() const { return m_box.y; }
    /** @} */

    /**
     * Get the center point of the widget.
     */
    inline Point center() const { return box().center(); }

    /**
     * Get the widget Palette.
     */
    Palette& palette();

    /**
     * Get the widget Palette.
     */
    const Palette& palette() const;

    /**
     * Set the widget Palette.
     *
     * @param palette The new palette to assign to the widget.
     * @note This will overwrite the entire widget Palette.
     */
    void set_palette(const Palette& palette);

    /**
     * Reset the widget's palette to a default state.
     */
    void reset_palette();

    /**
     * Get a pointer to the parent Frame, or nullptr if none exists.
     */
    Frame* parent();

    /**
     * Get a pointer to the parent Frame, or nullptr if none exists.
     */
    const Frame* parent() const;

    /**
     * Get a pointer to the IScreen instance, using using a parent as
     * necessary.
     *
     * @return An IScreen if available, or nullptr.
     */
    virtual IScreen* screen();

    /**
     * Test if the specified Widget flag(s) is/are set.
     * @param flag Bitmask of flags.
     */
    inline bool is_flag_set(widgetmask flag) const
    {
        return (m_flags & flag) == flag;
    }

    /**
     * Set the specified widget flag(s).
     * @param flag Bitmask of flags.
     */
    inline void flag_set(widgetmask flag) { m_flags |= flag; }

    /**
     * Clear, or unset, the specified widget flag(s).
     * @param flag Bitmask of flags.
     */
    inline void flag_clear(widgetmask flag) { m_flags &= ~flag; }

    /**
     * Align the widget.
     *
     * This will align the widget relative to the box of its parent frame.
     *
     * @param[in] a The alignmask.
     * @param[in] margin Optional margin size used for alignment.
     */
    virtual void set_align(alignmask a, int margin = 0);

    /**
     * Get the alignment of the widget.
     */
    inline alignmask align() const { return m_align; }

    /**
     * Return the alignment margin.
     */
    inline int margin() const { return m_margin; }

    /**
     * Get the name of the widget.
     */
    const std::string& name() const;

    /**
     * Set the name of the widget.
     *
     * @param[in] name Name to set for the widget.
     */
    inline void set_name(const std::string& name) { m_name = name; }

    /**
     * Paint the Widget using Painter.
     *
     * paint() is not part of the normal draw path.  This is a utility
     * function to get the widget to draw its contents to another
     * surface provided with a Painter.
     */
    virtual void paint(Painter& painter);

    /**
     * Draw the widget to a file.
     *
     * @param[in] filename Optional filename to save to.
     */
    virtual void paint_to_file(const std::string& filename = std::string());

#if 0
    virtual shared_cairo_surface_t surface();
#endif

    virtual void dump(std::ostream& out, int level = 0);

    /**
     * Convert a child rectangle to a parent rectangle.
     */
    Rect to_parent(const Rect& r);

    /**
     * Convert a local point to the coordinate system of the screen.
     *
     * In other words, work towards the entire screen so we can get
     * this point relative to the origin of the screen.
     */
    virtual Point to_screen(const Point& p);
    virtual Point to_screen_back(const Point& p);

    /**
     * Convert a screen point to a local point.
     *
     * In other words, walk towards the current widget to we can
     * get a point relative to the widget's own box.
     */
    virtual Point from_screen(const Point& p);
    virtual Point from_screen_back(const Point& p);

    /**
     * Get the widget theme, or the default theme if none is set.
     */
    Theme& theme();

    virtual void on_gain_focus();

    virtual void on_lost_focus();

    virtual bool focus() const { return m_focus; }

    /**
     * Set the widget's theme to a new theme.
     */
    void set_theme(const Theme& theme);

    /**
     * Reset the widget's theme to the default theme.
     */
    void reset_theme();

    void set_boxtype(const Theme::boxtype type);

    virtual ~Widget();

protected:

    virtual bool top_level() const { return false; }

    /**
     * Helper function to draw this widget's box using the appropriate
     * theme.
     */
    void draw_box(Painter& painter, const Rect& rect = Rect());

    virtual void set_parent(Frame* parent)
    {
        // cannot already have a parent
        assert(!m_parent);

        m_parent = parent;
        damage();
    }

    /**
     * Bounding box.
     */
    Rect m_box;

    /**
     * Pointer to this widget's parent.
     *
     * The parent is a Frame, which is capable of managing children.
     */
    Frame* m_parent{nullptr};

private:

    /**
     * When true, the widget is visible.
     */
    bool m_visible{true};

    /**
     * When true, the widget is active.
     *
     * The active state of a widget is usually a momentary state, unlike
     * focus, which exists until focu is changed. For example, when a button
     * is currently being held down, it its implementation may consider this
     * the active state and choose to draw the button diffeerently.
     *
     * This may change how the widget behaves or is draw.
     */
    bool m_active{false};

    /**
     * When true, the widget is disabled.
     *
     * Typically, when a widget is disabled it will not accept input.
     *
     * This may change how the widget behaves or is draw.
     */
    bool m_disabled{false};

    /**
     * Flags for the widget.
     */
    widgetmask m_flags{widgetmask::NONE};

    /**
     * Current palette for the widget.
     *
     * @note This should not be accessed directly.  Always use the accessor
     * functions because this is not set until it is modified.
     */
    std::unique_ptr<Palette> m_palette;

    /**
     * A user defined name for the widget.
     */
    std::string m_name;

    /**
     * Alignment hint for this widget within its parent.
     */
    alignmask m_align{alignmask::NONE};

    /**
     * Alignment margin.
     */
    int m_margin{0};

    /**
     * Focus state.
     */
    bool m_focus{false};

    Theme::boxtype m_boxtype{Theme::boxtype::none};

    /**
     * Current theme for the widget.
     *
     * @note This should not be accessed directly.  Always use the accessor
     * functions because this is not set until it is modified.
     */
    std::unique_ptr<Theme> m_theme;

    Widget(const Widget&) = delete;
    Widget& operator=(const Widget&) = delete;

    friend class Frame;
};

/**
 * A widget that has text and properties associated with text.
 *
 * This is not meant to be used directly as it does not implement at least a
 * draw() method.
 */
class TextWidget : public Widget
{
public:

    explicit TextWidget(const std::string& text = std::string(),
                        const Rect& rect = Rect(),
                        alignmask align = alignmask::CENTER,
                        const Font& font = Font(),
                        widgetmask flags = widgetmask::NONE) noexcept;

    /**
     * Set the text of the label.
     */
    virtual void set_text(const std::string& str);

    /**
     * Clear the text value.
     */
    virtual void clear();

    /**
     * Get the text of the Label.
     */
    virtual const std::string& text() const { return m_text; }

    /**
     * Set the alignment of the Label.
     */
    void set_text_align(alignmask align)
    {
        if (m_text_align != align)
        {
            m_text_align = align;
            damage();
        }
    }

    /**
     * Get the widget Font.
     */
    Font& font()
    {
        if (!m_font.get())
            m_font.reset(new Font(global_font()));

        return *m_font.get();
    }

    /**
     * Get the widget Font.
     */
    const Font& font() const
    {
        if (m_font.get())
            return *m_font.get();

        return global_font();
    }

    /**
     * Set the widget Font.
     *
     * @note This will overwrite the entire widget Font.
     */
    void set_font(const Font& font)
    {
        m_font.reset(new Font(font));
        damage();
    }

    virtual ~TextWidget()
    {}

    /**
     * Given a Font, text, and a target Size, scale the font size so that
     * the text will will fit and return the new Font.
     */
    static Font scale_font(const Size& target, const std::string& text, const Font& font);

protected:

    Size text_size();

    alignmask m_text_align{alignmask::CENTER};
    std::string m_text;

private:
    std::unique_ptr<Font> m_font;
};

namespace experimental
{
class ScrollWheel : public Widget
{
public:
    explicit ScrollWheel(const Rect& rect = Rect());

    virtual int handle(eventid event) override;

    virtual void draw(Painter& painter, const Rect& rect) override;

    int position() const
    {
        return m_pos;
    }

    inline void position(int pos)
    {
        if (pos < (int)m_values.size())
        {
            m_pos = pos;
            damage();
        }
    }

    void values(const std::vector<std::string>& v)
    {
        m_values = v;
    }

    virtual ~ScrollWheel()
    {}

protected:
    std::vector<std::string> m_values;

    int m_pos{0};
    int m_moving_x{0};
    int m_start_pos{0};
};

}

}
}

#endif
