/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "detail/window/planewindow.h"
#include "egt/detail/screen/kmsoverlay.h"
#include "egt/detail/screen/kmsscreen.h"
#include "egt/image.h"
#include "egt/painter.h"
#include <algorithm>
#include <spdlog/spdlog.h>

using namespace std;

namespace egt
{
inline namespace v1
{
namespace detail
{

PlaneWindow::PlaneWindow(Window* interface,
                         pixel_format format, windowhint hint)
    : BasicWindow(interface),
      m_format(format),
      m_hint(hint)
{
    assert(KMSScreen::instance());
    m_screen = nullptr;

    // hack to force some size
    if (m_interface->m_box.size().empty())
        m_interface->m_box.set_size(Size(32, 32));

    allocate_screen();
}

void PlaneWindow::resize(const Size& size)
{
    SPDLOG_TRACE(m_interface->name());

    if (!size.empty() && m_interface->m_box.size() != size)
    {
        if (m_screen)
        {
            auto screen = dynamic_cast<KMSOverlay*>(m_screen);
            assert(screen);
            if (screen)
            {
                screen->resize(size);
            }
        }

        m_interface->m_box.set_size(size);
        m_interface->damage();
    }
}

void PlaneWindow::set_scale(float scalex, float scaley)
{
    auto screen = dynamic_cast<KMSOverlay*>(m_screen);
    assert(screen);
    if (screen)
    {
        screen->set_scale(scalex, scaley);
        m_dirty = true;
    }

    /// @todo No way to save off scale.
    //m_interface->m_scale = scale;
    //m_interface->damage();
}

void PlaneWindow::move(const Point& point)
{
    if (point != m_interface->box().point())
    {
        m_interface->m_box.set_point(point);
        m_dirty = true;
    }
}

// damage to a plane window does not propagate up, unlike a normal frame
void PlaneWindow::damage(const Rect& rect)
{
    auto crect = Rect(rect.point() - m_interface->box().point(), rect.size());

    if (crect.empty())
        return;

    // don't damage if not even visible
    if (!m_interface->visible())
        return;

    m_dirty = true;

    m_interface->add_damage(crect);
}

void PlaneWindow::allocate_screen()
{
    if (!m_screen)
    {
        assert(!m_interface->box().size().empty());

        auto plane = KMSScreen::instance()->allocate_overlay(m_interface->box().size(),
                     m_format, m_hint);
        if (!plane)
            throw std::runtime_error("failed to allocate plane");
        m_screen = new KMSOverlay(plane);
    }
}

void PlaneWindow::top_draw()
{
    if (!m_interface->box().size().empty())
    {
        if (m_interface->visible())
        {
            if (m_dirty)
            {
                allocate_screen();

                KMSOverlay* s = dynamic_cast<KMSOverlay*>(m_screen);
                assert(s);
                if (s)
                {
                    s->set_position(m_interface->local_to_display(Point()));
                    s->apply();
                    m_dirty = false;
                }
            }

            m_interface->do_draw();
        }
    }
}

static shared_cairo_surface_t copy_surface(cairo_surface_t* surface)
{
    assert(cairo_surface_status(surface) == CAIRO_STATUS_SUCCESS);

    // cairo_surface_create_for_rectangle() would work here with one
    // exception - the resulting image has no width and height

    shared_cairo_surface_t copy =
        shared_cairo_surface_t(cairo_image_surface_create(CAIRO_FORMAT_ARGB32,
                               cairo_image_surface_get_width(surface),
                               cairo_image_surface_get_height(surface)),
                               cairo_surface_destroy);

    shared_cairo_t cr = shared_cairo_t(cairo_create(copy.get()), cairo_destroy);
    cairo_set_source_surface(cr.get(), surface, 0, 0);
    cairo_set_operator(cr.get(), CAIRO_OPERATOR_SOURCE);
    cairo_paint(cr.get());

    return copy;
}

void PlaneWindow::paint(Painter& painter)
{
    // There is something odd going on. The surface doesn't work unless it's
    // copied to a new surface first.

    auto copy = copy_surface(cairo_get_target(screen()->context().get()));
    auto image = Image(copy);
    auto p = m_interface->local_to_display(Point());
    painter.draw(Point(p.x(), p.y()));
    painter.draw(image);
}

void PlaneWindow::show()
{
    m_dirty = true;
    BasicWindow::show();
}

void PlaneWindow::hide()
{
    KMSOverlay* s = dynamic_cast<KMSOverlay*>(m_screen);
    if (s)
    {
        s->hide();
        m_dirty = false;
    }

    BasicWindow::hide();
}

void PlaneWindow::deallocate_screen()
{
    if (m_screen)
    {
        KMSOverlay* screen = dynamic_cast<KMSOverlay*>(m_screen);
        assert(screen);
        if (screen)
        {
            KMSScreen::instance()->deallocate_overlay(screen->s());
        }
        m_interface->m_box.set_size(Size());
        m_interface->m_box.set_point(Point());
    }
}

PlaneWindow::~PlaneWindow()
{
    deallocate_screen();
}

}
}
}
