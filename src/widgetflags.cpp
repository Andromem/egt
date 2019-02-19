/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/widgetflags.h"
#include <map>
#include <iostream>
#include <string>

using namespace std;

namespace egt
{
inline namespace v1
{

std::ostream& operator<<(std::ostream& os, const widgetflags& flags)
{
    static std::map<widgetflag, std::string> strings;
    if (strings.empty())
    {
#define MAPITEM(p) strings[p] = #p
        MAPITEM(widgetflag::PLANE_WINDOW);
        MAPITEM(widgetflag::WINDOW);
        MAPITEM(widgetflag::FRAME);
        MAPITEM(widgetflag::DISABLED);
        MAPITEM(widgetflag::READONLY);
        MAPITEM(widgetflag::ACTIVE);
        MAPITEM(widgetflag::INVISIBLE);
        MAPITEM(widgetflag::GRAB_MOUSE);
#undef MAPITEM
    }

    bool first = true;
    for (auto& flag : flags)
    {
        if (first)
            first = false;
        else
            os << " | ";
        os << strings[flag];
    }
    return os;
}

}
}
