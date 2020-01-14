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
#include <egt/detail/flags.h>
#include <egt/detail/meta.h>
#include <egt/detail/object.h>
#include <egt/font.h>
#include <egt/geometry.h>
#include <egt/palette.h>
#include <egt/theme.h>
#include <egt/widgetflags.h>
#include <iosfwd>
#include <memory>
#include <string>
#include <vector>

namespace egt
{
inline namespace v1
{

namespace detail
{
class Serializer;
}

class Painter;
class Frame;
class Screen;

/**
 * Base Widget class.
 *
 * This is the base class for all widgets. A Widget is a thing, a UI component,
 * with a basic set of properties.  In this case, it has a bounding rectangle,
 * some flags, and some states - and these properties can be manipulated. A
 * Widget can handle events, draw itself, and more.  However, the specifics
 * of what it means to handle an event or draw the Widget is implemented in
 * classes that are derived from this one, like a Button or a Label.
 */
class EGT_API Widget : public detail::Object, public detail::NonCopyable
{
public:

    /**
     * Common flags used for various widget properties.
     */
    enum class Flag : uint32_t
    {
        /**
         * This is an overlay plane window.
         */
        plane_window = detail::bit(0),

        /**
         * This is a window widget.
         */
        window = detail::bit(1),

        /**
         * This is a frame.
         */
        frame = detail::bit(2),

        /**
         * When set, the widget is disabled.
         *
         * Typically, when a widget is disabled it will not accept input.
         *
         * This may change how the widget behaves or is drawn.
         */
        disabled = detail::bit(3),

        /**
         * When set, the widget will not receive input events.
         */
        readonly = detail::bit(4),

        /**
         * When true, the widget is active.
         *
         * The active state of a widget is usually a momentary state, unlike
         * focus, which exists until focus is changed. For example, when a button
         * is currently being held down, it its implementation may consider this
         * the active state and choose to draw the button differently.
         *
         * This may change how the widget behaves or is draw.
         */
        active = detail::bit(5),

        /**
         * When set, the widget is not visible.
         */
        invisible = detail::bit(6),

        /**
         * Grab related mouse events.
         *
         * For example, if a button is pressed with the EventId::MOUSE_DOWN
         * event, make sure the button gets subsequent mouse events, including
         * the EventId::MOUSE_UP event.
         */
        grab_mouse = detail::bit(7),

        /**
         * Don't clip the child to drawing only in its box.
         *
         * Use this with caution, it's probably not what you want.
         */
        no_clip = detail::bit(8),

        /**
         * Tell any parent not to perform layout on this widget.
         */
        no_layout = detail::bit(9),

        /**
         * Do not automatically resize a widget to meet the minimal size hint.
         */
        no_autoresize = detail::bit(10),

        /**
         * Is the widget in a checked state.
         */
        checked = detail::bit(11),
    };

    using Flags = detail::Flags<Widget::Flag>;

    /**
     * @param[in] rect Initial rectangle of the Widget.
     * @param[in] flags Widget flags.
     */
    explicit Widget(const Rect& rect = {},
                    const Widget::Flags& flags = {}) noexcept;

    /**
     * @param[in] parent Parent Frame of the Widget.
     * @param[in] rect Initial rectangle of the Widget.
     * @param[in] flags Widget flags.
     */
    explicit Widget(Frame& parent, const Rect& rect = {},
                    const Widget::Flags& flags = {}) noexcept;

    Widget(Widget&& rhs) noexcept;
    Widget& operator=(Widget&& rhs) noexcept;

    /**
     * Draw the widget.
     *
     * To change how a widget is drawn, this function can be overloaded and
     * changed in a derived class, or it can be changed dynamically with the
     * Theme.
     *
     * To optimize drawing, a Widget may use the Rect parameter to limit
     * what needs to be redrawn, which may be smaller than the widget's box(),
     * but will never be outside of the widget's box().
     *
     * Painter will always be supplied in a default state to this function,
     * so there is no need to do any cleanup or state saving inside this
     * draw() function.
     *
     * @warning Normally this should not be called directly and instead the
     * EventLoop will call this function with an already established
     * Painter when the Widget needs to be drawn.
     *
     * @see @subpage colors and @subpage draw
     *
     * @param[in] painter Instance of the Painter for the Screen.
     * @param[in] rect The rectangle to draw.
     *
     */
    virtual void draw(Painter& painter, const Rect& rect) = 0;

    /**
     * Handle an event.
     * Override this function in a derived class to handle events.
     *
     * The default implementation in the Widget class, will dispatch the
     * event to any third party handlers that have been registered. What
     * this means is if you expect other handlers to receive the events
     * then this must be called from derived classes.  Or, manually call
     * Widget::invoke_handlers().
     *
     * @see @subpage events
     *
     * @param event The Event that occurred.
     */
    virtual void handle(Event& event);

    /**
     * Resize the widget.
     *
     * Changes the width and height of the Widget.
     *
     * @param[in] size The new size of the Widget.
     * @note This will cause a redraw of the Widget.
     */
    virtual void resize(const Size& size);

    /**
     * Scale the current size of the Widget given the ratio.
     *
     * There is no automatic undo for this operation.  Each call to this function
     * uses the current size of the Widget.
     *
     * @param[in] hratio Horizontal ratio of width().
     * @param[in] vratio Vertical ratio of height().
     *
     * @see Widget::resize()
     */
    virtual void resize_by_ratio(int hratio, int vratio);

    /**
     * Scale the current size of the Widget given the ratio.
     *
     * @param[in] ratio Horizontal and vertical ratio of size().
     *
     * @see Widget::resize()
     */
    inline void resize_by_ratio(int ratio)
    {
        resize_by_ratio(ratio, ratio);
    }

    /**
     * Set the scale of the widget.
     *
     * @warning This is experimental.
     */
    virtual void scale(float /*scalex*/, float /*scaley*/)
    {}

    /**
     * Set the scale of the widget.
     *
     * @warning This is experimental.
     */
    inline void scale(float scale)
    {
        this->scale(scale, scale);
    }

    /**
     * Change the width.
     *
     * @param[in] w The new width of the Widget.
     */
    inline void width(default_dim_type w) { resize(Size(w, height())); }

    /**
     * Change the height.
     *
     * @param[in] h The new height of the Widget.
     */
    inline void height(default_dim_type h) { resize(Size(width(), h)); }

    /**
     * Move the Widget to a new position.
     *
     * Changes the x and y position of the Widget.
     *
     * @param[in] point The new origin point for the widget relative to its parent.
     * @note This will cause a redraw of the Widget.
     */
    virtual void move(const Point& point);

    /**
     * Set the x coordinate of the box.
     *
     * @param[in] x The new origin X value for the widget relative to its parent.
     */
    inline void x(default_dim_type x) { move(Point(x, y())); }

    /**
     * Set the y coordinate of the box.
     *
     * @param[in] y The new origin Y value for the widget relative to its parent.
     */
    inline void y(default_dim_type y) { move(Point(x(), y)); }

    /**
     * Move the widget to the specified center point.
     *
     * Changes the x and y position of the widget relative to the center point.
     *
     * @note This will cause a redraw of the widget.
     */
    virtual void move_to_center(const Point& point);

    /**
     * Move the widget to the center of its parent.
     *
     * Changes the x and y position of the widget relative to the center point
     * of its parent.
     *
     * @note This will cause a redraw of the widget.
     */
    virtual void move_to_center();

    /**
     * Change the box.
     *
     * This is the same as calling Widget::move() and Widget::resize() at the
     * same time.
     */
    virtual void box(const Rect& rect);

    /**
     * Hide the Widget.
     *
     * A widget that is not visible will receive no draw() calls.
     *
     * @note This changes the visible() property of the Widget.
     */
    virtual void hide();

    /**
     * Show the Widget.
     *
     * @note This changes the visible() property of the Widget.
     */
    virtual void show();

    /**
     * Return true if the widget is visible.
     */
    inline bool visible() const
    {
        return !flags().is_set(Widget::Flag::invisible);
    }

    /**
     * Set the visible property.
     */
    virtual void visible(bool value);

    /**
     * Toggle the visibility state.
     *
     * @see Widget::show()
     * @see Widget::hide()
     */
    inline void visible_toggle()
    {
        if (visible())
            hide();
        else
            show();
    }

    /**
     * Return true if the widget is active.
     *
     * The meaning of active is largely up to the derived implementation.
     */
    virtual bool active() const;

    /**
     * Set the active property.
     *
     * The meaning of active is largely up to the derived implementation.
     */
    virtual void active(bool value);

    /**
     * Return true if the widget is readonly.
     */
    inline bool readonly() const
    {
        return flags().is_set(Widget::Flag::readonly);
    }

    /**
     * Set the readonly property of the widget.
     *
     * When a widget is readonly, it typically works as normal, except it does
     * not accept any user input.
     *
     * @param value Readonly when true.
     */
    virtual void readonly(bool value);

    /**
     * Toggle the readonly state.
     */
    inline void readonly_toggle()
    {
        readonly(!readonly());
    }

    /**
     * Set the disabled state to true.
     */
    virtual void disable();

    /**
     * Set the disabled state to false.
     */
    virtual void enable();

    /**
     * Return the disabled state of the widget.
     *
     * When a widget is disabled, it does not receive events. Also, the
     * color scheme may change when a widget is disabled.
     */
    inline bool disabled() const
    {
        return flags().is_set(Widget::Flag::disabled);
    }

    /**
     * Toggle the disabled state.
     */
    inline void disable_toggle()
    {
        if (disabled())
            enable();
        else
            disable();
    }

    /**
     * Set the disabled state.
     *
     * @param[in] value When true, the Widget is disabled.
     *
     * @see Widget::enable()
     * @see Widget::disable()
     */
    inline void disabled(bool value)
    {
        if (value)
            disable();
        else
            enable();
    }

    /**
     * Return true if the widget is visible.
     */
    inline float alpha() const
    {
        return m_alpha;
    }

    /**
     * Set the alpha property.
     *
     * @param[in] alpha Widget alpha component in range 0.0 - 1.0.
     */
    virtual void alpha(float alpha);

    /**
     * Damage the box() of the widget and cause a redraw.
     *
     * This is the same as calling damage(box()) in most cases.
     *
     * @warning damage() cannot be called while in or below a Widget::draw()
     * method.
     */
    virtual void damage();

    /**
     * Mark the specified rect as a damaged area of the widget.
     *
     * @note This call will propagate to a top level parent frame that owns a
     * Screen.
     *
     * @warning damage() cannot be called while in or below a Widget::draw()
     * method.
     *
     * @param rect The rectangle to save for damage.
     */
    virtual void damage(const Rect& rect);

    /**
     * Bounding box for the Widget.
     */
    inline const Rect& box() const
    {
        return m_box;
    }

    /**
     * Get the size.
     *
     * @see Widget::box()
     */
    inline const Size& size() const
    {
        return box().size();
    }

    /**
     * Get the origin point.
     *
     * @see Widget::box()
     */
    inline const Point& point() const
    {
        return box().point();
    }

    /**
     * Width of the widget.
     */
    inline default_dim_type width() const { return m_box.width(); }

    /**
     * Height of the widget.
     */
    inline default_dim_type height() const { return m_box.height(); }

    /**
     * X coordinate of the widget origin.
     */
    inline default_dim_type x() const { return m_box.x(); }

    /**
     * Y coordinate of the widget origin.
     */
    inline default_dim_type y() const { return m_box.y(); }

    /**
     * Get the center point.
     */
    inline Point center() const { return box().center(); }

    /**
     * Set the widget Color Palette.
     *
     * This will replace any Palette instance currently owned by the Widget.
     * This does not have to be a complete Palette, in which case the widget
     * will default to the Theme palette if a Color is not found in this
     * palette.
     *
     * @param palette The new palette to assign to the widget.
     * @note This will overwrite the entire widget Palette.
     */
    virtual void palette(const Palette& palette);

    /**
     * Reset the widget's palette to a default state.
     */
    virtual void reset_palette();

    /**
     * Get a Widget color.
     *
     * This will return a color for the Palette::GroupId reflecting the current
     * state of the Widget when the call is made.  To get a specific
     * Palette::GroupId color, use the overloaded function.
     *
     * @param id Palette::ColorId to get.
     */
    Palette::pattern_type color(Palette::ColorId id) const;

    /**
     * Get a Widget color.
     *
     * @param id Palette::ColorId to get.
     * @param group Palette::GroupId to get.
     */
    Palette::pattern_type color(Palette::ColorId id, Palette::GroupId group) const;

    /**
     * Set a Widget color.
     *
     * @param id Palette::ColorId to set.
     * @param color Color to set.
     * @param group Palette::GroupId to set.
     */
    void color(Palette::ColorId id,
               const Palette::pattern_type& color,
               Palette::GroupId group = Palette::GroupId::normal);

    /**
     * Get a pointer to the parent Frame, or nullptr if none exists.
     */
    Frame* parent();

    /**
     * Get a pointer to the parent Frame, or nullptr if none exists.
     */
    const Frame* parent() const;

    /**
     * Get a pointer to the Screen instance, using using a parent as
     * necessary.
     *
     * @return An Screen if available, or nullptr.
     */
    virtual Screen* screen() const;

    /**
     * Get a const ref of the flags.
     */
    inline const Widget::Flags& flags() const { return m_widget_flags; }

    /**
     * Get a modifiable ref of the flags.
     *
     * @warning Directly modifying flags may result in inconsistent state of the
     * widget. Always use the appropriate member functions (i.e. readonly(),
     * checked()).
     */
    inline Widget::Flags& flags() { return m_widget_flags; }

    /**
     * Align the widget.
     *
     * This will align the widget relative to the box of its parent frame.
     *
     * @param[in] a The AlignFlags.
     */
    virtual void align(const AlignFlags& a);

    /**
     * Get the alignment.
     */
    inline const AlignFlags& align() const { return m_align; }

    /**
     * Get the alignment.
     */
    inline AlignFlags& align() { return m_align; }

    /**
     * Set the alignment padding.
     */
    inline void padding(default_dim_type padding)
    {
        if (detail::change_if_diff<>(m_padding, padding))
        {
            damage();
            parent_layout();
        }
    }

    /**
     * Return the alignment padding.
     */
    inline default_dim_type padding() const { return m_padding; }

    /**
     * Set the margin.
     */
    inline void margin(default_dim_type margin)
    {
        if (detail::change_if_diff<>(m_margin, margin))
        {
            damage();
            parent_layout();
        }
    }

    /**
     * Get the margin.
     */
    inline default_dim_type margin() const { return m_margin; }

    /**
     * Set the border.
     */
    inline void border(default_dim_type border)
    {
        if (detail::change_if_diff<>(m_border, border))
        {
            damage();
            parent_layout();
        }
    }

    /**
     * Get the border.
     */
    inline default_dim_type border() const { return m_border; }

    /**
     * Set the horizontal and vertical ratio.
     *
     * @note This is the same as calling Widget::ratio(ratio, ratio).
     */
    inline void ratio(default_dim_type ratio)
    {
        this->ratio(ratio, ratio);
    }

    /**
     * Set the ratio.
     */
    inline void ratio(default_dim_type horizontal,
                      default_dim_type vertical)
    {
        auto a = detail::change_if_diff<>(m_horizontal_ratio, horizontal);
        auto b = detail::change_if_diff<>(m_vertical_ratio, vertical);
        if (a || b)
            parent_layout();
    }

    /**
     * Set the vertical ratio.
     */
    inline void vertical_ratio(default_dim_type vertical)
    {
        if (detail::change_if_diff<>(m_vertical_ratio, vertical))
            parent_layout();
    }

    /**
     * Get the vertical ratio.
     */
    inline default_dim_type vertical_ratio() const { return m_vertical_ratio; }

    /**
     * Set the horizontal ratio.
     */
    inline void horizontal_ratio(default_dim_type horizontal)
    {
        if (detail::change_if_diff<>(m_horizontal_ratio, horizontal))
            parent_layout();
    }

    /**
     * Get the horizontal ratio.
     */
    inline default_dim_type horizontal_ratio() const { return m_horizontal_ratio; }

    /**
     * Set the Y ratio.
     */
    inline void yratio(default_dim_type yratio)
    {
        if (detail::change_if_diff<>(m_yratio, yratio))
            parent_layout();
    }

    /**
     * Get the Y ratio.
     */
    inline default_dim_type yratio() const { return m_yratio; }

    /**
     * Set the X ratio.
     */
    inline void xratio(default_dim_type xratio)
    {
        if (detail::change_if_diff<>(m_xratio, xratio))
            parent_layout();
    }

    /**
     * Get the X ratio.
     */
    inline default_dim_type xratio() const { return m_xratio; }

    /**
     * Get a minimum size hint for the Widget.
     *
     * This is used by sizers to pick minimum and default dimensions when no
     * other force is used.
     */
    virtual Size min_size_hint() const;

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
    virtual void paint_to_file(const std::string& filename = {});

    /**
     * Dump the Widget state.
     *
     * Dump the state of the Widget to the specified ostream.
     * @param[in,out] out The output stream.
     * @param[in] level Indentation level.
     */
    virtual void dump(std::ostream& out, int level = 0);

    /**
     * Callback definition used by Widget::walk().
     */
    using WalkCallback = std::function<bool(Widget* widget, int level)>;

    /**
     * Walk the Widget tree and call @b callback with each Widget.
     *
     * @param callback Function to call for each widget.
     * @param level The current level of the widget hierarchy.
     */
    virtual void walk(WalkCallback callback, int level = 0);

    /**
     * Get the current focus state.
     *
     * @return True if in focus.
     */
    virtual bool focus() const { return m_focus; }

    /**
     * Set the Widget's theme to a new theme.
     */
    void theme(const Theme& theme);

    /**
     * Reset the Widget's Theme to the default Theme.
     */
    void reset_theme();

    /**
     * Set the boxtype.
     */
    inline void boxtype(const Theme::BoxFlags& type)
    {
        if (detail::change_if_diff<>(m_boxtype, type))
            damage();
    }

    /**
     * Get the boxtype.
     */
    inline const Theme::BoxFlags& boxtype() const
    {
        return m_boxtype;
    }

    inline Theme::BoxFlags& boxtype()
    {
        return m_boxtype;
    }

    /**
     * Get the Widget Theme.
     *
     * If a custom theme was set for the instance, it will be returned.
     * Otherwise, if this widget has a parent it will return the first parent in
     * the widget hierarchy that has a theme.  If no theme is found, the default
     * global theme will be returned.
     */
    const Theme& theme() const;

    /**
     * Move this widgets zorder down relative to other widgets with the same
     * parent.
     */
    virtual void zorder_down();

    /**
     * Move this widgets zorder up relative to other widgets with the same
     * parent.
     */
    virtual void zorder_up();

    /**
     * Move the widget to the bottom.
     */
    virtual void zorder_bottom();

    /**
     * Move the widget to the top.
     */
    virtual void zorder_top();

    /**
     * Get the zorder of the widget.
     */
    virtual size_t zorder() const;

    /**
     * Detach this widget from its parent.
     */
    void detach();

    /**
     * Convert a point with a local origin to a parent origin.
     */
    Point to_parent(const Point& r) const;

    /**
     * @see Widget::to_parent().
     */
    inline Rect to_parent(const Rect& r) const
    {
        return Rect(to_parent(r.point()), r.size());
    }

    /**
     * Get the display origin of the Widget.
     */
    virtual DisplayPoint display_origin();

    /**
     * Get the sum of the margin, padding, and border around the content of the
     * widget.
     */
    virtual size_t moat() const;

    /**
     * Return the area that content is allowed to be positioned into.
     *
     * In most cases, the normal box() area needs to be shrunk by the
     * margin + padding + border.  However, some widgets reserve more for
     * themselves, and only allow their children to be positioned in the
     * rest.
     */
    virtual Rect content_area() const;

    /**
     * Perform layout of the Widget.
     */
    virtual void layout();

    /**
     * Helper function to draw this widget's box using the appropriate
     * theme.
     */
    void draw_box(Painter& painter, Palette::ColorId bg,
                  Palette::ColorId border) const;

    void draw_circle(Painter& painter, Palette::ColorId bg,
                     Palette::ColorId border) const;

    /**
     * Convert a local point to a display point.
     *
     * A display point has origin in the top left of the display. A local point
     * has its origin at the top left of the position of this widget.
     */
    virtual DisplayPoint local_to_display(const Point& p);

    /**
     * Convert a display point to a local point.
     *
     * A display point has origin in the top left of the display. A local point
     * has its origin at the top left of the position of this widget.
     */
    virtual Point display_to_local(const DisplayPoint& p);

    /**
     * Get the widget Font.
     */
    const Font& font() const
    {
        if (m_font)
            return *m_font;

        return theme().font();
    }

    /**
     * Set the widget Font.
     *
     * @note This will overwrite the entire widget Font.
     */
    void font(const Font& font)
    {
        if (m_font && *m_font == font)
            return;

        m_font = detail::make_unique<Font>(font);
        damage();
        layout();
        parent_layout();
    }

    /**
     * Return the boolean checked state of the a widget.
     */
    inline bool checked() const
    {
        return flags().is_set(Widget::Flag::checked);
    }

    /**
     * Set checked state of the widget.
     */
    virtual void checked(bool value);

    inline std::string type() const
    {
        return detail::demangle(typeid(*this).name());
    }

    /**
     * Serialize the widget to the specified serializer.
     */
    virtual void serialize(detail::Serializer& serializer) const;

    /**
     * Deserialize widget properties.
     */
    virtual void deserialize(const std::string& name, const std::string& value,
                             const std::map<std::string, std::string>& attrs);

    virtual ~Widget() noexcept;

protected:

    /**
     * Is this widget a top level widget?
     */
    virtual bool top_level() const { return false; }

    /**
     * Set this widget's parent.
     */
    virtual void parent(Frame* parent);

    /**
     * Get a reference to the default palette.
     */
    virtual const Palette& default_palette() const;

    /**
     * Get the local box which is the same size as box(), but with the
     * origin zeroed.
     */
    inline Rect local_box() const
    {
        return size();
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

    /**
     * Unique ID of this widget.
     */
    int m_widgetid{0};

    /**
     * Call our parent to do a layout.
     */
    void parent_layout();

private:

    /**
     * Flags for the widget.
     */
    Widget::Flags m_widget_flags{};

    /**
     * Palette for the widget.
     *
     * This may or may not be a complete palette.  If a color does not exist in
     * this instance, it will refer to the default_palette().
     *
     * @note This should not be accessed directly.  Always use the access
     * functions because this may not be a complete Palette.
     */
    std::unique_ptr<Palette> m_palette;

    /**
     * Alignment hint for this widget within its parent.
     */
    AlignFlags m_align{};

    /**
     * Alignment padding.
     */
    default_dim_type m_padding{0};

    /**
     * Alignment border.
     */
    default_dim_type m_border{0};

    /**
     * Alignment margin.
     */
    default_dim_type m_margin{0};

    /**
     * Alignment X ratio.
     */
    default_dim_type m_xratio{0};

    /**
     * Alignment Y ratio.
     */
    default_dim_type m_yratio{0};

    /**
     * Horizontal alignment ratio.
     */
    default_dim_type m_horizontal_ratio{0};

    /**
     * Vertical alignment ratio.
     */
    default_dim_type m_vertical_ratio{0};

    /**
     * Focus state.
     */
    bool m_focus{false};

    /**
     * Widget alpha.
     */
    float m_alpha{1.0f};

    /**
     * The boxtype.
     */
    Theme::BoxFlags m_boxtype{};

    /**
     * Instance theme for the widget.
     *
     * @note This should not be accessed directly.  Always use the access
     * functions because this is not set until it is modified.
     */
    std::unique_ptr<Theme> m_theme;

    mutable std::unique_ptr<Font> m_font;

    friend class Frame;
};

EGT_API std::ostream& operator<<(std::ostream& os, const Widget::Flag& flag);
EGT_API std::ostream& operator<<(std::ostream& os, const Widget::Flags& flags);

}
}

#endif
