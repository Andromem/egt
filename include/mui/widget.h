/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifndef MUI_WIDGET_H
#define MUI_WIDGET_H

/**
 * @file
 * @brief Base class Widget definition.
 */

#include <mui/font.h>
#include <mui/geometry.h>
#include <mui/input.h>
#include <mui/palette.h>
#include <mui/screen.h>
#include <mui/utils.h>
#include <cairo.h>
#include <cassert>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace mui
{
    using shared_cairo_surface_t =
        std::shared_ptr<cairo_surface_t>;

    using shared_cairo_t =
        std::shared_ptr<cairo_t>;

    class Painter;

    enum
    {
        /**
         * Do not draw the background color.
         *
         * The background color is usually drawn by default. The background will
         * essentially be transparent.
         */
        FLAG_NO_BACKGROUND = (1 << 0),

        /**
         * This is an overlay plane window.
         */
        FLAG_PLANE_WINDOW = (1 << 1),

        /**
         * Don't draw any border.
         */
        FLAG_NO_BORDER = (1 << 2),

        /**
         * Widget always requires a full redraw.
         */
        FLAG_FULL_REDRAW = (1 << 3),

        /**
         * This is a window widget.
         */
        FLAG_WINDOW = (1 << 4),

        /**
         * This is a frame window.
         */
        FLAG_FRAME = (1 << 5),

        /**
         * Draw a border around the widget.
         */
        FLAG_BORDER = (1 << 6),

        /**
         * Default window flags.
         */
        FLAG_WINDOW_DEFAULT = FLAG_WINDOW,

        CUSTOM_WIDGET_FLAGS = 1000,
    };

    class Widget;

    class EventWidget
    {
    public:
        EventWidget() noexcept
        {}

        typedef std::function<void (EventWidget* widget)> handler_callback_t;

        virtual void add_handler(handler_callback_t handler)
        {
            m_handlers.push_back(handler);
        }

        virtual ~EventWidget()
        {}

    protected:

        virtual void invoke_handlers()
        {
            for (auto x : m_handlers)
                x(this);
        }

        std::vector<handler_callback_t> m_handlers;
    };

    class Frame;

    /**
     * Base widget class.
     *
     * This is the base class for all widgets. A Widget is a thing, with a basic
     *  set of properties.  In this case, it has a bounding rectagle,
     * some flags, and some states - and these properties can be manipulated. A
     * Widget can handle events, draw itself, and more.  However, the specifics
     * of what it means to handle an event or draw the widget is implemented in
     * classes that are derived from this one, like a Button.
     */
    class Widget : public EventWidget
    {
    public:

        /**
         * Alignment flags.
         */
        enum
        {
            /** No alignment. */
            ALIGN_NONE = 0,
            /**
             * Center alignment is a weak alignment both horizontal and
             * vertical. To break one of those dimensions to another
             * alignment, specify it in addiiton to ALIGN_CENTER.  If both
             * are broken, ALIGN_CENTER has no effect.
             */
            ALIGN_CENTER = (1 << 0),
            /** Horizontal alignment. */
            ALIGN_LEFT = (1 << 1),
            /** Horizontal alignment. */
            ALIGN_RIGHT = (1 << 2),
            /** Vertical alignment. */
            ALIGN_TOP = (1 << 3),
            /** Vertical alignment. */
            ALIGN_BOTTOM = (1 << 4),
            /** Don't align, expand. */
            ALIGN_EXPAND = (1 << 5),
        };

        /**
         * Given an item size, and a bounding box, and an alignment parameter,
         * return the rctangle the item box should be respositioned/resized to.
         *
         * @warning This is not for text. Only for origin at left,top.
         */
        static Rect align_algorithm(const Size& item, const Rect& bounding,
                                    uint32_t align, int margin = 0)
        {
            assert(align != ALIGN_NONE);

            if (align & ALIGN_EXPAND)
                return bounding;

            Point p;
            if (align & ALIGN_CENTER)
            {
                p.x = bounding.x + (bounding.w / 2) - (item.w / 2);
                p.y = bounding.y + (bounding.h / 2) - (item.h / 2);
            }

            if (align & ALIGN_LEFT)
                p.x = bounding.x + margin;
            if (align & ALIGN_RIGHT)
                p.x = bounding.x + bounding.w - item.w - margin;
            if (align & ALIGN_TOP)
                p.y = bounding.y + margin;
            if (align & ALIGN_BOTTOM)
                p.y = bounding.y + bounding.h - item.h - margin;

            return Rect(p, item);
        }

        /**
         * Construct a widget.
         *
         * @param[in] point Point to position the widget at.
         * @param[in] point Initial size of the widget.
         * @param[in] flags Widget flags.
         */
        Widget(const Point& point = Point(),
               const Size& size = Size(),
               uint32_t flags = 0) noexcept;

        /**
         * Draw the widget.
         *
         * This is called by the internal code to cause a redraw of the widget.
         * You must implement this function in a derived class to handle drawing
         * the widget.
         *
         * @warning Do not call this directly.
         */
        virtual void draw(Painter& painter, const Rect& rect) = 0;

        /**
         * Handle an event.
         * Override this function in a derived class to handle events.
         *
         * Only the event ID is passed to this function. To get data associated
         * with the event, you have to call other functions.
         */
        virtual int handle(int event);

        /**
         * Resize the widget.
         * Changes the width and height of the widget.
         *
         * @note This will cause a redraw of the widget.
         */
        virtual void resize(const Size& s);

        /**
         * Move the widget.
         * Changes the x and y position of the widget.
         *
         * @note This will cause a redraw of the widget.
         */
        virtual void move(const Point& point);

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
        virtual void set_box(const Rect& rect)
        {
            move(rect.point());
            resize(rect.size());
        }

        /**
         * Hide the widget.
         *
         * A widget that is not visible will receive no draw() calls.
         *
         * @note This changes the visible() property of the widget.
         */
        virtual void hide()
        {
            if (!m_visible)
                return;
            m_visible = false;
            damage();
        }

        /**
         * Show the widget.
         *
         * @note This changes the visible() property of the widget.
         */
        virtual void show()
        {
            if (m_visible)
                return;
            m_visible = true;
            damage();
        }

        virtual bool visible() const { return m_visible; }

        /**
         * Return true if the widget is in focus.
         */
        virtual bool focus() const { return m_focus; }

        /**
         * Set the focus property of the widget.
         */
        virtual void focus(bool value) { m_focus = value; }

        /**
         * Return true if the widget is active.
         *
         * The meaning of active is largely up to the derived implementation.
         */
        virtual bool active() const { return m_active; }
        /**
         * Set the active property of the widget.
         */
        virtual void active(bool value) { m_active = value; }

        /**
         * Return the disabled status of the widget.
         *
         * When a widget is disabled, it does not receive events. Also, the
         * color scheme may change when a widget is disabled.
         */
        virtual bool disabled() const { return m_disabled; }

        /**
         * Set the disabled status of the widget.
         */
        virtual void disable(bool value)
        {
            if (m_disabled != value)
                damage();
            m_disabled = value;
        }

        /**
         * Damage the box() of the widget.
         *
         * @note This is just a utility wrapper around damage(box()) in most cases.
         */
        virtual void damage();

        /**
         * Damage the specified rectangle.
         */
        virtual void damage(const Rect& rect);

        /**
         * Bounding box for the widgets.
         */
        virtual const Rect& box() const { return m_box; }

        /**
         * Get the size of the widget.
         */
        virtual Size size() const { return m_box.size(); }

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
         * Get the widget palette().
         */
        Palette& palette()
        {
            if (!m_palette.get())
                m_palette.reset(new Palette(global_palette()));

            return *m_palette.get();
        }

        /**
         * Get the widget palette().
         */
        const Palette& palette() const
        {
            if (m_palette.get())
                return *m_palette.get();

            return global_palette();
        }

        /**
         * Set the widget palette().
         * @note This will overwrite the entire palette.
         */
        void set_palette(const Palette& palette)
        {
            m_palette.reset(new Palette(palette));
        }

        Frame* parent()
        {
#if 1
            // @todo This whole thing is bad. */
            if (!m_parent)
            {
                std::cout << "bad parent pointer" << std::endl;
                while (1);
            }
#endif
            assert(m_parent);
            return m_parent;
        }

        const Frame* parent() const
        {
#if 1
            // @todo This whole thing is bad. */
            if (!m_parent)
            {
                std::cout << "bad parent pointer" << std::endl;
                while (1);
            }
#endif
            assert(m_parent);
            return m_parent;
        }

        virtual IScreen* screen();

        /**
         * Test if the specified Widget flag(s) is/are set.
         * @param flag Bitmask of flags.
         */
        inline bool is_flag_set(uint32_t flag) const
        {
            return (m_flags & flag) == flag;
        }

        /**
         * Set the specified widget flags.
         * @param flag Bitmask of flags.
         */
        inline void flag_set(uint32_t flag) { m_flags |= flag; }

        /**
         * Clear, or unset, the specified widget flags.
         * @param flag Bitmask of flags.
         */
        inline void flag_clear(uint32_t flag) { m_flags &= ~flag; }

        /**
         * Get the name of the widget.
         */
        const std::string& name() const { return m_name; }

        /**
         * Align the widget.
         *
         * This will align the widget relative to the box of its parent widget.
         */
        void align(int a, int margin = 0);

        /**
         * Read the alignment of the widget.
         */
        inline int align() const { return m_align; }

        /**
         * Set the name of the widget.
         */
        inline void name(const std::string& name) { m_name = name; }

        virtual ~Widget();

    protected:

        /**
         * Convert screen coordinates to frame coordinates.
         */
        Point screen_to_frame(const Point& p);

        /**
         * Bounding box.
         */
        Rect m_box;

        /**
         * Pointer to this widget's parent.
         *
         * The parent is a Frame, which is capable of managing children.
         */
        Frame* m_parent {nullptr};

    private:

        /**
         * When true, the widget is visible.
         */
        bool m_visible {true};

        /**
         * When true, the widget has focus.
         */
        bool m_focus {false};

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
        bool m_active {false};

        /**
         * When true, the widget is disabled.
         *
         * Typically, when a widget is disabled it will not accept input.
         *
         * This may change how the widget behaves or is draw.
         */
        bool m_disabled {false};

        /**
         * Flags for the widget.
         */
        uint32_t m_flags {0};

        /**
         * Current palette for the widget.
         *
         * @note This should not accessed directly.  Always use the accessor
         * functions because this is not set until it is modified.
         */
        std::shared_ptr<Palette> m_palette;

        /**
         * A user defined name for the widget.
         */
        std::string m_name;

        /**
         * Alignment hint for this widget within its parent.
         */
        int m_align {ALIGN_NONE};

        Widget(const Widget&) = delete;
        Widget& operator=(const Widget&) = delete;

        friend class Frame;
    };

#ifdef DEVELOPMENT
    /**
     * Combo box widget.
     */
    class Combo : public Widget
    {
    public:
        Combo(const std::string& label = std::string(),
              const Point& point = Point(),
              const Size& size = Size());

        virtual int handle(int event);

        virtual void draw(Painter& painter, const Rect& rect);

        virtual ~Combo();

    protected:
        std::string m_label;
    };
#endif

    class ListBox : public Widget
    {
    public:
        typedef std::vector<std::string> item_array;

        ListBox(const item_array& items,
                const Point& point = Point(),
                const Size& size = Size());

        /**
         * Set the font of the items.
         */
        virtual void font(const Font& font) { m_font = font; }

        virtual int handle(int event);

        virtual void draw(Painter& painter, const Rect& rect);

        void selected(uint32_t index);

        uint32_t selected() const { return m_selected; }

        virtual ~ListBox();

    protected:

        virtual void on_selected(int index) {}

        Rect item_rect(uint32_t index) const;

        item_array m_items;
        uint32_t m_selected = { 0 };
        Font m_font;
    };

#ifdef DEVELOPMENT
    class ScrollWheel : public Widget
    {
    public:
        ScrollWheel(const Point& point = Point(), const Size& size = Size());

        virtual int handle(int event);

        virtual void draw(Painter& painter, const Rect& rect);

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

    protected:
        std::vector<std::string> m_values;

        int m_pos;
        int m_moving_x;
        int m_start_pos;
    };
#endif
}

#endif
