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
     * Wait for an event to occur.
     */
    virtual int wait();

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
     */
    virtual int run();

    /**
     * Single step on the event loop.
     *
     * @note If calling this manually, this will not invoke any idle callbacks.
     */
    virtual int step();

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
    using event_callback = std::function<void ()>;

    /**
     * Add a callback to be called any time the event loop is idle.
     *
     * This is useful for executing long running tasks that should otherwise
     * not impact any other event handling performance.
     */
    void add_idle_callback(event_callback func);

    /// @private
    detail::PriorityQueue& queue();

    virtual ~EventLoop();

protected:

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
    std::vector<event_callback> m_idle;

    /**
     * Used internally to determine whether the event loop should exit.
     */
    bool m_do_quit{false};
};


}
}

#endif
