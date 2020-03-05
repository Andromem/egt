/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <egt/ui>
#include <memory>

struct ColorMapWidget : public egt::Widget
{
    explicit ColorMapWidget(const std::string& name,
                            const egt::experimental::ColorMap::StepsArray& steps,
                            egt::experimental::ColorMap::Interpolation interp = egt::experimental::ColorMap::Interpolation::rgba)
        : m_map(steps, interp),
          m_name(name)
    {}

    virtual void draw(egt::Painter& painter, const egt::Rect& rect) override
    {
        auto b = content_area();

        auto inc = 1.0 / b.width();

        for (auto x = 0.0; x <= 1.0; x += inc)
        {
            egt::RectF seg(b.x(), b.y(), b.width(), b.height());
            seg.x(x * seg.width());
            painter.set(m_map.interp(x));
            painter.draw(seg);
            painter.fill();
        }

        painter.set(m_map.interp(1.0));
        painter.set(font());
        painter.draw(b.point());
        painter.draw(m_name);
    }

    egt::experimental::ColorMap m_map;
    std::string m_name;
};

int main(int argc, char** argv)
{
    egt::Application app(argc, argv);

    egt::TopWindow window;
    egt::VerticalBoxSizer sizer;
    window.add(egt::expand(sizer));

    struct triple
    {
        std::string first;
        egt::experimental::ColorMap::StepsArray second;
        egt::experimental::ColorMap::Interpolation third;
    };

    std::vector<triple> maps =
    {
        { "RGB", {egt::Palette::white, egt::Palette::black}, egt::experimental::ColorMap::Interpolation::rgba },
        { "HSV", {egt::Palette::white, egt::Palette::black}, egt::experimental::ColorMap::Interpolation::hsv },
        { "HSL", {egt::Palette::white, egt::Palette::black}, egt::experimental::ColorMap::Interpolation::hsl },

        { "RGB", {egt::Palette::red, egt::Palette::green}, egt::experimental::ColorMap::Interpolation::rgba },
        { "HSV", {egt::Palette::red, egt::Palette::green}, egt::experimental::ColorMap::Interpolation::hsv },
        { "HSL", {egt::Palette::red, egt::Palette::green}, egt::experimental::ColorMap::Interpolation::hsl },

        { "RGB", {egt::Palette::red, egt::Palette::blue}, egt::experimental::ColorMap::Interpolation::rgba },
        { "HSV", {egt::Palette::red, egt::Palette::blue}, egt::experimental::ColorMap::Interpolation::hsv },
        { "HSL", {egt::Palette::red, egt::Palette::blue}, egt::experimental::ColorMap::Interpolation::hsl },

        { "RGB", {egt::Palette::white, egt::Palette::orange}, egt::experimental::ColorMap::Interpolation::rgba },
        { "HSV", {egt::Palette::white, egt::Palette::orange}, egt::experimental::ColorMap::Interpolation::hsv },
        { "HSL", {egt::Palette::white, egt::Palette::orange}, egt::experimental::ColorMap::Interpolation::hsl },

        { "RGB", {egt::Palette::purple, egt::Palette::yellow, egt::Palette::maroon}, egt::experimental::ColorMap::Interpolation::rgba },
        { "HSV", {egt::Palette::purple, egt::Palette::yellow, egt::Palette::maroon}, egt::experimental::ColorMap::Interpolation::hsv },
        { "HSL", {egt::Palette::purple, egt::Palette::yellow, egt::Palette::maroon}, egt::experimental::ColorMap::Interpolation::hsl },
    };

    for (auto& m : maps)
    {
        auto inst = std::make_shared<ColorMapWidget>(m.first, m.second, m.third);
        sizer.add(egt::expand(inst));
    }

    window.show();

    return app.run();
}
