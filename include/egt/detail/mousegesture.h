/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_DETAIL_MOUSEGESTURE_H
#define EGT_DETAIL_MOUSEGESTURE_H

#include <chrono>
#include <egt/detail/meta.h>
#include <egt/event.h>
#include <egt/geometry.h>
#include <egt/timer.h>
#include <string>
#include <vector>

/**
 * @file
 * @brief Mouse gesture support.
 */
namespace egt
{
inline namespace v1
{
namespace detail
{

/**
 * Basic class for interpreting mouse events.
 *
 * For now, this only supports single mouse click, long click, and drag events.
 * The premise behind this class is to interpret raw mouse events and turn them
 * into higher level meaning.  Because some of those events can be asynchronous,
 * all events are generated through callbacks registered with on_async_event().
 */
class EGT_API MouseGesture
{
public:

    MouseGesture() noexcept;
    EGT_OPS_NOCOPY_MOVE_EXCEPT(MouseGesture);
    virtual ~MouseGesture() noexcept = default;

    /// Type for mouse event callback.
    using MouseCallback = std::function<void(Event& event)>;

    /**
     * Register a callback function to handle the async mouse events.
     */
    void on_async_event(MouseCallback callback);

    /**
     * Pass the raw EventId to this function to get the emulated mouse event.
     */
    virtual Event handle(const Event& event);

    /**
     * Start.
     */
    virtual void start(const DisplayPoint& point);

    /// Get pointer start position.
    const DisplayPoint& mouse_start() const
    {
        return m_mouse_start_pos;
    }

    /// Is active?
    bool active() const { return m_active; }

    /// Is dragging?
    bool dragging() const { return m_dragging; }

    /**
     * Stop any active dragging state.
     */
    virtual void stop();

protected:

    /// Invoke an event on each of the handlers.
    virtual void invoke_handlers(Event& event);

    /// Currently processing subsequent events.
    bool m_active{false};

    /// Currently in the dragging state.
    bool m_dragging{false};

    /// The starting position of the mouse.
    DisplayPoint m_mouse_start_pos;

    /// Type for array of registered callbacks.
    using CallbackArray = std::vector<MouseCallback>;

    /// Registered callback functions.
    CallbackArray m_callbacks;

    /// Async timer for detecting long clicks.
    PeriodicTimer m_long_click_timer;
};

}

}
}

#endif
