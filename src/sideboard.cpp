/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/app.h"
#include "egt/sideboard.h"

namespace egt
{
inline namespace v1
{

SideBoard::SideBoard(flags f,
                     std::chrono::milliseconds open_duration,
                     easing_func_t open_func,
                     std::chrono::milliseconds close_duration,
                     easing_func_t close_func)
    : Window(Application::instance().screen()->size() + Size(f == flags::left || f == flags::right ? HANDLE_WIDTH : 0,
             f == flags::top || f == flags::bottom ? HANDLE_WIDTH : 0),
             pixel_format::rgb565),
      m_side_flags(f)
{
    m_oanim.set_duration(open_duration);
    m_canim.set_duration(close_duration);
    m_oanim.set_easing_func(open_func);
    m_canim.set_easing_func(close_func);

    reset_animations();

    if (is_set(flags::left))
    {
        m_oanim.on_change(std::bind(&SideBoard::set_x, this, std::placeholders::_1));
        m_canim.on_change(std::bind(&SideBoard::set_x, this, std::placeholders::_1));

        move(Point(m_oanim.starting(), 0));
    }
    else if (is_set(flags::right))
    {
        m_oanim.on_change(std::bind(&SideBoard::set_x, this, std::placeholders::_1));
        m_canim.on_change(std::bind(&SideBoard::set_x, this, std::placeholders::_1));

        move(Point(m_oanim.starting(), 0));
    }
    else if (is_set(flags::top))
    {
        m_oanim.on_change(std::bind(&SideBoard::set_y, this, std::placeholders::_1));
        m_canim.on_change(std::bind(&SideBoard::set_y, this, std::placeholders::_1));

        move(Point(0, m_oanim.starting()));
    }
    else if (is_set(flags::bottom))
    {
        m_oanim.on_change(std::bind(&SideBoard::set_y, this, std::placeholders::_1));
        m_canim.on_change(std::bind(&SideBoard::set_y, this, std::placeholders::_1));

        move(Point(0, m_oanim.starting()));
    }
}

void SideBoard::reset_animations()
{
    if (is_set(flags::left))
    {
        m_oanim.set_starting(-Application::instance().screen()->size().width());
        m_oanim.set_ending(0);
        m_canim.set_starting(m_oanim.ending());
        m_canim.set_ending(m_oanim.starting());
    }
    else if (is_set(flags::right))
    {
        m_oanim.set_starting(Application::instance().screen()->size().width() - HANDLE_WIDTH);
        m_oanim.set_ending(-HANDLE_WIDTH);
        m_canim.set_starting(m_oanim.ending());
        m_canim.set_ending(m_oanim.starting());
    }
    else if (is_set(flags::top))
    {
        m_oanim.set_starting(-Application::instance().screen()->size().height());
        m_oanim.set_ending(0);
        m_canim.set_starting(m_oanim.ending());
        m_canim.set_ending(m_oanim.starting());
    }
    else if (is_set(flags::bottom))
    {
        m_oanim.set_starting(Application::instance().screen()->size().height() - HANDLE_WIDTH);
        m_oanim.set_ending(-HANDLE_WIDTH);
        m_canim.set_starting(m_oanim.ending());
        m_canim.set_ending(m_oanim.starting());
    }
}

void SideBoard::handle(Event& event)
{
    Window::handle(event);

    switch (event.id())
    {
    case eventid::pointer_click:
    {
        if (m_dir)
            close();
        else
            open();

        break;
    }
    default:
        break;
    }
}

void SideBoard::close()
{
    m_canim.stop();
    auto running = m_oanim.running();
    m_oanim.stop();
    auto current = m_oanim.current();
    reset_animations();
    if (running)
        m_canim.set_starting(current);
    m_canim.start();
    m_dir = false;
}

void SideBoard::open()
{
    m_oanim.stop();
    auto running = m_canim.running();
    m_canim.stop();
    auto current = m_canim.current();
    reset_animations();
    if (running)
        m_oanim.set_starting(current);
    m_oanim.start();
    m_dir = true;
}

}
}
