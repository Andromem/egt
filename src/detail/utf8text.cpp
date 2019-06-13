/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "detail/utf8text.h"
#include "egt/detail/layout.h"
#include "egt/image.h"
#include <spdlog/fmt/ostr.h>
#include <spdlog/spdlog.h>

namespace egt
{
inline namespace v1
{
namespace detail
{

enum
{
    LAY_LEFT = 0x020,
    // anchor to top item or top side of parent
    LAY_TOP = 0x040,
    // anchor to right item or right side of parent
    LAY_RIGHT = 0x080,
    // anchor to bottom item or bottom side of parent
    LAY_BOTTOM = 0x100,
    // anchor to both left and right item or parent borders
    LAY_HFILL = 0x0a0,
    // anchor to both top and bottom item or parent borders
    LAY_VFILL = 0x140,
    // center horizontally, with left margin as offset
    LAY_HCENTER = 0x000,
    // center vertically, with top margin as offset
    LAY_VCENTER = 0x000,
    // center in both directions, with left/top margin as offset
    LAY_CENTER = 0x000,
    // anchor to all four directions
    LAY_FILL = 0x1e0,
    // When in a wrapping container, put this element on a new line. Wrapping
    // layout code auto-inserts LAY_BREAK flags as needed. See GitHub issues for
    // TODO related to this.
    //
    // Drawing routines can read this via item pointers as needed after
    // performing layout calculations.
    LAY_BREAK = 0x200
};


void draw_text(Painter& painter,
               const Rect& b,
               const std::string& text,
               const Font& font,
               TextBox::flags_type flags,
               alignmask text_align,
               justification justify,
               const Color& text_color,
               std::function<void(const Point& offset, size_t height)> draw_cursor,
               size_t cursor_pos,
               const Color& highlight_color,
               size_t select_start,
               size_t select_len)
{
    auto cr = painter.context().get();

    painter.set(font);
    cairo_font_extents_t fe;
    cairo_font_extents(cr, &fe);

    // tokenize based on words or codepoints
    const std::string delimiters = " \t\n\r";
    std::vector<std::string> tokens;
    if (flags.is_set(TextBox::flag::multiline) && flags.is_set(TextBox::flag::word_wrap))
    {
        detail::tokenize_with_delimiters(text.cbegin(),
                                         text.cend(),
                                         delimiters.cbegin(),
                                         delimiters.cend(),
                                         tokens);
    }
    else
    {
#ifdef UTF8CPP_CHECKED
        for (utf8_const_iterator ch(text.begin(), text.begin(), text.end());
             ch != utf8_const_iterator(text.end(), text.begin(), text.end()); ++ch)
#else
        for (utf8_const_iterator ch(text.begin());
             ch != utf8_const_iterator(text.end()); ++ch)
#endif
        {
#ifdef UTF8CPP_CHECKED
            tokens.emplace_back(utf8_char_to_string(ch.base(), text.cend()));
#else
            tokens.emplace_back(utf8_char_to_string(ch.base()));
#endif
        }
    }

    uint32_t default_behave = 0;
#if 0
    if ((text_align & alignmask::expand_horizontal) == alignmask::expand_horizontal)
        default_behave |= LAY_HFILL;
    else if ((text_align & alignmask::left) == alignmask::left)
        default_behave |= LAY_LEFT;
    else if ((text_align & alignmask::right) == alignmask::right)
        default_behave |= LAY_RIGHT;
    if ((text_align & alignmask::expand_vertical) == alignmask::expand_vertical)
        default_behave |= LAY_VFILL;
    else if ((text_align & alignmask::top) == alignmask::top)
        default_behave |= LAY_TOP;
    else if ((text_align & alignmask::bottom) == alignmask::bottom)
        default_behave |= LAY_BOTTOM;
#endif
    SPDLOG_INFO("  default_behave: {}", default_behave);

    // setup rects for each word/codepoint
    std::vector<detail::LayoutRect> rects;
    uint32_t behave = default_behave;
    for (auto t = tokens.begin(); t != tokens.end(); ++t)
    {
        if (*t == "\n")
        {
            detail::LayoutRect r;
            r.behave = behave;
            r.rect = Rect(0, 0, 1, fe.height);
            r.str = *t;
            rects.push_back(r);
        }
        else
        {
            cairo_text_extents_t te;
            cairo_text_extents(cr, (*t).c_str(), &te);
            detail::LayoutRect r(behave, Rect(0, 0, te.x_advance, fe.height));
            r.str = *t;
            rects.push_back(r);
        }

        if (*t == "\n")
            behave |= LAY_BREAK;
        else
            behave = default_behave;
    }

    detail::flex_layout(b, rects, justify, orientation::flex, text_align);

    // draw the codepoints, cursor, and selected box
    size_t pos = 0;
    std::string last_char;
    bool workaround = false;
    bool stop = false;
    for (auto r = rects.begin(); !stop && r != rects.end(); ++r)
    {
        float roff = 0.;
#ifdef UTF8CPP_CHECKED
        for (utf8_const_iterator ch(r->str.begin(), r->str.begin(), r->str.end()); ch != utf8_const_iterator(r->str.end(), r->str.begin(), r->str.end()); ++ch)
#else
        for (utf8_const_iterator ch(r->str.begin()); ch != utf8_const_iterator(r->str.end()); ++ch)
#endif
        {
            float char_width = 0;
#ifdef UTF8CPP_CHECKED
            last_char = utf8_char_to_string(ch.base(), r->str.cend());
#else
            last_char = utf8_char_to_string(ch.base());
#endif

            if (*ch != '\n')
            {
                cairo_text_extents_t te;
                cairo_text_extents(cr, last_char.c_str(), &te);
                char_width = te.x_advance;

                auto p = PointF(static_cast<float>(b.x) + static_cast<float>(r->rect.x) + roff + te.x_bearing,
                                static_cast<float>(b.y) + static_cast<float>(r->rect.y) + te.y_bearing - fe.descent + fe.height);

                if (workaround)
                    p.y += fe.height;

                auto s = SizeF(char_width, r->rect.h);

                if (pos >= select_start && pos < select_start + select_len)
                {
                    auto p2 = PointF(static_cast<float>(b.x) + static_cast<float>(r->rect.x) + roff,
                                     static_cast<float>(b.y) + static_cast<float>(r->rect.y));

                    if (workaround)
                        p2.y += fe.height;

                    auto rect = RectF(p2, s);
                    if (!rect.empty())
                    {
                        painter.set(highlight_color);
                        painter.draw(rect);
                        painter.fill();
                    }
                }

                painter.set(text_color);
                painter.draw(p);
                painter.draw(last_char);

                roff += char_width;
            }
            else
            {
                if (!flags.is_set(TextBox::flag::multiline))
                    break;

                // if first char is a "\n" layout doesn't respond right
                if (last_char.empty())
                    workaround = true;
            }

            // draw cursor if before current character
            if (pos == cursor_pos)
            {
                if (draw_cursor)
                {
                    auto p = Point(b.x + r->rect.x + roff - char_width, b.y + r->rect.y);
                    draw_cursor(p, fe.height);
                }
            }

            pos++;
        }
    }

    // handle cursor after last character
    if (pos == cursor_pos)
    {
        if (draw_cursor)
        {
            if (!rects.empty())
            {
                auto p = b.point() + rects.back().rect.point() + Point(rects.back().rect.w, 0);
                if (workaround)
                {
                    p.y += fe.height;
                }

                if (last_char == "\n")
                {
                    p.x = b.x;
                    p.y += fe.height;
                }

                draw_cursor(p, fe.height);
            }
            else
                draw_cursor(b.point(), fe.height);
        }
    }
}


void draw_text(Painter& painter,
               const Rect& b,
               const std::string& text,
               const Font& font,
               TextBox::flags_type flags,
               alignmask text_align,
               justification justify,
               const Color& text_color,
               alignmask image_align,
               const Image& image,
               std::function<void(const Point& offset, size_t height)> draw_cursor,
               size_t cursor_pos,
               const Color& highlight_color,
               size_t select_start,
               size_t select_len)
{
    auto cr = painter.context().get();

    painter.set(font);
    cairo_font_extents_t fe;
    cairo_font_extents(cr, &fe);

    // tokenize based on words or codepoints
    const std::string delimiters = " \t\n\r";
    std::vector<std::string> tokens;
    if (flags.is_set(TextBox::flag::multiline) && flags.is_set(TextBox::flag::word_wrap))
    {
        detail::tokenize_with_delimiters(text.cbegin(),
                                         text.cend(),
                                         delimiters.cbegin(),
                                         delimiters.cend(),
                                         tokens);
    }
    else
    {
#ifdef UTF8CPP_CHECKED
        for (utf8_const_iterator ch(text.begin(), text.begin(), text.end());
             ch != utf8_const_iterator(text.end(), text.begin(), text.end()); ++ch)
#else
        for (utf8_const_iterator ch(text.begin());
             ch != utf8_const_iterator(text.end()); ++ch)
#endif
        {
#ifdef UTF8CPP_CHECKED
            tokens.emplace_back(utf8_char_to_string(ch.base(), text.cend()));
#else
            tokens.emplace_back(utf8_char_to_string(ch.base()));
#endif
        }
    }

    uint32_t default_behave = 0;
#if 0
    if ((text_align & alignmask::expand_horizontal) == alignmask::expand_horizontal)
        default_behave |= LAY_HFILL;
    else if ((text_align & alignmask::left) == alignmask::left)
        default_behave |= LAY_LEFT;
    else if ((text_align & alignmask::right) == alignmask::right)
        default_behave |= LAY_RIGHT;
    if ((text_align & alignmask::expand_vertical) == alignmask::expand_vertical)
        default_behave |= LAY_VFILL;
    else if ((text_align & alignmask::top) == alignmask::top)
        default_behave |= LAY_TOP;
    else if ((text_align & alignmask::bottom) == alignmask::bottom)
        default_behave |= LAY_BOTTOM;
#endif

    SPDLOG_INFO("  default_behave: {}", default_behave);

    // setup rects for each word/codepoint
    std::vector<detail::LayoutRect> rects;

    uint32_t behave = default_behave;

    for (auto t = tokens.begin(); t != tokens.end(); ++t)
    {
        if (*t == "\n")
        {
            detail::LayoutRect r;
            r.behave = behave;
            r.rect = Rect(0, 0, 1, fe.height);
            r.str = *t;
            rects.push_back(r);
        }
        else
        {
            cairo_text_extents_t te;
            cairo_text_extents(cr, (*t).c_str(), &te);
            detail::LayoutRect r(behave, Rect(0, 0, te.x_advance, fe.height));
            r.str = *t;
            rects.push_back(r);
        }

        if (*t == "\n")
            behave |= LAY_BREAK;
        else
            behave = default_behave;
    }

    if ((image_align & alignmask::top) == alignmask::top)
    {
        detail::LayoutRect r;
        r.behave = LAY_BREAK;
        r.rect = Rect(0, 0, 1, fe.height);
        r.str = "\n";
        rects.insert(rects.begin(), r);

        r.behave = default_behave;
        r.rect = Rect(Point(), image.size());
        r.str.clear();
        rects.insert(rects.begin(), r);
    }
    else if ((image_align & alignmask::right) == alignmask::right)
    {
        detail::LayoutRect r;
        r.behave = default_behave;
        r.rect = Rect(Point(), image.size());
        r.str.clear();
        rects.push_back(r);
    }
    else if ((image_align & alignmask::bottom) == alignmask::bottom)
    {
        detail::LayoutRect r;
        r.behave = LAY_BREAK;
        r.rect = Rect(0, 0, 1, fe.height);
        r.str = "\n";
        rects.push_back(r);

        r.behave = default_behave;
        r.rect = Rect(Point(), image.size());
        r.str.clear();
        rects.push_back(r);
    }
    else
    {
        detail::LayoutRect r;
        r.behave = default_behave;
        r.rect = Rect(Point(), image.size());
        rects.insert(rects.begin(), r);
    }

    detail::flex_layout(b, rects, justify, orientation::flex, text_align);

    // draw the codepoints, cursor, and selected box
    size_t pos = 0;
    std::string last_char;
    bool workaround = false;
    bool stop = false;
    for (auto r = rects.begin(); !stop && r != rects.end(); ++r)
    {
        if (r->str.empty())
        {
            auto p = PointF(static_cast<float>(b.x) + static_cast<float>(r->rect.x),
                            static_cast<float>(b.y) + static_cast<float>(r->rect.y));

            painter.draw(p);
            painter.draw(image);
            continue;
        }

        float roff = 0.;
#ifdef UTF8CPP_CHECKED
        for (utf8_const_iterator ch(r->str.begin(), r->str.begin(), r->str.end()); ch != utf8_const_iterator(r->str.end(), r->str.begin(), r->str.end()); ++ch)
#else
        for (utf8_const_iterator ch(r->str.begin()); ch != utf8_const_iterator(r->str.end()); ++ch)
#endif
        {
            float char_width = 0;
#ifdef UTF8CPP_CHECKED
            last_char = utf8_char_to_string(ch.base(), r->str.cend());
#else
            last_char = utf8_char_to_string(ch.base());
#endif

            if (*ch != '\n')
            {
                cairo_text_extents_t te;
                cairo_text_extents(cr, last_char.c_str(), &te);
                char_width = te.x_advance;

                auto p = PointF(static_cast<float>(b.x) + static_cast<float>(r->rect.x) + roff + te.x_bearing,
                                static_cast<float>(b.y) + static_cast<float>(r->rect.y) + te.y_bearing - fe.descent + fe.height);

                if (workaround)
                    p.y += fe.height;

                auto s = SizeF(char_width, r->rect.h);

                if (pos >= select_start && pos < select_start + select_len)
                {
                    auto p2 = PointF(static_cast<float>(b.x) + static_cast<float>(r->rect.x) + roff,
                                     static_cast<float>(b.y) + static_cast<float>(r->rect.y));

                    if (workaround)
                        p2.y += fe.height;

                    auto rect = RectF(p2, s);
                    if (!rect.empty())
                    {
                        painter.set(highlight_color);
                        painter.draw(rect);
                        painter.fill();
                    }
                }

                painter.set(text_color);
                painter.draw(p);
                painter.draw(last_char);

                roff += char_width;
            }
            else
            {
                if (!flags.is_set(TextBox::flag::multiline))
                    break;

                // if first char is a "\n" layout doesn't respond right
                if (last_char.empty())
                    workaround = true;
            }

            // draw cursor if before current character
            if (pos == cursor_pos)
            {
                if (draw_cursor)
                {
                    auto p = Point(b.x + r->rect.x + roff - char_width, b.y + r->rect.y);
                    draw_cursor(p, fe.height);
                }
            }

            pos++;
        }
    }

    // handle cursor after last character
    if (pos == cursor_pos)
    {
        if (draw_cursor)
        {
            if (!rects.empty())
            {
                auto p = b.point() + rects.back().rect.point() + Point(rects.back().rect.w, 0);
                if (workaround)
                {
                    p.y += fe.height;
                }

                if (last_char == "\n")
                {
                    p.x = b.x;
                    p.y += fe.height;
                }

                draw_cursor(p, fe.height);
            }
            else
                draw_cursor(b.point(), fe.height);
        }
    }
}

}
}
}
