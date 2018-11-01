/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifndef MUI_LIST_H
#define MUI_LIST_H

/**
 * @file
 * @brief ListBox definition.
 */

#include <mui/widget.h>
#include <mui/painter.h>
#include <mui/palette.h>
#include <mui/frame.h>
#include <vector>

namespace mui
{

    /**
     * An item in a ListBox.
     */
    class ListBoxItem
    {
    public:
        virtual void draw(Painter& painter, const Rect& rect, bool selected);
    };

    /**
     * A specialized ListBoxItem that holds a simple string.
     */
    class StringItem : public ListBoxItem
    {
    public:

        /**
         * Construct a List item.
         */
        // cppcheck-suppress noExplicitConstructor
        StringItem(const char* text)
            : m_text(text)
        {}

        /**
         * Construct a List item.
         */
        // cppcheck-suppress noExplicitConstructor
        StringItem(const std::string& text)
            : m_text(text)
        {}

        virtual void draw(Painter& painter, const Rect& rect, bool selected) override
        {
            ListBoxItem::draw(painter, rect, selected);

            painter.set_color(global_palette().color(Palette::TEXT));
            painter.set_font(m_font);
            painter.draw_text(rect, m_text, alignmask::CENTER);
        }

        /**
         * Set the font of the items.
         */
        virtual void font(const Font& font) { m_font = font; }

        virtual ~StringItem()
        {}

    protected:

        std::string m_text;
        Font m_font;
    };

    /**
     * ListBox that manages a selectable list of items.
     *
     * Items are derived from type ListBoxItem. Only one item may be selected at
     * a time.
     *
     * @image html widget_listbox.png
     * @image latex widget_listbox.png "widget_listbox" height=10cm
     */
    class ListBox : public Widget
    {
    public:
        using item_array = std::vector<ListBoxItem*>;

        explicit ListBox(const Rect& rect = Rect())
            : Widget(rect)
        {
        }

        explicit ListBox(Frame& parent,
                         const Rect& rect = Rect())
            : ListBox(rect)
        {
            parent.add(this);
        }

        template<class T>
        explicit ListBox(const std::vector<T>& items,
                         const Rect& rect = Rect())
            : ListBox(rect)
        {
            std::copy(items.begin(), items.end(), back_inserter(m_items));
        }

        template<class T>
        explicit ListBox(Frame& parent,
                         const std::vector<T>& items,
                         const Rect& rect = Rect())
            : ListBox(items, rect)
        {
            parent.add(this);
        }

        virtual int handle(eventid event) override;

        virtual void draw(Painter& painter, const Rect& rect) override;

        /**
         * Select an item by index.
         */
        virtual void select(uint32_t index);

        /**
         * Return the number of items in the list.
         */
        virtual size_t count() const { return m_items.size(); }

        uint32_t selected() const { return m_selected; }

        void add_item(ListBoxItem* item)
        {
            m_items.push_back(item);
        }

        virtual ~ListBox();

    protected:

        virtual void on_selected(int index) {ignoreparam(index);}

        Rect item_rect(uint32_t index) const;

        item_array m_items;
        uint32_t m_selected{0};
    };

}

#endif
