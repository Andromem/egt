/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <egt/ui>
#include <egt/uiloader.h>

#define USE_LOADER
#define SHARED_PATH "../share/egt/examples/basic/"

int main(int argc, const char** argv)
{
    egt::Application app(argc, argv);

#ifdef USE_LOADER
    egt::experimental::UiLoader loader;
    auto window = loader.load(SHARED_PATH "ui.xml");
    window->show();
#else
    egt::TopWindow window;
    egt::Button button(window, "Press Me");
    button.align(egt::alignmask::CENTER);
    window.show();
#endif

    return app.run();
}
