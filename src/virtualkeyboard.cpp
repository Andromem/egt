#include "detail/utf8text.h"
#include "egt/button.h"
#include "egt/input.h"
#include "egt/keycode.h"
#include "egt/popup.h"
#include "egt/sizer.h"
#include "egt/virtualkeyboard.h"

using namespace std;

namespace egt
{
inline namespace v1
{
using Panel = VirtualKeyboard::Panel;

VirtualKeyboard::Key::Key(uint32_t unicode, double length)
    : m_button(make_shared<Button>()),
      m_unicode(unicode),
      m_length(length)
{
    string tmp;
    utf8::append(unicode, std::back_inserter(tmp));
    m_button->set_text(tmp);
    m_button->ncflags().set(Widget::flag::no_autoresize);
}

VirtualKeyboard::Key::Key(const std::string& label, KeyboardCode keycode, double length)
    : m_button(make_shared<Button>(label)),
      m_keycode(keycode),
      m_length(length)
{
    m_button->ncflags().set(Widget::flag::no_autoresize);
}

VirtualKeyboard::Key::Key(const string& label, int link, double length)
    : m_button(make_shared<Button>(label)),
      m_link(link),
      m_length(length)
{
    m_button->ncflags().set(Widget::flag::no_autoresize);
}

VirtualKeyboard::Key::Key(uint32_t unicode,
                          shared_ptr<Panel> multichoice,
                          double length, KeyboardCode keycode)
    : m_button(make_shared<Button>()),
      m_unicode(unicode),
      m_length(length),
      m_multichoice(multichoice)
{
    string tmp;
    utf8::append(unicode, std::back_inserter(tmp));
    m_button->set_text(tmp);
    m_button->ncflags().set(Widget::flag::no_autoresize);
}

Panel::Panel(vector<vector<shared_ptr<VirtualKeyboard::Key>>> keys,
             Size key_size,
             int spacing)
    : m_vsizer(make_shared<VerticalBoxSizer>()),
      m_keys(std::move(keys))
{
    set_align(alignmask::center);

    m_vsizer->set_align(alignmask::center);
    add(m_vsizer);

    for (auto& row : m_keys)
    {
        auto hsizer = make_shared<HorizontalBoxSizer>();
        hsizer->set_align(alignmask::center | alignmask::top);
        m_vsizer->add(hsizer);

        for (auto& key : row)
        {
            key->m_button->resize(Size(key_size.width * key->length() + 2 * spacing,
                                       key_size.height + 2 * spacing));
            key->m_button->set_margin(spacing / 2);

            hsizer->add(key->m_button);
        }
    }
}

void VirtualKeyboard::set_key_link(const shared_ptr<Key>& k)
{
    k->m_button->on_event([this, k](Event&)
    {
        m_main_panel.set_select(k->link());
    }, {eventid::pointer_click});
}

void VirtualKeyboard::set_key_input_value(const shared_ptr<Key>& k)
{
    k->m_button->on_event([this, k](Event & event)
    {
        if (!k->m_button->text().empty())
        {
            Event event2(eventid::keyboard_down);
            event2.key().unicode = k->m_unicode;
            event2.key().keycode = k->m_keycode;
            m_in.dispatch(event2);

            event2.set_id(eventid::keyboard_up);
            event2.key().unicode = k->m_unicode;
            event2.key().keycode = k->m_keycode;
            m_in.dispatch(event2);
        }

        return 0;
    }, {eventid::pointer_click});
}

void VirtualKeyboard::set_key_multichoice(const shared_ptr<Key>& k, unsigned id)
{
    for (auto& multichoice_raw : k->m_multichoice->m_keys)
    {
        for (auto& multichoice_key : multichoice_raw)
        {
            multichoice_key->m_button->on_event([this, k, multichoice_key](Event & event)
            {
                // hide popup first as it is modal
                m_multichoice_popup.hide();

                if (!multichoice_key->m_button->text().empty())
                {
                    Event down(eventid::keyboard_down);
                    down.key().unicode = multichoice_key->m_unicode;
                    down.key().keycode = multichoice_key->m_keycode;
                    m_in.dispatch(down);
                    Event up(eventid::keyboard_up);
                    up.key().unicode = multichoice_key->m_unicode;
                    up.key().keycode = multichoice_key->m_keycode;
                    m_in.dispatch(up);
                    // the modal popup caught the raw_pointer_up event
                    k->m_button->set_active(false);
                }
                // User may just move his finger so prefer the raw_pointer_up event to the pointer_click one.
            }, {eventid::raw_pointer_up});
        }
    }

    m_multichoice_popup.m_notebook.add(k->m_multichoice);

    k->m_button->on_event([this, k, id](Event&)
    {
        m_multichoice_popup.m_notebook.set_select(id);
        m_multichoice_popup.resize(k->m_multichoice->m_vsizer->size());

        auto display_origin = k->m_button->display_origin();
        auto main_window_origin = main_window()->display_to_local(display_origin);
        // Popup on top of the key.
        main_window_origin.y -= m_multichoice_popup.size().height;
        // Popup aligned with key center.
        main_window_origin.x -= m_multichoice_popup.size().width / 2;
        main_window_origin.x += k->m_button->size().width / 2;

        m_multichoice_popup.move(main_window_origin);
        m_multichoice_popup.show_modal();
    }, {eventid::pointer_hold});
}

VirtualKeyboard::VirtualKeyboard(vector<shared_ptr<Panel>> panels, Size size)
    : Frame(Rect(Point(), size))
{
    m_main_panel.set_align(alignmask::expand);
    add(m_main_panel);
    m_multichoice_popup.resize(Size(100, 100));
    main_window()->add(m_multichoice_popup);

    unsigned multichoice_id = 0;

    for (auto& panel : panels)
    {
        for (auto& row : panel->m_keys)
        {
            for (auto& key : row)
            {
                if (key->link() >= 0)
                {
                    set_key_link(key);
                }
                else
                {
                    set_key_input_value(key);

                    if (key->m_multichoice)
                        set_key_multichoice(key, multichoice_id++);
                }
            }
        }

        m_main_panel.add(panel);
    }
}

}
}
