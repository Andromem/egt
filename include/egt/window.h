/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_WINDOW_H
#define EGT_WINDOW_H

/**
 * @file
 * @brief Working with windows.
 */

#include <egt/detail/meta.h>
#include <egt/frame.h>
#include <egt/image.h>
#include <egt/label.h>
#include <egt/screen.h>
#include <egt/widgetflags.h>
#include <memory>
#include <vector>

namespace egt
{
inline namespace v1
{

namespace detail
{
class WindowImpl;
class PlaneWindow;
}

/**
 * A Window is a Widget that handles drawing to a Screen.
 *
 * A Window is a Frame that optionally manages and draws to a Screen. If the
 * Window does not have a Screen itself, it will refer to its parent for the
 * Screen.
 *
 * Windows, unlike other basic widgets, are hidden by default. Windows always
 * require a call to show() before they will be drawn.
 *
 * This class acts as normal Frame/Widget but punts many operations to a
 * dynamically selected backend to work with the screen.
 */
class EGT_API Window : public Frame
{
public:

    /**
     * Default pixel format used for Window.
     */
    static const PixelFormat DEFAULT_FORMAT;

    /**
     * Construct a window.
     *
     * @param[in] format_hint Requested format of the Window. This only applies
     *            if this Window will be responsible for creating a backing
     *            screen. Otherwise, the Window will use whatever format the
     *            existing screen has. This is only a hint.
     * @param[in] hint Requested Window type. This only applies if this Window
     *            will be responsible for creating a backing screen.  This is
     *            only a hint.
     */
    explicit Window(PixelFormat format_hint = DEFAULT_FORMAT,
                    WindowHint hint = WindowHint::automatic)
        : Window({}, format_hint, hint)
    {}

    /**
     * Construct a window.
     *
     * @param[in] rect Initial rectangle of the Window.
     * @param[in] format_hint Requested format of the Window. This only applies
     *            if this Window will be responsible for creating a backing
     *            screen. Otherwise, the Window will use whatever format the
     *            existing screen has. This is only a hint.
     * @param[in] hint Requested Window type. This only applies if this Window
     *            will be responsible for creating a backing screen.  This is
     *            only a hint.
     */
    explicit Window(const Rect& rect,
                    PixelFormat format_hint = DEFAULT_FORMAT,
                    WindowHint hint = WindowHint::automatic);

    virtual void damage() override
    {
        Frame::damage();
    }

    virtual void damage(const Rect& rect) override;

    /**
     * The buck stops on this call to Widget::screen() with a Window
     * because the Window contains the screen.
     */
    virtual Screen* screen() const override;

    virtual bool has_screen() const override;

    virtual void move(const Point& point) override;

    virtual void show() override;

    virtual void hide() override;

    virtual void resize(const Size& size) override;

    using Frame::scale;

    virtual void scale(float hscale, float vscale) override;

    virtual void paint(Painter& painter) override;

    /*
     * Damage rectangles propagate up the widget tree and stop at a top level
     * widget, which can only be a window. As it propagates up, the damage
     * rectangle origin changes value to respect the current frame.  When
     * drawing those rectangles, as they propagate down the widget hierarchy
     * the opposite change happens to the rectangle origin.
     */
    virtual void begin_draw() override;

    /**
     * Set a background image of the window.
     *
     * The background image will automatically be scaled to fit the size of the
     * window.
     *
     * @note This is not a special child widget, in the sense that you can
     * technically put another child widget before the background using
     * zorder.
     */
    virtual void background(const Image& image);

    virtual PixelFormat format() const
    {
        auto frame = find_screen();
        if (frame)
            return frame->screen()->format();

        return PixelFormat::invalid;
    }

    virtual ~Window();

protected:

    /**
     * Perform the actual drawing.  Allocate the Painter and call draw() on each
     * child.
     */
    virtual void do_draw();

    virtual void allocate_screen();

    /**
     * Select and allocate the backend implementation for the window.
     */
    void create_impl(const Rect& rect,
                     PixelFormat format_hint,
                     WindowHint hint);

    virtual void default_damage(const Rect& rect)
    {
        Frame::damage(rect);
    }

    virtual void default_resize(const Size& size)
    {
        Frame::resize(size);
    }

    virtual void default_scale(float scalex, float scaley)
    {
        Frame::scale(scalex, scaley);
    }

    virtual void default_move(const Point& point)
    {
        Frame::move(point);
    }

    virtual void default_begin_draw()
    {
        if (m_parent)
        {
            m_parent->begin_draw();
            return;
        }

        do_draw();
    }

    virtual void default_show()
    {
        Frame::show();
    }

    virtual void default_hide()
    {
        Frame::hide();
    }

    virtual void default_paint(Painter& painter)
    {
        Frame::paint(painter);
    }

    std::unique_ptr<detail::WindowImpl> m_impl;

    /**
     * Change this window as the main window.
     */
    void main_window();

    /**
     * Optional background image.
     */
    std::shared_ptr<ImageLabel> m_background;

    friend class detail::WindowImpl;
    friend class detail::PlaneWindow;
};

/**
 * Top level Window.
 *
 * Special Window type that is the top level Window, or main Window.  It
 * provides features like a pointer cursor, that usually only make sense as a
 * top level Window.
 */
class EGT_API TopWindow : public Window
{
public:

    using Window::Window;

    /**
     * Show the cursor.
     */
    virtual void show_cursor(const Image& image = Image("@cursor.png"));

    /**
     * Hide the cursor.
     */
    virtual void hide_cursor();

protected:

    /**
     * Handle mouse events.
     */
    virtual void handle_mouse(Event& event);

    /**
     * Cursor window, if created.
     */
    std::shared_ptr<Window> m_cursor;
};

/**
 * Get a pointer reference to the main window.
 */
EGT_API Window* main_window();

/**
 * Get a pointer reference to the modal window.
 *
 * The modal window is a single window that will receive all events. Only
 * one window can be modal at any given time.
 */
EGT_API Window* modal_window();

namespace detail
{

/**
 * Set the modal window.
 */
EGT_API void set_modal_window(Window* window);

}

/**
 * Get the list of all currently allocated Windows.
 */
EGT_API std::vector<Window*>& windows();

}
}

#endif
