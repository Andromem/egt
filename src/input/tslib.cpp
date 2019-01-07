/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "egt/input.h"
#include "egt/app.h"
#include <chrono>

#ifdef HAVE_TSLIB
#include "tslib.h"

using namespace std;

namespace egt
{
    inline namespace v1
    {
        static const int SLOTS = 1;
        static const int SAMPLES = 20;

        namespace detail
        {
            struct tslibimpl
            {
                struct tsdev* ts;
                struct ts_sample_mt** samp_mt;
                std::chrono::time_point<std::chrono::steady_clock> last_down;
            };
        }

        InputTslib::InputTslib(const string& path)
            : m_input(main_app().event().io()),
              m_impl(new detail::tslibimpl)
        {
            const int NONBLOCKING = 1;
            m_impl->ts = ts_setup(path.c_str(), NONBLOCKING);

            if (m_impl->ts)
            {
                INFO("added tslib " << path);

                m_impl->samp_mt = (struct ts_sample_mt**)malloc(SAMPLES * sizeof(struct ts_sample_mt*));
                assert(m_impl->samp_mt);

                for (int i = 0; i < SAMPLES; i++)
                {
                    m_impl->samp_mt[i] = (struct ts_sample_mt*)calloc(SLOTS, sizeof(struct ts_sample_mt));
                    if (!m_impl->samp_mt[i])
                    {
                        free(m_impl->samp_mt);
                        ts_close(m_impl->ts);
                        assert(0);
                    }
                }

                m_input.assign(ts_fd(m_impl->ts));

                asio::async_read(m_input, asio::null_buffers(),
                                 std::bind(&InputTslib::handle_read, this, std::placeholders::_1));
            }
            else
            {
                ERR("ts device not found: " << path);
            }
        }

        static bool delta(const Point& lhs, const Point& rhs, int d)
        {
            return (std::abs(lhs.x - rhs.x) >= d ||
                    std::abs(lhs.y - rhs.y) >= d);
        }

        void InputTslib::handle_read(const asio::error_code& error)
        {
            if (error)
            {
                ERR(error);
                return;
            }

            struct ts_sample_mt** samp_mt = m_impl->samp_mt;

            int ret = ts_read_mt(m_impl->ts, samp_mt, SLOTS, SAMPLES);
            if (ret < 0)
            {
                ERR("ts_read_mt");
                return;
            }

            bool move = false;

            for (int j = 0; j < ret; j++)
            {
                for (int i = 0; i < SLOTS; i++)
                {
#ifdef TSLIB_MT_VALID
                    if (!(samp_mt[j][i].valid & TSLIB_MT_VALID))
                        continue;
#else
                    if (samp_mt[j][i].valid < 1)
                        continue;
#endif

#if 0
                    printf("%ld.%06ld: (slot %d) %d %6d %6d %6d %d %d\n",
                           samp_mt[j][i].tv.tv_sec,
                           samp_mt[j][i].tv.tv_usec,
                           samp_mt[j][i].slot,
                           samp_mt[j][i].tool_type,
                           samp_mt[j][i].x,
                           samp_mt[j][i].y,
                           samp_mt[j][i].pressure,
                           samp_mt[j][i].distance,
                           samp_mt[j][i].pen_down);
#endif
                    if (samp_mt[j][i].x < 0 || samp_mt[j][i].y < 0)
                        continue;

                    if (m_active)
                    {
                        if (samp_mt[j][i].pen_down == 0)
                        {
                            event_mouse() = Point(samp_mt[j][i].x, samp_mt[j][i].y);
                            m_active = false;
                            DBG("mouse up " << event_mouse());
                            dispatch(eventid::MOUSE_UP);
                        }
                        else
                        {
                            Point point(samp_mt[j][i].x, samp_mt[j][i].y);
                            if (delta(event_mouse(), point, 5))
                            {
                                event_mouse() = point;
                                move = true;
                            }
                        }
                    }
                    else
                    {
                        if (samp_mt[j][i].pen_down == 1)
                        {
                            event_mouse() = Point(samp_mt[j][i].x, samp_mt[j][i].y);

                            std::chrono::time_point<std::chrono::steady_clock, std::chrono::milliseconds> tv
                            {
                                std::chrono::milliseconds{samp_mt[j][i].tv.tv_sec * 1000 + samp_mt[j][i].tv.tv_usec / 1000}
                            };

                            auto DOUBLE_CLICK_DELTA = 300;

                            if (m_impl->last_down.time_since_epoch().count() &&
                                chrono::duration<double, milli>(tv - m_impl->last_down).count() < DOUBLE_CLICK_DELTA)
                            {
                                dispatch(eventid::MOUSE_DBLCLICK);
                            }
                            else
                            {
                                DBG("mouse down " << event_mouse());
                                dispatch(eventid::MOUSE_DOWN);
                                m_active = true;
                            }

                            m_impl->last_down = tv;
                        }
                    }
                }
            }

            if (move)
            {
                DBG("mouse move " << event_mouse());
                dispatch(eventid::MOUSE_MOVE);
            }

            asio::async_read(m_input, asio::null_buffers(),
                             std::bind(&InputTslib::handle_read, this, std::placeholders::_1));
        }

        InputTslib::~InputTslib()
        {
            ts_close(m_impl->ts);

            for (int i = 0; i < SAMPLES; i++)
            {
                free(m_impl->samp_mt[i]);
            }

            free(m_impl->samp_mt);
        }

    }
}

#endif
