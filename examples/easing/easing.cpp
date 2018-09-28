/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <mui/ui.h>
#include <math.h>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <iostream>
#include <random>
#include <cmath>

using namespace std;
using namespace mui;

#ifdef HAVE_LIBPLANES
#define USE_HARDWARE
#endif

static Animation* animation = 0;

static easing_func curves[] =
{
    easing_linear,
    easing_easy,
    easing_easy_slow,
    easing_extend,
    easing_drop,
    easing_drop_slow,
    easing_snap,
    easing_bounce,
    easing_bouncy,
    easing_rubber,
    easing_spring,
    easing_boing
};

static vector<string> curves_names =
{
    "linear",
    "easy",
    "easy slow",
    "extend",
    "drop",
    "drop slow",
    "snap",
    "bounce",
    "bouncy",
    "rubber",
    "spring",
    "boing"
};

class MyListBox : public ListBox
{
public:
    MyListBox(const std::vector<std::string>& items,
              const Point& point = Point(),
              const Size& size = Size())
        : ListBox(items, point, size)
    {}

    void on_selected(int index)
    {
        animation->stop();
        animation->set_easing_func(curves[index]);
        animation->start();
    }
};

class MainWindow : public Window
{
public:
    MainWindow()
        : Window()
    {}

    int load()
    {
        Image* img = new Image("background.png");
        double scale = (double)w() / (double)img->w();
        add(img);
        img->scale(scale, scale);

        MyListBox* list1 = new MyListBox(curves_names, Point(w() - 100, 0), Size(100, h()));
        add(list1);
        list1->selected(7);

#ifndef USE_HARDWARE
        m_box = new Image("ball.png");
        add(m_box);
#else
        Image* image = new Image("ball.png");
        // There is a bug on 9x5 that if the plane is all the way out of view
        // then it will cause glitches. So, we create the height (which will
        // be invisible), to always keep a portion of the plane on screen
        // alternate of making the plane the same exact size as the image.
        m_box = new PlaneWindow(Size(100, 200));
        m_box->add(image);
        m_box->show();
#endif
        m_box->position(w() / 2 - m_box->w() / 2, -110);

        return 0;
    }

    void move_item(int x)
    {
        int pos = x;

        Rect to(m_box->box());
        to.y = pos;
        bool visible = Rect::is_intersect(Rect::merge(to, m_box->box()), this->box());

        if (visible)
        {
            m_box->move(m_box->x(), pos);
        }
        else
        {
            m_box->position(m_box->x(), pos);
        }
    }

private:
#ifndef USE_HARDWARE
    Image* m_box;
#else
    PlaneWindow* m_box;
#endif
};

static struct ResetTimer : public Timer
{
    ResetTimer()
        : Timer(1000)
    {}

    void timeout()
    {
        animation->start();
    }

} reset_timer;

static struct MyAnimationTimer : public PeriodicTimer
{
    MyAnimationTimer()
        : PeriodicTimer(30)
    {}

    void timeout()
    {
        if (animation->running())
            animation->next();
        else
            reset_timer.start();
    }

} animation_timer;

int main()
{
    Application app;

    set_image_path("/root/mui/share/mui/examples/easing/");

    MainWindow window;

    animation = new Animation(-110, window.h() - 100/*380*/, [](float value, void* data)
    {
        MainWindow* window = reinterpret_cast<MainWindow*>(data);
        window->move_item(value);
    }, 2000, easing_linear, &window);

    window.load();

    Label label1("CPU: 0%",
                 Point(40, window.size().h - 40),
                 Size(100, 40));
    label1.palette().set(Palette::TEXT, Palette::GROUP_NORMAL, Color::WHITE)
    .set(Palette::BG, Palette::GROUP_NORMAL, Color::TRANSPARENT);
    window.add(&label1);

    CPUMonitorUsage tools;
    PeriodicTimer cputimer(1000);
    cputimer.add_handler([&label1, &tools]()
    {
        tools.update();
        ostringstream ss;
        ss << "CPU: " << (int)tools.cpu_usage[0] << "%";
        label1.text(ss.str());
    });
    cputimer.start();

    animation_timer.start();

    return app.run();
}
