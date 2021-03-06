/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/frame.h"
#include "egt/painter.h"
#include "egt/shapes.h"
#include "egt/theme.h"

namespace egt
{
inline namespace v1
{

CircleWidget::CircleWidget(const Circle& circle)
    : Widget(circle.rect()),
      m_radius(circle.radius())
{
    name("CircleWidget" + std::to_string(m_widgetid));
    fill_flags(Theme::FillFlag::blend);
}

CircleWidget::CircleWidget(Frame& parent, const Circle& circle)
    : CircleWidget(circle)
{
    parent.add(*this);
}

void CircleWidget::draw(Painter& painter, const Rect&)
{
    auto b = content_area();

    theme().draw_circle(painter,
                        fill_flags(),
                        b,
                        color(Palette::ColorId::border),
                        color(Palette::ColorId::button_bg),
                        border(),
                        margin());
}

void LineWidget::draw(Painter& painter, const Rect&)
{
    auto b = content_area();

    auto width = border();
    if (width <= 0)
        width = 1;

    painter.set(color(Palette::ColorId::border));
    painter.line_width(width);

    if (m_horizontal)
        painter.draw(b.center() - Point(b.width() / 2., 0),
                     b.center() + Point(b.width() / 2., 0));
    else
        painter.draw(b.center() - Point(0, b.height() / 2.),
                     b.center() + Point(0, b.height() / 2.));

    painter.stroke();
}

void RectangleWidget::draw(Painter& painter, const Rect&)
{
    auto b = content_area();

    theme().draw_box(painter,
                     fill_flags(),
                     b,
                     color(Palette::ColorId::border),
                     color(Palette::ColorId::button_bg),
                     border(),
                     margin());

}

void LineWidget::serialize(Serializer& serializer) const
{
    Widget::serialize(serializer);

    if (!horizontal())
        serializer.add_property("horizontal", horizontal());
}

void LineWidget::deserialize(const std::string& name, const std::string& value,
                             const Serializer::Attributes& attr)
{
    switch (detail::hash(name))
    {
    case detail::hash("horizontal"):
        horizontal(detail::from_string(value));
        break;
    default:
        Widget::deserialize(name, value, attr);
        break;
    }
}
}
}
