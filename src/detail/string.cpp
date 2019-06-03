/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/detail/string.h"

namespace egt
{
inline namespace v1
{
namespace detail
{

std::string replace_all(std::string str, const std::string& from, const std::string& to)
{
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos)
    {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
    return str;
}

std::string rtrim(const std::string& source, const std::string& t)
{
    std::string str = source;
    return str.erase(str.find_last_not_of(t) + 1);
}

std::string ltrim(const std::string& source, const std::string& t)
{
    std::string str = source;
    return str.erase(0, source.find_first_not_of(t));
}

std::string trim(const std::string& source, const std::string& t)
{
    if (source.empty())
        return source;

    return ltrim(rtrim(source, t), t);
}

void tokenize(const std::string& str, char delimiter, std::vector<std::string>& tokens)
{
    size_t start = str.find_first_not_of(delimiter);

    while (start != std::string::npos)
    {
        auto end = str.find(delimiter, start);
        tokens.push_back(str.substr(start, end - start));
        start = str.find_first_not_of(delimiter, end);
    }
}

}
}
}
