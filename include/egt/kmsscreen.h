/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_KMSSCREEN_H
#define EGT_KMSSCREEN_H

/**
 * @file
 * @brief Working with KMS screens.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <egt/geometry.h>
#include <egt/kmsoverlay.h>
#include <egt/screen.h>
#include <egt/window.h>

#ifdef HAVE_LIBPLANES
#include <planes/kms.h>

struct plane_data;
struct kms_device;

namespace egt
{

    /**
     * Screen in an KMS dumb buffer.
     *
     * This uses libplanes to modeset and configure planes.
     */
    class KMSScreen : public IScreen
    {
    public:
        explicit KMSScreen(bool primary = true);

        /**
         *
         */
        uint32_t count_planes(int type = DRM_PLANE_TYPE_OVERLAY);

        static KMSScreen* instance();

        void schedule_flip() override;

        uint32_t index() override;

        void close();

        virtual ~KMSScreen();

        struct plane_data* allocate_overlay(const Size& size,
                                            pixel_format format = pixel_format::argb8888,
                                            bool heo = false);

    protected:

        int m_fd;
        struct kms_device* m_device;
        struct plane_data* m_plane;
        uint32_t m_index;

        friend class KMSOverlay;
    };

}
#endif

#endif
