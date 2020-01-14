/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <cmath>
#include <egt/detail/string.h>
#include <egt/ui>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

using namespace std;
using namespace egt;

using WindowType = Window;

int main(int argc, const char** argv)
{
    Application app(argc, argv, "frames");

    TopWindow win0;
    win0.flags().set(Widget::Flag::no_layout);

    auto a = AlignFlag::top | AlignFlag::center;

    WindowType win1(Size(400, 400));
    win1.color(Palette::ColorId::bg, Palette::red);
    win1.name("red");
    {
        auto label = make_shared<Label>("x,y", Rect(0, 0, 100, 50));
        win1.on_event([&win1, label](Event & event)
        {
            auto p = win1.display_to_local(event.pointer().point);
            label->text(detail::to_string(p));
        }, {EventId::raw_pointer_move});
        label->align(a);
        win1.add(label);
    }
    win0.add(win1);
    win1.move(Point(50, 50));
    auto l1 = make_shared<Label>(detail::to_string(win1.box()), Rect(0, 0, 100, 50));
    l1->align(AlignFlag::center | AlignFlag::bottom);
    win1.add(l1);

    WindowType win2(Size(300, 300));
    win2.color(Palette::ColorId::bg, Palette::blue);
    win2.name("blue");
    {
        auto label = make_shared<Label>("x,y", Rect(0, 0, 100, 50));
        win2.on_event([&win2, label](Event & event)
        {
            auto p = win2.display_to_local(event.pointer().point);
            label->text(detail::to_string(p));
        }, {EventId::raw_pointer_move});
        label->align(a);
        win2.add(label);
    }
    win1.add(win2);
    win2.move(Point(50, 50));
    auto l2 = make_shared<Label>(detail::to_string(win2.box()), Rect(0, 0, 100, 50));
    l2->align(AlignFlag::center | AlignFlag::bottom);
    win2.add(l2);

    WindowType win3(Size(200, 200));
    win3.color(Palette::ColorId::bg, Palette::green);
    auto l3 = make_shared<Label>("win3", Rect(0, 0, 50, 50));
    l3->align(a);
    win3.add(l3);
    win3.name("green");
    win2.add(win3);
    win3.move(Point(50, 50));

    WindowType win4(Size(100, 100));
    win4.color(Palette::ColorId::bg, Palette::purple);
    auto l4 = make_shared<Label>("win4", Rect(0, 0, 50, 50));
    l4->align(a);
    win4.add(l4);
    win4.name("purple");
    win3.add(win4);
    win4.move(Point(50, 50));

    win0.show();
    win1.show();
    win2.show();
    win3.show();
    win4.show();

    app.dump(cout);

    return app.run();
}
