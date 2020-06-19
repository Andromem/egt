/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "detail/egtlog.h"
#include "egt/script.h"

#ifdef HAVE_LUA
#include "detail/lua/script.h"
#endif

namespace egt
{
inline namespace v1
{
namespace experimental
{

double lua_evaluate(const std::string& expr)
{
    double y = 0.;
#ifdef HAVE_LUA
    int cookie;
    char* msg = nullptr;

    if (!script_init(nullptr))
    {
        detail::error("can't init lua");
        return y;
    }
    cookie = script_load(expr.c_str(), &msg);
    if (msg)
    {
        detail::error("can't load expr: {}", msg);
        goto error;
    }

    y = script_eval(cookie, &msg);
    if (msg)
    {
        detail::error("can't eval: {}", msg);
        goto error;
    }

error:
    if (msg)
        free(msg);
    script_unref(cookie);
    script_close();
#else
    detail::ignoreparam(expr);
    detail::warn("lua script support not available");
#endif
    return y;
}

}
}
}
