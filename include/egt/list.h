/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_LIST_H
#define EGT_LIST_H

/**
 * @file
 * @brief ListBox definition.
 */

#include <egt/detail/meta.h>
#include <egt/frame.h>
#include <egt/label.h>
#include <egt/signal.h>
#include <egt/sizer.h>
#include <egt/view.h>
#include <string>

namespace egt
{
inline namespace v1
{

/**
 * ListBox string helper.
 *
 * Light wrapper around a list item to make each item in the ListBox a little
 * bit bigger.
 */
struct EGT_API StringItem : public ImageLabel
{
    /**
     * @param[in] image The image to display.
     * @param[in] text The text to display.
     * @param[in] text_align Alignment for the text.
     */
    explicit StringItem(const std::string& text = {},
                        const Image& image = {},
                        const AlignFlags& text_align = AlignFlag::center) noexcept
        : ImageLabel(image, text, text_align)
    {
        fill_flags(Theme::FillFlag::blend);
    }

    /**
     * @param[in] image The image to display.
     * @param[in] text The text to display.
     * @param[in] rect Initial rectangle of the widget.
     * @param[in] text_align Alignment for the text.
     */
    StringItem(const std::string& text,
               const Image& image,
               const Rect& rect,
               const AlignFlags& text_align = AlignFlag::center) noexcept
        : ImageLabel(image, text, rect, text_align)
    {
        fill_flags(Theme::FillFlag::blend);
    }

    /**
     * @param[in] text The text to display.
     * @param[in] rect Initial rectangle of the widget.
     * @param[in] text_align Alignment for the text.
     */
    StringItem(const std::string& text,
               const Rect& rect,
               const AlignFlags& text_align = AlignFlag::center) noexcept
        : ImageLabel(Image(), text, rect, text_align)
    {
        fill_flags(Theme::FillFlag::blend);
    }

    /**
     * @param[in] parent The parent Frame.
     * @param[in] text The text to display.
     * @param[in] text_align Alignment for the text.
     */
    explicit StringItem(Frame& parent,
                        const std::string& text = {},
                        const AlignFlags& text_align = AlignFlag::center) noexcept
        : ImageLabel(parent, Image(), text, text_align)
    {
        fill_flags(Theme::FillFlag::blend);
    }

    /**
     * @param[in] parent The parent Frame.
     * @param[in] text The text to display.
     * @param[in] rect Initial rectangle of the widget.
     * @param[in] text_align Alignment for the text.
     */
    StringItem(Frame& parent,
               const std::string& text,
               const Rect& rect,
               const AlignFlags& text_align = AlignFlag::center) noexcept
        : ImageLabel(parent, Image(), text, rect, text_align)
    {
        fill_flags(Theme::FillFlag::blend);
    }

    StringItem() noexcept = default;
    EGT_OPS_NOCOPY_MOVE(StringItem);
    ~StringItem() noexcept override = default;

    using ImageLabel::min_size_hint;

    Size min_size_hint() const override
    {
        if (!m_min_size.empty())
            return m_min_size;
        return {100, 40};
    }
};

/**
 * ListBox that manages a selectable list of widgets.
 *
 * Only one item may be selected at a time. The items are based on Widget, so
 * any Widget can be used.
 *
 * @image html widget_listbox.png
 * @image latex widget_listbox.png "widget_listbox" width=5cm
 *
 * @ingroup controls
 *
 * @note This interface only supports a vertical Orientation.
 */
class EGT_API ListBox : public Frame
{
public:

    /**
     * Event signal.
     * @{
     */
    /**
     * Invoked when the selection changes.
     */
    Signal<> on_selected_changed;

    /**
     * Invoked when items are added or removed.
     */
    Signal<> on_items_changed;
    /** @} */

    /// Item array type
    using ItemArray = std::vector<std::shared_ptr<Widget>>;

    /**
     * @param[in] items Array of items to insert into the list.
     */
    explicit ListBox(const ItemArray& items = ItemArray()) noexcept;

    /**
     * @param[in] rect Initial rectangle of the widget.
     */
    explicit ListBox(const Rect& rect) noexcept;

    /**
     * @param[in] items Array of items to insert into the list.
     * @param[in] rect Initial rectangle of the widget.
     */
    ListBox(const ItemArray& items, const Rect& rect) noexcept;

    /**
     * @param[in] parent The parent Frame.
     * @param[in] items Array of items to insert into the list.
     * @param[in] rect Initial rectangle of the widget.
     */
    explicit ListBox(Frame& parent, const ItemArray& items = {}, const Rect& rect = {}) noexcept;

    EGT_OPS_NOCOPY_MOVE(ListBox);
    ~ListBox() noexcept override = default;

    void handle(Event& event) override;

    void resize(const Size& s) override
    {
        if (s != size())
        {
            Frame::resize(s);
            auto carea = content_area();
            if (!carea.empty())
            {
                m_view.box(to_child(carea));
                m_sizer.resize(carea.size());
            }
        }
    }

    /**
     * Select an item by index.
     */
    void selected(size_t index);

    /**
     * Get the currently selected index.
     */
    ssize_t selected() const { return m_selected; }

    /**
     * Return the number of items in the list.
     */
    size_t item_count() const { return m_sizer.count_children(); }

    /**
     * Add a new item to the end of the list.
     */
    void add_item(const std::shared_ptr<Widget>& widget);

    /**
     * Get the currently selected index item from list.
     */
    std::shared_ptr<Widget> item_at(size_t index) const;

    /**
     * Remove an item from the list.
     */
    void remove_item(Widget* widget);

    /**
     * Remove all items from the list.
     */
    void clear();

protected:

    /**
     * Currently selected index.
     */
    ssize_t m_selected{-1};

    /**
     * View used to contain the possible large sizer.
     */
    ScrolledView m_view;

    /**
     * Internal sizer used to layout items.
     */
    BoxSizer m_sizer;

private:

    void add_item_private(const std::shared_ptr<Widget>& widget);
};

}
}

#endif
