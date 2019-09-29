/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/button.h"
#include "egt/buttongroup.h"
#include <cassert>

using namespace std;

namespace egt
{
inline namespace v1
{

ButtonGroup::ButtonGroup(bool imperative, bool exclusive) noexcept
    : m_exclusive(exclusive), m_imperative(imperative)
{
}

void ButtonGroup::set_exclusive(bool exclusive)
{
    m_exclusive = exclusive;
}

bool ButtonGroup::exclusive() const
{
    return m_exclusive;
}

void ButtonGroup::set_imperative(bool imperative)
{
    m_imperative = imperative;
}

bool ButtonGroup::imperative() const
{
    return m_imperative;
}

void ButtonGroup::add(const shared_ptr<Button>& button)
{
    /*
     * Cannot belong to several groups or several times to the same
     * group.
     */
    assert(!button->m_group);
    button->m_group = this;

    /* If imperative is set, the first button has to be checked */
    if (imperative() && m_buttons.empty() && !button->checked())
        button->m_checked = true;

    m_buttons.push_back(button);

    if (button->checked())
        checked_state_change(*button, true);
}

void ButtonGroup::remove(Button* button)
{
    if (!button)
        return;

    auto pos = std::find_if(m_buttons.begin(), m_buttons.end(), [button](const std::weak_ptr<Button>& weak)
    {
        auto real = weak.lock();
        return button == real.get();
    });
    if (pos != m_buttons.end())
    {
        button->m_group = nullptr;
        m_buttons.erase(pos);
    }
}

void ButtonGroup::checked_state_change(Button& button, bool checked) const
{
    /* Nothing to do */
    if (!exclusive() && !imperative())
        return;

    /* Is it okay to uncheck the button? */
    if (imperative() && !checked)
    {
        /* Only one button is checked so it can't be unchecked */
        if (exclusive())
        {
            button.m_checked = true;
            return;
        }

        /*
         * The button can be unchecked only if, at least, another
         * button is checked.
         */
        auto other_button_checked = std::any_of(m_buttons.begin(),
                                                m_buttons.end(),
                                                [](const std::weak_ptr<Button>& weak)
        {
            auto button = weak.lock();
            if (button)
                return button->checked();
            return false;
        });

        if (!other_button_checked)
            button.m_checked = true;

        return;
    }

    if (exclusive() && checked)
    {
        for (auto& b : m_buttons)
        {
            auto real = b.lock();
            if (real && real.get() != &button)
            {
                real->m_checked = false;
                real->damage();
                Event event(eventid::property_changed);
                real->invoke_handlers(event);
            }
        }
    }

    /*
     * We don't care about other cases:
     * - exclusive and unchecked
     * - imperative and checked
     */
}

void ButtonGroup::foreach_checked(const foreach_checked_callback_t& callback)
{
    for (auto& b : m_buttons)
    {
        auto real = b.lock();
        if (real && real->checked())
            callback(*real);
    }
}

ButtonGroup::~ButtonGroup()
{
    for (auto& button : m_buttons)
    {
        auto real = button.lock();
        if (real)
            real->m_group = nullptr;
    }
}

}

}
