/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/label.h"
#include "egt/imagecache.h"
#include "egt/painter.h"
#include "egt/frame.h"

using namespace std;

namespace egt
{

    Label::Label(const std::string& text, const Rect& rect,
                 alignmask align, const Font& font, widgetmask flags) noexcept
        : TextWidget(text, rect, align, font, flags)
    {
    }

    Label::Label(Frame& parent, const std::string& text, const Rect& rect,
                 alignmask align, const Font& font, widgetmask flags) noexcept
        : Label(text, rect, align, font, flags)
    {
        parent.add(this);
        first_resize();
    }

    void Label::first_resize()
    {
        if (box().size().empty())
        {
            if (!m_text.empty())
            {
                Painter painter(screen()->context());
                painter.set_font(font());
                Size s = painter.text_size(m_text);
                resize(s);
            }
        }
    }

    void Label::set_text(const std::string& str)
    {
        if (m_text != str)
        {
            bool doresize = m_text.empty();
            m_text = str;
            if (doresize)
            {
                first_resize();
            }
            damage();
        }
    }

    void Label::draw(Painter& painter, const Rect& rect)
    {
        ignoreparam(rect);

        if (is_flag_set(widgetmask::NO_BACKGROUND))
            painter.draw_box(*this, Painter::boxtype::border);
        else if (is_flag_set(widgetmask::NO_BORDER))
            painter.draw_box(*this, Painter::boxtype::fill);

        painter.set_color(palette().color(Palette::TEXT));
        painter.set_font(font());
        painter.draw_text(box(), m_text, m_text_align, 5);
#if 0
        auto cr = painter.context();
        cairo_text_extents_t textext;
        cairo_text_extents(cr.get(), "A", &textext);

        std::stringstream ss;//(m_text_wrapped);

        size_t n = 0;
        for (size_t i = 0; i < m_text.size(); i++)
        {
            if (i % int(box().w / textext.width) == 0)
            {
                n++;
                ss << '\n';
            }
            ss << m_text[i];
        }

        //std::stringstream ss(m_text_wrapped);
        std::string line;

        Rect linerect = box();
        linerect.y += textext.height / 3;
        linerect.h = textext.height + textext.height / 3;

        //size_t n = std::count(ss.str().begin(), ss.str().end(), '\n');
        size_t lines = box().h / linerect.h;

        while (std::getline(ss, line, '\n'))
        {
            if (n-- >= lines)
                continue;

            painter.draw_text(line, linerect,
                              palette().color(Palette::TEXT),
                              m_text_align,
                              5,
                              m_font);
            linerect.y += linerect.h;
        }
#endif
    }

    Label::~Label()
    {}

    CheckBox::CheckBox(const std::string& text,
                       const Rect& rect)
        : Label(text, rect)
    {
        flag_set(widgetmask::GRAB_MOUSE);
        palette().set(Palette::BG, Palette::GROUP_ACTIVE, palette().color(Palette::HIGHLIGHT));
    }

    int CheckBox::handle(eventid event)
    {
        auto ret = Widget::handle(event);

        switch (event)
        {
        case eventid::MOUSE_DOWN:
            check(!checked());
            return 1;
        default:
            break;
        }

        return ret;
    }

    void CheckBox::draw(Painter& painter, const Rect& rect)
    {
        ignoreparam(rect);

        static const int STANDOFF = 5;

        // image
        Rect r(x() + STANDOFF,
               y() + STANDOFF,
               h() - STANDOFF * 2,
               h() - STANDOFF * 2);

        if (checked())
        {
            painter.draw_box(*this,
                             Painter::boxtype::rounded_gradient,
                             r);

            // draw an "X"
            static const int OFFSET = 5;
            auto cr = painter.context();
            painter.set_color(palette().color(Palette::DARK));
            cairo_move_to(cr.get(), r.x + OFFSET, r.y + OFFSET);
            cairo_line_to(cr.get(), r.x + r.w - OFFSET, r.y + r.h - OFFSET);
            cairo_move_to(cr.get(), r.x + r.w - OFFSET, r.y + OFFSET);
            cairo_line_to(cr.get(), r.x + OFFSET, r.y + r.h - OFFSET);
            cairo_set_line_width(cr.get(), 2.0);
            cairo_stroke(cr.get());
        }
        else
        {
            painter.draw_box(*this,
                             Painter::boxtype::rounded_gradient,
                             r);
        }

        // text
        painter.draw_text(m_text,
                          box(),
                          palette().color(Palette::TEXT),
                          alignmask::LEFT | alignmask::CENTER,
                          h());
    }

