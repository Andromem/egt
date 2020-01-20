/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <egt/ui>
#include <iostream>

using namespace std;
using namespace egt;

int main(int argc, const char** argv)
{
    Application app(argc, argv);
    egt::experimental::CameraCapture capture("output.avi");

    capture.on_error([&capture]()
    {
        cout << "error: " << capture.error_message() << endl;
    });

    Timer stop_timer(std::chrono::seconds(10));
    stop_timer.on_timeout([&capture, &app]()
    {
        cout << "stopping capture." << endl;
        capture.stop();
        app.quit();
    });

    if (capture.start())
    {
        cout << "capturing for 10 seconds..." << flush;
        stop_timer.start();
    }

    capture.on_error([&capture, &app]()
    {
        cout << " Error:  " << capture.error_message() << endl;
        capture.stop();
        app.quit();
    });

    return app.run();
}
