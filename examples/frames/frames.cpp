/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <egt/detail/string.h>
#include <egt/ui>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

using WindowType = egt::Window;

static std::shared_ptr<WindowType> create_window(const egt::Size& size,
        const egt::Color& color,
        const std::string& name)
{
    auto win = std::make_shared<WindowType>(size);
    win->color(egt::Palette::ColorId::bg, color);
    win->name(name);

    auto label = std::make_shared<egt::Label>("x,y", egt::Rect(0, 0, 100, 50));
    win->on_event([win, label](egt::Event & event)
    {
        auto p = win->display_to_local(event.pointer().point);
        label->text(egt::detail::to_string(p));
    }, {egt::EventId::raw_pointer_move});
    label->align(egt::AlignFlag::top | egt::AlignFlag::center);
    win->add(label);

    win->move(egt::Point(50, 50));
    auto l1 = std::make_shared<egt::Label>(egt::detail::to_string(win->box()),
                                           egt::Rect(0, 0, 100, 50));
    l1->align(egt::AlignFlag::center | egt::AlignFlag::bottom);
    win->add(l1);

    return win;
}

int main(int argc, const char** argv)
{
    egt::Application app(argc, argv, "frames");

    egt::TopWindow win0;
    win0.flags().set(egt::Widget::Flag::no_layout);
    win0.show();

    const std::vector<std::pair<egt::Color, std::string>> items =
    {
        {egt::Palette::red, "red"},
        {egt::Palette::blue, "blue"},
        {egt::Palette::green, "green"},
        {egt::Palette::purple, "purple"},
    };

    WindowType* parent = &win0;
    egt::Size size(400, 400);

    for (auto& item : items)
    {
        auto win = create_window(size, item.first, item.second);
        parent->add(win);
        win->show();

        parent = win.get();
        size -= egt::Size(100, 100);
    }

    app.dump(std::cout);

    return app.run();
}
