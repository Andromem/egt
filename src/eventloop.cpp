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
#include <cstdlib>
#include <deque>
#include <egt/asio.hpp>
#include <numeric>
#include <spdlog/spdlog.h>

namespace egt
{
inline namespace v1
{
namespace detail
{
struct EventLoopImpl
{
    asio::io_context m_io;
    asio::executor_work_guard<asio::io_context::executor_type> m_work{egt::asio::make_work_guard(m_io)};
};
}

EventLoop::EventLoop() noexcept
    : m_impl(new detail::EventLoopImpl)
{}

asio::io_context& EventLoop::io()
{
    return m_impl->m_io;
}

static bool time_event_loop_enabled()
{
    static int value = 0;
    if (value == 0)
    {
        if (std::getenv("EGT_TIME_EVENTLOOP"))
            value += 1;
        else
            value -= 1;
    }
    return value == 1;
}

int EventLoop::wait()
{
    int ret = 0;

    detail::code_timer(time_event_loop_enabled(), "wait: ", [&]()
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
    detail::code_timer(time_event_loop_enabled(), "draw: ", [&]()
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
    m_impl->m_io.restart();
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

    SPDLOG_TRACE("EventLoop::run() exiting");

    return 0;
}

void EventLoop::add_idle_callback(event_callback func)
{
    m_idle.emplace_back(std::move(func));
}

void EventLoop::invoke_idle_callbacks()
{
    for (auto& i : m_idle)
    {
        i();
    }
}

EventLoop::~EventLoop() = default;

}
}
