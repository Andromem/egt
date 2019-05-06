/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/app.h"
#include "egt/geometry.h"
#include "egt/detail/input/inputevdev.h"
#include "egt/utils.h"
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <errno.h>
#include <fcntl.h>
#include <linux/input.h>
#include <string>
#include <sys/time.h>
#include <unistd.h>

using namespace std;

namespace egt
{
inline namespace v1
{
namespace detail
{

InputEvDev::InputEvDev(const string& path)
    : m_input(main_app().event().io()),
      m_input_buf(sizeof(struct input_event) * 10)
{
    int fd = open(path.c_str(), O_RDONLY);
    if (fd >= 0)
    {
        INFO("input device: " << path);

        m_input.assign(fd);

        asio::async_read(m_input, asio::buffer(m_input_buf.data(), m_input_buf.size()),
                         std::bind(&InputEvDev::handle_read, this,
                                   std::placeholders::_1,
                                   std::placeholders::_2));
    }
}

void InputEvDev::handle_read(const asio::error_code& error, std::size_t length)
{
    if (error)
    {
        ERR(error);
        return;
    }

    struct input_event* ev = reinterpret_cast<struct input_event*>(m_input_buf.data());
    struct input_event* e, *end;

    int dx = 0;
    int dy = 0;
    int x = 0;
    int y = 0;
    bool absolute_event = false;

    if (length == 0 || length % sizeof(e[0]) != 0)
    {
        assert(0);
    }

    e = ev;
    end = ev + (length / sizeof(e[0]));
    for (e = ev; e < end; e++)
    {
        int value = e->value;

        DBG(value);
        switch (e->type)
        {
        case EV_REL:
            switch (e->code)
            {
            case REL_X:
                dx += value;
                break;
            case REL_Y:
                dy += value;
                break;
            default:
                assert(0);
            }
            break;

        case EV_ABS:
            absolute_event = true;
            switch (e->code)
            {
            case ABS_X:
                x = static_cast<double>(value);
                break;
            case ABS_Y:
                y = static_cast<double>(value);
                break;
            }
            break;

        case EV_KEY:
            switch (e->code)
            {
            case BTN_TOUCH:
            case BTN_TOOL_PEN:
            case BTN_TOOL_RUBBER:
            case BTN_TOOL_BRUSH:
            case BTN_TOOL_PENCIL:
            case BTN_TOOL_AIRBRUSH:
            case BTN_TOOL_FINGER:
            case BTN_TOOL_MOUSE:
            case BTN_TOOL_LENS:
                break;
            case BTN_LEFT:
                dispatch(value ? eventid::raw_pointer_down : eventid::raw_pointer_up);
                break;
            case BTN_RIGHT:
                dispatch(value ? eventid::raw_pointer_down : eventid::raw_pointer_up);
                break;
            case BTN_MIDDLE:
                dispatch(value ? eventid::raw_pointer_down : eventid::raw_pointer_up);
                break;
            default:
                eventid v = eventid::none;
                if (value == 1)
                    v = eventid::keyboard_down;
                else if (value == 0)
                    v = eventid::keyboard_up;
                else if (value == 2)
                    v = eventid::keyboard_repeat;
                if (v != eventid::none)
                {
                    m_keys.key = e->code;
                    dispatch(v);
                }
            }
        }
    }

    if (absolute_event)
    {
        m_pointer.point = DisplayPoint(x, y);
        dispatch(eventid::raw_pointer_move);
    }
    else
    {
        if (dx != 0 || dy != 0)
        {
            m_pointer.point = DisplayPoint(m_pointer.point.x + dx, m_pointer.point.y + dy);
            dispatch(eventid::raw_pointer_move);
        }
    }

    asio::async_read(m_input, asio::buffer(m_input_buf.data(), m_input_buf.size()),
                     std::bind(&InputEvDev::handle_read, this,
                               std::placeholders::_1,
                               std::placeholders::_2));
}

InputEvDev::~InputEvDev()
{
}

}
}
}
