/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/app.h"
#include "egt/eventloop.h"
#include "egt/timer.h"

using namespace std;

namespace egt
{

    Timer::Timer() noexcept
        : m_timer(main_app().event().io())
    {}

    Timer::Timer(std::chrono::milliseconds duration) noexcept
        : m_timer(main_app().event().io()),
          m_duration(duration)
    {}

    void Timer::start()
    {
        m_timer.cancel();
        m_timer.expires_from_now(m_duration);
        m_timer.async_wait(std::bind(&Timer::timer_callback, this, std::placeholders::_1));
        m_running = true;
    }

    void Timer::start_with_duration(std::chrono::milliseconds duration)
    {
        m_duration = duration;
        start();
    }

    void Timer::change_duration(std::chrono::milliseconds duration)
    {
        if (m_running)
        {
            cancel();
            m_duration = duration;
            start();
        }
        else
        {
            m_duration = duration;
        }
    }

    void Timer::cancel()
    {
        do_cancel();
    }

    void Timer::do_cancel()
    {
        m_timer.cancel();
    }

    void Timer::timer_callback(const asio::error_code& error)
    {
        m_running = false;

        if (error)
            return;

        timeout();
    }

    void Timer::timeout()
    {
        invoke_handlers();
    }

    void Timer::invoke_handlers()
    {
        for (auto& callback : m_callbacks)
            callback();
    }

    Timer::~Timer()
    {
        do_cancel();
    }

    PeriodicTimer::PeriodicTimer() noexcept
    {}

    PeriodicTimer::PeriodicTimer(std::chrono::milliseconds interval) noexcept
        : Timer(interval)
    {}

    void PeriodicTimer::start()
    {
        m_timer.cancel();
        m_timer.expires_from_now(m_duration);
        m_timer.async_wait(std::bind(&PeriodicTimer::timer_callback, this,
                                     std::placeholders::_1));
        m_running = true;
    }

    void PeriodicTimer::timer_callback(const asio::error_code& error)
    {
        m_running = false;

        if (error)
            return;

        start();
        timeout();
    }

}
