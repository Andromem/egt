/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_APP_H
#define EGT_APP_H

#include <clocale>
#include <egt/asio.hpp>
#include <egt/detail/meta.h>
#include <egt/detail/object.h>
#include <egt/eventloop.h>
#include <iosfwd>
#include <memory>
#include <ostream>
#include <string>
#include <vector>

/**
 * @file
 * @brief Working with app.
 */

/**
 * EGT framework namespace.
 */
namespace egt
{
/**
 * EGT API Version 1 inline namespace.
 */
inline namespace v1
{

class Screen;
class Input;

/**
 * Application definition.
 *
 * This is basically just a helper class that does standard setup for
 * inputs, outputs, the event loop, and more. It also acts as sort of a
 * global access to get to these things because of this.
 */
class EGT_API Application : public detail::NonCopyable
{
public:
    /**
     * @param[in] argc Application argument count.
     * @param[in] argv Application argument array.
     * @param[in] primary Is this the primary display application.
     * @param[in] name Application name.  This is used for several things,
     *             including identifying the text domain used by gettext().
     */
    explicit Application(int argc = 0, const char** argv = nullptr,
                         const std::string& name = {}, bool primary = true);

    /**
     * Reference to the main Application instance.
     *
     * @throws std::runtime_error If there is no application instance.
     */
    static Application& instance();

    /**
     * Check if there is an available instance.
     */
    static bool check_instance();

    /**
     * Run the application.
     *
     * This will initialize the application and start running the event loop.
     * This function will block until the event loop is told to exit by calling
     * quit().
     */
    virtual int run();

    /**
     * Calls EventLoop::quit() by default.
     */
    virtual void quit();

    /**
     * Get a reference to the application event loop instance.
     */
    inline EventLoop& event() { return m_event; }

    /**
     * Get a pointer to the Screen instance.
     */
    inline Screen* screen() const { return m_screen.get(); }

    /**
     * Paint the entire Screen to a file.
     */
    void paint_to_file(const std::string& filename = {});

    /**
     * Dump the widget hierarchy and properties to the specified std::ostream.
     *
     * Example:
     * @code{.cpp}
     * app.dump(cout);
     * @endcode
     *
     * @deprecated This will eventually be removed/changed in favor of using
     * the detail::Serializer classes.
     */
    void dump(std::ostream& out) const;

    /**
     * Get a list of input devices configured with the EGT_INPUT_DEVICES
     * environment variable.
     */
    const std::vector<std::pair<std::string, std::string>>& get_input_devices();

    inline int argc() const { return m_argc; }

    inline const char** argv() const { return m_argv; }

    virtual ~Application();

protected:

    void setup_info();
    void setup_logging();
    void setup_locale(const std::string& name);
    void setup_search_paths();
    void setup_backend(bool primary);
    void setup_inputs();
    void setup_events();

    /**
     * The event loop instance.
     */
    EventLoop m_event;

    /**
     * Argument count.
     */
    int m_argc{0};

    /**
     * Argument list.
     */
    const char** m_argv{nullptr};

    /**
     * Signal instance for registered signal handles by the Application.
     */
    asio::signal_set m_signals;

    /**
     * List of configured input devices.
     */
    std::vector<std::pair<std::string, std::string>> m_input_devices;

private:

    /**
     * Signal handler to handle some default signals to the application.
     */
    void signal_handler(const asio::error_code& error, int signum);

    /**
     * The screen instance.
     */
    std::unique_ptr<Screen> m_screen;

    /**
     * Array of inputs.
     */
    std::vector<std::unique_ptr<Input>> m_inputs;

    detail::Object::RegisterHandle m_handle{0};
};

}
}

#endif
