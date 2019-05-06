/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "detail/screen/keysym_to_unicode.h"
#include "detail/screen/keyboard_code_conversion_x.h"
#include "detail/screen/x11wrap.h"
#include "egt/app.h"
#include "egt/detail/screen/x11screen.h"
#include "egt/eventloop.h"
#include "egt/input.h"
#include "egt/keycode.h"
#include "egt/utils.h"
#include <cairo-xlib.h>
#include <cairo.h>

using namespace std;

namespace egt
{
inline namespace v1
{
namespace detail
{

struct X11Data
{
    Display* display;
    Drawable window;
    Atom wmDeleteMessage;
};

X11Screen::X11Screen(const Size& size, bool borderless)
    : m_priv(new detail::X11Data),
      m_input(main_app().event().io())
{
    m_priv->display = XOpenDisplay(nullptr);
    if (!m_priv->display)
        throw std::runtime_error("unable to connect to X11 display");

    m_input.assign(ConnectionNumber(m_priv->display));

    auto screen = DefaultScreen(m_priv->display);
    auto win = RootWindow(m_priv->display, screen);

    m_priv->window = XCreateSimpleWindow(m_priv->display,
                                         win,
                                         0, 0,
                                         size.w, size.h,
                                         0, 0, 0);
    if (!m_priv->window)
        throw std::runtime_error("unable to connect to X11 window");

    if (borderless)
    {
        XSetWindowAttributes winattr;
        winattr.override_redirect = 1;
        XChangeWindowAttributes(m_priv->display, m_priv->window, CWOverrideRedirect, &winattr);
        XSetWindowBorder(m_priv->display, m_priv->window, 0);
    }

    XStoreName(m_priv->display, m_priv->window, "EGT");

    auto sh = unique_ptr<XSizeHints, decltype(&::XFree)>(XAllocSizeHints(), ::XFree);
    sh->flags = PMinSize | PMaxSize;
    sh->min_width = sh->max_width = size.w;
    sh->min_height = sh->max_height = size.h;
    XSetWMNormalHints(m_priv->display, m_priv->window, sh.get());

    XSelectInput(m_priv->display, m_priv->window,
                 ExposureMask | KeyPressMask | ButtonPress |
                 StructureNotifyMask | ButtonReleaseMask |
                 KeyReleaseMask | EnterWindowMask | LeaveWindowMask |
                 PointerMotionMask | Button1MotionMask | VisibilityChangeMask |
                 ColormapChangeMask);

    init(nullptr, 0, size.w, size.h);

    // instead of using init() to create the buffer, create our own using cairo_xlib_surface_create
    DisplayBuffer buffer;
    buffer.surface =
        shared_cairo_surface_t(cairo_xlib_surface_create(m_priv->display, m_priv->window,
                               DefaultVisual(m_priv->display, screen),
                               size.w, size.h),
                               cairo_surface_destroy);
    cairo_xlib_surface_set_size(buffer.surface.get(), size.w, size.h);

    buffer.cr = shared_cairo_t(cairo_create(buffer.surface.get()), cairo_destroy);
    buffer.damage.emplace_back(0, 0, size.w, size.h);
    m_buffers.push_back(buffer);

    XMapWindow(m_priv->display, m_priv->window);
    XFlush(m_priv->display);
    XSync(m_priv->display, false);

    m_priv->wmDeleteMessage = XInternAtom(m_priv->display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(m_priv->display, m_priv->window, &m_priv->wmDeleteMessage, 1);

    // start the async read from the server
    asio::async_read(m_input, asio::null_buffers(),
                     std::bind(&X11Screen::handle_read, this, std::placeholders::_1));
}

void X11Screen::flip(const damage_array& damage)
{
    Screen::flip(damage);
    XFlush(m_priv->display);
}

void X11Screen::handle_read(const asio::error_code& error)
{
    if (error)
    {
        ERR(error);
        return;
    }

    while (XPending(m_priv->display))
    {
        XEvent e;
        XNextEvent(m_priv->display, &e);

        DBG("x11 event: " << e.type);

        switch (e.type)
        {
        case MapNotify:
            break;
        case ConfigureNotify:
            break;
        case Expose:
        {
            damage_array damage;
            damage.emplace_back(e.xexpose.x, e.xexpose.y,
                                e.xexpose.width, e.xexpose.height);
            flip(damage);
            break;
        }
        case ButtonPress:
            m_in.m_pointer.point = DisplayPoint(e.xbutton.x, e.xbutton.y);
            if (e.xbutton.button == Button1)
                m_in.m_pointer.button = pointer_button::left;
            else if (e.xbutton.button == Button2)
                m_in.m_pointer.button = pointer_button::middle;
            else if (e.xbutton.button == Button3)
                m_in.m_pointer.button = pointer_button::right;

            m_in.dispatch(eventid::raw_pointer_down);
            break;
        case ButtonRelease:
            m_in.m_pointer.point = DisplayPoint(e.xbutton.x, e.xbutton.y);
            if (e.xbutton.button == Button1)
                m_in.m_pointer.button = pointer_button::left;
            else if (e.xbutton.button == Button2)
                m_in.m_pointer.button = pointer_button::middle;
            else if (e.xbutton.button == Button3)
                m_in.m_pointer.button = pointer_button::right;
            m_in.dispatch(eventid::raw_pointer_up);
            break;
        case EnterNotify:
        case LeaveNotify:
        case MotionNotify:
            m_in.m_pointer.point = DisplayPoint(e.xbutton.x, e.xbutton.y);
            m_in.dispatch(eventid::raw_pointer_move);
            break;
        case KeyPress:
        case KeyRelease:
        {
            KeySym keysym = NoSymbol;
            XLookupString(&e.xkey, nullptr, 0, &keysym, nullptr);
            m_in.m_keys.key = detail::GetUnicodeCharacterFromXKeySym(keysym);
            m_in.m_keys.code = detail::KeyboardCodeFromXKeyEvent(&e);

            m_in.dispatch(e.type == KeyPress ? eventid::keyboard_down : eventid::keyboard_up);
            break;
        }
        case ClientMessage:
            if (static_cast<int>(e.xclient.data.l[0]) == static_cast<int>(m_priv->wmDeleteMessage))
                main_app().event().quit();
            break;
        default:
            DBG("x11 unhandled event: " << e.type);
            break;
        }
    }

    asio::async_read(m_input, asio::null_buffers(),
                     std::bind(&X11Screen::handle_read, this, std::placeholders::_1));
}

X11Screen::~X11Screen()
{
    XCloseDisplay(m_priv->display);
}

}
}
}
