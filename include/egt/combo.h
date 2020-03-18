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
#include <egt/textwidget.h>
#include <memory>
#include <string>
#include <vector>

namespace egt
{
inline namespace v1
{
class ComboBox;

namespace detail
{

/**
 * Popup class used by ComboBox.
 */
class EGT_API ComboBoxPopup : public Popup
{
public:
    explicit ComboBoxPopup(ComboBox& parent);

    virtual void handle(Event& event) override;

    virtual void show() override;

    virtual ~ComboBoxPopup() = default;

protected:

    void smart_pos();

    /**
     * ListBox of ComboBox items.
     */
    std::shared_ptr<ListBox> m_list;

    /**
     * Parent ComboBox.
     */
    ComboBox& m_parent;

    friend class egt::ComboBox;
};

}

/**
 * Combo box widget.
 *
 * This manages a list of select-able items, but otherwise just shows only what
 * is selected.
 *
 * @ingroup controls
 */
class EGT_API ComboBox : public TextWidget
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

    using TextWidget::parent;

    virtual void parent(Frame* parent) override;

    using TextWidget::min_size_hint;

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
    virtual std::string item_at(size_t index) const { return m_items[index]; }

    /**
     * Return the number of items in the list.
     */
    inline size_t item_count() const
    {
        return m_items.size();
    }

    /**
     * Remove all items from the list.
     */
    virtual void clear();

    virtual ~ComboBox() = default;

protected:

    /**
     * Item array.
     */
    ItemArray m_items;

    /**
     * Currently selected index.
     */
    ssize_t m_selected{-1};

    /**
     * Popup associated with the ComboBox.
     */
    std::shared_ptr<detail::ComboBoxPopup> m_popup;

    friend class detail::ComboBoxPopup;
};

}
}

#endif
