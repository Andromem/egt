/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "egt/app.h"
#include "egt/eventloop.h"
#include "egt/painter.h"
#include "egt/timer.h"
#include "egt/tools.h"
#include "egt/widget.h"
#include "egt/window.h"
#include <chrono>
#include <deque>
#include <egt/asio.hpp>
#include <numeric>

namespace egt
{
inline namespace v1
{
namespace detail
{
struct EventLoopImpl
{
    EventLoopImpl()
        : m_work(egt::asio::make_work_guard(m_io))
    {}

    asio::io_context m_io;
    asio::executor_work_guard<asio::io_context::executor_type> m_work;
};
}

EventLoop::EventLoop() noexcept
    : m_impl(new detail::EventLoopImpl)
{}

asio::io_context& EventLoop::io()
{
    return m_impl->m_io;
}

int EventLoop::wait()
{
    int ret = 0;

    experimental::code_timer(false, "wait: ", [&]()
    {
        ret = m_impl->m_io.run_one();
        if (ret)
        {
            // hmm, libinput async_read will always return something on poll_one()
            // until we have satisfied the handler, so we have to give up at
            // some point
            int count = 10;
            while (m_impl->m_io.poll_one() && count--)
            {}

#ifdef USE_PRIORITY_QUEUE
            m_queue.execute_all();
#endif
        }
    });

    if (!ret)
        invoke_idle_callbacks();

    return ret;
}

static bool do_quit = false;

void EventLoop::quit()
{
    do_quit = true;
    m_impl->m_io.stop();
}

void EventLoop::draw()
{
    experimental::code_timer(false, "draw: ", [&]()
    {
        for (auto& w : windows())
        {
            if (!w->visible())
                continue;

            // draw top level frames and plane frames
            if (w->top_level() || w->flags().is_set(Widget::flag::plane_window))
                w->top_draw();
        }
    });
}

int EventLoop::step()
{
    int ret = 0;
    int count = 10;
    while (count--)
    {
        auto r = m_impl->m_io.poll_one();
        if (!r)
            break;
        ret += r;
    }

    if (ret)
        draw();

    return ret;
}

int EventLoop::run(bool enable_fps)
{
    experimental::Fps fps;

    draw();

    do_quit = false;
    while (!do_quit)
    {
        if (wait())
        {
            draw();

            if (enable_fps)
                fps.end_frame();

            if (enable_fps && fps.ready())
            {
                std::cout << "fps: " << std::round(fps.fps()) << std::endl;
            }
        }
    }

    return 0;
}

void EventLoop::add_idle_callback(event_callback func)
{
    m_idle.push_back(func);
}

void EventLoop::invoke_idle_callbacks()
{
    for (auto& i : m_idle)
    {
        i();
    }
}

EventLoop::~EventLoop()
{}
}
}
