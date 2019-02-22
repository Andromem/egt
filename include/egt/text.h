/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_TEXT_H
#define EGT_TEXT_H

/**
 * @file
 * @brief Working with text input.
 */

#include <egt/font.h>
#include <egt/textwidget.h>
#include <egt/timer.h>
#include <string>
#include <unordered_set>

namespace egt
{
inline namespace v1
{

/**
 * Input text box.
 *
 * Supported Features:
 * - Text entry
 * - Cursor movement
 * - Selection Copy/Delete
 * - Multiline
 *
 * @note Right now this is limited to english. Support for full UTF-8 encoding
 * requires some additional changes.
 *
 * @image html widget_textbox.png
 * @image latex widget_textbox.png "widget_txtbox" height=5cm
 */
class TextBox : public TextWidget
{
public:

    enum class flag
    {
        /**
         * Support multiline
         */
        multiline,

        /**
         * Wrap lines longer than the width of the textbox.  Must be multiline.
         */
        word_wrap,
    };

    struct textbox_flags_hash
    {
        std::size_t operator()(flag const& s) const noexcept
        {
            return std::hash<int> {}(static_cast<int>(s));
        }
    };

    using flags = std::unordered_set<flag, TextBox::textbox_flags_hash>;

    TextBox(const std::string& str,
            const Rect& rect = Rect(),
            alignmask align = alignmask::CENTER | alignmask::LEFT,
            const flags& flags = flags());

    TextBox(const Rect& rect = Rect(),
            alignmask align = alignmask::CENTER | alignmask::LEFT,
            const flags& flags = flags());

    virtual int handle(eventid event) override;

    virtual void draw(Painter& painter, const Rect& rect) override;

    virtual void on_gain_focus() override;

    virtual void on_lost_focus() override;

    virtual void set_text(const std::string& str) override;

    virtual void clear() override;

    /**
     * Test if the specified textbox flag is set.
     * @param flag The flag to test.
     */
    inline bool is_text_flag_set(flag flag) const
    {
        return m_text_flags.find(flag) != m_text_flags.end();
    }

    /**
     * Test if the specified txtbox flags are set.
     * @param flags The flags to test.
     */
    inline bool is_text_flag_set(flags flags) const
    {
        for (auto& flag : flags)
            if (!is_text_flag_set(flag))
                return false;
        return !m_text_flags.empty() && !flags.empty();
    }

    /**
     * Set the specified txtbox flag.
     * @param flag The flag to set.
     */
    inline void set_text_flag(flag flag) { m_text_flags.insert(flag); }

    /**
     * Set the specified textbox flags.
     * @param flags Flags to set.
     */
    inline void set_txt_flag(flags flags) { m_text_flags.insert(flags.begin(), flags.end()); }

    /**
     * Clear, or unset, the specified textbox flag(s).
     * @param flag Bitmask of flags.
     */
    inline void clear_text_flag(flag flag) { m_text_flags.erase(m_text_flags.find(flag)); }

    /**
     * Move the cursor to the end and insert.
     *
     * The is the same as doing:
     * @code{.cpp}
     * cursor_set_end();
     * insert(str);
     * @endcode
     *
     * @param str The string to append.
     */
    size_t append(const std::string& str);

    /**
     * Insert text at the cursor.
     *
     * @param str The string to insert.
     * @return The number of characters inserted.
     */
    size_t insert(const std::string& str);

    /**
     * Get the cursor position.
     */
    size_t cursor() const;

    /**
     * Move the cursor to the beginning.
     */
    void cursor_set_begin();

    /**
     * Move the cursor to the end.
     */
    void cursor_set_end();

    /**
     * Move the cursor forward by the specified count from the current
     * position.
     *
     * @param count The number of characters to move.
     */
    void cursor_forward(size_t count = 1);

    /**
     * Move the cursor backward by the specified count from the current
     * position.
     *
     * @param count The number of characters to move.
     */
    void cursor_backward(size_t count = 1);

    /**
     * Set the cursor to the specific position.
     *
     * @param pos The nw cursor position.
     */
    void cursor_set(size_t pos);

    /**
     * Select all of the text.
     */
    void set_selection_all();

    /**
     * Set the selection of text.
     *
     * @param pos The starting posiiton.
     * @param length The length of the selection.
     */
    void set_selection(size_t pos, size_t length);

    /**
     * Clear any selection.
     *
     * @note This does not delete text, it just clears the selection. To delete
     * the selected text call delete_selection().
     */
    void clear_selection();

    /**
     * Get, or copy, the selected text.
     *
     * @param The selected text, or an empty string if there is no selection.
     */
    std::string get_selected_text() const;

    /**
     * Delete the selected text.
     */
    void delete_selection();

    virtual ~TextBox() = default;

protected:

    /**
     * Show/enable the visibility of the cursor.
     */
    void show_cursor();

    /**
     * Hide/disable the visibility of the cursor.
     */
    void hide_cursor();

    /**
     * Process key events.
     */
    int handle_key(eventid event);

    /**
     * Timer for blinking the cursor.
     */
    PeriodicTimer m_timer;

    /**
     * The current position of the cursor.
     */
    size_t m_cursor_pos{0};

    /**
     * Selection start position.
     */
    size_t m_select_start{0};

    /**
     * Selection length.
     */
    size_t m_select_len{0};

private:

    /**
     * Callback for the cursor timeout.
     */
    void cursor_timeout();

    /**
     * The current visible state of the cursor.
     */
    bool m_cursor_state{false};

    /**
     * TextBox flags.
     */
    flags m_text_flags;
};

}
}

#endif
