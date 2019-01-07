/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef EGT_LIBINPUT_H
#define EGT_LIBINPUT_H

/**
 * @file
 * @brief Working with libinput devices.
 */

#include <asio.hpp>
#include <linux/input.h>
#include <egt/input.h>
#include <memory>

struct libinput;
struct libinput_event;

namespace egt
{
    inline namespace v1
    {
        /**
         * Handles populating and reading input events from libinput.
         */
        class LibInput : public detail::IInput
        {
        public:

            LibInput();

            virtual ~LibInput();

        private:

            void handle_event_device_notify(struct libinput_event* ev);
            void handle_event_motion(struct libinput_event* ev);
            void handle_event_absmotion(struct libinput_event* ev);
            bool handle_event_touch(struct libinput_event* ev);
            void handle_event_axis(struct libinput_event* ev);
            void handle_event_keyboard(struct libinput_event* ev);
            void handle_event_button(struct libinput_event* ev);
            void handle_event_swipe(struct libinput_event* ev);
            void handle_event_pinch(struct libinput_event* ev);
            void handle_event_tablet(struct libinput_event* ev);

            void handle_read(const asio::error_code& error);

            asio::posix::stream_descriptor m_input;

            struct libinput* li;
        };

    }
}

#endif
