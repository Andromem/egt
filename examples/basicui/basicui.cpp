/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/// @[Example]
#include <egt/ui>
#include <egt/uiloader.h>

int main(int argc, const char** argv)
{
    egt::Application app(argc, argv, "basicui");

    egt::experimental::UiLoader loader;
    auto window = loader.load(egt::resolve_file_path("ui.xml"));
    window->show();

    return app.run();
}
/// @[Example]
