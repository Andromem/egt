/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#include "mui/list.h"
#include "mui/painter.h"
#include "mui/frame.h"

namespace mui
{
    static auto ITEM_HEIGHT = 40;

    /*
    ListBox::ListBox(const item_array& items,
                     const Rect& rect)
        : Widget(rect),
          m_items(items)
    {}

    ListBox::ListBox(Frame& parent,
                     const item_array& items,
                     const Rect& rect)
        : ListBox(items, rect)
    {
        parent.add(this);
    }
    */

    Rect ListBox::item_rect(uint32_t index) const
    {
        Rect r(box());
        r.h = ITEM_HEIGHT;
        r.y += (r.h * index);
        return r;
    }

    int ListBox::handle(int event)
    {
        switch (event)
        {
        case EVT_MOUSE_DOWN:
        {
            Point mouse = screen_to_frame(mouse_position());
            for (size_t i = 0; i < m_items.size(); i++)
            {
                if (Rect::point_inside(mouse, item_rect(i)))
                {
                    select(i);
                    break;
                }
            }

            return 1;
        }
        }

        return Widget::handle(event);
    }

    void ListBox::draw(Painter& painter, const Rect& rect)
    {
        ignoreparam(rect);

        painter.draw_basic_box(box() /*Rect(x(), y(), w(), ITEM_HEIGHT * m_items.size())*/,
                               palette().color(Palette::BORDER),
                               palette().color(Palette::BG));

        if (m_selected < m_items.size())
        {
            for (size_t i = 0; i < m_items.size(); i++)
            {
                m_items[i]->draw(painter, item_rect(i), m_selected == i);
            }
        }
    }

    void ListBox::select(uint32_t index)
    {
        if (m_selected != index)
        {
            //damage(item_rect(m_selected));
            m_selected = index;
            //damage(item_rect(m_selected));
            damage();
            on_selected(index);
        }
    }

    ListBox::~ListBox()
    {
    }

}
