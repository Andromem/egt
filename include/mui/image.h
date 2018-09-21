/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifndef MUI_IMAGE_H
#define MUI_IMAGE_H

#include <mui/widget.h>

namespace mui
{

    class Image : public Widget
    {
    public:
        Image(const std::string& filename, int x = 0, int y = 0);

        virtual void draw(const Rect& rect);

        virtual void scale(double hscale, double vscale);

        // hmm, should resize really mean scale like this?
        void resize(int w, int h)
        {
            if (box().w != w || box().h != h)
            {
                double hs = (double)box().w / (double)w;
                double vs = (double)box().h / (double)h;
                scale(hs, vs);
            }
        }

        double hscale() const
        {
            return m_hscale;
        }
        double vscale() const
        {
            return m_vscale;
        }

        shared_cairo_surface_t surface() const
        {
            return m_image;
        }

        virtual ~Image();

    protected:
        shared_cairo_surface_t m_image;
        std::string m_filename;
        double m_hscale;
        double m_vscale;
    };

}

#endif
