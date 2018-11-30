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

#include <egt/frame.h>
#include <vector>

namespace egt
{
    class Window;

    /**
     * Get a pointer reference to the main window.
     */
    Window*& main_window();

    /**
     * Get a pointer reference to the modal window.
     *
     * The modal window is a single window that will receive all events. Only
     * one window can be modal at any given time.
     */
    Window*& modal_window();

    /**
     * Get the list of all currently allocated Windows.
     */
    std::vector<Window*>& windows();

    /**
     * A Window is a Frame Widget that contains and is backed by a Screen.
     *
     * The first window created will automatically become the main window. It
     * will also be forced to the size of the main_screen().  To change this,
     * call set_main_window() on any Window instance.
     *
     * Any top level widget must be a Window.
     */
    class Window : public Frame
    {
    public:

        /**
         * Construct a Window.
         *
         * @param[in] size The size of the Window.  The origin will be default to 0,0.
         * @param[in] flags Mask of widget flags.
         */
        Window(const Size& size = Size(),
               widgetmask flags = widgetmask::WINDOW_DEFAULT);

        /**
         * Construct a Window.
         *
         * @param[in] rect The rectangle for the Window.
         * @param[in] flags Mask of widget flags.
         */
        Window(const Rect& rect,
               widgetmask flags = widgetmask::WINDOW_DEFAULT);

        virtual void enter()
        {
            show();
        }

        virtual void exit()
        {
            hide();
        }

        /**
         * Change this Window to be the main Window.
         */
        virtual void set_main_window();

        /**
         * The buck stops on this call to Widget::screen() with a Window
         * because the Window contains the screen.
         */
        virtual IScreen* screen() override
        {
            assert(m_screen);
            return m_screen;
        }

        /**
         * Cause the frame to draw itself and al of its children.
         *
         * @warning Normally this should not be called directly and instead the
         * event loop will call this function.
         */
        virtual void top_draw() override;

        virtual ~Window();

    protected:

        /**
         * Perform the draw starting from this frame.
         */
        virtual void do_draw();

        virtual bool have_screen() const override
        {
            return m_screen;
        }

        IScreen* m_screen{nullptr};

    private:

        // unsupported
        virtual void resize(const Size& size) override
        {
            ignoreparam(size);
        }
    };


    /**
     * Popup window.
     */
    template <class T>
    class Popup : public T
    {
    public:
        explicit Popup(const Size& size = Size(),
                       const Point& point = Point())
            : T(size)
        {
            this->move(point);
        }

        /**
         * Show the window.
         *
         * @param[in] center Move the window to the center of the screen first.
         */
        virtual void show(bool center = false)
        {
            if (center)
            {
                if (T::parent())
                    this->move_to_center(T::parent()->box().center());
                else
                    this->move_to_center(main_screen()->box().center());
            }

            T::show();
        }

        virtual void show_modal(bool center = false)
        {
            if (!modal_window())
            {
                modal_window() = this;
                this->show(center);
            }
        }

        virtual void hide() override
        {
            T::hide();

            if (modal_window() == this)
            {
                modal_window() = nullptr;
            }
        }
    };

}

#endif
