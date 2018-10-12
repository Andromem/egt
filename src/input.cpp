/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "mui/app.h"
#include "mui/event_loop.h"
#include "mui/geometry.h"
#include "mui/input.h"
#include "mui/widget.h"
#include "mui/window.h"
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

namespace mui
{

    static Point pointer_abs_pos;
    Point& mouse_position()
    {
        return pointer_abs_pos;
    }

    static int event_key;
    int& key_value()
    {
        return event_key;
    }

    static int event_button;
    int& button_value()
    {
        return event_button;
    }

    void IInput::dispatch(int event)
    {
        DBG("event: " << event);

        for (auto& w : windows())
        {
            if (!w->visible())
                continue;

            if (!w->top_level())
                continue;

            w->handle(event);
        }
    }

    InputEvDev::InputEvDev(const string& path)
        : m_input(main_app().event().io()),
          m_input_buf(sizeof(struct input_event) * 10)
    {
        int fd = open(path.c_str(), O_RDONLY);
        if (fd >= 0)
        {
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
            cout << error << endl;
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

            case EV_ABS:
                absolute_event = true;
                switch (e->code)
                {
                case ABS_X:
                    x = (double)value;
                    break;
                case ABS_Y:
                    y = (double)value;
                    break;
                }

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
                    dispatch(value ? EVT_MOUSE_DOWN : EVT_MOUSE_UP);
                    break;
                case BTN_RIGHT:
                    dispatch(value ? EVT_MOUSE_DOWN : EVT_MOUSE_UP);
                    break;
                case BTN_MIDDLE:
                    dispatch(value ? EVT_MOUSE_DOWN : EVT_MOUSE_UP);
                    break;
                default:
                    int v = -1;
                    if (value == 1)
                        v = EVT_KEY_DOWN;
                    else if (value == 0)
                        v = EVT_KEY_UP;
                    else if (value == 2)
                        v = EVT_KEY_REPEAT;
                    if (v != -1)
                    {
                        event_key = e->code;
                        dispatch(v);
                    }
                }
            }
        }

        if (absolute_event)
        {
            pointer_abs_pos = Point(x, y);
            dispatch(EVT_MOUSE_MOVE);
        }
        else
        {
            if (dx != 0 || dy != 0)
            {
                pointer_abs_pos = Point(pointer_abs_pos.x + dx, pointer_abs_pos.y + dy);
                dispatch(EVT_MOUSE_MOVE);
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
