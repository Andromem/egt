/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */

#ifndef MUI_INPUT_H
#define MUI_INPUT_H

/**
 * @file
 * @brief Working with input devices.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "asio.hpp"
#include <linux/input.h>
#include <memory>
#include <mui/geometry.h>
#include <string>

namespace mui
{

    /**
     * Event identifiers.
     */
    enum
    {
        EVT_NONE = 0,

        EVT_MOUSE_DOWN = 1,
        EVT_MOUSE_UP,
        EVT_MOUSE_MOVE,
        EVT_BUTTON_DOWN,
        EVT_BUTTON_UP,
        EVT_MOUSE_DBLCLICK,

        /**
         * Sent when a widget gets focus.
         */
        EVT_ENTER,

        /**
         * Sent when a widget loses focus.
         */
        EVT_LEAVE,
        EVT_KEY_DOWN,
        EVT_KEY_UP,
        EVT_KEY_REPEAT,

        EVT_PROPERTY_CHANGE,
    };

    /**
     * Global mouse position.
     *
     * Call this to retrieve the last mouse position, usually in response to a
     * mouse event.
     */
    Point& mouse_position();

    /**
     * Global key value.
     *
     * Call this to retrieve the last key value, usually in response to a key
     * event.
     */
    int& key_value();

    /**
     * Global button value.
     *
     * BTN_LEFT, BTN_RIGHT, BTN_MIDDLE
     */
    int& button_value();

    /**
     * Base input class.
     */
    class IInput
    {
    protected:
        static void dispatch(int event);
    };

    /**
     * Handles reading input events from evdev devices.
     */
    class InputEvDev : public IInput
    {
    public:

        InputEvDev(const std::string& path);

        virtual ~InputEvDev();

    private:
        void handle_read(const asio::error_code& error, std::size_t length);

        asio::posix::stream_descriptor m_input;
        std::vector<char> m_input_buf;
    };

#ifdef HAVE_TSLIB
    namespace detail
    {
        struct tslibimpl;
    }

    class InputTslib : public IInput
    {
    public:

        InputTslib(const std::string& path);

        virtual ~InputTslib();

        //asio::io_context m_io;

    private:

        void handle_read(const asio::error_code& error);

        asio::posix::stream_descriptor m_input;
        bool m_active;

        std::unique_ptr<detail::tslibimpl> m_impl;
    };
#endif

}

#endif
