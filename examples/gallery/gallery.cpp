/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <cmath>
#include <cstdio>
#include <iostream>
#include <map>
#include <memory>
#include <egt/ui>
#include <string>
#include <vector>

using namespace std;
using namespace egt;

#define SHARED_PATH "../share/egt/examples/gallery/"

int main(int argc, const char** argv)
{
    Application app(argc, argv);

    set_image_path(SHARED_PATH);

    std::vector<std::string> files = experimental::glob(SHARED_PATH "*trailer*.png");

    TopWindow win;
    win.set_name("win");
    win.palette().set(Palette::BG, Palette::GROUP_NORMAL, Color::BLACK);

    ImageLabel logo(Image("@microchip_logo_white.png"));
    win.add(&logo);
    logo.set_align(alignmask::LEFT | alignmask::TOP, 10);

    auto grid_height = (win.size().h - logo.h()) / 2;

    ScrolledView view0(Rect(0, logo.h(), win.size().w, grid_height));
    view0.palette().set(Palette::BG, Palette::GROUP_NORMAL, Color::BLACK);
    view0.set_name("view0");
    win.add(&view0);

    StaticGrid grid0(Rect(0, 0, files.size() * 150, grid_height), files.size(), 1, 0);
    grid0.set_name("grid0");
    view0.add(&grid0);

    for (auto& file : files)
    {
        grid0.add(new ImageLabel(Image(file)), alignmask::CENTER);
    }

    ScrolledView view1(Rect(0, logo.h() + grid_height + 1, win.size().w, grid_height));
    view1.palette().set(Palette::BG, Palette::GROUP_NORMAL, Color::BLACK);
    view1.set_name("view1");
    win.add(&view1);

    StaticGrid grid1(Rect(0, 0, files.size() * 150, grid_height), files.size(), 1, 0);
    grid1.set_name("grid1");
    view1.add(&grid1);

    for (auto& file : files)
    {
        grid1.add(new ImageLabel(Image(file)), alignmask::CENTER);
    }

    Popup<Window> popup(Size(main_screen()->size().w / 2, main_screen()->size().h / 2));
    popup.set_name("popup");
    Button button("Hello World");
    popup.add(button);
    button.set_align(alignmask::CENTER);
    button.set_name("hw");

    ImageButton settings(Image("settings.png"), "", Rect());
    win.add(&settings);
    settings.set_boxtype(Theme::boxtype::none);
    settings.set_align(alignmask::RIGHT | alignmask::TOP, 10);
    settings.on_event([&popup](eventid event)
    {
        if (event == eventid::MOUSE_UP)
        {
            if (popup.visible())
                popup.hide();
            else
                popup.show(true);
            return 1;
        }
        return 0;
    });
    win.add(&popup);

    win.show();

    return app.run();
}
