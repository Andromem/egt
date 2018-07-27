/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifndef IMAGECACHE_H
#define IMAGECACHE_H

#include <cairo.h>
#include <map>
#include <memory>
#include <string>

namespace mui
{
    using shared_cairo_surface_t =
	std::shared_ptr<cairo_surface_t>;

    using shared_cairo_t =
	std::shared_ptr<cairo_t>;

    /**
     * Internal image cache.
     *
     * Provides an in-memory cache for images based on filename and scale. This
     * prevents multiple attempts at loading the same file as well as rescaling
     * the image to the same scale multiple times.
     *
     * This is a tradeoff in consuming more memory instead of possibly
     * constantly reloading or scaling the same image.
     */
    class ImageCache
    {
    public:

	shared_cairo_surface_t get(const std::string& filename,
				   float scale = 1.0, bool approximate = true);

	void clear();

    protected:

	static float round(float v, float fraction);

	std::string id(const std::string& filename, float scale);

	static shared_cairo_surface_t
	scale_surface(shared_cairo_surface_t old_surface,
		      int old_width, int old_height,
		      int new_width, int new_height);

	std::map<std::string,shared_cairo_surface_t> m_cache;
    };


    extern ImageCache image_cache;

}


#endif
