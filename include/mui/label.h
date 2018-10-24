/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifndef MUI_LABEL_H
#define MUI_LABEL_H

/**
 * @file
 * @brief Working with labels.
 */

#include <mui/widget.h>

namespace mui
{

    /**
     * A Label in its basic form is just some text.
     *
     * It handles no events, has a font and alignment, but otherwise is pretty
     * basic.
     */
    class Label : public Widget
    {
    public:
        explicit Label(const std::string& text = std::string(),
                       const Rect& rect = Rect(),
                       alignmask align = alignmask::CENTER,
                       const Font& font = Font(),
                       widgetmask flags = widgetmask::NO_BORDER) noexcept;

        explicit Label(Frame& parent,
                       const std::string& text = std::string(),
                       const Rect& rect = Rect(),
                       alignmask align = alignmask::CENTER,
                       const Font& font = Font(),
                       widgetmask flags = widgetmask::NO_BORDER) noexcept;

        /**
         * Set the text of the label.
         */
        virtual void text(const std::string& str);

        /**
         * Get the text of the Label.
         */
        virtual const std::string& text() const { return m_text; }

        /**
         * Set the Font of the Label.
         */
        virtual void font(const Font& font) { m_font = font; }

        /**
        * Get the Font of the Label.
         */
        virtual const Font& font() const { return m_font; }

        /**
         * Set the alignment of the Label.
         */
        virtual void text_align(alignmask align) { m_text_align = align; }

        virtual void draw(Painter& painter, const Rect& rect) override;

        virtual ~Label();

    protected:
        alignmask m_text_align;
        std::string m_text;
        Font m_font;
    };

    /**
     * Boolean checkbox.
     *
     * @todo This should be a ValueWidget<bool>.
     */
    class CheckBox : public Label
    {
    public:
        CheckBox(const std::string& text = std::string(),
                 const Rect& rect = Rect());

        /**
         * Return the boolean state of the checkbox.
         */
        inline bool checked() const
        {
            return m_checked;
        }

        /**
         * Set the checked state of the checkbox.
         */
        void check(bool value)
        {
            if (m_checked != value)
            {
                m_checked = value;
                damage();
            }
        }

        virtual int handle(int event) override;

        virtual void draw(Painter& painter, const Rect& rect) override;

        virtual ~CheckBox();

    protected:
        bool m_checked;
    };

    /**
     * CheckBox with a boolean slider style interface.
     */
    class SlidingCheckBox : public CheckBox
    {
    public:
        SlidingCheckBox(const Rect& rect = Rect());

        virtual void draw(Painter& painter, const Rect& rect) override;

        virtual ~SlidingCheckBox();
    };

    /**
     * A Label widget that also contains an image.
     */
    class ImageLabel : public Label
    {
    public:
        ImageLabel(const std::string& image,
                   const std::string& text = std::string(),
                   const Rect& rect = Rect(),
                   const Font& font = Font());

        virtual void draw(Painter& painter, const Rect& rect) override;

        virtual ~ImageLabel();

    protected:
        shared_cairo_surface_t m_image;
    };

}

#endif
