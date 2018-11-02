/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifndef MUI_TEXT_H
#define MUI_TEXT_H

/**
 * @file
 * @brief Working with text input.
 */

#include <mui/font.h>
#include <mui/timer.h>
#include <mui/widget.h>
#include <string>

namespace mui
{

    /**
     * Input text box.
     *
     * This supports text on a single line.
     *
     * @image html widget_textbox.png
     * @image latex widget_textbox.png "widget_txtbox" height=10cm
     */
    class TextBox : public Widget
    {
    public:
        TextBox(const std::string& str = std::string(),
                const Rect& rect = Rect());

        virtual int handle(eventid event) override;

        virtual void draw(Painter& painter, const Rect& rect) override;

        virtual void focus(bool value) override;

        /**
         * Set the text value.
         */
        virtual void text(const std::string& str);

        /**
         * Append text to the existing contents.
         */
        virtual void append(const std::string& str);

        /**
         * Clear the text value.
         */
        virtual void clear();

        /**
         * Get the value of the text.
         */
        inline const std::string& text() const { return m_text; }

	const Font& font() const { return m_font; }

        /**
         * Set the font of the label.
         */
        virtual void font(const Font& font) { m_font = font; }

        /**
         * Set the alignment of the text.
         */
        virtual void text_align(alignmask align) { m_text_align = align; }

        virtual ~TextBox();

    protected:
        void start_cursor();
        void stop_cursor();

        std::string m_text;
        alignmask m_text_align {alignmask::CENTER | alignmask::LEFT};
        Font m_font;
        PeriodicTimer m_timer;

    private:
        void cursor_timeout();
        bool m_cursor_state{false};
    };

    /**
     * Multi-line text box.
     *
     * This supports more than one line of text.
     */
    class MultilineTextBox : public TextBox
    {
    public:
        MultilineTextBox(const std::string& str = std::string(),
                         const Rect& rect = Rect());

        virtual void draw(Painter& painter, const Rect& rect) override;

        /**
         * Get the last line of the text box.
         */
        std::string last_line() const;

        virtual ~MultilineTextBox();
    };

}

#endif
