/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "detail/utf8text.h"
#include "egt/detail/alignment.h"
#include "egt/detail/imagecache.h"
#include "egt/detail/meta.h"
#include "egt/frame.h"
#include "egt/label.h"
#include "egt/painter.h"

namespace egt
{
inline namespace v1
{
const alignmask Label::DEFAULT_TEXT_ALIGN = alignmask::center;

Label::Label(const std::string& text, alignmask text_align) noexcept
    : Label(text, {}, text_align)
{
}

Label::Label(const std::string& text, const Rect& rect, alignmask text_align) noexcept
    : TextWidget(text, rect, text_align)
{
    set_name("Label" + std::to_string(m_widgetid));

    set_boxtype(Theme::boxtype::none);
}

Label::Label(Frame& parent, const std::string& text, alignmask text_align) noexcept
    : Label(text, text_align)
{
    parent.add(*this);
}

Label::Label(Frame& parent, const std::string& text, const Rect& rect,
             alignmask text_align) noexcept
    : Label(text, rect, text_align)
{
    parent.add(*this);
}

void Label::set_text(const std::string& text)
{
    if (detail::change_if_diff<>(m_text, text))
    {
        damage();
        layout();
    }
}

void Label::draw(Painter& painter, const Rect& rect)
{
    Drawer<Label>::draw(*this, painter, rect);
}

void Label::default_draw(Label& widget, Painter& painter, const Rect&)
{
    widget.draw_box(painter, Palette::ColorId::label_bg, Palette::ColorId::border);

    detail::draw_text(painter,
                      widget.content_area(),
                      widget.text(),
                      widget.font(),
                      TextBox::flags_type({TextBox::flag::multiline, TextBox::flag::word_wrap}),
                      widget.text_align(),
                      justification::middle,
                      widget.color(Palette::ColorId::label_text).color());
}

void Label::set_parent(Frame* parent)
{
    TextWidget::set_parent(parent);
    layout();
}

Size Label::min_size_hint() const
{
    if (!m_text.empty())
    {
        auto s = text_size(m_text);
        return s + Widget::min_size_hint();
    }

    return Widget::min_size_hint();
}

ImageLabel::ImageLabel(const std::string& text,
                       alignmask text_align) noexcept
    : ImageLabel(Image(), text, {}, text_align)
{
}

ImageLabel::ImageLabel(const Image& image,
                       const std::string& text,
                       alignmask text_align) noexcept
    : ImageLabel(image, text, {}, text_align)
{
}

ImageLabel::ImageLabel(const Image& image,
                       const std::string& text,
                       const Rect& rect,
                       alignmask text_align) noexcept
    : Label(text, rect, text_align),
      m_image(image)
{
    set_name("ImageLabel" + std::to_string(m_widgetid));

    if (text.empty())
        set_image_align(alignmask::center);
    do_set_image(image);
}

ImageLabel::ImageLabel(Frame& parent,
                       const Image& image,
                       const std::string& text,
                       alignmask text_align) noexcept
    : ImageLabel(image, text, text_align)
{
    parent.add(*this);
}

ImageLabel::ImageLabel(Frame& parent,
                       const Image& image,
                       const std::string& text,
                       const Rect& rect,
                       alignmask text_align) noexcept
    : ImageLabel(image, text, rect, text_align)
{
    parent.add(*this);
}

void ImageLabel::draw(Painter& painter, const Rect& rect)
{
    Drawer<ImageLabel>::draw(*this, painter, rect);
}

void ImageLabel::default_draw(ImageLabel& widget, Painter& painter, const Rect& rect)
{
    detail::ignoreparam(rect);

    widget.draw_box(painter, Palette::ColorId::label_bg, Palette::ColorId::border);

    if (!widget.text().empty())
    {
        std::string text;
        if (widget.show_label())
            text = widget.text();

        if (!widget.image().empty())
        {
            detail::draw_text(painter,
                              widget.content_area(),
                              text,
                              widget.font(),
                              TextBox::flags_type({TextBox::flag::multiline, TextBox::flag::word_wrap}),
                              widget.text_align(),
                              justification::middle,
                              widget.color(Palette::ColorId::label_text).color(),
                              widget.image_align(),
                              widget.image());
        }
        else
        {
            detail::draw_text(painter,
                              widget.content_area(),
                              text,
                              widget.font(),
                              TextBox::flags_type({TextBox::flag::multiline, TextBox::flag::word_wrap}),
                              widget.text_align(),
                              justification::middle,
                              widget.color(Palette::ColorId::label_text).color());
        }
    }
    else if (!widget.image().empty())
    {
        const auto b = widget.content_area();
        const auto target = detail::align_algorithm(widget.m_image.size(),
                            b,
                            widget.image_align());

        const auto hs = static_cast<double>(target.width()) /
                        static_cast<double>(widget.image().size_orig().width());
        const auto vs = static_cast<double>(target.height()) /
                        static_cast<double>(widget.image().size_orig().height());
        widget.image().scale(hs, vs);

        painter.draw(target.point());
        painter.draw(widget.image());
    }
}

Size ImageLabel::min_size_hint() const
{
    Rect size = Label::min_size_hint() - Size(moat() * 2, moat() * 2);

    if (!m_image.size().empty())
    {
        if ((image_align() & alignmask::left) == alignmask::left ||
            (image_align() & alignmask::right) == alignmask::right)
        {
            size += Size(m_image.width(), 0);
        }
        else if ((image_align() & alignmask::top) == alignmask::top ||
                 (image_align() & alignmask::bottom) == alignmask::bottom)
        {
            size += Size(0, m_image.height());
        }

        size = Rect::merge(size, m_image.size());
    }

    auto res = size.size() + Size(moat() * 2, moat() * 2);
    return res;
}

void ImageLabel::do_set_image(const Image& image)
{
    if (size().empty() && !image.empty())
        resize(image.size() + Size(moat() * 2, moat() * 2));

    m_image = image;
    damage();
}

void ImageLabel::set_image(const Image& image)
{
    do_set_image(image);
}

void ImageLabel::set_show_label(bool value)
{
    if (detail::change_if_diff<>(m_show_label, value))
        damage();
}

}
}
