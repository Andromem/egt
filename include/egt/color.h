/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_COLOR_H
#define EGT_COLOR_H

/**
 * @file
 * @brief Working with colors.
 */

#include <array>
#include <cmath>
#include <cstdint>
#include <egt/detail/math.h>
#include <egt/detail/meta.h>
#include <initializer_list>
#include <iosfwd>
#include <map>
#include <string>
#include <utility>
#include <vector>

namespace egt
{
inline namespace v1
{

/**
 * 32 bit RGBA color.
 *
 * This manages the definition of a color, internally stored as separate
 * red, green, blue, and alpha components.
 *
 * @ref colors_colors
 */
class EGT_API Color
{
public:

    /// Red/Green/Blue type.
    using RGBType = uint32_t;
    /// Red/Green/Blue/Alpha type.
    using RGBAType = uint32_t;
    /// Single component of Red/Green/Blue/Alpha type.
    using ComponentType = uint32_t;

    constexpr Color() = default;

    /**
     * Create a color with the specified RGBA value.
     *
     * @b Example
     * @code{.cpp}
     * auto a = Color(0xRRGGBBAA);
     * @endcode
     *
     * @param[in] c RGBA value.
     */
    explicit constexpr Color(RGBAType c) noexcept
        : m_rgba
    {

        (c >> 24) & 0xff,
        (c >> 16) & 0xff,
        (c >> 8) & 0xff,
        c & 0xff

    }
    {}

    /**
     * Create a color from an existing color, but with the specified alpha value.
     *
     * @param[in] color Pre-existing color.
     * @param[in] alpha Specific alpha value.
     */
    constexpr Color(const Color& color, ComponentType alpha) noexcept
        : m_rgba
    {
        color.m_rgba[0],
                     color.m_rgba[1],
                     color.m_rgba[2],
                     alpha
    }
    {}

    /**
     * Create a color with the specified RGBA component values.
     *
     * @param[in] r Red component in range 0 - 255.
     * @param[in] g Green component in range 0 - 255.
     * @param[in] b Blue component in range 0 - 255.
     * @param[in] a Alpha component in range 0 - 255.
     */
    constexpr explicit Color(ComponentType r, ComponentType g, ComponentType b, ComponentType a = 255) noexcept
        : m_rgba
    {
        r & 0xff,
        g & 0xff,
        b & 0xff,
        a & 0xff
    }
    {}

    //@{
    /** Get RGBA component value as a float from 0.0 to 1.0. */
    constexpr float redf() const { return m_rgba[0] / 255.f; }
    constexpr float greenf() const { return m_rgba[1] / 255.f; }
    constexpr float bluef() const { return m_rgba[2] / 255.f; }
    constexpr float alphaf() const { return m_rgba[3] / 255.f; }
    //@}

    //@{
    /** Set RGBA component value as a float from 0.0 to 1.0. */
    constexpr void redf(float v) { m_rgba[0] = detail::clamp(v, 0.f, 1.f) * 255.f; }
    constexpr void greenf(float v) { m_rgba[1] = detail::clamp(v, 0.f, 1.f) * 255.f; }
    constexpr void bluef(float v) { m_rgba[2] = detail::clamp(v, 0.f, 1.f) * 255.f; }
    constexpr void alphaf(float v) { m_rgba[3] = detail::clamp(v, 0.f, 1.f) * 255.f; }
    //@}

    //@{
    /** Get RGBA component value as value from 0 to 255. */
    constexpr ComponentType red() const { return assert(m_rgba[0] <= 255), m_rgba[0]; }
    constexpr ComponentType green() const { return assert(m_rgba[1] <= 255), m_rgba[1]; }
    constexpr ComponentType blue() const { return assert(m_rgba[2] <= 255), m_rgba[2]; }
    constexpr ComponentType alpha() const { return assert(m_rgba[3] <= 255), m_rgba[3]; }
    //@}

    //@{
    /** Set RGBA component value individually from 0 to 255. */
    constexpr void red(ComponentType r) { m_rgba[0] = r & 0xff; }
    constexpr void green(ComponentType g) { m_rgba[1] = g & 0xff; }
    constexpr void blue(ComponentType b) { m_rgba[2] = b & 0xff; }
    constexpr void alpha(ComponentType a) { m_rgba[3] = a & 0xff; }
    //@}

    /// Get a 16 bit pixel representation of the Color
    constexpr uint16_t pixel16() const
    {
        const uint16_t b = (blue() >> 3) & 0x1f;
        const uint16_t g = ((green() >> 2) & 0x3f) << 5;
        const uint16_t r = ((red() >> 3) & 0x1f) << 11;

        return (uint16_t)(r | g | b);
    }

    /// Create a Color from a 16 bit pixel representation
    static constexpr Color pixel16(uint16_t c)
    {
        const uint16_t b = (c) & 0x1f;
        const uint16_t g = (c >> 5) & 0x3f;
        const uint16_t r = (c >> 11) & 0x1f;

        return Color(r, g, b, 0xff);
    }

    /// Get a 24 bit pixel representation of the Color
    constexpr RGBAType pixel24() const
    {
        return (red() << 16) |
               (green() << 8) |
               blue() |
               (0xff);
    }

    /// Create a Color from a 24 bit pixel representation
    static constexpr Color pixel24(RGBAType c)
    {
        return Color((c >> 16) & 0xff,
                     (c >> 8) & 0xff,
                     c & 0xff,
                     0xff);
    }

    /// Get a 32 bit pixel representation of the Color
    constexpr RGBAType pixel32() const
    {
        return (red() << 16) |
               (green() << 8) |
               blue() |
               (alpha() << 24);
    }

    /// Create a Color from a 24 bit pixel representation
    static constexpr Color pixel32(RGBAType c)
    {
        return Color((c >> 16) & 0xff,
                     (c >> 8) & 0xff,
                     c & 0xff,
                     (c >> 24) & 0xff);
    }

    /**
     * Get the hue, saturation, value, and alpha channel components of the color.
     *
     * @param[out] h Hue component in range 0 - 1.
     * @param[out] s Saturation component in range 0 - 1.
     * @param[out] v Value component in range 0 - 1.
     * @param[out] alpha Alpha component in range 0 - 1.
     */
    void get_hsvf(float& h, float& s, float& v, float* alpha = nullptr);

    /**
     * Get the hue, saturation, lightness, and alpha channel components of the color.
     *
     * @param[out] h Hue component in range 0 - 1.
     * @param[out] s Saturation component in range 0 - 1.
     * @param[out] l Lightness component in range 0 - 1.
     * @param[out] alpha Alpha component in range 0 - 1.
     */
    void get_hslf(float& h, float& s, float& l, float* alpha = nullptr);

    /**
     * Get the red, green, blue, and alpha channel components of the color.
     *
     * @param[out] r Red component in range 0 - 1.
     * @param[out] g Green component in range 0 - 1.
     * @param[out] b Blue component in range 0 - 1.
     * @param[out] alpha Alpha component in range 0 - 1.
     */
    void get_rgbaf(float& r, float& g, float& b, float* alpha = nullptr);

    /**
     * Create a color from only a RGB value with separate alpha channel component.
     *
     * @b Example
     * @code{.cpp}
     * auto a = Color::rgb(0xRRGGBB);
     * @endcode
     *
     * @param[in] c RGB value.
     * @param[in] alpha Alpha component in range 0 - 255.
     */
    static constexpr Color rgb(RGBType c, ComponentType alpha = 255) noexcept
    {
        return Color(c << 8 | alpha);
    }

    /**
     * Create a color from float values.
     *
     * @param[in] r Component value as a float from 0.0 to 1.0.
     * @param[in] g Component value as a float from 0.0 to 1.0.
     * @param[in] b Component value as a float from 0.0 to 1.0.
     * @param[in] a Component value as a float from 0.0 to 1.0.
     */
    static constexpr Color rgbaf(float r, float g, float b, float a = 1.0)
    {
        Color result;
        result.redf(r);
        result.greenf(g);
        result.bluef(b);
        result.alphaf(a);
        return result;
    }

    /**
     * Create a Color with a hex CSS string.
     *
     * For example, the string #0074D9 can be used to specify a blue-like
     * color from red, green, blue values. Alpha is not included.
     *
     * @b Example
     * @code{.cpp}
     * auto a = Color::css("#0074D9");
     * auto b = Color::css("0074D9");
     * @endcode
     */
    static Color css(const std::string& hex);

    /**
     * Return a string hex representation of the color.
     */
    std::string hex() const;

    /**
     * Create a color from HSV values.
     *
     * @param[in] h Hue component in range 0 - 1.
     * @param[in] s Saturation component in range 0 - 1.
     * @param[in] v Value component in range 0 - 1.
     * @param[in] alpha Alpha component in range 0 - 1.
     */
    static Color hsvf(float h, float s, float v, float alpha = 1.0);

    /**
     * Create a color from HSL values.
     *
     * @param[in] h Hue component in range 0 - 1.
     * @param[in] s Saturation component in range 0 - 1.
     * @param[in] l Lightness component in range 0 - 1.
     * @param[in] alpha Alpha component in range 0 - 1.
     */
    static Color hslf(float h, float s, float l, float alpha = 1.0);

    /**
     * Create a shade (darker) color based off this color given a factor.
     *
     * The larger the factor, the darker the shade.
     *
     * @param[in] factor Value from 0.0 to 1.0.
     */
    constexpr Color shade(float factor) const
    {
        return Color(red() * (1. - factor),
                     green() * (1. - factor),
                     blue() * (1. - factor),
                     alpha());
    }

    /**
     * Create a tint (lighter) color based off this color given a factor.
     *
     * The larger the factor the lighter the tint.
     *
     * @param[in] factor Value from 0.0 to 1.0.
     */
    constexpr Color tint(float factor) const
    {
        return Color(red() + ((255 - red()) * factor),
                     green() + ((255 - green()) * factor),
                     blue() + ((255 - blue()) * factor),
                     alpha());
    }

    /**
     * Create a new color by applying a hue value.
     */
    Color hue(float h) const
    {
        auto u = std::cos(h * detail::pi<float>() / 180.);
        auto w = std::sin(h * detail::pi<float>() / 180.);

        Color ret;
        ret.red((.299 + .701 * u + .168 * w) * red()
                + (.587 - .587 * u + .330 * w) * green()
                + (.114 - .114 * u - .497 * w) * blue());
        ret.green((.299 - .299 * u - .328 * w) * red()
                  + (.587 + .413 * u + .035 * w) * green()
                  + (.114 - .114 * u + .292 * w) * blue());
        ret.blue((.299 - .3 * u + 1.25 * w) * red()
                 + (.587 - .588 * u - 1.05 * w) * green()
                 + (.114 + .886 * u - .203 * w) * blue());
        return ret;
    }

    /**
     * Assign an RGBA value.
     *
     * @b Example
     * @code{.cpp}
     * Color a = 0xRRGGBBAA;
     * @endcode
     *
     * @param[in] c RGBA value.
     */
    Color& operator=(RGBAType c)
    {
        m_rgba[0] = (c >> 24) & 0xff;
        m_rgba[1] = (c >> 16) & 0xff;
        m_rgba[2] = (c >> 8) & 0xff;
        m_rgba[3] = c & 0xff;

        return *this;
    }

    /// Color operator
    template<class T>
    constexpr Color operator+(T scalar) const
    {
        return Color::rgbaf(redf() + scalar,
                            greenf() + scalar,
                            bluef() + scalar,
                            alphaf() + scalar);
    }

    /// Color operator
    template<class T>
    constexpr Color operator-(T scalar) const
    {
        return Color::rgbaf(redf() - scalar,
                            greenf() - scalar,
                            bluef() - scalar,
                            alphaf() - scalar);
    }

    /// Color operator
    template<class T>
    constexpr Color operator*(T scalar) const
    {
        return Color::rgbaf(redf() * scalar,
                            greenf() * scalar,
                            bluef() * scalar,
                            alphaf() * scalar);
    }

    /// Color operator
    template<class T>
    constexpr Color operator/(T scalar) const
    {
        return Color::rgbaf(redf() / scalar,
                            greenf() / scalar,
                            bluef() / scalar,
                            alphaf() / scalar);
    }

    /// Color operator
    constexpr Color operator+(const Color& rhs) const
    {
        return Color::rgbaf(redf() + rhs.redf(),
                            greenf() + rhs.greenf(),
                            bluef() + rhs.bluef(),
                            alphaf() + rhs.alphaf());
    }

    /// Color operator
    constexpr Color operator-(const Color& rhs) const
    {
        return Color::rgbaf(redf() - rhs.redf(),
                            greenf() - rhs.greenf(),
                            bluef() - rhs.bluef(),
                            alphaf() - rhs.alphaf());
    }

    /// Color operator
    Color operator*(const Color& rhs) const
    {
        return Color::rgbaf(redf() * rhs.redf(),
                            greenf() * rhs.greenf(),
                            bluef() * rhs.bluef(),
                            alphaf() * rhs.alphaf());
    }

    /// Color operator
    constexpr Color operator/(const Color& rhs) const
    {
        return Color::rgbaf(redf() / rhs.redf(),
                            greenf() / rhs.greenf(),
                            bluef() / rhs.bluef(),
                            alphaf() / rhs.alphaf());
    }

    /**
     * Perform linear interpolation between two colors in the HSV colorspace.
     *
     * @param a The first color.
     * @param b The second color.
     * @param t Value from 0 to 1.
     */
    static Color interp_hsv(const Color& a, const Color& b, float t);

    /**
     * Perform linear interpolation between two colors in the HSL colorspace.
     *
     * @param a The first color.
     * @param b The second color.
     * @param t Value from 0 to 1.
     */
    static Color interp_hsl(const Color& a, const Color& b, float t);

    /**
     * Perform linear interpolation between two colors in the RGB colorspace.
     *
     * @param a The first color.
     * @param b The second color.
     * @param t Value from 0 to 1.
     *
     * @note The interpolation happens in the RGB colorspace with the formula
     * c = a + (b - a) * t.
     */
    static Color interp_rgba(const Color& a, const Color& b, float t);

protected:

    /**
     * RGBA value.
     *
     * red[0], green[1], blue[2], alpha[3]
     *
     * @note std::valarray would have made more sense due to all the scalar
     * operations, but we lose constexpr with it.
     */
    ComponentType m_rgba[4] {};
};

static_assert(detail::rule_of_5<Color>(),
              "Color : must fulfill rule of 5");

/// Color operator
constexpr bool operator==(const Color& lhs, const Color& rhs)
{
    return lhs.red() == rhs.red() &&
           lhs.green() == rhs.green() &&
           lhs.blue() == rhs.blue() &&
           lhs.alpha() == rhs.alpha();
}

/// Color operator
constexpr bool operator!=(const Color& lhs, const Color& rhs)
{
    return !(lhs == rhs);
}

/// Color operator
template<class T>
constexpr Color operator+(T scalar, const Color& rhs)
{
    return Color::rgbaf(rhs.redf() + scalar,
                        rhs.greenf() + scalar,
                        rhs.bluef() + scalar,
                        rhs.alphaf() + scalar);
}

/// Color operator
template<class T>
constexpr Color operator-(T scalar, const Color& rhs)
{
    return Color::rgbaf(rhs.redf() - scalar,
                        rhs.greenf() - scalar,
                        rhs.bluef() - scalar,
                        rhs.alphaf() - scalar);
}

/// Color operator
template<class T>
constexpr Color operator*(T scalar, const Color& rhs)
{
    return Color::rgbaf(rhs.redf() * scalar,
                        rhs.greenf() * scalar,
                        rhs.bluef() * scalar,
                        rhs.alphaf() * scalar);
}

/// Color operator
template<class T>
constexpr Color operator/(T scalar, const Color& rhs)
{
    return Color::rgbaf(rhs.redf() / scalar,
                        rhs.greenf() / scalar,
                        rhs.bluef() / scalar,
                        rhs.alphaf() / scalar);
}

/// Overloaded std::ostream insertion operator
EGT_API std::ostream& operator<<(std::ostream& os, const Color& color);

/**
 * A Pattern which can store one or more colors at different offsets (steps)
 * which can be used to create complex gradients.
 */
class EGT_API Pattern
{
public:
    Pattern() noexcept = default;

    /**
     * @param[in] color Solid color of the pattern.
     */
    // cppcheck-suppress noExplicitConstructor
    Pattern(const Color& color)
    {
        m_steps.insert(std::make_pair(0.f, color));
    }

    /**
     * @param[in] steps Color steps of the pattern.
     */
    explicit Pattern(const std::vector<std::pair<float, Color>>& steps)
    {
        for (auto& s : steps)
            m_steps.insert(s);
    }

    /**
     * Get the first color of the pattern.
     */
    operator Color() const
    {
        if (!m_steps.empty())
            return m_steps.begin()->second;

        return {};
    }

    /**
     * Get the first color of the pattern.
     */
    const Color& color() const
    {
        if (!m_steps.empty())
            return m_steps.begin()->second;

        static Color tmp;
        return tmp;
    }

    /**
     * Get the first color of the pattern.
     */
    Color& color()
    {
        if (!m_steps.empty())
            return m_steps.begin()->second;

        static Color tmp;
        return tmp;
    }

    /**
     * Add a new step to the pattern.
     */
    void step(float offset, const Color& color)
    {
        m_steps.insert(std::make_pair(offset, color));
    }

    /**
     * Get each step of the pattern.
     */
    const std::map<float, Color>& steps() const
    {
        return m_steps;
    }

protected:

    /// Steps of the pattern
    std::map<float, Color> m_steps;
};

namespace experimental
{

/**
 * A ColorMap contains a series of sequential color steps that can be used for
 * generating colors by interpolation.
 */
class EGT_API ColorMap : private detail::NonCopyable<ColorMap>
{
public:

    /// Interpolation colorspace method
    enum class Interpolation
    {
        rgba,
        hsv,
        hsl
    };

    /// Type used for color steps array
    using StepsArray = std::vector<Color>;

    ColorMap() noexcept = default;

    /**
     * @param[in] interp Interpolation colorspace.
     */
    explicit ColorMap(Interpolation interp) noexcept
        : m_interp(interp)
    {}

    /**
     * @param[in] steps Pre-defined color steps.
     * @param[in] interp Interpolation colorspace.
     */
    explicit ColorMap(StepsArray steps, Interpolation interp = Interpolation::rgba)
        : m_steps(std::move(steps)),
          m_interp(interp)
    {}

    /**
     * @param[in] steps Pre-defined color steps.
     * @param[in] interp Interpolation colorspace.
     */
    template<class T>
    explicit ColorMap(std::initializer_list<T> steps, Interpolation interp = Interpolation::rgba)
        : m_interp(interp)
    {
        m_steps.insert(m_steps.end(), steps.begin(), steps.end());
    }

    /// Append a color step.
    void step(const Color& color)
    {
        m_steps.emplace_back(color);
        for (auto& x : m_cache)
            x.clear();
    }

    /// Set the color steps.
    void steps(const StepsArray& steps)
    {
        m_steps = steps;
        for (auto& x : m_cache)
            x.clear();
    }

    /**
     * Get a color at the specified offset.
     *
     * @param[in] t Offset from 0 to 1.
     */
    Color interp(float t) const;

    /**
     * Get a color at the specified offset.
     *
     * This will use a cache to speed up repetative calls to interpolate.
     *
     * @param[in] t Offset from 0 to 1.
     */
    Color interp_cached(float t) const;

    /// Get a reference to the color steps array.
    const StepsArray& steps() const { return m_steps; }

    /// Are there any color steps?
    bool empty() const { return m_steps.empty(); }

    /// Count the number of color steps.
    size_t count() const { return m_steps.size(); }

protected:

    /// Steps in the color map.
    StepsArray m_steps;

    /// Interpolation cache.
    mutable std::array<std::map<size_t, Color>, 3> m_cache{};

    /// Interpolation colorspace method.
    Interpolation m_interp{Interpolation::rgba};
};

}

}
}

#endif
