/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */

#ifndef INPUT_H
#define INPUT_H

#include <stdio.h>
#include <stdlib.h>
#include <linux/input.h>
#include <fcntl.h>
#include <unistd.h>
#include <string>
#include <cassert>
#include "geometry.h"

namespace mui
{

    enum
    {
	EVT_MOUSE_DOWN = 1,
	EVT_MOUSE_UP,
	EVT_MOUSE_MOVE,
	EVT_ENTER,
	EVT_LEAVE,
	EVT_KEYDOWN,
	EVT_KEYUP,
    };

    Point& mouse_position();

    int& key_position();

    class Input
    {
    public:

    };

    class InputEvDev : public Input
    {
    public:

	InputEvDev(const std::string& path);

	static void process(int fd, uint32_t mask, void *data);

	virtual ~InputEvDev();

    private:
	int m_fd;
    };

#ifdef HAVE_TSLIB
    class InputTslib : public Input
    {
    public:

	InputTslib(const std::string& path);

	static void process(int fd, uint32_t mask, void *data);

	virtual ~InputTslib();

    private:

	static void timer_callback(int fd, void* data);

	int m_fd;
	int m_timerfd;
	bool m_active;
    };
#endif

}

#endif
