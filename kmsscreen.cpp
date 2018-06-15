/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifdef HAVE_LIBPLANES

#include "event_loop.h"
#include "input.h"
#include "kmsscreen.h"
#include "planes/fb.h"
#include "planes/kms.h"
#include "planes/plane.h"
#include "widget.h"
#include "window.h"
#include <cairo.h>
#include <drm_fourcc.h>
#include <xf86drm.h>

using namespace std;

namespace mui
{

    KMSOverlayScreen::KMSOverlayScreen(struct plane_data* plane)
	: m_plane(plane)
    {
	init(plane->buf, plane_width(plane), plane_height(plane));
    }

    void KMSOverlayScreen::position(int x, int y)
    {
	plane_set_pos(m_plane, x, y);
	plane_apply(m_plane);
    }

    /*
      void KMSOverlayScreen::apply()
      {
      plane_apply(m_plane);
      }
    */

    KMSOverlayScreen::~KMSOverlayScreen()
    {
    }

    KMSScreen::KMSScreen()
    {
	m_fd = drmOpen("atmel-hlcdc", NULL);
	assert(m_fd >= 0);

	m_device = kms_device_open(m_fd);
	assert(m_device);

	//kms_device_dump(device);

	struct plane_data* plane = plane_create(m_device,
						DRM_PLANE_TYPE_PRIMARY,
						0,
						m_device->screens[0]->width,
						m_device->screens[0]->height,
						DRM_FORMAT_ARGB8888);

	assert(plane);
	plane_fb_map(plane);
	assert(plane->buf);

	cout << "dumb buffer " << plane_width(plane) << "," <<
	    plane_height(plane) << endl;

	plane_apply(plane);

	init(plane->buf, plane_width(plane), plane_height(plane));
    }

    struct plane_data* KMSScreen::allocate_overlay(const Rect& rect)
    {
	// TODO
	static int index = 0;
	struct plane_data* plane = plane_create(m_device,
						DRM_PLANE_TYPE_OVERLAY,
						index++,
						rect.w,
						rect.h,
						DRM_FORMAT_ARGB8888);

	assert(plane);
	plane_fb_map(plane);
	assert(plane->buf);

	plane_set_pos(plane, rect.x, rect.y);

	cout << "overlay dumb buffer " << plane_width(plane) << "," <<
	    plane_height(plane) << endl;

	plane_apply(plane);

	return plane;
    }

    void KMSScreen::flip(const vector<Rect>& damage)
    {
	IScreen::flip(damage);
    }

    KMSScreen::~KMSScreen()
    {
	kms_device_close(m_device);
	drmClose(m_fd);
    }

}
#endif
