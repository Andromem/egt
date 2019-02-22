/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/detail/math.h"
#include "egt/input.h"
#include "egt/painter.h"
#include "egt/view.h"
#include <sstream>

using namespace std;

namespace egt
{
inline namespace v1
{

// orientation to size dimension
static inline int o2d(orientation o, const Size& size)
{
    return (o == orientation::horizontal) ? size.w : size.h;
}

// orientation to size dimension
static inline int o2d(orientation o, const Rect& rect)
{
    return o2d(o, rect.size());
}

// orientation to point axis
template<class T>
static inline int o2p(orientation o, const T& point)
{
    return (o == orientation::horizontal) ? point.x : point.y;
}

ScrolledView::ScrolledView(const Rect& rect, orientation orient)
    : Frame(rect),
      m_slider(0, 100, 0, orient),
      m_orient(orient)
{
    set_name("ScrolledView" + std::to_string(m_widgetid));
    set_boxtype(Theme::boxtype::none);

    if (orient == orientation::vertical)
        m_slider.slider_flags().set({Slider::flag::rectangle_handle,
                                     Slider::flag::origin_opposite,
                                     Slider::flag::consistent_line});
    else
        m_slider.slider_flags().set({Slider::flag::rectangle_handle,
                                     Slider::flag::consistent_line});

    resize_slider();

    m_slider.flags().set(Widget::flag::readonly);
}

ScrolledView::ScrolledView(orientation orient)
    : ScrolledView(Rect(), orient)
{
}

int ScrolledView::handle(eventid event)
{
    switch (event)
    {
    case eventid::pointer_drag_start:
        m_start_offset = m_offset;
        break;
    case eventid::pointer_drag:
    {
        auto diff = o2p(m_orient, event::pointer().point) -
                    o2p(m_orient, event::pointer().drag_start);
        set_offset(m_start_offset + diff);
        break;
    }
    default:
        break;
    }

    return Frame::handle(event);
}

bool ScrolledView::scrollable() const
{
    Rect super = super_rect();
    return o2d(m_orient, super) > o2d(m_orient, size());
}

void ScrolledView::draw(Painter& painter, const Rect& rect)
{
    ignoreparam(rect);

    Painter::AutoSaveRestore sr(painter);
    auto cr = painter.context();

    // change the origin to the offset
    if (m_orient == orientation::horizontal)
        cairo_translate(cr.get(), m_offset, 0);
    else
        cairo_translate(cr.get(), 0, m_offset);

    Rect r = box();
    if (m_orient == orientation::horizontal)
        r.x -= m_offset;
    else
        r.y -= m_offset;

    Frame::draw(painter, r);

    if (scrollable())
    {
        if (Rect::intersect(m_slider.box(), r))
            m_slider.draw(painter, Rect::intersection(m_slider.box(), r));
    }
}

auto SLIDER_DIM = 10;

Rect ScrolledView::child_area() const
{
    // reserve the slider area to keep away from children
    if (!scrollable())
        return box();

    if (m_orient == orientation::horizontal)
        return box() - Size(0, SLIDER_DIM);

    return box() - Size(SLIDER_DIM, 0);
}

void ScrolledView::resize(const Size& size)
{
    Frame::resize(size);
    resize_slider();
}

void ScrolledView::resize_slider()
{
    auto b = box();
    if (m_orient == orientation::horizontal)
    {
        b.x += std::abs(m_offset);
        b.y = b.y + b.h - SLIDER_DIM;
        b.h = SLIDER_DIM;
    }
    else
    {
        b.x = b.x + b.w - SLIDER_DIM;
        b.y += std::abs(m_offset);
        b.w = SLIDER_DIM;
    }

    m_slider.move(b.point());
    m_slider.resize(b.size());
}

Rect ScrolledView::super_rect() const
{
    Rect result;
    for (auto& child : m_children)
    {
        result = Rect::merge(result, child->box());
    }
    return result;
}

void ScrolledView::set_offset(int offset)
{
    if (scrollable())
    {
        Rect super = super_rect();
        auto offset_max = o2d(m_orient, super) - o2d(m_orient, box());
        if (offset > 0)
            offset = 0;
        else if (-offset > offset_max)
            offset = -offset_max;

        if (m_offset != offset)
        {
            m_offset = offset;

            auto slider_value =
                egt::detail::normalize<float>(std::abs(m_offset), 0, offset_max,
                                              0, 100);

            m_slider.set_value(slider_value);
            resize_slider();

            damage();
        }
    }
}

ScrolledView::~ScrolledView()
{
}

}
}
