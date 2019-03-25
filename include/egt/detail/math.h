/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_DETAIL_MATH_H
#define EGT_DETAIL_MATH_H

#include <cassert>
#include <cmath>

namespace egt
{
inline namespace v1
{
namespace detail
{

/**
 * Normalize a value, given its min and max, to a different target min and
 * max.
 *
 * @param value The value between min and max.
 * @param min The minimum value.
 * @param max The maximum value.
 * @param target_min The minimum of the range of your desired target scaling.
 * @param target_max The maximum of the range of your desired target scaling.
 *
 * @f$
 * m\mapsto \frac{m-r_{\text{min}}}{r_{\text{max}}-r_{\text{min}}}\times (t_{\text{max}}-t_{\text{min}}) + t_{\text{min}}
 * @f$
 */
template<class T>
T normalize(T value, T min, T max, T target_min, T target_max)
{
    auto r = ((value - min) / (max - min)) *
             (target_max - target_min) + target_min;
    return r;
}

template<class T>
T mmod(T a, T n)
{
    return a - std::floor(a / n) * n;
}

template<class T>
T angle_diff(T angle_start, T angle_stop, bool clockwise = true)
{
    if (clockwise)
    {
        angle_stop -= angle_start;
        if (angle_stop < 0.0)
            angle_stop += 360.;
        return angle_stop;
    }

    angle_stop -= angle_start;
    return mmod(360. - angle_stop, 360.);
}

template<class T>
T normalize_to_angle(T value, T min, T max, T angle_start, T angle_stop, bool clockwise = true)
{
    auto v = normalize<T>(value, min, max, 0, angle_diff(angle_start, angle_stop, clockwise));
    return clockwise ? angle_start + v : angle_start - v;
}

/**
 * Convert from radians to degrees.
 */
template <class T>
inline T to_degrees(T radians)
{
    return radians * (180.0 / M_PI);
}

/**
 * Convert from degrees to radians.
 */
template <class T>
inline T to_radians(T zero, T degrees)
{
    degrees += zero;
    return degrees * (M_PI / 180.0);
}

}
}
}

#endif
