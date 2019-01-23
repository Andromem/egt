/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_VIEW_H
#define EGT_VIEW_H

/**
 * @file
 * @brief View definition.
 */

#include <egt/frame.h>

namespace egt
{
inline namespace v1
{

/**
 * A scrollable view.
 *
 * A ScrolledView is a sort of kitchen window. You can have a large surface
 * area on the other side of the window, but you only see a small portion of
 * it through the window.  The surface can be scrolled, or panned, in a single
 * orientation to see the rest.
 *
 * This is used internally by Widgets, but can also be used directly.  For
 * example, if ther are too many elements in a ListBox to be displayed due
 * to the size of the widget, the View usage in a ListBox makes the list
 * scrollable.
 */
class ScrolledView : public Frame
{
public:

    explicit ScrolledView(const Rect& rect = Rect());

    virtual int handle(eventid event) override;

    virtual void draw(Painter& painter, const Rect& rect) override;

    /**
     * Get the position.
     */
    int position() const { return m_offset; }

    /**
     * Set the position.
     */
    void set_position(int offset)
    {
        Rect super = super_rect();

        if (offset <= 0 && -offset < super.w)
        {
            if (m_offset != offset)
            {
                m_offset = offset;
                damage();
            }
        }
    }

    virtual ~ScrolledView();

protected:

    Rect super_rect() const;

    /**
     * Current offset of the view.
     */
    int m_offset{0};

    /**
     * The orientation of the scroll.
     * @todo Only horizontal is currently implemented.
     */
    orientation m_orientation{orientation::HORIZONTAL};

    /**
     * Are we currently scrolling.
     * @todo Replace implementation with class MouseDrag.
     */
    bool m_moving{false};
    Point m_start_pos;
    int m_start_offset{0};
};

}
}

#endif
