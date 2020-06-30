/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_CAPTURE_H
#define EGT_CAPTURE_H

/**
 * @file
 * @brief Camera capture support.
 */

#include <egt/object.h>
#include <egt/signal.h>
#include <egt/types.h>
#include <memory>
#include <string>

namespace egt
{
inline namespace v1
{

namespace detail
{
class CaptureImpl;
}

namespace experimental
{

/**
 * Capture a camera video feed directly to an output file.
 *
 * @code{.cpp}
 * experimental::CameraCapture capture("output.avi");
 * capture.start();
 * ...
 * capture.stop();
 * @endcode
 */
class EGT_API CameraCapture : public Object
{
public:

    /**
     * Event signal.
     * @{
     */
    /**
     * Invoked when an error occurs.
     */
    Signal<const std::string&> on_error;

    /**
     * Generated when an USB camera connected.
     */
    Signal<const std::string&> on_connect;

    /**
     * Generated when an USB camera disconnected.
     */
    Signal<const std::string&> on_disconnect;
    /** @} */

    /**
     * Output container type.
     */
    enum class ContainerType
    {
        avi,
        mpeg2ts
    };

    CameraCapture();

    /**
     * @param[in] output The output file path.
     * @param[in] container The output container format.
     * @param[in] format The input pixel format.
     * @param[in] device The camera device.
     */
    explicit CameraCapture(const std::string& output,
                           ContainerType container = ContainerType::avi,
                           PixelFormat format = PixelFormat::yuyv,
                           const std::string& device = "/dev/video0");

    CameraCapture(const CameraCapture&) = delete;
    CameraCapture& operator=(const CameraCapture&) = delete;
    CameraCapture(CameraCapture&&) = default;
    CameraCapture& operator=(CameraCapture&&) = default;

    /**
     * Set the output.
     *
     * @warning If this is called while the camera is currently capturing, it
     * will automatically stop the capture.
     *
     * @param[in] output The output file path.
     * @param[in] container The output container format.
     * @param[in] format The input pixel format.
     */
    void set_output(const std::string& output,
                    ContainerType container = ContainerType::avi,
                    PixelFormat format = PixelFormat::yuyv);

    /**
     * Initialize camera pipeline to capture from the camera.
     *
     * @return true on success
     */
    bool start();

    /**
     * Stop camera capture and finish the output.
     */
    void stop();

    ~CameraCapture() override;

protected:

    /// Internal capture implementation.
    std::unique_ptr<detail::CaptureImpl> m_impl;
};

}
}
}

#endif