    CheckBox::~CheckBox()
    {}

    SlidingCheckBox::SlidingCheckBox(const Rect& rect)
        : CheckBox("", rect)
    {
        palette().set(Palette::BG, Palette::GROUP_ACTIVE, palette().color(Palette::BG));
    }

    void SlidingCheckBox::draw(Painter& painter, const Rect& rect)
    {
        ignoreparam(rect);

        painter.draw_box(*this, Painter::boxtype::rounded_border);

        if (checked())
        {
            Rect rect = box();
            rect.w /= 2;
            rect.x += rect.w;
            painter.draw_rounded_gradient_box(rect,
                                              palette().color(Palette::BORDER),
                                              palette().color(Palette::HIGHLIGHT));
        }
        else
        {
            Rect rect = box();
            rect.w /= 2;
            painter.draw_rounded_gradient_box(rect,
                                              palette().color(Palette::BORDER),
                                              palette().color(Palette::MID));
        }
    }

    SlidingCheckBox::~SlidingCheckBox()
    {}

    ImageLabel::ImageLabel(const Image& image,
                           const std::string& text,
                           const Rect& rect,
                           const Font& font)
        : Label(text, rect, alignmask::LEFT | alignmask::CENTER, font),
          m_image(image)
    {
        if (rect.empty())
            m_box = Rect(rect.point(), m_image.size());
    }

    ImageLabel::ImageLabel(const Image& image,
                           const Point& point)
        : ImageLabel(image, "", Rect(point, image.size()), Font())
    {
    }

    ImageLabel::ImageLabel(const Image& image,
                   const std::string& text,
                   const Point& point,
                   const Font& font)
        : ImageLabel(image, text, Rect(point, image.size()), font)
    {
    }

    ImageLabel::ImageLabel(Frame& parent,
                           const Image& image,
                           const std::string& text,
                           const Rect& rect,
                           const Font& font)
        : ImageLabel(image, text, rect, font)
    {
        parent.add(this);
    }

    void ImageLabel::draw(Painter& painter, const Rect& rect)
    {
        ignoreparam(rect);

        // image
        Rect target = Widget::align_algorithm(m_image.size(), box(), m_text_align, 0);
        painter.draw_image(target.point(), m_image);

        // text
        if (m_label && !m_text.empty())
        {
            auto tbox = box();

            if ((m_text_align & alignmask::LEFT) == alignmask::LEFT)
            {
                tbox += Point(m_image.size().w, 0);
                tbox -= Size(m_image.size().w, 0);
            }
            else if ((m_text_align & alignmask::RIGHT) == alignmask::RIGHT)
            {
                tbox -= Size(m_image.size().w, 0);
            }
            else if ((m_text_align & alignmask::TOP) == alignmask::TOP)
            {
                tbox -= Size(0, m_image.size().h);
            }
            else if ((m_text_align & alignmask::BOTTOM) == alignmask::BOTTOM)
            {
                tbox += Point(0, m_image.size().h);
                tbox -= Size(0, m_image.size().h);
            }

            painter.draw_text(m_text, tbox, palette().color(Palette::TEXT),
                              m_text_align, 5, font());
        }
    }

    void ImageLabel::label_enabled(bool value)
    {
        if (m_label != value)
        {
            m_label = value;
            damage();
        }
    }

    ImageLabel::~ImageLabel()
    {
    }

}
