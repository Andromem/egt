/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_EVENTLOOP_H
#define EGT_EVENTLOOP_H

/**
 * @file
 * @brief Working with the event loop.
 */

#include <egt/detail/meta.h>
#include <functional>
#include <memory>
#include <vector>

namespace egt
{

namespace asio
{
class io_context;
}

inline namespace v1
{

namespace detail
{
class PriorityQueue;
}

/**
 * Event loop interface.
 */
class EGT_API EventLoop : public detail::NonCopyable
{
public:

    EventLoop() noexcept;

    /**
     * Get a reference to the internal ASIO io_context object.
     */
    asio::io_context& io();

    /**
     * Perform a draw.
     *
     * @note You do not normally need to call this directly.  It is called by
     * step() and run() automatically.
     */
    virtual void draw();

    /**
     * Run the event loop.
     *
     * This will not return until quit() is called.
     *
     * @return The number of events handled.
     */
    virtual int run();

    /**
     * Single step on the event loop.
     *
     * This is the same as calling:
     * @code{.cpp}
     * auto ret = poll();
     * if (ret)
     *     draw();
     * @endcode
     *
     * @note If calling this manually, this will not invoke any idle callbacks.
     * @return The number of events handled.
     */
    virtual int step();

    /**
     * Run some pending events and return.
     *
     * @note This does not call draw().  If poll() returns a non-zero value,
     * you must manually call draw().
     * @return The number of events handled.
     */
    virtual int poll();

    /**
     * Quit the event loop.
     *
     * This will cause the run() function to return.
     */
    virtual void quit();

    /**
     * Event callback function definition.
     */
    using IdleCallback = std::function<void ()>;

    /**
     * Add a callback to be called any time the event loop is idle.
     */
    void add_idle_callback(IdleCallback func);

    /// @private
    detail::PriorityQueue& queue();

    virtual ~EventLoop();

protected:

    /**
     * Wait for an event to occur.
     */
    int wait();

    /**
     * Called to invoke idle callbacks.
     */
    void invoke_idle_callbacks();

    struct EventLoopImpl;

    /**
     * Internal event loop implementation.
     */
    std::unique_ptr<EventLoopImpl> m_impl;

    /**
     * Registered idle callbacks.
     */
    std::vector<IdleCallback> m_idle;

    /**
     * Used internally to determine whether the event loop should exit.
     */
    bool m_do_quit{false};
};


}
}

#endif
