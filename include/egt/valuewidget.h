/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifndef EGT_VALUEWIDGET_H
#define EGT_VALUEWIDGET_H

#include <egt/widget.h>
#include <egt/painter.h>
#include <string>

namespace egt
{

    /**
     * A widget that manages an unbounded value.
     *
     * While the value is technically unbounded, a type of bool will only allow
     * a boolean value.
     */
    template<class T>
    class ValueWidget : public Widget
    {
    public:

        ValueWidget(const Rect& rect, T value = T())
            : Widget(rect),
              m_value(value)
        {}

        /**
         * Set the value.
         *
         * If this results in changing the value, it will damage() the widget.
         */
        virtual void value(T v)
        {
            if (v != m_value)
            {
                m_value = v;
                damage();
                invoke_handlers(eventid::PROPERTY_CHANGED);
            }
        }

        /**
         * Get the value.
         */
        inline T value() const { return m_value; }

        virtual ~ValueWidget()
        {}

    protected:
        T m_value;
    };

    /**
     * A widget that manages a value that must reside between a min and max
     * value range.
     */
    template<class T>
    class ValueRangeWidget : public Widget
    {
    public:

        ValueRangeWidget(const Rect& rect, T min, T max,
                         T value = T())
            : Widget(rect),
              m_min(min),
              m_max(max),
              m_value(value)
        {}

        /**
         * Set value.
         *
         * If the value is above max, the value will be set to max.  If the
         * value is below min, the value will be set to min.
         *
         * If this results in changing the value, it will damage() the widget.
         */
        virtual void value(T v)
        {
            if (v > m_max)
                v = m_max;

            if (v < m_min)
                v = m_min;

            if (v != m_value)
            {
                m_value = v;
                damage();
                invoke_handlers(eventid::PROPERTY_CHANGED);
            }
        }

        /**
         * Get the current value.
         */
        T value() const
        {
            return m_value;
        }

        virtual ~ValueRangeWidget()
        {}

    protected:
        T m_min;
        T m_max;
        T m_value;
    };

    /**
     * Radial dial widget that a user uses to select a value.
     *
     * @image html widget_radial.png
     * @image latex widget_radial.png "widget_radial" height=10cm
     */
    template<class T>
    class Radial : public ValueRangeWidget<T>
    {
    public:

        Radial(const Rect& rect, T min, T max, T value = T())
            : ValueRangeWidget<T>(rect, min, max, value)
        {}

        inline T value2() const
        {
            return m_value2;
        }

        inline void value2(T v)
        {
            if (v > this->m_max)
                v = this->m_max;

            if (v < this->m_min)
                v = this->m_min;

            if (v != this->m_value2)
            {
                this->m_value2 = v;
                this->damage();
                this->invoke_handlers(eventid::PROPERTY_CHANGED);
            }
        }

        /**
         * Set the center label text of the dial.
         */
        void text(const std::string& text)
        {
            m_text = text;
        }

        virtual int handle(eventid event) override
        {
            switch (event)
            {
            case eventid::MOUSE_DOWN:
            {
                this->set_active(true);
                return 1;
            }
            case eventid::MOUSE_UP:
            {
                this->set_active(false);
                return 1;
            }
            case eventid::MOUSE_MOVE:
            {
                if (this->active())
                {
                    auto angle = this->touch_to_degrees(this->screen_to_frame(event_mouse()));
                    auto v = this->degrees_to_value(angle);
                    this->value(v);

                    return 1;
                }
                break;
            }
            default:
                break;
            }

            return Widget::handle(event);
        }

        virtual void draw(Painter& painter, const Rect& rect) override
        {
            ignoreparam(rect);

            auto v = this->value_to_degrees(this->value());
            float linew = 40;

            Color color1 = this->palette().color(Palette::BG);
            color1.alpha(0x55);
            Color color2 = this->palette().color(Palette::HIGHLIGHT);
            Color color3 = this->palette().color(Palette::MID);

            float radius = this->w() / 2 - (linew / 2);
            double angle1 = to_radians<float>(-90, 0);
            double angle2 = to_radians<float>(-90, v);

            Point c = this->center();

            auto cr = painter.context();

            cairo_save(cr.get());

            // bottom full circle
            cairo_set_source_rgba(cr.get(),
                                  color1.redf(),
                                  color1.greenf(),
                                  color1.bluef(),
                                  color1.alphaf());
            cairo_set_line_width(cr.get(), linew);
            cairo_arc(cr.get(), c.x, c.y, radius, 0, 2 * M_PI);
            cairo_stroke(cr.get());

            // value arc
            cairo_set_source_rgb(cr.get(),
                                 color2.redf(),
                                 color2.greenf(),
                                 color2.bluef());
            cairo_set_line_width(cr.get(), linew - (linew / 3));
            cairo_arc(cr.get(), c.x, c.y, radius, angle1, angle2);
            cairo_stroke(cr.get());

            // handle
            cairo_set_source_rgb(cr.get(),
                                 color3.redf(),
                                 color3.greenf(),
                                 color3.bluef());
            cairo_set_line_width(cr.get(), linew);
            cairo_arc(cr.get(), c.x, c.y, radius, angle2 - 0.3, angle2);
            cairo_stroke(cr.get());


            // secondary value
            Color color4 = Color::RED;
            double angle3 = to_radians<float>(-90,
                                              this->value_to_degrees(this->value2()));
            cairo_set_source_rgb(cr.get(),
                                 color4.redf(),
                                 color4.greenf(),
                                 color4.bluef());
            cairo_set_line_width(cr.get(), linew);
            cairo_arc(cr.get(), c.x, c.y, radius, angle3 - 0.01, angle3 + 0.01);
            cairo_stroke(cr.get());

            if (!m_text.empty())
                painter.draw_text(m_text, this->box(), this->palette().color(Palette::TEXT),
                                  alignmask::CENTER, 0, Font(72));

            cairo_restore(cr.get());
        }

