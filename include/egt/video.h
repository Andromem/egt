/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_VIDEO_H
#define EGT_VIDEO_H

#include <egt/window.h>

namespace egt
{
inline namespace v1
{
namespace detail
{
bool is_target_sama5d4();
class GstDecoderImpl;
class GstKmsSinkImpl;
class GstAppSinkImpl;
}

/**
 * A VideoWindow is a widget to decode video and render it to a screen.
 *
 * It has a bounding rectangle, format and windowhint. These properties can
 * be manipulated to create a video window either as a basic window or an
 * overlay plane.
 *
 * The video decoding is done through gstreamer media framework.
 *
 * @ingroup media
 */
class VideoWindow : public Window
{
public:

    /**
     * Create a video window to decode video and render it to a screen.
     *
     * @param rect Size of window with offset x & y.
     * @param format Pixel format of window or a overlay plane.
     * @param hint Used for configuring window backend's.
     *
     * @note Only windowhint::heo_overlay can use yuyv, nv21 and yuv420 pixel
     * formats.
     */
    explicit VideoWindow(const Rect& rect = {},
                         pixel_format format = pixel_format::xrgb8888,
                         windowhint hint = windowhint::overlay);

    /**
    * Create a video window to decode video and render it to a screen.
    *
    * @param rect Size of window with offset x & y.
    * @param uri Media file
    * @param format Pixel format of window or a overlay plane.
    * @param hint Used for configuring window backend's.
    *
    * @note Only windowhint::heo_overlay can use yuyv, nv21 and yuv420 pixel
    * formats.
    */
    VideoWindow(const Rect& rect,
                const std::string& uri,
                pixel_format format = pixel_format::xrgb8888,
                windowhint hint = windowhint::overlay);

    virtual void do_draw() override
    {
        // video windows don't draw
    }

    virtual void draw(Painter& painter, const Rect& rect) override;

    /**
     * Initialize gstreamer pipeline for specified media file.
     *
     * @param uri Media file
     * @return true if success
     */
    virtual bool set_media(const std::string& uri);

    /**
     * Play the video.
     *
     * @return true if success
     */
    virtual bool play();

    /**
     * Pause the video.
     *
     * @return true if success
     */
    virtual bool pause();

    /**
     * Check is video in play state.
     *
     * @return true if success
     */
    virtual bool playing() const;

    /**
     * Get the current position of the video being played.
     *
     * @return 64bit time value nanosec's
     */
    int64_t position() const;

    /**
     * Get the total duration of the video.
     *
     * @return 64bit time value in nanosec's
     */
    int64_t duration() const;

    /**
     * Adjust volume of the video being played.
     *
     * @param volume is in range of 0 (no sound) to 10 (normal sound)
     * @return true if success
     */
    virtual bool set_volume(double volume);

    /**
     * Get the volume value for the video being played.
     */
    virtual double get_volume() const;

    /**
     * Seek the video being played.
     *
     * @param time in nanoseconds
     * @return true if seek success
     */
    virtual bool seek(int64_t time);

    /**
     * Enable/disable continues loop-back mode of the video
     * being played. by default this is disabled.
     *
     * @param enable to enable/disable loop-back mode.
     */
    inline void set_loopback(bool enable)
    {
        m_loopback = enable;
    }

    /**
     * Get loop-back state
     *
     * @return true/false based on loop-back state
     */
    inline bool get_loopback() const
    {
        return m_loopback;
    }

    /**
     * Scale the video by scale value.
     *
     * @param[in] scale
     */
    virtual void set_scale(float scale) override;

    /**
     * Get video scale value.
     */
    virtual float scale() const
    {
        return m_scale;
    }

    /**
     * Get error message
     */
    std::string get_error_message() const;

    virtual ~VideoWindow();

protected:
    bool m_loopback;
    float m_scale = {1.0};

    void createImpl(const Size& size);

    std::shared_ptr<detail::GstDecoderImpl> m_decoderImpl;

    friend class detail::GstDecoderImpl;
    friend class detail::GstKmsSinkImpl;
    friend class detail::GstAppSinkImpl;

};

} //namespace v1

} //namespace egt

#endif
