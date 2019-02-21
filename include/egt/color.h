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

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iosfwd>
#include <string>

namespace egt
{
inline namespace v1
{

/**
 * 32 bit RGBA color.
 *
 * This manages the definition of a color, internally stored as seperate
 * red, gleen, blue, and alpha components.
 */
class Color
{
public:

    /**
     * Create a color with the specified RGBA value.
     *
     * @param[in] c RGBA value.
     */
    explicit Color(uint32_t c = 0) noexcept
    {
        m_r = (c >> 24) & 0xFF;
        m_g = (c >> 16) & 0xFF;
        m_b = (c >> 8) & 0xFF;
        m_a = c & 0xFF;
    }

    /**
     * Create a color with the specified RGBA component values.
     *
     * @param[in] r Red component in range 0 - 255.
     * @param[in] g Green component in range 0 - 255.
     * @param[in] b Blue component in range 0 - 255.
     * @param[in] a Alpha component in range 0 - 255.
     */
    explicit Color(uint32_t r, uint32_t g, uint32_t b, uint32_t a = 255) noexcept
        : m_r(r),
          m_g(g),
          m_b(b),
          m_a(a)
    {}

    Color(const Color&) noexcept = default;
    Color(Color&&) noexcept = default;
    Color& operator=(const Color&) noexcept = default;
    Color& operator=(Color&&) noexcept = default;

    /**
     * Create a Color with a hex CSS string.
     *
     * For example, the string #0074D9 can be used to specify a blue-like
     * color.
     */
    static inline Color CSS(const std::string& hex)
    {
        std::string str = hex;
        str.erase(std::remove(str.begin(), str.end(), '#'), str.end());
        return Color((std::stoi(str, nullptr, 16) << 8) | 0xff);
    }

    static inline Color hue(const Color& in, float h)
    {
        float u = std::cos(h * M_PI / 180);
        float w = std::sin(h * M_PI / 180);

        Color ret;
        ret.red((.299 + .701 * u + .168 * w)*in.red()
                + (.587 - .587 * u + .330 * w)*in.green()
                + (.114 - .114 * u - .497 * w)*in.blue());
        ret.green((.299 - .299 * u - .328 * w)*in.red()
                  + (.587 + .413 * u + .035 * w)*in.green()
                  + (.114 - .114 * u + .292 * w)*in.blue());
        ret.blue((.299 - .3 * u + 1.25 * w)*in.red()
                 + (.587 - .588 * u - 1.05 * w)*in.green()
                 + (.114 + .886 * u - .203 * w)*in.blue());
        return ret;
    }

    //@{
    /** @brief RGBA component value as a float from 0.0 to 1.0. */
    inline float redf() const { return m_r / 255.; }
    inline float greenf() const { return m_g / 255.; }
    inline float bluef() const { return m_b / 255.; }
    inline float alphaf() const { return m_a / 255.; }
    //@}

    //@{
    /** @brief Set RGBA component value as a float from 0.0 to 1.0. */
    inline void redf(float v) { m_r = v; }
    inline void greenf(float v) { m_g = v; }
    inline void bluef(float v) { m_b = v; }
    inline void alphaf(float v) { m_a  = v; }
    //@}

    //@{
    /** @brief RGBA component value as value from 0 to 255. */
    inline uint32_t red() const { return m_r; }
    inline uint32_t green() const { return m_g; }
    inline uint32_t blue() const { return m_b; }
    inline uint32_t alpha() const { return m_a; }
    //@}

    //@{
    /** @brief Set RGBA component value individually from 0 to 255. */
    inline void red(uint32_t r) { m_r = r; }
    inline void green(uint32_t g) { m_g = g; }
    inline void blue(uint32_t b) { m_b = b; }
    inline void alpha(uint32_t a) { m_a = a; }
    //@}

    /**
     * Create a shade (darker) color based off this color given a factor.
     *
     * The smaller the factor, the darker the shade.
     *
     * @param[in] factor Value from 0.0 to 1.0.
     */
    inline Color shade(float factor) const
    {
        return Color(red() * (1. - factor),
                     green() * (1. - factor),
                     blue() * (1. - factor),
                     alpha());
    }

    /**
     * Create a tint (lighter) color based off this color given a factor.
     *
     * The greater the factor the lighter the tint.
     *
     * @param[in] factor Value from 0.0 to 1.0.
     */
    inline Color tint(float factor) const
    {
        return Color(red() + ((255. - red()) * factor),
                     green() + ((255. - green()) * factor),
                     blue() + ((255. - blue()) * factor),
                     alpha());
    }

    /**
     * Return a 32 bit ARGB pixel value for this color.
     */
    inline uint32_t pixel_argb() const
    {
        return m_a << 24 | m_r << 16 | m_g << 8 | m_b;
    }

    /**
     * Return a 32 bit ARGB pre-multipled alpha pixel value for this color.
     */
    inline uint32_t prepixel_argb() const
    {
        return (m_a << 24) |
               (((m_r * m_a / 255) & 0xff) << 16) |
               (((m_g * m_a / 255) & 0xff) << 8) |
               ((m_b * m_a / 255) & 0xff);
    }

    Color& operator=(uint32_t c)
    {
        m_r = (c >> 24) & 0xFF;
        m_g = (c >> 16) & 0xFF;
        m_b = (c >> 8) & 0xFF;
        m_a = c & 0xFF;

        return *this;
    }

protected:
    uint32_t m_r{0};
    uint32_t m_g{0};
    uint32_t m_b{0};
    uint32_t m_a{0};

    friend bool operator==(const Color& lhs, const Color& rhs);
};

inline bool operator==(const Color& lhs, const Color& rhs)
{
    return lhs.m_r == rhs.m_r &&
           lhs.m_g == rhs.m_g &&
           lhs.m_b == rhs.m_b &&
           lhs.m_a == rhs.m_a;
}

inline bool operator!=(const Color& lhs, const Color& rhs)
{
    return !(lhs == rhs);
}

std::ostream& operator<<(std::ostream& os, const Color& color);

}
}

#endif
