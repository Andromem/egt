/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifndef MUI_FONT_H
#define MUI_FONT_H

/**
 * @file
 * @brief Working with fonts.
 */

#include <string>

namespace mui
{

    /**
     * Manages a single font and properties of a font.
     *
     * A Font consists of a type face, a size, and a weight. The font face must
     * be installed on the system in order to use it.  Usually, if the specified
     * fonr face cannot be found on the system, a similar font face will be
     * selected.
     */
    class Font
    {
    public:

        /**
         * Font weights.
         */
        enum
        {
            WEIGHT_NORMAL = 0,
            WEIGHT_BOLD = 1,
            WEIGHT_ITALIC = 2
        };

        //@{
        /** @brief Pre-defined font faces. */
        static const std::string FACE_ARIAL;
        static const std::string FACE_TIMES;
        static const std::string FACE_COURIER;
        //@}

        /**
         * Set the global default Font.
         *
         * Call this before allocating any Font directly or indirectly (i.e.
         * with a Widget) to set the default font.
         */
        static void set_default_font(const Font& font);

        /**
         * Create a font based on the global default font.
         */
        Font();

        /**
         * Create a font based on the supplied parameters.
         */
        explicit Font(const std::string& face, int size, int weight);

        /**
         * Create a font based on the global default font, but with the
         * specified size.
         */
        explicit Font(int size);

        /**
         * Create a font based on the global default font, but with the
         * specified size and weight.
         */
        explicit Font(int size, int weight);

        /**
         * Get the face name the font.
         */
        const std::string& face() const { return m_face; }

        /**
         * Get the size of the font.
         */
        virtual int size() const { return m_size; }

        /**
         * Get the weight of the font.
         */
        virtual int weight() const { return m_weight; }

        virtual ~Font()
        {}

    protected:

        std::string m_face;
        int m_size;
        int m_weight;
    };

}

#endif
