/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "detail/input/inputkeyboard.h"
#include "egt/app.h"
#include "egt/detail/input/inputevdev.h"
#include "egt/geometry.h"
#include "egt/keycode.h"
#include <cassert>
#include <cerrno>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <linux/input.h>
#include <spdlog/fmt/ostr.h>
#include <spdlog/spdlog.h>
#include <sstream>
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

InputEvDev::InputEvDev(Application& app, const string& path)
    : m_input(app.event().io()),
      m_input_buf(sizeof(struct input_event) * 10),
      m_keyboard(make_unique<InputKeyboard>())
{
    m_fd = open(path.c_str(), O_RDONLY);
    if (m_fd >= 0)
    {
        spdlog::info("input device: {}", path);

        m_input.assign(m_fd);

        asio::async_read(m_input, asio::buffer(m_input_buf.data(), m_input_buf.size()),
                         std::bind(&InputEvDev::handle_read, this,
                                   std::placeholders::_1,
                                   std::placeholders::_2));
    }
    else
    {
        ostringstream ss;
        ss << "could not open evdev device " << path;
        throw std::runtime_error(ss.str());
    }
}

void InputEvDev::handle_read(const asio::error_code& error, std::size_t length)
{
    if (error)
    {
        spdlog::error("{}", error);
        return;
    }

    auto ev = reinterpret_cast<struct input_event*>(m_input_buf.data());
    struct input_event* e, *end;

    int dx = 0;
    int dy = 0;
    int x = 0;
    int y = 0;
    bool absolute_event = false;

    if (length == 0 || length % sizeof(e[0]) != 0)
    {
        assert(0);
        return;
    }

    end = ev + (length / sizeof(e[0]));
    for (e = ev; e < end; e++)
    {
        int value = e->value;

        SPDLOG_DEBUG("{}", value);
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
                break;
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
            {
                Event event(value ? eventid::raw_pointer_down : eventid::raw_pointer_up,
                            Pointer(m_last_point, Pointer::button::left));
                dispatch(event);
                break;
            }
            case BTN_RIGHT:
            {
                Event event(value ? eventid::raw_pointer_down : eventid::raw_pointer_up,
                            Pointer(m_last_point, Pointer::button::right));
                dispatch(event);
                break;
            }
            case BTN_MIDDLE:
            {
                Event event(value ? eventid::raw_pointer_down : eventid::raw_pointer_up,
                            Pointer(m_last_point, Pointer::button::middle));
                dispatch(event);
                break;
            }
            default:
            {
                switch (value)
                {
                case 0:
                {
                    const auto unicode = m_keyboard->on_key(e->code, eventid::keyboard_up);
                    Event event(eventid::keyboard_up, Key(linux_to_ekey(e->code), unicode));
                    dispatch(event);
                    break;
                }
                case 1:
                {
                    const auto unicode = m_keyboard->on_key(e->code, eventid::keyboard_down);
                    Event event(eventid::keyboard_down, Key(linux_to_ekey(e->code), unicode));
                    dispatch(event);
                    break;
                }
                case 2:
                {
                    const auto unicode = m_keyboard->on_key(e->code, eventid::keyboard_repeat);
                    Event event(eventid::keyboard_repeat, Key(linux_to_ekey(e->code), unicode));
                    dispatch(event);
                    break;
                }
                }

                break;
            }
            }
        }
    }

    if (absolute_event)
    {
        m_last_point = DisplayPoint(x, y);
        Event event(eventid::raw_pointer_move, Pointer(m_last_point));
        dispatch(event);
    }
    else
    {
        if (dx != 0 || dy != 0)
        {
            m_last_point = DisplayPoint(m_last_point.x() + dx, m_last_point.y() + dy);
            Event event(eventid::raw_pointer_move, Pointer(m_last_point));
            dispatch(event);
        }
    }

    asio::async_read(m_input, asio::buffer(m_input_buf.data(), m_input_buf.size()),
                     std::bind(&InputEvDev::handle_read, this,
                               std::placeholders::_1,
                               std::placeholders::_2));
}

InputEvDev::~InputEvDev()
{
    if (m_fd >= 0)
        close(m_fd);
}

}
}
}
