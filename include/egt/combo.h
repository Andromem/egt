/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_COMBO_H
#define EGT_COMBO_H

/**
 * @file
 * @brief ComboBox definition.
 */

#include <egt/detail/meta.h>
#include <egt/detail/signal.h>
#include <egt/list.h>
#include <egt/popup.h>
#include <egt/widget.h>
#include <memory>
#include <string>
#include <vector>

namespace egt
{
inline namespace v1
{
class ComboBox;
class Serializer;

namespace detail
{
class ComboBoxPopup;
}

/**
 * Combo box widget.
 *
 * This manages a list of select-able items, but otherwise just shows only what
 * is selected.
 *
 * @ingroup controls
 */
class EGT_API ComboBox : public Widget
{
public:

    /**
     * Event signal.
     * @{
     */
    /**
     * Invoked when the selection changes.
     */
    detail::Signal<> on_selected_changed;
    /** @} */

    /// Item array type
    using ItemArray = std::vector<std::string>;

    /**
     * @param[in] items Array of items to insert into the list.
     */
    explicit ComboBox(const ItemArray& items = ItemArray()) noexcept;

    /**
     * @param[in] rect Rectangle for the widget.
     */
    explicit ComboBox(const Rect& rect) noexcept;

    /**
     * @param[in] items Array of items to insert into the list.
     * @param[in] rect Rectangle for the widget.
     */
    ComboBox(const ItemArray& items, const Rect& rect) noexcept;

    /**
     * @param[in] parent The parent Frame.
     * @param[in] items Array of items to insert into the list.
     * @param[in] rect Rectangle for the widget.
     */
    explicit ComboBox(Frame& parent,
                      const ItemArray& items = {},
                      const Rect& rect = {}) noexcept;

    virtual void handle(Event& event) override;

    virtual void resize(const Size& s) override;

    virtual void move(const Point& point) override;

    virtual void draw(Painter& painter, const Rect& rect) override;

    using Widget::parent;

    virtual void parent(Frame* parent) override;

    using Widget::min_size_hint;

    virtual Size min_size_hint() const override;

    /**
     * Default draw method for the ComboBox.
     */
    static void default_draw(ComboBox& widget, Painter& painter, const Rect& rect);

    /**
     * Set the index of the selected item.
     */
    virtual void selected(size_t index);

    /**
     * Get the index of the selected item.
     */
    virtual ssize_t selected() const { return m_selected; }

    /**
     * Append a new item to the ComboBox.
     */
    virtual void add_item(const std::string& item);

    /**
     * Remove a item from a ComboBox.
     */
    virtual bool remove(const std::string& item);

    /**
     * Get an item at the specified index.
     */
    virtual std::string item_at(size_t index) const { return m_items.at(index); }

    /**
     * Return the number of items in the list.
     */
    size_t item_count() const
    {
        return m_items.size();
    }

    /**
     * Remove all items from the list.
     */
    virtual void clear();

    /**
     * Set the text alignment within the Label.
     *
     * @param[in] align Alignment for the text.
     */
    void text_align(const AlignFlags& align)
    {
        if (detail::change_if_diff<>(m_text_align, align))
            damage();
    }

    /**
     * Get the text alignment within the Label.
     */
    AlignFlags text_align() const { return m_text_align; }

    virtual void serialize(Serializer& serializer) const override;

    virtual void deserialize(const std::string& name, const std::string& value,
                             const std::map<std::string, std::string>& attrs) override;

    virtual ~ComboBox();

protected:

    /// Item array.
    ItemArray m_items;

    /// Currently selected index.
    ssize_t m_selected{-1};

    /// Popup associated with the ComboBox.
    std::shared_ptr<detail::ComboBoxPopup> m_popup;

    /// Alignment of the text.
    AlignFlags m_text_align{AlignFlag::left | AlignFlag::center};

    friend class detail::ComboBoxPopup;
};

}
}

#endif
