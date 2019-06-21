/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <egt/ui>
#include <cmath>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <iostream>

using namespace std;
using namespace egt;
using namespace egt::experimental;

static LineChart::data_array create_sin_data()
{
    LineChart::data_array data;
    for (float i = 0.; i < M_PI * 4; i += 0.2)
    {
        LineChart::DataPair p;
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
        LineChart::DataPair p;
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
        LineChart::DataPair p;
        p.x = i;
        p.y = std::atan(i);
        data.push_back(p);
    }
    return data;
}

struct LinePage : public NotebookTab
{
    LinePage()
    {
        auto line = std::make_shared<LineChart>(Rect(50, 10, 600, 400));
        line->set_name("linechart");
        line->set_color(Palette::ColorId::bg, Palette::black);
        add(line);

        auto sin_checkbox = std::make_shared<CheckBox>("sin", Rect(Point(0, 410), Size(80, 40)));
        sin_checkbox->set_color(Palette::ColorId::text, Palette::white);
        sin_checkbox->set_name("sin");
        add(sin_checkbox);

        auto cos_checkbox = std::make_shared<CheckBox>("cos", Rect(Point(100, 410), Size(80, 40)));
        cos_checkbox->set_color(Palette::ColorId::text, Palette::white);
        cos_checkbox->set_name("cos");
        add(cos_checkbox);

        auto atan_checkbox = std::make_shared<CheckBox>("atan", Rect(Point(200, 410), Size(80, 40)));
        atan_checkbox->set_color(Palette::ColorId::text, Palette::white);
        atan_checkbox->set_name("atan");
        add(atan_checkbox);

        auto handle_checkbox = [line, sin_checkbox, cos_checkbox, atan_checkbox](Event & event)
        {
            if (event.id() == eventid::property_changed)
            {
                line->clear();
                if (sin_checkbox->checked())
                    line->add_data(create_sin_data(), LineChart::chart_type::points);
                if (cos_checkbox->checked())
                    line->add_data(create_cos_data(), LineChart::chart_type::lines);
                if (atan_checkbox->checked())
                    line->add_data(create_atan_data(), LineChart::chart_type::linespoints);
            }
        };

        sin_checkbox->on_event(handle_checkbox);
        cos_checkbox->on_event(handle_checkbox);
        atan_checkbox->on_event(handle_checkbox);

        sin_checkbox->set_check(true);
        cos_checkbox->set_check(true);

        auto line_width = std::make_shared<Slider>(Rect(Point(300, 410), Size(200, 40)), 1, 10, 1);
        line_width->set_name("line_width");
        add(line_width);

        line_width->on_event([line, line_width](Event & event)
        {
            line->set_line_width(line_width->value());
        }, {eventid::property_changed});

        line_width->set_value(2);

        auto gridx_checkbox = std::make_shared<CheckBox>("grid X", Rect(Point(500, 410), Size(80, 40)));
        gridx_checkbox->set_color(Palette::ColorId::text, Palette::white);
        add(gridx_checkbox);

        auto gridy_checkbox = std::make_shared<CheckBox>("grid Y", Rect(Point(600, 410), Size(80, 40)));
        gridy_checkbox->set_color(Palette::ColorId::text, Palette::white);
        add(gridy_checkbox);

        auto handle_grid_checkbox = [gridx_checkbox, gridy_checkbox, line](Event & event)
        {
            if (event.id() == eventid::property_changed)
            {
                uint32_t flags = 0;
                if (gridx_checkbox->checked())
                    flags |= LineChart::GRIDX;
                if (gridy_checkbox->checked())
                    flags |= LineChart::GRIDY;
                line->set_grid(flags);
            }
        };

        gridx_checkbox->on_event(handle_grid_checkbox);
        gridy_checkbox->on_event(handle_grid_checkbox);
    }
};

struct PiePage : public NotebookTab
{
    PiePage()
    {
        auto pie = std::make_shared<PieChart>(Rect(10, 10, 600, 400));
        std::map<std::string, float> data;
        data.insert(make_pair("truck", .25));
        data.insert(make_pair("car", .55));
        data.insert(make_pair("bike", .10));
        data.insert(make_pair("motorcycle", .05));
        data.insert(make_pair("plane", .05));
        pie->data(data);

        add(pie);
    }
};

int main(int argc, const char** argv)
{
    Application app(argc, argv, "charts");

    global_theme().palette().set(Palette::ColorId::bg, Palette::GroupId::normal, Palette::black);
    global_theme().palette().set(Palette::ColorId::text, Palette::GroupId::normal, Palette::white);
    global_theme().palette().set(Palette::ColorId::text, Palette::GroupId::active, Palette::black);

    TopWindow win;

    BoxSizer hsizer(orientation::horizontal);
    hsizer.set_align(alignmask::expand);
    win.add(hsizer);

    ListBox list(Size(win.width() * 0.15, 0));
    list.set_align(alignmask::expand_vertical | alignmask::left);
    list.add_item(make_shared<StringItem>("Line"));
    list.add_item(make_shared<StringItem>("Pie"));
    hsizer.add(list);

    Notebook notebook;
    notebook.set_align(alignmask::expand);
    hsizer.add(notebook);
    notebook.add(make_shared<LinePage>());
    notebook.add(make_shared<PiePage>());

    list.on_event([&](Event&)
    {
        notebook.set_select(list.selected());
    }, {eventid::property_changed});

    win.show();

    return app.run();
}
