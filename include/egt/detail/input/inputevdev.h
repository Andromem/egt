/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_INPUTEVDEV_H
#define EGT_INPUTEVDEV_H

/**
 * @file
 * @brief Working with input devices.
 */

#include <asio.hpp>
#include <egt/input.h>
#include <string>
#include <vector>

namespace egt
{
inline namespace v1
{
namespace detail
{

/**
 * Handles reading input events from evdev devices.
 */
class InputEvDev : public Input
{
public:

    explicit InputEvDev(const std::string& path);

    virtual ~InputEvDev();

private:
    void handle_read(const asio::error_code& error, std::size_t length);

    asio::posix::stream_descriptor m_input;
    std::vector<char> m_input_buf;
};

}
}
}

#endif
