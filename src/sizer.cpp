/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/detail/enum.h"
#include "egt/detail/layout.h"
#include "egt/detail/serialize.h"
#include "egt/sizer.h"

namespace egt
{
inline namespace v1
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

void BoxSizer::layout()
{
    if (!visible())
        return;

    if (m_in_layout)
        return;

    if (m_children.empty())
        return;

    m_in_layout = true;
    detail::ScopeExit reset([this]() { m_in_layout = false; });

    resize(super_rect());

    std::vector<detail::LayoutRect> rects;

    for (auto& child : m_children)
    {
        auto min = child->box();

        if (!child->flags().is_set(Widget::Flag::no_autoresize))
        {
            if (min.width() < child->min_size_hint().width())
                min.width(child->min_size_hint().width());
            if (min.height() < child->min_size_hint().height())
                min.height(child->min_size_hint().height());
        }

        uint32_t behave = 0;

        if (child->align().is_set(AlignFlag::expand_horizontal))
            behave |= LAY_HFILL;
        else if (child->align().is_set(AlignFlag::left))
            behave |= LAY_LEFT;
        else if (child->align().is_set(AlignFlag::right))
            behave |= LAY_RIGHT;

        if (child->align().is_set(AlignFlag::expand_vertical))
            behave |= LAY_VFILL;
        else if (child->align().is_set(AlignFlag::top))
            behave |= LAY_TOP;
        else if (child->align().is_set(AlignFlag::bottom))
            behave |= LAY_BOTTOM;

        rects.emplace_back(behave, min);
    }

    detail::flex_layout(content_area(), rects, justify(), orient());

    auto child = m_children.begin();
    for (const auto& r : rects)
    {
        (*child)->box(r.rect - point() + content_area().point());
        child++;
    }

    resize(super_rect());
}

void BoxSizer::serialize(detail::Serializer& serializer) const
{
    Frame::serialize(serializer);

    serializer.add_property("orient", detail::enum_to_string(m_orient));
    serializer.add_property("justify", detail::enum_to_string(m_justify));
}

void BoxSizer::deserialize(const std::string& name, const std::string& value,
                           const std::map<std::string, std::string>& attrs)
{
    if (name == "orient")
        orient(detail::enum_from_string<Orientation>(value));
    else if (name == "justify")
        justify(detail::enum_from_string<Justification>(value));
    else
        Frame::deserialize(name, value, attrs);
}

}
}
