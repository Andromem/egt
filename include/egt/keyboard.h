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

#include <egt/button.h>
#include <egt/input.h>
#include <egt/notebook.h>
#include <egt/popup.h>
#include <egt/sizer.h>
#include <memory>
#include <string>
#include <vector>

namespace egt
{
inline namespace v1
{

class Keyboard : public Frame
{
public:
    class Panel;

    /**
     * @brief Key widget for the Keyboard widget.
     *
     * This widget allows to describe a key that will be used by the Keyboard
     * widget.
     */
    class Key : public Button
    {
    public:

        /**
         * @param[in] label Label of the key.
         * @param[in] length Length of the key. It multiplies the width of the
         * default key size which is configured when creating a Panel.
         */
        Key(std::string label, double length = 1.0);
        Key(std::string label, int link, double length = 1.0);
        Key(std::string label, std::shared_ptr<Panel> multichoice, double length = 1.0);
        double length() const;
        int link() const;

    protected:
        int m_link{-1};
        double m_length;
        std::shared_ptr<Panel> m_multichoice{nullptr};
        friend class Keyboard;
    };

    class Panel : public VerticalBoxSizer
    {
    public:
        Panel(std::vector<std::vector<std::shared_ptr<Key>>>  p,
              Size key_size = Size(0, 0));

    protected:
        std::vector<std::vector<std::shared_ptr<Key>>> m_keys;
        friend class Keyboard;
    };

    Keyboard(std::vector<std::shared_ptr<Panel>> panels, Size size = Size());

private:
    struct MultichoicePopup : public Popup
    {
        MultichoicePopup()
        {
            m_notebook.set_align(alignmask::expand);
            add(m_notebook);
        }

        Notebook m_notebook;
        friend class Keyboard;
    };

    struct KeyboardInput : public Input
    {
        using Input::Input;
        friend class Keyboard;
    };
    KeyboardInput m_in;
    Notebook m_main_panel;
    MultichoicePopup m_multichoice_popup;
};

}
}

#endif
