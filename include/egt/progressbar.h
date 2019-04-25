/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_PROGRESSBAR_H
#define EGT_PROGRESSBAR_H

#include <egt/valuewidget.h>

namespace egt
{
inline namespace v1
{

/**
 * @brief Displays a progress bar based on a value.
 *
 * @ingroup controls
 */
class ProgressBar : public ValueRangeWidget<int>
{
public:

    /**
     * @param[in] rect Rectangle for the widget.
     * @param[in] min Minimum value for the range.
     * @param[in] max Maximum value in the range.
     * @param[in] value Current value in the range.
     */
    explicit ProgressBar(const Rect& rect = Rect(),
                         int min = 0, int max = 100, int value = 0) noexcept;

    /// @todo Constructors

    virtual std::unique_ptr<Widget> clone() override
    {
        return std::unique_ptr<Widget>(make_unique<ProgressBar>(*this).release());
    }

    virtual void draw(Painter& painter, const Rect& rect) override;

    virtual ~ProgressBar()
    {}
protected:
    Font m_dynamic_font;
};

/**
 * @brief Displays a spinning progress meter.
 *
 * @ingroup controls
 */
class SpinProgress : public ValueRangeWidget<int>
{
public:

    /**
     * @param[in] rect Rectangle for the widget.
     * @param[in] min Minimum value for the range.
     * @param[in] max Maximum value in the range.
     * @param[in] value Current value in the range.
     */
    explicit SpinProgress(const Rect& rect = Rect(),
                          int min = 0, int max = 100, int value = 0) noexcept;

    /// @todo Constructors

    virtual std::unique_ptr<Widget> clone() override
    {
        return std::unique_ptr<Widget>(make_unique<SpinProgress>(*this).release());
    }

    virtual void draw(Painter& painter, const Rect& rect) override;

    virtual ~SpinProgress()
    {}

protected:
    Font m_dynamic_font;
};

/**
 * @brief Displays a level meter based on a value.
 *
 * @image html widget_levelmeter.png
 * @image latex widget_levelmeter.png "widget_levelmeter" width=5cm
 *
 * @ingroup controls
 */
class LevelMeter : public ValueRangeWidget<int>
{
public:

    /**
     * @param[in] rect Rectangle for the widget.
     * @param[in] min Minimum value for the range.
     * @param[in] max Maximum value in the range.
     * @param[in] value Current value in the range.
     */
    explicit LevelMeter(const Rect& rect = Rect(),
                        int min = 0, int max = 100, int value = 0) noexcept;

    /// @todo Constructors

    virtual std::unique_ptr<Widget> clone() override
    {
        return std::unique_ptr<Widget>(make_unique<LevelMeter>(*this).release());
    }

    virtual void draw(Painter& painter, const Rect& rect) override;

    virtual void set_num_bars(uint32_t bars)
    {
        m_num_bars = bars;
    }

    virtual uint32_t num_bars() const
    {
        return m_num_bars;
    }

    virtual ~LevelMeter()
    {}

protected:

    /**
     * The number of bars to display.
     */
    uint32_t m_num_bars{10};
};

/**
 * @brief Displays an analog meter based on a percentage value.
 *
 * @image html widget_analogmeter.png
 * @image latex widget_analogmeter.png "widget_analogmeter" width=5cm
 *
 * @ingroup controls
 */
class AnalogMeter : public ValueRangeWidget<int>
{
public:

    /**
     * @param[in] rect Rectangle for the widget.
     */
    explicit AnalogMeter(const Rect& rect = Rect());

    /// @todo Constructors

    virtual std::unique_ptr<Widget> clone() override
    {
        return std::unique_ptr<Widget>(make_unique<AnalogMeter>(*this).release());
    }

    virtual void draw(Painter& painter, const Rect& rect) override;

    virtual ~AnalogMeter()
    {}

protected:
    Font m_font;
};


}
}

#endif
