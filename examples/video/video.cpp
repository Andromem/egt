/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <chrono>
#include <cxxopts.hpp>
#include <egt/detail/string.h>
#include <egt/ui>
#include <iostream>
#include <sstream>

// warning: not utf-8 safe
static std::string line_break(const std::string& in, size_t width = 50)
{
    std::string out;
    std::string tmp;
    char last = '\0';
    size_t i = 0;

    for (auto& cur : in)
    {
        if (++i == width)
        {
            tmp = egt::detail::ltrim(tmp);
            out += "\n" + tmp;
            i = tmp.length();
            tmp.clear();
        }
        else if (isspace(cur) && !isspace(last))
        {
            out += tmp;
            tmp.clear();
        }
        tmp += cur;
        last = cur;
    }
    return out + tmp;
}

template<class T>
static inline T ns2ms(T n)
{
    return n / 1000000UL;
}

int main(int argc, char** argv)
{
    cxxopts::Options options(argv[0], "play video file");
    options.add_options()
    ("h,help", "Show help")
    ("i,input", "URI to video file", cxxopts::value<std::string>())
    ("width", "Width of the stream", cxxopts::value<int>()->default_value("320"))
    ("height", "Height of the stream", cxxopts::value<int>()->default_value("192"))
    ("f,format", "Pixel format", cxxopts::value<std::string>()->default_value("yuv420"), "[egt::PixelFormat]");
    auto args = options.parse(argc, argv);

    if (args.count("help") ||
        !args.count("input"))
    {
        std::cout << options.help() << std::endl;
        return 0;
    }

    egt::Size size(args["width"].as<int>(), args["height"].as<int>());
    auto format = egt::detail::enum_from_string<egt::PixelFormat>(args["format"].as<std::string>());
    auto input(args["input"].as<std::string>());

    egt::Application app(argc, argv, "video");
#ifdef EXAMPLEDATA
    egt::add_search_path(EXAMPLEDATA);
#endif

    egt::TopWindow win;
    win.color(egt::Palette::ColorId::bg, egt::Palette::black);

    egt::Label errlabel;
    errlabel.color(egt::Palette::ColorId::label_text, egt::Palette::white);
    errlabel.align(egt::AlignFlag::expand);
    errlabel.text_align(egt::AlignFlag::center | egt::AlignFlag::top);
    win.add(errlabel);

    // player after label to handle drag
    egt::VideoWindow player(size, format, egt::WindowHint::overlay);
    player.move_to_center(win.center());
    player.volume(5);
    win.add(player);

    egt::Window ctrlwindow(egt::Size(win.width(), 72), egt::PixelFormat::argb8888);
    ctrlwindow.align(egt::AlignFlag::bottom | egt::AlignFlag::center);
    ctrlwindow.color(egt::Palette::ColorId::bg, egt::Palette::transparent);
    if (!ctrlwindow.flags().is_set(egt::Window::Flag::plane_window))
        ctrlwindow.fill_flags(egt::Theme::FillFlag::blend);
    win.add(ctrlwindow);

    egt::HorizontalBoxSizer hpos;
    hpos.resize(ctrlwindow.size());
    ctrlwindow.add(hpos);

    auto logo = std::make_shared<egt::ImageLabel>(egt::Image("icon:egt_logo_icon.png;32"));
    logo->margin(10);
    hpos.add(logo);

    egt::ImageButton playbtn(egt::Image("res:pause_png"));
    playbtn.fill_flags().clear();
    hpos.add(playbtn);

    playbtn.on_click([&playbtn, &player](egt::Event&)
    {
        if (player.playing())
        {
            if (player.pause())
                playbtn.image(egt::Image("res:play_png"));
        }
        else
        {
            if (player.play())
                playbtn.image(egt::Image("res:pause_png"));
        }
    });

    egt::Slider position(0, 100, 0, egt::Orientation::horizontal);
    position.width(ctrlwindow.width() * 0.20);
    position.align(egt::AlignFlag::expand_vertical);
    position.slider_flags().set({egt::Slider::SliderFlag::round_handle});
    hpos.add(position);

    position.on_value_changed([&position, &player]()
    {
        auto state = player.playing();
        if (state)
            player.pause();

        player.seek((player.duration() * position.value()) / position.end());

        if (state)
            player.play();

    });

    egt::ImageButton volumei(egt::Image("res:volumeup_png"));
    volumei.fill_flags().clear();
    hpos.add(volumei);

    egt::Slider volume(egt::Size(ctrlwindow.width() * 0.10,
                                 ctrlwindow.height()),
                       0, 10, 0, egt::Orientation::horizontal);
    hpos.add(volume);
    volume.slider_flags().set({egt::Slider::SliderFlag::round_handle});
    volume.value(5);
    player.volume(5.0);
    volume.on_value_changed([&volume, &player]()
    {
        auto val = static_cast<double>(volume.value());
        player.volume(val);
    });
    volume.value(5);

    egt::ImageButton fullscreenbtn(egt::Image("res:fullscreen_png"));
    fullscreenbtn.fill_flags().clear();
    hpos.add(fullscreenbtn);

    const auto vscale
        = static_cast<float>(egt::Application::instance().screen()->size().width()) / size.width();

    fullscreenbtn.on_click([&fullscreenbtn, &player, vscale, &win](egt::Event&)
    {
        static bool scaled = true;
        if (scaled)
        {
            player.move(egt::Point(0, 0));
            player.scale(vscale, vscale);
            fullscreenbtn.image(egt::Image("res:fullscreen_exit_png"));
            scaled = false;
        }
        else
        {
            player.move_to_center(win.center());
            player.scale(1.0, 1.0);
            fullscreenbtn.image(egt::Image("res:fullscreen_png"));
            scaled = true;
        }
    });

    egt::ImageButton loopbackbtn(egt::Image("res:repeat_one_png"));
    loopbackbtn.fill_flags().clear();
    hpos.add(loopbackbtn);

    loopbackbtn.on_click([&loopbackbtn, &player](egt::Event&)
    {
        if (player.loopback())
        {
            loopbackbtn.image(egt::Image("res:repeat_one_png"));
            player.loopback(false);
        }
        else
        {
            loopbackbtn.image(egt::Image("res:repeat_png"));
            player.loopback(true);
        }
    });

    egt::Label cpulabel("CPU: 0%");
    cpulabel.color(egt::Palette::ColorId::label_text, egt::Palette::white);
    cpulabel.margin(5);
    hpos.add(cpulabel);

    egt::experimental::CPUMonitorUsage tools;
    egt::PeriodicTimer cputimer(std::chrono::seconds(1));
    cputimer.on_timeout([&cpulabel, &tools]()
    {
        tools.update();
        std::ostringstream ss;
        ss << "CPU: " << static_cast<int>(tools.usage()) << "%";
        cpulabel.text(ss.str());
    });
    cputimer.start();

    // wait to start playing the video until the window is shown
    win.on_show([&player, input, &position, &ctrlwindow, &volume, &volumei, &hpos]()
    {
        player.media(input);

        if (!player.has_audio())
        {
            position.width(ctrlwindow.width() * 0.45);
            hpos.remove(&volume);
            hpos.remove(&volumei);
        }

        player.play();
    });

    player.on_position_changed([&player, &position]()
    {
        if (player.playing())
        {
            position.value((ns2ms<double>(player.position()) /
                            ns2ms<double>(player.duration())) * 100.);
        }
    });

    player.on_error([&errlabel](const std::string & err)
    {
        errlabel.text(line_break(err));
    });

    player.on_event([&player, &win, vscale, size](egt::Event & event)
    {
        static egt::Point drag_start_point;
        switch (event.id())
        {
        case egt::EventId::pointer_drag_start:
        {
            drag_start_point = player.box().point();
            break;
        }
        case egt::EventId::pointer_drag:
        {
            if (!(egt::detail::float_equal(player.hscale(), vscale)))
            {
                auto diff = event.pointer().drag_start - event.pointer().point;
                auto p = drag_start_point - egt::Point(diff.x(), diff.y());
                auto max_x = win.width() - size.width();
                auto max_y = win.height() - size.height();
                if (p.x() >= max_x)
                    p.x(max_x);
                if (p.x() < 0)
                    p.x(0);
                if (p.y() >= max_y)
                    p.y(max_y);
                if (p.y() < 0)
                    p.y(0);
                player.move(p);
            }
            break;
        }
        default:
            break;
        }
    });

    ctrlwindow.show();
    player.show();
    win.show();

    return app.run();
}
