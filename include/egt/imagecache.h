/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifndef EGT_IMAGECACHE_H
#define EGT_IMAGECACHE_H

#include <cairo.h>
#include <map>
#include <memory>
#include <egt/utils.h>
#include <string>

namespace egt
{
    using shared_cairo_surface_t =
        std::shared_ptr<cairo_surface_t>;

    using shared_cairo_t =
        std::shared_ptr<cairo_t>;

    /**
     * Set the default relative image path.
     *
     * The default image path is the current working directly.  To change this,
     * call this function with a new path.
     */
    void set_image_path(const std::string& path);

    namespace detail
    {

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
        class ImageCache : public detail::noncopyable
        {
        public:

            /**
             * Get an image surface.
             */
            shared_cairo_surface_t get(const std::string& filename,
                                       float hscale = 1.0, float vscale = 1.0,
                                       bool approximate = true);

            /**
             * Clear the image cache.
             */
            void clear();

        protected:

            static float round(float v, float fraction);

            std::string id(const std::string& filename, float hscale, float vscale);

            static shared_cairo_surface_t scale_surface(shared_cairo_surface_t old_surface,
                    int old_width, int old_height,
                    int new_width, int new_height);

            std::map<std::string, shared_cairo_surface_t> m_cache;
        };

        /**
         * Global image cache instance.
         */
        extern ImageCache image_cache;

    }

}

#endif
