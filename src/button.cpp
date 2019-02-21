/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/button.h"
#include "egt/detail/alignment.h"
#include "egt/detail/imagecache.h"
#include "egt/frame.h"
#include "egt/painter.h"
#include "egt/theme.h"
#include "egt/widget.h"

using namespace std;

namespace egt
{
inline namespace v1
{
static const auto DEFAULT_BUTTON_SIZE = Size(100, 50);

Button::Button(const std::string& text, const Rect& rect,
               const Font& font, const widgetflags& flags) noexcept
    : TextWidget(text, rect, alignmask::CENTER, font, flags)
{
    set_name("Button" + std::to_string(m_widgetid));

    set_boxtype(Theme::boxtype::rounded_gradient);
    palette().set(Palette::BG, Palette::GROUP_NORMAL, palette().color(Palette::HIGHLIGHT));
    set_flag(widgetflag::GRAB_MOUSE);
}

Button::Button(Frame& parent, const std::string& text, const Rect& rect,
               const Font& font, const widgetflags& flags) noexcept
    : Button(text, rect, font, flags)
{
    parent.add(this);
}

Button::Button(Frame& parent, const std::string& text,
               const Font& font, const widgetflags& flags) noexcept
    : Button(parent, text, Rect(), font, flags)
{
}

int Button::handle(eventid event)
{
    auto ret = TextWidget::handle(event);

    switch (event)
    {
    case eventid::RAW_POINTER_DOWN:
    {
        set_active(true);
        break;
    }
    case eventid::RAW_POINTER_UP:
    {
        set_active(false);
        break;
    }
    default:
        break;
    }

    return ret;
}

void Button::draw(Painter& painter, const Rect& rect)
{
    Drawer<Button>::draw(*this, painter, rect);
}

void Button::default_draw(Button& widget, Painter& painter, const Rect& rect)
{
    ignoreparam(rect);

    widget.draw_box(painter);

    auto group = Palette::GROUP_NORMAL;
    if (widget.disabled())
        group = Palette::GROUP_DISABLED;
    else if (widget.active())
        group = Palette::GROUP_ACTIVE;

    // text
    painter.set_color(widget.palette().color(Palette::ColorId::TEXT_INVERT, group));
    painter.set_font(widget.font());
    painter.draw_text(widget.box(), widget.text(), widget.text_align(), 5);
}

bool Button::checked() const
{
    return m_checked;
}

void Button::check(bool value)
{
    if (m_checked != value)
    {
        m_checked = value;
        if (m_group)
            m_group->checked_state_change(*this, value);

        /* Check if the button group has not cancelled the change. */
        if (m_checked == value)
        {
            damage();
            invoke_handlers(eventid::PROPERTY_CHANGED);
        }
    }
}

Button::~Button()
{
    if (m_group)
        m_group->remove(*this);
}

void Button::set_parent(Frame* parent)
{
    TextWidget::set_parent(parent);
    first_resize();
}

void Button::first_resize()
{
    if (box().size().empty())
    {
        if (!m_text.empty())
        {
            auto s = text_size(m_text);
            s += Size(10, 10);
            resize(s);
        }
        else
        {
            resize(DEFAULT_BUTTON_SIZE);
        }
    }
}

ImageButton::ImageButton(const Image& image,
                         const std::string& text,
                         const Rect& rect,
                         const widgetflags& flags) noexcept
    : Button(text, rect, Font(), flags)
{
    set_name("ImageButton" + std::to_string(m_widgetid));

    if (text.empty())
        set_image_align(alignmask::CENTER);
    do_set_image(image);
}

ImageButton::ImageButton(Frame& parent,
                         const Image& image,
                         const std::string& text,
                         const Rect& rect,
                         const widgetflags& flags) noexcept
    : ImageButton(image, text, rect, flags)
{
    parent.add(this);
}

void ImageButton::do_set_image(const Image& image)
{
    if (!image.empty())
    {
        m_image = image;
#if 0
        auto width = cairo_image_surface_get_width(m_image.get());
        auto height = cairo_image_surface_get_height(m_image.get());
        m_box.w = width;
        m_box.h = height;
#endif
        damage();
    }
}

void ImageButton::set_image(const Image& image)
{
    do_set_image(image);
}

void ImageButton::draw(Painter& painter, const Rect& rect)
{
    Drawer<ImageButton>::draw(*this, painter, rect);
}

void ImageButton::default_draw(ImageButton& widget, Painter& painter, const Rect& rect)
{
    ignoreparam(rect);

    widget.draw_box(painter);

    auto group = Palette::GROUP_NORMAL;
    if (widget.disabled())
        group = Palette::GROUP_DISABLED;
    else if (widget.active())
        group = Palette::GROUP_ACTIVE;

    if (!widget.text().empty())
    {
        auto text_size = widget.text_size(widget.text());

        Rect tbox;
        Rect ibox;

        if (widget.m_position_image_first)
            detail::double_align(widget.box(),
                                 widget.image().size(), widget.image_align(), ibox,
                                 text_size, widget.text_align(), tbox, 5);
        else
            detail::double_align(widget.box(),
                                 text_size, widget.text_align(), tbox,
                                 widget.image().size(), widget.image_align(), ibox, 5);

        painter.draw_image(ibox.point(), widget.image(), widget.disabled());

        painter.set_color(widget.palette().color(Palette::TEXT_INVERT, group));
        painter.draw_text(widget.text(), tbox, alignmask::CENTER, 0, widget.font());
    }
    else
    {
        Rect target = detail::align_algorithm(widget.image().size(), widget.box(),
                                              widget.image_align(), 0);
        painter.draw_image(target.point(), widget.image(), widget.disabled());
    }
}

ImageButton::~ImageButton()
{}

void ImageButton::first_resize()
{
    if (box().size().empty())
    {
        if (!m_text.empty())
        {
            auto text_size = this->text_size(m_text);

            Rect tbox;
            Rect ibox;

            if (m_position_image_first)
                detail::double_align(box(),
                                     m_image.size(), m_image_align, ibox,
                                     text_size, m_text_align, tbox, 5);
            else
                detail::double_align(box(),
                                     text_size, m_text_align, tbox,
                                     m_image.size(), m_image_align, ibox, 5);

            auto s = Rect::merge(tbox, ibox);
            resize(s.size() + Size(10, 10));
        }
        else
        {
            resize(m_image.size());
        }
    }
}

}
}
