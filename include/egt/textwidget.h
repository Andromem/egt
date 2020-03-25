/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_TEXTWIDGET_H
#define EGT_TEXTWIDGET_H

/**
 * @file
 * @brief Base class TextWidget definition.
 */

#include <egt/detail/meta.h>
#include <egt/detail/signal.h>
#include <egt/image.h>
#include <egt/widget.h>
#include <memory>
#include <string>

namespace egt
{
inline namespace v1
{

/**
 * A widget with text and text related properties.
 *
 * This is not meant to be used directly as it does not implement at least a
 * draw() method.
 */
class EGT_API TextWidget : public Widget
{
public:

    /**
     * Event signal.
     * @{
     */
    /**
     * Invoked when the text of the widget changes.
     */
    detail::Signal<> on_text_changed;
    /** @} */

    /**
     * @param[in] text The text to display.
     * @param[in] rect Initial rectangle of the widget.
     * @param[in] text_align Alignment for the text.
     */
    explicit TextWidget(std::string text = {},
                        const Rect& rect = {},
                        const AlignFlags& text_align = AlignFlag::center) noexcept;

    EGT_OPS_NOCOPY_MOVE(TextWidget);
    ~TextWidget() noexcept override = default;

    /**
     * Set the text.
     *
     * @param str The text string to set.
     */
    virtual void text(const std::string& str);

    /**
     * Clear the text value.
     */
    virtual void clear();

    /**
     * Get the text of the Label.
     */
    virtual const std::string& text() const { return m_text; }

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

    /**
     * Get the length of the text.
     */
    virtual size_t len() const;

    /**
     * Given a Font, text, and a target Size, scale the font size so that
     * the text will will fit and return the new Font.
     */
    static Font scale_font(const Size& target, const std::string& text, const Font& font);

    void serialize(Serializer& serializer) const override;

    void deserialize(const std::string& name, const std::string& value,
                     const Serializer::Attributes& attrs) override;

protected:

    /// Get the size of the text.
    Size text_size(const std::string& text) const;

    /// Alignment of the text.
    AlignFlags m_text_align{AlignFlag::center};

    /// The text.
    std::string m_text;
};

}
}

#endif
