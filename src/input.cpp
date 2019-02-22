/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "egt/input.h"
#include "egt/window.h"
#include <chrono>
#include <linux/input.h>

using namespace std;

namespace egt
{
inline namespace v1
{

Input::Input()
    : m_mouse(new detail::MouseGesture)
{
    m_mouse->on_async_event(std::bind(&Input::dispatch,
                                      this, std::placeholders::_1));
}

/**
 * @todo No mouse positions should be allowed off the screen box().  This is
 * possible with some input devices currentl and we need to limit.  Be careful
 * not to drop events (like pointer up) when correcting.
 */
void Input::dispatch(eventid event)
{
    // can't support recursive calls into the same dispatch function
    // using the m_diapcthing variable like this is not exception safe
    // one potential solution would be to asio::post() the call to dispatch if
    // we are currently dispatching already
    assert(!m_dispatching);

    m_dispatching = true;

    m_current_input = this;

    if (event == eventid::keyboard_down)
    {
        m_keys.states[m_keys.code] = true;
    }
    else if (event == eventid::keyboard_up)
    {
        m_keys.states[m_keys.code] = false;
    }

    auto eevent = m_mouse->handle(event);

    DBG("input event: " << event);
    if (eevent != eventid::none)
    {
        DBG("input event: " << eevent);
        if (eevent == eventid::pointer_drag_start)
            m_pointer.drag_start = m_mouse->mouse_start();
    }

    // then give it to any global input handlers
    if (m_global_handler.invoke_handlers(event))
        return;
    if (eevent != eventid::none)
        if (m_global_handler.invoke_handlers(eevent))
            return;


    if (modal_window())
    {
        auto target = modal_window();
        // give event to the modal window
        target->handle(event);
        if (eevent != eventid::none)
            target->handle(eevent);
    }
    else
    {
        if (mouse_grab() && (event == eventid::raw_pointer_down ||
                             event == eventid::raw_pointer_up ||
                             event == eventid::raw_pointer_move ||
                             event == eventid::pointer_click ||
                             event == eventid::pointer_dblclick ||
                             event == eventid::pointer_hold ||
                             event == eventid::pointer_drag_start ||
                             event == eventid::pointer_drag ||
                             event == eventid::pointer_drag_stop))
        {
            auto target = mouse_grab();
            target->handle(event);
            if (eevent != eventid::none)
                target->handle(eevent);
        }
        else if (keyboard_focus() && (event == eventid::keyboard_down ||
                                      event == eventid::keyboard_up ||
                                      event == eventid::keyboard_repeat))
        {
            auto target = keyboard_focus();
            target->handle(event);
            if (event != eventid::none)
                target->handle(eevent);
        }
        else
        {
            // give event to any top level and visible windows
            for (auto& w : detail::reverse_iterate(windows()))
            {
                if (!w->top_level())
                    continue;

                if (w->readonly())
                    continue;

                if (w->disabled())
                    continue;

                if (!w->visible())
                    continue;

                w->handle(event);
                if (eevent != eventid::none)
                    w->handle(eevent);
            }
        }
    }

    m_dispatching = false;
}

Input* Input::m_current_input = nullptr;
detail::Object Input::m_global_handler;

namespace detail
{
static Widget* grab = nullptr;
static Widget* kfocus = nullptr;
}

Widget* mouse_grab()
{
    return detail::grab;
}

void mouse_grab(Widget* widget)
{
    if (widget)
    {
        DBG("mouse grab by " << widget->name());
    }
    else if (detail::grab)
    {
        DBG("mouse release by " << detail::grab->name());
    }
    detail::grab = widget;
}

void keyboard_focus(Widget* widget)
{
    if (detail::kfocus == widget)
        return;

    if (detail::kfocus)
        detail::kfocus->handle(eventid::on_lost_focus);

    detail::kfocus = widget;

    if (widget)
        widget->handle(eventid::on_gain_focus);
}

Widget* keyboard_focus()
{
    return detail::kfocus;
}

}
}
