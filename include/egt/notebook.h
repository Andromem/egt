/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_NOTEBOOK_H
#define EGT_NOTEBOOK_H

/**
 * @file
 * @brief Working with notebooks.
 */

#include <egt/frame.h>
#include <vector>

namespace egt
{
inline namespace v1
{

/**
 * @brief A single layer of a Notebook.
 */
class NotebookTab : public Frame
{
public:

    NotebookTab()
    {
        set_name("NotebookTab" + std::to_string(m_widgetid));

        // tabs are not transparent by default
        set_boxtype(Theme::boxtype::blank | Theme::boxtype::solid);
    }

    virtual std::unique_ptr<Widget> clone() override
    {
        return std::unique_ptr<Widget>(make_unique<NotebookTab>(*this).release());
    }

    /**
     * @return true if allowed to leave, false otherwise.
     */
    virtual bool leave()
    {
        return true;
    }

    /**
     * Called when this tab is entered and before show() is invoked.
     */
    virtual void enter()
    {
    }

    virtual ~NotebookTab() = default;
};

/**
 * @brief Allows a collection of NotebookTab widgets to be shown one at a time.
 *
 * @ingroup controls
 */
class Notebook : public Frame
{
public:
    explicit Notebook(const Rect& rect = Rect());

    virtual std::unique_ptr<Widget> clone() override
    {
        return std::unique_ptr<Widget>(make_unique<Notebook>(*this).release());
    }

    /**
     * @todo This should explicitly only allow NotebookTab widgets.
     */
    virtual void add(const std::shared_ptr<Widget>& widget) override;

    virtual void remove(Widget* widget) override;

    virtual void set_select(uint32_t index);

    virtual ~Notebook();

protected:

    /// @private
    struct Cell
    {
        // cppcheck-suppress unusedStructMember
        std::shared_ptr<NotebookTab> widget;
        std::string name;
    };

    using cell_array = std::vector<Cell>;

    cell_array m_cells;

    int m_current_index{-1};
};

}
}

#endif
