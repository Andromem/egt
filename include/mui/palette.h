/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifndef MUI_PALETTE_H
#define MUI_PALETTE_H

#include <mui/color.h>
#include <memory>
#include <vector>

namespace mui
{

    /**
     * Color palette.
     *
     * The color palette defines a bank of colors used by widgets. Each widget
     * is capable of optionally maintaining its own palette if one is explicitly
     * set, otherwise the widget will default to a reference of the global
     * palette.
     *
     * The palette can be extended by simply inserting more groups and color
     * ids in the event the palette needs to be expanded to support more colors.
     */
    class Palette
    {
    public:

        enum ColorGroup
        {
            GROUP_NORMAL = 0,
            GROUP_ACTIVE,
            GROUP_DISABLED
        };

        enum ColorId
        {
            BG = 0,
            FG,
            LIGHT,
            DARK,
            MID,
            TEXT,
            TEXTBG,
            HIGHLIGHT,
            BORDER
        };

        const Color& color(ColorId id, ColorGroup group = GROUP_NORMAL) const;

        Palette& set(ColorId id, ColorGroup group, const Color& color);

        /**
         * Reset known colors back to a default state.
         */
        void reset();

    protected:

        std::vector<std::vector<Color>> m_colors;
    };

    /**
     * Get a reference to the global palette.
     */
    Palette& global_palette();

}

#endif