        virtual ~Radial()
        {}

    protected:

        float touch_to_degrees(const Point& point)
        {
            const Point c = this->center();
            float radians = c.angle_to<float>(point);
            float angle = to_degrees(radians);
            if (angle < 0)
                angle = angle + 360.;
            return angle;
        }

        /**
         * Normalize a value to degrees.
         */
        float value_to_degrees(T value)
        {
            float n = (static_cast<float>(value) -
                       static_cast<float>(this->m_min)) /
                      (static_cast<float>(this->m_max) - static_cast<float>(this->m_min));
            return n * 360.;
        }

        /**
         * Normalize degrees to a value.
         */
        T degrees_to_value(float degrees)
        {
            float n = degrees / 360.;
            return (n * (this->m_max - this->m_min)) + this->m_min;
        }

        std::string m_text;

        T m_value2{};

        Radial() = delete;
    };

    /**
     * Displays a progress bar based on a value.
     */
    class ProgressBar : public ValueRangeWidget<int>
    {
    public:
        explicit ProgressBar(const Rect& rect = Rect());

        virtual void draw(Painter& painter, const Rect& rect);
    };

    /**
     * Displays a level meter based on a value.
     *
     * @image html widget_levelmeter.png
     * @image latex widget_levelmeter.png "widget_levelmeter" height=10cm
     */
    class LevelMeter : public ValueRangeWidget<int>
    {
    public:
        explicit LevelMeter(const Rect& rect = Rect());

        virtual void draw(Painter& painter, const Rect& rect);
    };

    /**
     * Displays an analog meter based on a value.
     *
     * @image html widget_analogmeter.png
     * @image latex widget_analogmeter.png "widget_analogmeter" height=10cm
     *
     * <http://www.peteronion.org.uk/GtkExamples/GladeTutorials.html>
     */
    class AnalogMeter : public ValueRangeWidget<int>
    {
    public:
        explicit AnalogMeter(const Rect& rect = Rect());

        virtual void draw(Painter& painter, const Rect& rect);

    protected:
        Font m_font;
    };

    class SpinProgress : public ValueRangeWidget<int>
    {
    public:
        explicit SpinProgress(const Rect& rect = Rect());

        virtual void draw(Painter& painter, const Rect& rect);
    };

    enum orientation
    {
        HORIZONTAL,
        VERTICAL,
    };

    /**
     * This is a slider that can be used to select fro a range of values.
     *
     * @todo This should be a ValueRangeWidget<int>.
     *
     * @image html widget_slider1.png
     * @image latex widget_slider1.png "widget_slider1" height=10cm
     * @image html widget_slider2.png
     * @image latex widget_slider2.png "widget_slider2" height=10cm
     */
    class Slider : public Widget
    {
    public:
        Slider(int min, int max, const Rect& rect = Rect(),
               orientation orient = orientation::HORIZONTAL);

        virtual int handle(eventid event) override;

        virtual void draw(Painter& painter, const Rect& rect) override;

        /**
         * Get the current position.
         */
        inline int position() const { return m_pos; }

        /**
         * Change the current position.
         */
        inline void position(int pos)
        {
            if (pos > m_max)
            {
                pos = m_max;
            }
            else if (pos < m_min)
            {
                pos = m_min;
            }

            if (pos != m_pos)
            {
                m_pos = pos;
                damage();

                // live update to handlers?
                if (false)
                    this->invoke_handlers(eventid::PROPERTY_CHANGED);
                else
                    m_invoke_pending = true;
            }
        }

        virtual ~Slider();

    protected:

        // position to offset
        inline int normalize(int pos)
        {
            if (m_orientation == orientation::HORIZONTAL)
            {
                auto dim = w() / 6;
                if (dim > h())
                    dim = h();


                return float(w() - dim) / float(m_max - m_min) * float(pos - m_min);
            }
            else
            {
                auto dim = h() / 6;
                if (dim > w())
                    dim = w();

                pos = m_min + m_max - pos;
                return float(h() - dim) / float(m_max - m_min) * float(pos - m_min);
            }
        }

        // offset to position
        inline int denormalize(int diff)
        {
            if (m_orientation == orientation::HORIZONTAL)
            {
                auto dim = w() / 6;
                if (dim > h())
                    dim = h();

                return float(m_max - m_min) / float(w() - dim) * float(diff);
            }
            else
            {
                auto dim = h() / 6;
                if (dim > w())
                    dim = w();

                return float(m_max - m_min) / float(h() - dim) * float(diff);
            }
        }

        int m_min;
        int m_max;
        int m_pos;
        int m_moving_x{0};
        int m_start_pos{0};
        orientation m_orientation;
        bool m_invoke_pending{false};
    };

}

#endif
