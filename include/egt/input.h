/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_INPUT_H
#define EGT_INPUT_H

/**
 * @file
 * @brief Working with input devices.
 */

#include <asio.hpp>
#include <egt/geometry.h>
#include <egt/object.h>
#include <linux/input.h>
#include <memory>
#include <string>

namespace egt
{
inline namespace v1
{
class Widget;

/**
 * Global mouse position.
 *
 * Call this to retrieve the last mouse position, usually in response to a
 * mouse event.
 */
Point& event_mouse();

/**
 * Global button value.
 *
 * BTN_LEFT, BTN_RIGHT, BTN_MIDDLE
 */
int& event_button();

/**
 * Global key value.
 *
 * Call this to retrieve the last key value, usually in response to a key
 * event.
 */
int& event_key();

/**
 * Global key code.
 */
int& event_code();

/**
 * Get the current widget which has the mouse grabbed, or nullptr.
 */
Widget* mouse_grab();

/**
 * Set the current widget to grab the mouse.
 *
 * @param[in] widget The widget to grab all mouse events, or nullptr.
 */
void mouse_grab(Widget* widget);

/**
 * Set the keyboatd focus of a widget.
 *
 * This will result in calling on_gain_focus() for the new widget and
 * on_lost_focus() for any previous widget.
 */
void keyboard_focus(Widget* widget);

/**
 * Get the current widget which has the keyboard focus, or nullptr.
 */
Widget* keyboard_focus();

namespace detail
{
/**
 * Abstract input class.
 */
class IInput
{
public:
    /**
     * Dispatch the event globally.
     */
    static void dispatch(eventid event);

    static detail::Object& global_input()
    {
        return m_global_input;
    }

protected:
    static detail::Object m_global_input;
};

}

/**
 * Handles reading input events from evdev devices.
 */
class InputEvDev : public detail::IInput
{
public:

    explicit InputEvDev(const std::string& path);

    virtual ~InputEvDev();

private:
    void handle_read(const asio::error_code& error, std::size_t length);

    asio::posix::stream_descriptor m_input;
    std::vector<char> m_input_buf;
};

namespace detail
{
struct tslibimpl;
}

/**
 * Handles reading input from a tslib supported device.
 */
class InputTslib : public detail::IInput
{
public:

    explicit InputTslib(const std::string& path);

    virtual ~InputTslib();

private:

    void handle_read(const asio::error_code& error);

    asio::posix::stream_descriptor m_input;
    std::unique_ptr<detail::tslibimpl> m_impl;
    bool m_active{false};
};

}
}

#endif
