/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <egt/ui>
#include <math.h>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <iostream>

using namespace std;
using namespace egt;

static LineChart::data_array create_sin_data()
{
    LineChart::data_array data;
    for (float i = 0.; i < M_PI * 4; i += 0.2)
    {
        LineChart::data_pair p;
        p.x = i;
        p.y = std::sin(i);
        data.push_back(p);
    }
    return data;
}

static LineChart::data_array create_cos_data()
{
    LineChart::data_array data;
    for (float i = 0.; i < M_PI * 4; i += 0.2)
    {
        LineChart::data_pair p;
        p.x = i;
        p.y = std::cos(i);
        data.push_back(p);
    }
    return data;
}


static LineChart::data_array create_atan_data()
{
    LineChart::data_array data;
    for (float i = 0.; i < M_PI * 4; i += 0.2)
    {
        LineChart::data_pair p;
        p.x = i;
        p.y = std::atan(i);
        data.push_back(p);
    }
    return data;
}

int main(int argc, const char** argv)
{
    Application app(argc, argv, "charts");

    TopWindow win;
    win.palette().set(Palette::BG, Palette::GROUP_NORMAL, Color::BLACK);

    PieChart pie(Rect(10, 10, 600, 400));
    pie.palette().set(Palette::BG, Palette::GROUP_NORMAL, Color::BLACK);
    pie.hide();
    win.add(&pie);

    std::map<std::string, float> data;
    data.insert(make_pair("truck", .25));
    data.insert(make_pair("car", .55));
    data.insert(make_pair("bike", .10));
    data.insert(make_pair("motorcycle", .05));
    data.insert(make_pair("plane", .05));
    pie.data(data);

#ifdef HAVE_KPLOT
    LineChart line(Rect(50, 10, 600, 400));
    line.set_name("linechart");
    line.palette().set(Palette::BG, Palette::GROUP_NORMAL, Color::BLACK);
    win.add(line);
#endif

    CheckBox sin_checkbox("sin", Rect(Point(0, 410), Size(80, 40)));
    sin_checkbox.palette().set(Palette::TEXT, Palette::GROUP_NORMAL, Color::WHITE);
    sin_checkbox.set_name("sin");
    win.add(&sin_checkbox);

    CheckBox cos_checkbox("cos", Rect(Point(100, 410), Size(80, 40)));
    cos_checkbox.palette().set(Palette::TEXT, Palette::GROUP_NORMAL, Color::WHITE);
    cos_checkbox.set_name("cos");
    win.add(&cos_checkbox);

    CheckBox atan_checkbox("atan", Rect(Point(200, 410), Size(80, 40)));
    atan_checkbox.palette().set(Palette::TEXT, Palette::GROUP_NORMAL, Color::WHITE);
    atan_checkbox.set_name("atan");
    win.add(&atan_checkbox);

    auto handle_checkbox = [&](eventid event)
    {
        if (event == eventid::PROPERTY_CHANGED)
        {
            line.clear();
            if (sin_checkbox.checked())
                line.add_data(create_sin_data(), LineChart::chart_type::points);
            if (cos_checkbox.checked())
                line.add_data(create_cos_data(), LineChart::chart_type::lines);
            if (atan_checkbox.checked())
                line.add_data(create_atan_data(), LineChart::chart_type::linespoints);
        }

        return 0;
    };

    sin_checkbox.on_event(handle_checkbox);
    cos_checkbox.on_event(handle_checkbox);
    atan_checkbox.on_event(handle_checkbox);

    sin_checkbox.check(true);
    cos_checkbox.check(true);

    Slider line_width(Rect(Point(300, 410), Size(200, 40)), 1, 10, 1);
    line_width.set_name("line_width");
    win.add(&line_width);

    line_width.on_event([&](eventid event)
    {
        if (event == eventid::PROPERTY_CHANGED)
        {
            line.set_line_width(line_width.value());
        }
        return 0;
    });

    line_width.set_value(2);

    CheckBox gridx_checkbox("grid X", Rect(Point(500, 410), Size(80, 40)));
    gridx_checkbox.palette().set(Palette::TEXT, Palette::GROUP_NORMAL, Color::WHITE);
    win.add(&gridx_checkbox);

    CheckBox gridy_checkbox("grid Y", Rect(Point(600, 410), Size(80, 40)));
    gridy_checkbox.palette().set(Palette::TEXT, Palette::GROUP_NORMAL, Color::WHITE);
    win.add(&gridy_checkbox);

    auto handle_grid_checkbox = [&](eventid event)
    {
        if (event == eventid::PROPERTY_CHANGED)
        {
            uint32_t flags = 0;
            if (gridx_checkbox.checked())
                flags |= LineChart::GRIDX;
            if (gridy_checkbox.checked())
                flags |= LineChart::GRIDY;
            line.set_grid(flags);
        }
        return 0;
    };

    gridx_checkbox.on_event(handle_grid_checkbox);
    gridy_checkbox.on_event(handle_grid_checkbox);

    static vector<StringItem> chart_types =
    {
        "Line",
        "Pie",
    };

    ListBox::item_array items;
    items.resize(chart_types.size());
    transform(chart_types.begin(), chart_types.end(), items.begin(),
    [](const StringItem & v) { return new StringItem(v);});

    ListBox list(items, Rect(Point(win.w() - 100, 0), Size(100, win.h())));
    list.palette().set(Palette::TEXT, Palette::GROUP_NORMAL, Color::WHITE);
    list.palette().set(Palette::BG, Palette::GROUP_NORMAL, Color::BLACK);
    win.add(list);

    list.on_event([&list, &pie, &line](eventid event)
    {
        if (event == eventid::PROPERTY_CHANGED)
        {
            if (list.selected() == 0)
            {
                pie.hide();
                line.show();
            }
            else
            {
                pie.show();
                line.hide();
            }

            return 1;
        }

        return 0;
    });

    win.show();

    return app.run();
}
