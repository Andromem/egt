/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifndef EGT_TOOLS_H
#define EGT_TOOLS_H

/**
 * @file tools.h
 * @brief Collection of various tools.
 */

namespace egt
{

    /**
     * Monitor CPU usage of the system.
     */
    class CPUMonitorUsage
    {
    public:

        CPUMonitorUsage();

        double usage(unsigned int index = 0) const { return m_cpu_usage[index]; }

        /**
         * Update the CPU usage.
         * The value is stored in the public cpu_usage array
         */
        void update();

        virtual ~CPUMonitorUsage();

    private:

        double m_total_cpu_last_time[4] = {0};
        double m_work_cpu_last_time[4] = {0};
        double m_cpu_usage[4] = {0};
    };

}

#endif
