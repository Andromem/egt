/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_PROGRESSBAR_H
#define EGT_PROGRESSBAR_H

/**
 * @file
 * @brief Working with progress meters.
 */

#include <egt/detail/alignment.h>
#include <egt/detail/math.h>
#include <egt/detail/meta.h>
#include <egt/detail/serialize.h>
#include <egt/frame.h>
#include <egt/painter.h>
#include <egt/textwidget.h>
#include <egt/valuewidget.h>
#include <string>

namespace egt
{
inline namespace v1
{

/**
 * Displays a progress bar based on a value.
 *
 * Typically @ref ProgressBar, @ref ProgressBarF are used as aliases.
 *
 * @ingroup controls
 */
template<class T>
class EGT_API ProgressBarType : public ValueRangeWidget<T>
{
public:

    /**
     * @param[in] rect Rectangle for the widget.
     * @param[in] start Starting value for the range.
     * @param[in] end Ending value in the range.
     * @param[in] value Current value in the range.
     */
    explicit ProgressBarType<T>(const Rect& rect = {},
                                T start = {}, T end = 100, T value = {}) noexcept
        : ValueRangeWidget<T>(rect, start, end, value)
    {
        this->fill_flags(Theme::FillFlag::blend);
        this->border(this->theme().default_border());
    }

    /**
     * @param[in] parent The parent Frame.
     * @param[in] rect Rectangle for the widget.
     * @param[in] start Starting value for the range.
     * @param[in] end Ending value in the range.
     * @param[in] value Current value in the range.
     */
    explicit ProgressBarType(Frame& parent, const Rect& rect = {},
                             T start = {}, T end = 100, T value = {}) noexcept
        : ProgressBarType(rect, start, end, value)
    {
        parent.add(*this);
    }

    virtual void draw(Painter& painter, const Rect& rect) override
    {
        Drawer<ProgressBarType<T>>::draw(*this, painter, rect);
    }

    /**
     * Default draw method for the ProgressBar.
     */
    static void default_draw(ProgressBarType<T>& widget, Painter& painter, const Rect&)
    {
        widget.draw_box(painter, Palette::ColorId::bg, Palette::ColorId::border);

        auto b = widget.content_area();
        auto width = detail::normalize<float>(widget.value(),
                                              widget.start(),
                                              widget.end(), 0, b.width());

        if (width > 0.f)
        {
            widget.theme().draw_box(painter,
                                    Theme::FillFlag::blend,
                                    Rect(b.x(), b.y(), width, b.height()),
                                    Color(),
                                    widget.color(Palette::ColorId::button_bg));
        }

        if (widget.show_label())
        {
            std::string text = std::to_string(widget.value()) + "%";
            auto f = TextWidget::scale_font(Size(b.width() * 0.75,
                                                 b.height() * 0.75),
                                            text, widget.font());

            painter.set(widget.color(Palette::ColorId::label_text).color());
            painter.set(f);
            auto size = painter.text_size(text);
            auto target = detail::align_algorithm(size, b, AlignFlag::center);
            painter.draw(target.point());
            painter.draw(text);
        }
    }


    using ValueRangeWidget<T>::min_size_hint;

    static const Size DEFAULT_PROGRESSBAR_SIZE;

    virtual Size min_size_hint() const override
    {
        if (!this->m_min_size.empty())
            return this->m_min_size;

        return DEFAULT_PROGRESSBAR_SIZE + Widget::min_size_hint();
    }

    /**
     * Enable/disable showing the label text.
     *
     * @param[in] value When true, the label text is shown.
     */
    virtual void show_label(bool value)
    {
        if (m_show_label != value)
        {
            m_show_label = value;
            this->damage();
        }
    }

    /**
     * Get the show label state.
     */
    inline bool show_label() const { return m_show_label; }

    virtual ~ProgressBarType() = default;

protected:
    /**
     * When true, the label text is shown.
     */
    bool m_show_label{true};
};

/**
 * Helper type alias.
 * @copybrief ProgressBarType
 * @ingroup controls
 */
using ProgressBar = ProgressBarType<int>;

/**
 * Helper type alias.
 * @copybrief ProgressBarType
 * @ingroup controls
 */
using ProgressBarF = ProgressBarType<float>;

template <class T>
const Size ProgressBarType<T>::DEFAULT_PROGRESSBAR_SIZE{200, 30};

/**
 * Displays a spinning progress meter.
 *
 * Typically @ref SpinProgress, @ref SpinProgressF are used as aliases.
 *
 * @ingroup controls
 */
template <class T>
class EGT_API SpinProgressType : public ValueRangeWidget<T>
{
public:

    /**
     * @param[in] rect Rectangle for the widget.
     * @param[in] start Starting value for the range.
     * @param[in] end Ending value in the range.
     * @param[in] value Current value in the range.
     */
    explicit SpinProgressType(const Rect& rect = {},
                              T start = 0, T end = 100, T value = 0) noexcept
        : ValueRangeWidget<T>(rect, start, end, value)
    {
        this->fill_flags(Theme::FillFlag::blend);
    }

    /**
     * @param[in] parent The parent Frame.
     * @param[in] rect Rectangle for the widget.
     * @param[in] start Starting value for the range.
     * @param[in] end Ending value in the range.
     * @param[in] value Current value in the range.
     */
    explicit SpinProgressType(Frame& parent, const Rect& rect = {},
                              T start = 0, T end = 100, T value = 0) noexcept
        : SpinProgressType(rect, start, end, value)
    {
        parent.add(*this);
    }


    virtual void draw(Painter& painter, const Rect& rect) override
    {
        Drawer<SpinProgressType<T>>::draw(*this, painter, rect);
    }

    /**
     * Default draw method for the SpinProgress.
     */
    static void default_draw(SpinProgressType<T>& widget, Painter& painter, const Rect& rect)
    {
        widget.draw_box(painter, Palette::ColorId::bg, Palette::ColorId::border);

        auto b = widget.content_area();

        auto dim = std::min(b.width(), b.height());
        float linew = dim / 10.;
        float radius = dim / 2. - (linew / 2.);
        auto angle1 = detail::to_radians<float>(180, 0);
        auto angle2 = detail::to_radians<float>(180, widget.value() / 100. * 360.);

        painter.line_width(linew);
        painter.set(widget.color(Palette::ColorId::button_fg, Palette::GroupId::disabled).color());
        painter.draw(Arc(widget.center(), radius, 0.0f, 2 * detail::pi<float>()));
        painter.stroke();

        painter.set(widget.color(Palette::ColorId::button_fg).color());
        painter.draw(Arc(widget.center(), radius, angle1, angle2));
        painter.stroke();

        if (widget.show_label())
        {
            std::string text = std::to_string(widget.value());
            auto f = TextWidget::scale_font(Size(dim * 0.75, dim * 0.75), text, widget.font());
            painter.set(f);
            painter.set(widget.color(Palette::ColorId::text).color());
            auto size = painter.text_size(text);
            auto target = detail::align_algorithm(size, b, AlignFlag::center);
            painter.draw(target.point());
            painter.draw(text);
        }
    }

    using ValueRangeWidget<T>::min_size_hint;

    static const Size DEFAULT_SPINPROGRESS_SIZE;

    virtual Size min_size_hint() const override
    {
        if (!this->m_min_size.empty())
            return this->m_min_size;

        return DEFAULT_SPINPROGRESS_SIZE + Widget::min_size_hint();
    }

    /**
    * Enable/disable showing the label text.
    *
    * @param[in] value When true, the label text is shown.
    */
    virtual void show_label(bool value)
    {
        if (m_show_label != value)
        {
            m_show_label = value;
            this->damage();
        }
    }

    /**
     * Get the show label state.
     */
    inline bool show_label() const { return m_show_label; }

    virtual void serialize(detail::Serializer& serializer) const override;

    virtual void deserialize(const std::string& name, const std::string& value,
                             const std::map<std::string, std::string>& attrs) override;

    virtual ~SpinProgressType() = default;
protected:
    /**
     * When true, the label text is shown.
     */
    bool m_show_label{true};
};

/**
 * Helper type alias.
 * @copybrief SpinProgressType
 * @ingroup controls
 */
using SpinProgress = SpinProgressType<int>;

/**
 * Helper type alias.
 * @copybrief SpinProgressType
 * @ingroup controls
 */
using SpinProgressF = SpinProgressType<float>;

template <class T>
const Size SpinProgressType<T>::DEFAULT_SPINPROGRESS_SIZE{100, 100};

template <class T>
void SpinProgressType<T>::serialize(detail::Serializer& serializer) const
{
    ValueRangeWidget<T>::serialize(serializer);

    serializer.add_property("show_label", detail::to_string(this->m_show_label));
}

template <class T>
void SpinProgressType<T>::deserialize(const std::string& name, const std::string& value,
                                      const std::map<std::string, std::string>& attrs)
{
    if (name == "show_label")
        m_show_label = detail::from_string(value);
    else
        ValueRangeWidget<T>::deserialize(name, value, attrs);
}

/**
 * Displays a level meter based on a value.
 *
 * Typically @ref LevelMeter, @ref LevelMeterF are used as aliases.
 *
 * @image html widget_levelmeter.png
 * @image latex widget_levelmeter.png "widget_levelmeter" width=5cm
 *
 * @ingroup controls
 */
template <class T>
class EGT_API LevelMeterType : public ValueRangeWidget<T>
{
public:

    /**
     * @param[in] rect Rectangle for the widget.
     * @param[in] start Starting value for the range.
     * @param[in] end Ending value in the range.
     * @param[in] value Current value in the range.
     */
    explicit LevelMeterType(const Rect& rect = {},
                            T start = 0, T end = 100, T value = 0) noexcept
        : ValueRangeWidget<T>(rect, start, end, value)
    {
        this->fill_flags(Theme::FillFlag::blend);
        this->padding(2);
    }


    /**
     * @param[in] parent The parent Frame.
     * @param[in] rect Rectangle for the widget.
     * @param[in] start Starting value for the range.
     * @param[in] end Ending value in the range.
     * @param[in] value Current value in the range.
     */
    explicit LevelMeterType(Frame& parent, const Rect& rect = {},
                            T start = 0, T end = 100, T value = 0) noexcept
        : LevelMeterType(rect, start, end, value)
    {
        parent.add(*this);
    }

    virtual void draw(Painter& painter, const Rect& rect) override
    {
        Drawer<LevelMeterType<T>>::draw(*this, painter, rect);
    }

    /**
     * Default draw method for the LevelMeter.
     */
    static void default_draw(LevelMeterType<T>& widget, Painter& painter, const Rect& rect)
    {
        widget.draw_box(painter, Palette::ColorId::bg, Palette::ColorId::border);

        const auto b = widget.content_area();

        const auto limit = egt::detail::normalize<float>(widget.value(),
                           widget.start(),
                           widget.end(),
                           widget.num_bars(), 0);
        const auto barheight = b.height() / widget.num_bars();

        for (size_t i = 0; i < widget.num_bars(); i++)
        {
            auto color = widget.color(Palette::ColorId::button_fg, Palette::GroupId::disabled);
            if (i >= limit)
                color = widget.color(Palette::ColorId::button_fg);

            Rect rect(b.x(),  b.y() + i * barheight, b.width(), barheight - widget.padding());

            widget.theme().draw_box(painter,
                                    Theme::FillFlag::blend,
                                    rect,
                                    widget.color(Palette::ColorId::border),
                                    color);
        }
    }

    /**
     * Set the number of bars to show.
     */
    virtual void num_bars(size_t bars)
    {
        m_num_bars = bars;
    }

    /**
     * Get the number of bars to show.
     */
    inline size_t num_bars() const { return m_num_bars; }

    using ValueRangeWidget<T>::min_size_hint;

    static const Size DEFAULT_LEVELMETER_SIZE;

    virtual Size min_size_hint() const override
    {
        if (!this->m_min_size.empty())
            return this->m_min_size;

        return DEFAULT_LEVELMETER_SIZE + Widget::min_size_hint();
    }

    virtual void serialize(detail::Serializer& serializer) const override;

    virtual void deserialize(const std::string& name, const std::string& value,
                             const std::map<std::string, std::string>& attrs) override;

    virtual ~LevelMeterType() = default;

protected:

    /**
     * The number of bars to display.
     */
    size_t m_num_bars{10};
};

/**
 * Helper type alias.
 * @copybrief LevelMeterType
 * @ingroup controls
 */
using LevelMeter = LevelMeterType<int>;

/**
 * Helper type alias.
 * @copybrief LevelMeterType
 * @ingroup controls
 */
using LevelMeterF = LevelMeterType<float>;

template <class T>
const Size LevelMeterType<T>::DEFAULT_LEVELMETER_SIZE{40, 100};

template <class T>
void LevelMeterType<T>::serialize(detail::Serializer& serializer) const
{
    ValueRangeWidget<T>::serialize(serializer);

    serializer.add_property("num_bars", std::to_string(this->m_num_bars));
}

template <class T>
void LevelMeterType<T>::deserialize(const std::string& name, const std::string& value,
                                    const std::map<std::string, std::string>& attrs)
{
    if (name == "num_bars")
        m_num_bars = std::stoi(value);
    else
        ValueRangeWidget<T>::deserialize(name, value, attrs);
}

/**
 * Displays an analog meter based on a percentage value.
 *
 * Typically @ref AnalogMeter, @ref AnalogMeterF are used as aliases.
 *
 * @image html widget_analogmeter.png
 * @image latex widget_analogmeter.png "widget_analogmeter" width=5cm
 *
 * @ingroup controls
 */
template <class T>
class EGT_API AnalogMeterType : public ValueRangeWidget<T>
{
public:

    /**
     * @param[in] rect Rectangle for the widget.
     */
    explicit AnalogMeterType(const Rect& rect = {}) noexcept
        : ValueRangeWidget<T>(rect, 0, 100, 0)
    {
        this->fill_flags(Theme::FillFlag::blend);
    }

    /**
     * @param[in] parent The parent Frame.
     * @param[in] rect Rectangle for the widget.
     */
    explicit AnalogMeterType(Frame& parent, const Rect& rect = {}) noexcept
        : AnalogMeterType(rect)
    {
        parent.add(*this);
    }

    virtual void draw(Painter& painter, const Rect& rect) override
    {
        Drawer<AnalogMeterType<T>>::draw(*this, painter, rect);
    }

    /**
     * Default draw method for the AnalogMeter.
     */
    static void default_draw(AnalogMeterType<T>& widget, Painter& painter, const Rect& rect)
    {
        static const auto tick_width = 1.0;

        widget.draw_box(painter, Palette::ColorId::bg, Palette::ColorId::border);

        const auto b = widget.content_area();
        painter.line_width(tick_width);
        painter.set(widget.font());
        const auto text_size = painter.text_size("999");

        auto cr = painter.context().get();
        cairo_translate(cr, b.center().x(), b.y() + b.height() - text_size.height());

        const auto dim = std::min(b.width() / 2, b.height());
        const float hw = dim - (text_size.width() * 2.0);

        // ticks and labels
        for (auto tick = 0; tick <= 100; tick += 10)
        {
            const auto xangle = std::cos(detail::pi<float>() * tick * 0.01);
            const auto yangle = std::sin(detail::pi<float>() * tick * 0.01);
            painter.set(widget.color(Palette::ColorId::button_fg,
                                     Palette::GroupId::disabled).color());
            painter.draw(Point(hw * xangle,
                               -hw * yangle),
                         Point((hw + 10.0) * xangle,
                               -(hw + 10.0) * yangle));
            painter.stroke();

            const auto text = std::to_string(tick);
            painter.set(widget.color(Palette::ColorId::text).color());
            const auto size = painter.text_size(text);
            painter.draw(Point(-(hw + 30.0) * xangle - size.width() / 2.0,
                               -(hw + 30.0) * yangle - size.height() / 2.0));
            painter.draw(text);
            painter.stroke();
        }

        // needle
        const auto dest = Point((-hw - 15.0) * std::cos(detail::pi<float>() * widget.value() * 0.01),
                                (-hw - 15.0) * std::sin(detail::pi<float>() * widget.value() * 0.01));

        painter.set(widget.color(Palette::ColorId::button_fg).color());
        painter.line_width(tick_width * 2.0);
        painter.draw(Point(), dest);
        painter.stroke();
        painter.draw(Circle(Point(), 5));
        painter.draw(Circle(dest, 2));
        painter.fill();
    }

    using ValueRangeWidget<T>::min_size_hint;

    static const Size DEFAULT_ANALOGMETER_SIZE;

    virtual Size min_size_hint() const override
    {
        if (!this->m_min_size.empty())
            return this->m_min_size;

        return DEFAULT_ANALOGMETER_SIZE + Widget::min_size_hint();
    }

    virtual ~AnalogMeterType() = default;
};

/**
 * Helper type alias.
 * @copybrief AnalogMeterType
 * @ingroup controls
 */
using AnalogMeter = AnalogMeterType<int>;

/**
 * Helper type alias.
 * @copybrief AnalogMeterType
 * @ingroup controls
 */
using AnalogMeterF = AnalogMeterType<float>;

template <class T>
const Size AnalogMeterType<T>::DEFAULT_ANALOGMETER_SIZE{200, 100};

}
}

#endif
