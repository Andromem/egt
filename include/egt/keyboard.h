/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_KEYBOARD_H
#define EGT_KEYBOARD_H

/**
 * @file
 * @brief On-screen Keyboard.
 */

#include <egt/grid.h>
#include <vector>

namespace egt
{

    /**
     * On-screen keyboard.
     */
    template <class T>
    class Keyboard : public T
    {
    public:
        Keyboard()
            : T(Size(800, 200)),
              m_grid(Rect(), 10, 4, 5)
        {
            this->add(&m_grid);
            this->m_grid.align(alignmask::EXPAND);

            /*
            template <class T>
                struct Key : public T
            {

                unsigned int code;
            };
            */

            std::vector<std::vector<std::string>> buttons =
            {
                {"q", "w", "e", "r", "t", "y", "u", "i", "o", "p" },
                {"", "a", "s", "d", "f", "g", "h", "j", "k", "l" },
                {"icons/arrow_up.png", "z", "x", "c", "v", "b", "n", "m", "", "icons/arrow_left.png"},
                {"123", ",", "", "", "     ", "", "", "", "", "."}
            };

            for (size_t r = 0; r < buttons.size(); r++)
            {
                for (size_t c = 0; c < buttons[r].size(); c++)
                {
                    std::string label = buttons[r][c];
                    if (label.empty())
                        continue;

                    Button* b;

                    if (label.find(".png") != std::string::npos)
                        b = new ImageButton(label);
                    else
                        b = new Button(label);

                    b->on_event([b](eventid event)
                    {
                        if (!b->get_text().empty())
                        {
                            if (event == eventid::MOUSE_DOWN)
                            {
                                event_key() = b->get_text()[0];
                                detail::IInput::dispatch(eventid::KEYBOARD_DOWN);
                            }
                            else if (event == eventid::MOUSE_UP)
                            {
                                event_key() = b->get_text()[0];
                                detail::IInput::dispatch(eventid::KEYBOARD_UP);
                            }
                        }
                    });

                    this->m_grid.add(b, c, r);
                }
            }

            this->m_grid.reposition();
        }

    protected:
        StaticGrid m_grid;
    };

}

#endif
