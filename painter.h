/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifndef PAINTER_H
#define PAINTER_H

namespace mui
{

        using shared_cairo_surface_t =
	std::shared_ptr<cairo_surface_t>;

    using shared_cairo_t =
	std::shared_ptr<cairo_t>;

/**
 * <https://www.cairographics.org/manual/index.html>
 */
class Painter
{
public:

    Painter()
    {}

    Painter(shared_cairo_t cr)
	: m_cr(cr)
    {
	begin();
    }

    virtual ~Painter()
    {
	end();
    }

    void begin()
    {
	cairo_save(m_cr.get());
    }

    void end()
    {
	cairo_restore(m_cr.get());
    }

    void set_color(const Color& color)
    {
	cairo_set_source_rgba(m_cr.get(),
			      color.redf(),
			      color.greenf(),
			      color.bluef(),
			      color.alphaf());
    }

    void draw_rectangle(const Rect& rect)
    {
	cairo_rectangle(m_cr.get(),
			rect.x,
			rect.y,
			rect.w,
			rect.h);
    }

    void set_line_width(float width)
    {
	cairo_set_line_width(m_cr.get(), width);
    }

    void draw_line(const Point& start, const Point& end)
    {
	cairo_move_to(m_cr.get(), start.x, start.y);
	cairo_line_to(m_cr.get(), end.x, end.y);
    }

    void draw_line(const Point& start, const Point& end, float width)
    {
	set_line_width(width);
	cairo_move_to(m_cr.get(), start.x, start.y);
	cairo_line_to(m_cr.get(), end.x, end.y);
    }

    void draw_image(const Point& point, shared_cairo_surface_t surface)
    {
	double w = cairo_image_surface_get_width(surface.get());
	double h = cairo_image_surface_get_height(surface.get());
	cairo_set_source_surface(m_cr.get(), surface.get(), point.x, point.y);
	cairo_rectangle(m_cr.get(), point.x, point.y, w, h);
	cairo_set_operator(m_cr.get(), CAIRO_OPERATOR_OVER);
	cairo_fill(m_cr.get());
    }

    void draw_image(const Rect& rect, const Point& point, shared_cairo_surface_t surface)
    {
	cairo_set_source_surface(m_cr.get(), surface.get(), point.x, point.y);
	cairo_rectangle(m_cr.get(), rect.x, rect.y, rect.w, rect.h);
	cairo_set_operator(m_cr.get(), CAIRO_OPERATOR_OVER);
	cairo_fill(m_cr.get());
    }

    void draw_arc(const Point& point, float radius, float angle1, float angle2)
    {
	cairo_arc(m_cr.get(), point.x, point.y, radius, angle1, angle2);
    }

    void stroke()
    {
	cairo_stroke(m_cr.get());
    }

    void paint()
    {
	cairo_paint(m_cr.get());
    }

    void fill()
    {
	cairo_fill(m_cr.get());
    }

    void set_font(const Font& font)
    {
	cairo_select_font_face(m_cr.get(),
			       font.face().c_str(),
			       CAIRO_FONT_SLANT_NORMAL,
			       CAIRO_FONT_WEIGHT_BOLD);
	cairo_set_font_size(m_cr.get(), font.size());
    }

    void draw_text(const Point& point, const std::string& str)
    {
	cairo_move_to(m_cr.get(), point.x, point.y);
	cairo_show_text(m_cr.get(), str.c_str());
    }

    shared_cairo_t context() const { return m_cr; }

protected:

    shared_cairo_t m_cr;
};

}

#endif
