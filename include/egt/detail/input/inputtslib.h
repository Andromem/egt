/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_DETAIL_INPUT_INPUTTSLIB_H
#define EGT_DETAIL_INPUT_INPUTTSLIB_H

/**
 * @file
 * @brief Working with input devices.
 */

#include <egt/asio.hpp>
#include <egt/input.h>
#include <memory>
#include <string>

namespace egt
{
inline namespace v1
{
class Application;

namespace detail
{
struct tslibimpl;

/**
 * Handles reading input from a tslib supported device.
 */
class InputTslib : public Input
{
public:

    explicit InputTslib(Application& app, const std::string& path);

    virtual ~InputTslib();

private:

    void handle_read(const asio::error_code& error);

    /**
     * Application instance.
     */
    Application& m_app;

    /**
     * Input handler to read from the evdev fd.
     */
    asio::posix::stream_descriptor m_input;

    /**
     * tslib implementation container.
     */
    std::unique_ptr<tslibimpl> m_impl;

    /**
     * Whether a current touch event is active internally.
     */
    bool m_active{false};

    /**
     * The last point seen, used for reference internally.
     */
    DisplayPoint m_last_point;
};

}
}
}

#endif
