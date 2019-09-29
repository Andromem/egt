/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <egt/ui>
#include <functional>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

using namespace egt;
using namespace std;

static std::vector<std::pair<std::string, std::function<int(Application& app)>>> examples =
{
    {
        "label1", [](Application & app)
        {
            /// @[label1]
            TopWindow window;
            Label label(window, "I'm a Label");
            center(label);
            window.show();
            /// @[label1]
            return app.run();
        }
    },
    {
        "label2", [](Application & app)
        {
            /// @[label2]
            TopWindow window;
            Label label(window, "I'm a Bigger Label");
            label.set_font(Font("Sans", 30));
            center(label);
            window.show();
            /// @[label2]
            return app.run();
        }
    },
    {
        "label3", [](Application & app)
        {
            /// @[label3]
            TopWindow window;
            Label label(window, "I'm a Red Label");
            label.set_color(Palette::ColorId::label_text, Palette::red);
            center(label);
            window.show();
            /// @[label3]
            return app.run();
        }
    },
    {
        "button0", [](Application & app)
        {
            /// @[button0]
            TopWindow window;
            auto button = std::make_shared<Button>("Button");
            window.add(center(button));
            window.show();
            /// @[button0]
            return app.run();
        }
    },
    {
        "button1", [](Application & app)
        {
            /// @[button1]
            TopWindow window;
            Button button(window, "Button");
            center(button);
            window.show();
            /// @[button1]
            return app.run();
        }
    },
    {
        "button2", [](Application & app)
        {
            /// @[button2]
            TopWindow window;
            Button button(window, "Press Me");
            center(button);

            button.on_event([](Event & event)
            {
                std::cout << "received event " << event.id() << std::endl;
            });
            window.show();
            /// @[button2]
            return app.run();
        }
    },
    {
        "button3", [](Application & app)
        {
            /// @[button3]
            TopWindow window;
            Button button(window, "Change Colors");
            center(button);

            experimental::ColorMap colors({Palette::red, Palette::green});
            button.on_click([&button, &colors](Event & event)
            {
                static float step = 0;
                button.set_color(Palette::ColorId::button_bg,
                                 colors.interp(step += 0.1));
                button.set_color(Palette::ColorId::button_bg,
                                 colors.interp(1.0 - step),
                                 Palette::GroupId::active);
                if (step >= 1.0)
                    step = 0.;
            });

            window.show();
            /// @[button3]
            return app.run();
        }
    },
    {
        "image1", [](Application & app)
        {
            /// @[image1]
            TopWindow window;
            HorizontalBoxSizer sizer;
            window.add(center(sizer));

            ImageLabel label1(sizer, Image("@unlock.png"));
            ImageLabel label2(sizer, Image("@paint.png"));
            ImageLabel label3(sizer, Image("@battery.png"));
            ImageLabel label4(sizer, Image("@ok.png"));

            window.show();
            /// @[image1]
            return app.run();
        }
    },
    {
        "animation0", [](Application & app)
        {
            /// @[animation0]
            TopWindow window;
            Button button(window, "Can you see me now?");
            center(button);

            PropertyAnimatorF animation;
            animation.set_starting(0.0);
            animation.set_ending(1.0);
            animation.set_duration(std::chrono::seconds(5));
            animation.set_easing_func(easing_linear);
            animation.on_change(std::bind(&Button::set_alpha,
                                          &button, std::placeholders::_1));
            animation.start();

            window.show();
            /// @[animation0]
            return app.run();
        }
    },
    {
        "animation1", [](Application & app)
        {
            /// @[animation1]
            TopWindow window;
            Button button(window, "I Move");
            button.move_to_center(window.center());

            PropertyAnimator animation;
            animation.set_starting(button.y());
            animation.set_ending(button.y() + 100);
            animation.set_duration(std::chrono::seconds(5));
            animation.set_easing_func(easing_linear);
            animation.on_change(std::bind(&Button::set_y,
                                          &button, std::placeholders::_1));

            animation.start();

            window.show();
            /// @[animation1]
            return app.run();
        }
    },
    {
        "animation2", [](Application & app)
        {
            /// @[animation2]
            TopWindow window;
            Button button(window, "I Move");
            button.move_to_center(window.center());
            const Point starting_point = button.point();

            PropertyAnimator animation;
            animation.set_starting(0);
            animation.set_ending(100);
            animation.set_duration(std::chrono::seconds(5));
            animation.set_easing_func(easing_linear);
            animation.on_change([&button, starting_point](int value)
            {
                auto point = starting_point;
                point += value;
                button.move(point);
            });

            animation.start();

            window.show();
            /// @[animation2]
            return app.run();
        }
    },
    {
        "animation3", [](Application & app)
        {
            /// @[animation3]
            TopWindow window;
            Label label(window, "EGT");
            center(label);

            PropertyAnimatorF animation;
            animation.set_starting(8.f);
            animation.set_ending(120.f);
            animation.set_duration(std::chrono::seconds(5));
            animation.set_easing_func(easing_linear);
            animation.on_change([&label](float value)
            {
                auto font = label.font();
                font.set_size(value);
                label.set_font(font);
            });

            animation.start();

            window.show();
            /// @[animation3]
            return app.run();
        }
    },
    {
        "animation4", [](Application & app)
        {
            /// @[animation4]
            TopWindow window;
            Button button(window, "Hello World");
            center(button);

            PropertyAnimatorF animation;
            animation.set_starting(0.0);
            animation.set_ending(main_screen()->max_brightness());
            animation.set_duration(std::chrono::seconds(5));
            animation.set_easing_func(easing_linear);
            animation.on_change(std::bind(&Screen::set_brightness,
                                          main_screen(),
                                          std::placeholders::_1));
            animation.start();

            window.show();
            /// @[animation4]
            return app.run();
        }
    },
    {
        "events1", [](Application & app)
        {
            /// @[events1]
            TopWindow window;
            VerticalBoxSizer sizer;
            window.add(expand(sizer));

            Button button(sizer, "Disable Me");
            CheckBox checkbox(sizer, "Button Disabled");
            checkbox.on_event([&button, &checkbox](Event & event)
            {
                button.set_disabled(checkbox.checked());
            }, {eventid::property_changed});

            window.show();
            /// @[events1]
            return app.run();
        }
    },
    {
        "timer1", [](Application & app)
        {
            /// @[timer1]
            TopWindow window;
            Label label(window);
            label.set_text_align(alignmask::center);
            center(label);

            PeriodicTimer timer(std::chrono::seconds(1));
            timer.on_timeout([&label]()
            {
                static auto count = 0;
                std::ostringstream ss;
                ss << ++count;
                label.set_text(ss.str());
            });
            timer.start();

            window.show();
            /// @[timer1]
            return app.run();
        }
    },
    {
        "theme1", [](Application & app)
        {
            /// @[theme1]
            TopWindow window;

            struct CustomTheme : public Theme
            {
                using Theme::Theme;
                void apply() override
                {
                    Theme::apply();

                    palette().set(Palette::ColorId::button_bg,
                                  Palette::GroupId::normal,
                                  Palette::green);
                    palette().set(Palette::ColorId::bg,
                                  Palette::GroupId::normal,
                                  Palette::blue);
                }
            } theme;

            theme.apply();
            window.set_theme(theme);

            Button button0(window, "Button");
            center(left(button0));
            Button button1(window, "Button");
            center(right(button1));

            window.show();
            /// @[theme1]
            return app.run();
        }
    },
    {
        "theme2", [](Application & app)
        {
            /// @[theme2]
            TopWindow window;

            auto theme = window.theme();
            theme.palette().set(Palette::ColorId::button_bg,
                                Palette::GroupId::normal,
                                Palette::green);
            theme.palette().set(Palette::ColorId::bg,
                                Palette::GroupId::normal,
                                Palette::blue);

            window.set_theme(theme);

            Button button0(window, "Button");
            center(left(button0));

            Button button1(window, "Button");
            center(right(button1));

            window.show();
            /// @[theme2]
            return app.run();
        }
    },
    {
        "sound0", [](Application & app)
        {
            TopWindow window;

            /// @[sound0]
            experimental::Sound sound("assets/tom.wav");
            sound.play();
            /// @[sound0]

            window.show();
            return app.run();
        }
    },
    {
        "sound1", [](Application & app)
        {
            /// @[sound1]
            TopWindow window;

            experimental::Sound sound("assets/tom.wav");

            Button button(window, "Play Sound");
            center(button);
            button.on_click([&sound](Event&)
            {
                sound.play();
            });

            window.show();
            /// @[sound1]
            return app.run();
        }
    },
    {
        "audio1", [](Application & app)
        {
            TopWindow window;

            window.add(center(std::make_shared<ImageLabel>(Image("@volume_up.png"))));
            window.show();

            /// @[audio1]
            AudioPlayer player("assets/concerto.mp3");
            player.play();
            /// @[audio1]

            window.show();
            return app.run();
        }
    },
    {
        "video1", [](Application & app)
        {
            /// @[video1]
            TopWindow window;

            VideoWindow player(window.content_area(), "assets/video.mp4");
            window.add(player);
            player.set_volume(5);
            player.show();
            player.play();

            window.show();
            /// @[video1]
            return app.run();
        }
    },
};

int main(int argc, const char** argv)
{
    Application app;

    if (argc == 2)
    {
        auto f = std::find_if(examples.begin(),
                              examples.end(),
                              [argv](const std::pair<std::string, std::function<int(Application& app)>>& a)
        {
            return a.first == std::string(argv[1]);
        });

        if (f != examples.end())
            return f->second(app);

        return 1;
    }
    else if (argc == 3)
    {
        for (const auto& f : examples)
            cout << f.first << endl;

        return 0;
    }

    for (const auto& f : examples)
    {
        Timer killtimer(std::chrono::seconds(5));
        killtimer.on_timeout([&app]()
        {
            app.event().quit();
        });
        killtimer.start();
        f.second(app);
    }

    return 1;
}
