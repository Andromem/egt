/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "egt/utils.h"

namespace egt
{
inline namespace v1
{

std::string egt_version()
{
    return PACKAGE_VERSION; // NOLINT
}

}
}
