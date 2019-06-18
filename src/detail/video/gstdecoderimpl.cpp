/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "detail/video/gstdecoderimpl.h"
#include <egt/app.h>
#include <exception>
#include <fstream>
#include <spdlog/fmt/ostr.h>
#include <spdlog/spdlog.h>
#include <sstream>
#include <string>

namespace egt
{
inline namespace v1
{
namespace detail
{

GstDecoderImpl::GstDecoderImpl(VideoWindow& interface, const Size& size)
    : m_interface(interface),
      m_size(size)
{
    GError* err = nullptr;
    if (!gst_init_check(nullptr, nullptr, &err))
    {
        std::ostringstream ss;
        ss << "failed to initialize gstreamer: ";
        if (err && err->message)
        {
            ss << err->message;
            g_error_free(err);
        }
        else
        {
            ss << "unknown error";
        }

        throw std::runtime_error(ss.str());
    }

    m_gmainLoop = g_main_loop_new(nullptr, FALSE);
    m_gmainThread = std::thread(g_main_loop_run, m_gmainLoop);
}

bool GstDecoderImpl::playing() const
{
    GstState state = GST_STATE_VOID_PENDING;

    if (m_pipeline)
    {
        (void)gst_element_get_state(m_pipeline, &state, NULL,
                                    GST_CLOCK_TIME_NONE);
        return state == GST_STATE_PLAYING;
    }
    SPDLOG_DEBUG("VideoWindow: Done");
    return false;
}

bool GstDecoderImpl::play()
{
    if (m_pipeline)
    {
        auto ret = gst_element_set_state(m_pipeline, GST_STATE_PLAYING);
        if (ret == GST_STATE_CHANGE_FAILURE)
        {
            SPDLOG_DEBUG("VideoWindow: Unable to set the pipeline to the play state.");
            destroyPipeline();
            return false;
        }
    }
    SPDLOG_DEBUG("VideoWindow: Done");
    return true;
}

bool GstDecoderImpl::pause()
{
    if (m_pipeline)
    {
        auto ret = gst_element_set_state(m_pipeline, GST_STATE_PAUSED);
        if (ret == GST_STATE_CHANGE_FAILURE)
        {
            SPDLOG_DEBUG("VideoWindow: Unable to set the pipeline to the Pause state.");
            destroyPipeline();
            return false;
        }
    }
    SPDLOG_DEBUG("VideoWindow: Done");
    return true;
}

double GstDecoderImpl::get_volume() const
{
    if (!m_volume)
        return 0;

    gdouble volume = 0;
    g_object_get(m_volume, "volume", &volume, NULL);
    return volume;
}

bool GstDecoderImpl::set_volume(double volume)
{
    if (!m_volume)
        return false;

    /* allowed values 0 to 10 */
    if (volume <= 0)
        volume = 0;
    else if (volume > 10)
        volume = 10;

    g_object_set(m_volume, "volume", volume, NULL);
    return true;
}

bool GstDecoderImpl::seek(const int64_t time)
{
    if (playing())
    {
        /* If seeking is enabled, we have not done it yet */
        if (m_seek_enabled && !m_seekdone)
        {
            m_seekdone = true;
            if (gst_element_seek(m_pipeline, 1.0, GST_FORMAT_TIME, GST_SEEK_FLAG_FLUSH,
                                 GST_SEEK_TYPE_SET, (gint64) time,
                                 GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE))
            {
                m_seekdone = false;
                return true;
            }
            m_seekdone = false;
        }
    };
    return false;
}

std::string GstDecoderImpl::get_error_message() const
{
    return m_err_message;
}

int64_t GstDecoderImpl::get_duration() const
{
    return m_duration;
}

int64_t GstDecoderImpl::get_position() const
{
    return m_position;
}

void GstDecoderImpl::destroyPipeline()
{
    if (m_pipeline)
    {
        GstStateChangeReturn ret = gst_element_set_state(m_pipeline, GST_STATE_NULL);
        if (GST_STATE_CHANGE_FAILURE == ret)
        {
            spdlog::error("VideoWindow: failed to set pipeline to GST_STATE_NULL");
        }
        g_object_unref(m_pipeline);
        m_pipeline = nullptr;
    }

    if (m_bus)
    {
        g_source_remove(m_bus_watchid);
        gst_object_unref(m_bus);
    }
}

GstDecoderImpl::~GstDecoderImpl()
{
    if (m_gmainLoop)
    {
        /*
         * check loop is running to avoid race condition when stop is called too early
         */
        if (g_main_loop_is_running(m_gmainLoop))
        {
            //stop loop and wait
            g_main_loop_quit(m_gmainLoop);
        }
        m_gmainThread.join();
        g_main_loop_unref(m_gmainLoop);
    }
}

gboolean GstDecoderImpl::bus_callback(GstBus* bus, GstMessage* message, gpointer data)
{
    ignoreparam(bus);

    GError* error;
    gchar* debug;

    auto decodeImpl = reinterpret_cast<GstDecoderImpl*>(data);

    switch (GST_MESSAGE_TYPE(message))
    {
    case GST_MESSAGE_ERROR:
    {
        gst_message_parse_error(message, &error, &debug);
        decodeImpl->m_err_message = error->message;
        SPDLOG_DEBUG("VideoWindow: GST_MESSAGE_ERROR from element {} {}", GST_OBJECT_NAME(message->src), error->message);
        SPDLOG_DEBUG("VideoWindow: GST_MESSAGE_ERROR Debugging info: {}", (debug ? debug : "none"));
        g_error_free(error);
        g_free(debug);

        asio::post(Application::instance().event().io(), [decodeImpl]()
        {
            Event event(eventid::event2);
            decodeImpl->m_interface.invoke_handlers(event);
        });
        break;
    }
    case GST_MESSAGE_WARNING:
    {
        gst_message_parse_warning(message, &error, &debug);
        SPDLOG_DEBUG("VideoWindow: GST_MESSAGE_WARNING from element {}", GST_OBJECT_NAME(message->src), error->message);
        SPDLOG_DEBUG("VideoWindow: GST_MESSAGE_WARNING Debugging info: {}", (debug ? debug : "none"));
        g_error_free(error);
        g_free(debug);
        break;
    }
    case GST_MESSAGE_INFO:
    {
        gchar* name = gst_object_get_path_string(GST_MESSAGE_SRC(message));
        gst_message_parse_info(message, &error, &debug);
        SPDLOG_DEBUG("VideoWindow: GST_MESSAGE_INFO: {}", error->message);
        if (debug)
        {
            SPDLOG_DEBUG("VideoWindow: GST_MESSAGE_INFO: {}", debug);
        }
        g_clear_error(&error);
        g_free(debug);
        g_free(name);
        break;
    }
    break;
    case GST_MESSAGE_CLOCK_PROVIDE:
        SPDLOG_DEBUG("VideoWindow: GST_MESSAGE_CLOCK_PROVIDE");
        break;
    case GST_MESSAGE_CLOCK_LOST:
        SPDLOG_DEBUG("VideoWindow: GST_MESSAGE_CLOCK_LOST");
        break;
    case GST_MESSAGE_NEW_CLOCK:
        SPDLOG_DEBUG("VideoWindow: GST_MESSAGE_NEW_CLOCK");
        break;
    case GST_MESSAGE_EOS:
    {
        SPDLOG_DEBUG("VideoWindow: GST_MESSAGE_EOS: LoopMode: {}", (decodeImpl->m_interface.get_loopback() ? "TRUE" : "FALSE"));
        if (decodeImpl->m_interface.get_loopback())
        {
            gst_element_seek(decodeImpl->m_pipeline, 1.0, GST_FORMAT_TIME,
                             GST_SEEK_FLAG_FLUSH,
                             GST_SEEK_TYPE_SET, 0,
                             GST_SEEK_TYPE_NONE, -1);

            gst_element_set_state(decodeImpl->m_pipeline, GST_STATE_PLAYING);
        }
        else
        {
            asio::post(Application::instance().event().io(), [decodeImpl]()
            {
                Event event(eventid::event1);
                decodeImpl->m_interface.invoke_handlers(event);
            });
        }
        break;
    }
    case GST_MESSAGE_PROGRESS:
        SPDLOG_DEBUG("VideoWindow: GST_MESSAGE_PROGRESS");
        break;
    case GST_MESSAGE_DURATION_CHANGED:
        SPDLOG_DEBUG("VideoWindow: GST_MESSAGE_DURATION_CHANGED");
        break;
    case GST_MESSAGE_ELEMENT:
    {
        const GstStructure* info = gst_message_get_structure(message);
        if (!std::string(gst_structure_get_name(info)).compare("progress"))
        {
            gint64 pos, len;
            if (gst_element_query_position(decodeImpl->m_pipeline, GST_FORMAT_TIME, &pos)
                && gst_element_query_duration(decodeImpl->m_pipeline, GST_FORMAT_TIME, &len))
            {
                decodeImpl->m_position = pos;
                decodeImpl->m_duration = len;
            }

            asio::post(Application::instance().event().io(), [decodeImpl]()
            {
                Event event(eventid::property_changed);
                decodeImpl->m_interface.invoke_handlers(event);
            });
        }
        break;
    }
    case GST_MESSAGE_STATE_CHANGED:
    {
        GstState old_state, new_state, pending_state;
        gst_message_parse_state_changed(message, &old_state, &new_state, &pending_state);
        if (GST_MESSAGE_SRC(message) == GST_OBJECT(decodeImpl->m_pipeline))
        {
            SPDLOG_DEBUG("VideoWindow: GST_MESSAGE_STATE_CHANGED: from {} to {}",
                         gst_element_state_get_name(old_state),
                         gst_element_state_get_name(new_state));

            if (decodeImpl->playing())
            {
                GstQuery* query = gst_query_new_seeking(GST_FORMAT_TIME);
                if (gst_element_query(decodeImpl->m_pipeline, query))
                {
                    gst_query_parse_seeking(query, NULL, &decodeImpl->m_seek_enabled, &decodeImpl->m_start, &decodeImpl->m_duration);
                }
                else
                {
                    SPDLOG_DEBUG("VideoWindow: Seeking query failed.");
                }
                gst_query_unref(query);
            }
        }
        break;
    }
    default:
        break;
    }

    /* we want to be notified again next time if there is a message
     * on the bus, so returning true (false means we want to stop watching
     * for messages on the bus and our callback should not be called again)
     */
    return true;
}

} // end of namespace detail

} // end of namespace v1

} // end of namespace egt
