/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/detail/mousegesture.h"
#include "egt/input.h"

namespace egt
{
inline namespace v1
{
namespace detail
{

MouseGesture::MouseGesture()
{
    // setup long click timer handler
    m_long_click_timer.on_timeout([this]()
    {
        Event event(eventid::pointer_hold, Pointer(mouse_start()));
        invoke_handlers(event);
    });
}

void MouseGesture::on_async_event(mouse_callback_t callback)
{
    m_callbacks.emplace_back(std::move(callback));
}

Event MouseGesture::handle(const Event& event)
{
    switch (event.id())
    {
    case eventid::raw_pointer_down:
    {
        start(event.pointer().point);
        break;
    }
    case eventid::raw_pointer_up:
    {
        if (m_active)
        {
            auto dragging = m_dragging;

            stop();

            if (dragging)
            {
                Event eevent(eventid::pointer_drag_stop, event.pointer());
                eevent.pointer().drag_start = mouse_start();
                return eevent;
            }
            else
                return {eventid::pointer_click, event.pointer()};
        }
        break;
    }
    case eventid::raw_pointer_move:
    {
        if (m_active)
        {
            bool dragging_started = false;
            if (!m_dragging)
            {
                static const auto DRAG_ENABLE_DISTANCE = 10;
                auto distance = std::abs(mouse_start().distance_to(event.pointer().point));
                if (distance >= DRAG_ENABLE_DISTANCE)
                {
                    m_dragging = true;
                    dragging_started = true;

                    /// @bug This may be a problem.  It is possible the long click
                    /// event will still be generated.
                    m_long_click_timer.cancel();
                }
            }

            if (dragging_started)
            {
                Event eevent(eventid::pointer_drag_start, event.pointer());
                eevent.pointer().drag_start = mouse_start();
                return eevent;
            }

            if (m_dragging)
            {
                Event eevent(eventid::pointer_drag, event.pointer());
                eevent.pointer().drag_start = mouse_start();
                return eevent;
            }
        }

        break;
    }
    default:
        break;
    }

    return Event();
}

void MouseGesture::start(const DisplayPoint& point)
{
    m_long_click_timer.start_with_duration(std::chrono::milliseconds(500));
    m_mouse_start_pos = point;
    m_active = true;
    m_dragging = false;
}

void MouseGesture::stop()
{
    m_active = false;
    m_dragging = false;
    m_long_click_timer.cancel();
}

void MouseGesture::invoke_handlers(Event& event)
{
    for (auto& callback : m_callbacks)
        callback(event);
}

}
}
}
