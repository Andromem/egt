/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * On a SOM1-EK you must first remove the gpio_keys block from DTS to free up
 * the USER button.
 *
 * Then, compile and run like this to receive events for the USER button on
 * PIN_PA29.
 *
 *     ./egt_gpio /dev/gpiochip0 29
 */
#include <egt/ui>
#include <iostream>
#include <linux/gpio.h>

/**
 * Monitors and inserts a gpioevent_request fd into the EGT event loop.
 *
 * To use this, you must first setup a gpioevent_request with the right
 * parameters and then just give the fd to this to monitor asynchronously.
 */
class GpioEventMonitor
{
public:

    using Callback = std::function<void(gpioevent_data& event)>;

    GpioEventMonitor() = delete;

    GpioEventMonitor(int fd, Callback callback)
        : input(egt::Application::instance().event().io()),
          callback(callback),
          buffer(sizeof(gpioevent_data))
    {
        // assign fd to input stream
        input.assign(fd);

        // register first async read
        egt::asio::async_read(input, egt::asio::buffer(buffer),
                              std::bind(&GpioEventMonitor::handle_read,
                                        this,
                                        std::placeholders::_1));
    }

    void handle_read(const egt::asio::error_code& error)
    {
        if (error)
        {
            std::cout << "handle_read: " << error.message() << std::endl;
            return;
        }

        const auto event = reinterpret_cast<gpioevent_data*>(buffer.data());
        callback(*event);

        // register any additional async read
        egt::asio::async_read(input, egt::asio::buffer(buffer),
                              std::bind(&GpioEventMonitor::handle_read,
                                        this,
                                        std::placeholders::_1));
    }

private:
    // input wrapper around the fd
    egt::asio::posix::stream_descriptor input;
    // registered callback
    Callback callback;
    // buffer to hold the gpioevent_data
    std::vector<char> buffer;
};

int main(int argc, char** argv)
{
    if (argc != 3)
    {
        std::cerr << argv[0] << " CHARDEV LINE" << std::endl;
        return 1;
    }

    egt::Application app(argc, argv);
    egt::TopWindow window;

    // label to show the GPIO state
    egt::Label label("none");
    window.add(egt::center(label));

    int fd = open(argv[1], 0);
    if (fd < 0)
    {
        std::cerr << "failed to open " << argv[1] << std::endl;
        return 1;
    }

    // setup the input and edges
    gpioevent_request ereq;
    ereq.lineoffset = std::atoi(argv[2]);
    ereq.handleflags = GPIOHANDLE_REQUEST_INPUT;
    ereq.eventflags = GPIOEVENT_REQUEST_BOTH_EDGES;
    if (ioctl(fd, GPIO_GET_LINEEVENT_IOCTL, &ereq) < 0)
        std::cerr << "GPIO_GET_LINEEVENT_IOCTL failed" << std::endl;

    // read initial state and get it out of the way
    gpiohandle_data data;
    if (ioctl(ereq.fd, GPIOHANDLE_GET_LINE_VALUES_IOCTL, &data) < 0)
        std::cerr << "GPIOHANDLE_GET_LINE_VALUES_IOCTL failed" << std::endl;

    // start a monitor and set the label text when the GPIO changes
    GpioEventMonitor monitor(ereq.fd, [&label](gpioevent_data & event)
    {
        auto status = event.id == GPIOEVENT_EVENT_RISING_EDGE ? "off" : "on";
        std::cout << status << std::endl;
        label.text(status);
    });

    window.show();

    return app.run();
}
