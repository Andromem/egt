/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <egt/themes/coconut.h>
#include <egt/themes/lapis.h>
#include <egt/themes/midnight.h>
#include <egt/themes/shamrock.h>
#include <egt/themes/sky.h>
#include <egt/themes/ultraviolet.h>
#include <egt/ui>
#include <iostream>
#include <math.h>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

using namespace std;
using namespace egt;

struct ButtonPage : public NotebookTab
{
    ButtonPage()
    {
        auto grid0 = make_shared<StaticGrid>(Tuple(3, 6), 5);
        add(expand(grid0));

        grid0->add(expand(make_shared<Button>("Button", Size(100, 40))));

        auto btn2 = make_shared<Button>("Disabled");
        btn2->disable();
        grid0->add(expand(btn2));

        grid0->add(expand(make_shared<ImageButton>(Image("@calculator.png"), "Calculator")));

        auto imagebutton1 = make_shared<ImageButton>(Image("@calculator.png"), "Calculator");
        grid0->add(expand(imagebutton1));
        imagebutton1->set_text_align(alignmask::center | alignmask::left);

        auto imagebutton2 = make_shared<ImageButton>(Image("@calculator.png"), "Calculator");
        grid0->add(expand(imagebutton2));
        imagebutton2->set_text_align(alignmask::center | alignmask::right);

        auto imagebutton3 = make_shared<ImageButton>(Image("@calculator.png"), "Calculator");
        grid0->add(expand(imagebutton3));
        imagebutton3->set_text_align(alignmask::center | alignmask::top);

        auto imagebutton4 = make_shared<ImageButton>(Image("@calculator.png"), "Calculator");
        grid0->add(expand(imagebutton4));
        imagebutton4->set_text_align(alignmask::center | alignmask::bottom);
        imagebutton4->set_image_align(alignmask::right);

        auto imagebutton5 = make_shared<ImageButton>(Image("@calculator.png"));
        grid0->add(expand(imagebutton5));

        auto button6 = make_shared<Button>("Multiline\nButton Text");
        grid0->add(expand(button6));
    }
};

struct CheckBoxPage : public NotebookTab
{
    CheckBoxPage()
    {
        auto grid0 = std::make_shared<StaticGrid>(Tuple(3, 10), 5);
        add(expand(grid0));

        auto toggle1 = std::make_shared<ToggleBox>();
        toggle1->set_toggle_text("Off", "On");
        grid0->add(expand(toggle1));

        auto toggle2 = std::make_shared<ToggleBox>();
        toggle2->set_checked(true);
        grid0->add(expand(toggle2));

        auto toggle3 = std::make_shared<ToggleBox>();
        toggle3->disable();
        grid0->add(expand(toggle3));

        auto checkbox1 = std::make_shared<CheckBox>("checkbox 1");
        grid0->add(expand(checkbox1));

        auto checkbox2 = std::make_shared<CheckBox>("checkbox 2");
        grid0->add(expand(checkbox2));

        auto checkbox3 = std::make_shared<CheckBox>("checkbox 3");
        grid0->add(expand(checkbox3));

        auto checkbox4 = std::make_shared<CheckBox>("Disabled");
        checkbox4->disable();
        grid0->add(expand(checkbox4));

        m_checkbox_group = detail::make_unique<ButtonGroup>(true, false);
        m_checkbox_group->add(checkbox1);
        m_checkbox_group->add(checkbox2);
        m_checkbox_group->add(checkbox3);
        m_checkbox_group->add(checkbox4);
        m_checkbox_group->remove(checkbox3.get());

        auto radiobox1 = std::make_shared<RadioBox>("radiobox 1");
        grid0->add(expand(radiobox1));

        auto radiobox2 = std::make_shared<RadioBox>("radiobox 2");
        grid0->add(expand(radiobox2));
        radiobox2->set_checked(true);

        auto radiobox3 = std::make_shared<RadioBox>("Disabled");
        grid0->add(expand(radiobox3));
        radiobox3->disable();

        m_radiobox_group = detail::make_unique<ButtonGroup>(true);
        m_radiobox_group->add(radiobox1);
        m_radiobox_group->add(radiobox2);
        m_radiobox_group->add(radiobox3);
    }

    unique_ptr<ButtonGroup> m_checkbox_group;
    unique_ptr<ButtonGroup> m_radiobox_group;
};

struct LabelPage : public NotebookTab
{
    LabelPage()
    {
        auto grid0 = std::make_shared<StaticGrid>(Tuple(3, 5), 5);
        add(expand(grid0));

        auto label1 = std::make_shared<Label>("left align",
                                              alignmask::left | alignmask::center);
        label1->set_boxtype(Theme::boxtype::blank);
        label1->set_border(1);
        grid0->add(expand(label1));

        auto label2 = std::make_shared<Label>("right align",
                                              alignmask::right | alignmask::center);
        label2->set_boxtype(Theme::boxtype::blank);
        label2->set_border(1);
        grid0->add(expand(label2));

        auto label3 = std::make_shared<Label>("top align",
                                              alignmask::top | alignmask::center);
        label3->set_boxtype(Theme::boxtype::blank);
        label3->set_border(1);
        grid0->add(expand(label3));

        auto label4 = std::make_shared<Label>("bottom align",
                                              alignmask::bottom | alignmask::center);
        label4->set_boxtype(Theme::boxtype::blank);
        label4->set_border(1);
        grid0->add(expand(label4));

        auto label5 = std::make_shared<Label>("Multiline\nLabel Text", alignmask::center);
        label5->set_boxtype(Theme::boxtype::blank);
        label5->set_border(1);
        grid0->add(expand(label5));

        auto imagelabel0 = std::make_shared<ImageLabel>(Image("@lock.png"), "Lock");
        imagelabel0->set_boxtype(Theme::boxtype::blank);
        imagelabel0->set_border(1);
        grid0->add(expand(imagelabel0));
        imagelabel0->set_image_align(alignmask::top);
        imagelabel0->set_text_align(alignmask::center);

        auto imagelabel1 = std::make_shared<ImageLabel>(Image("@camera.png"), "Camera");
        imagelabel1->set_boxtype(Theme::boxtype::blank);
        imagelabel1->set_border(1);
        grid0->add(expand(imagelabel1));
        imagelabel1->set_text_align(alignmask::center);

        auto imagelabel2 = std::make_shared<ImageLabel>(Image("@camera.png"), "Camera");
        imagelabel2->set_boxtype(Theme::boxtype::blank);
        imagelabel2->set_border(1);
        grid0->add(expand(imagelabel2));
        imagelabel2->set_text_align(alignmask::center | alignmask::right);
        imagelabel2->set_image_align(alignmask::right);

        auto imagelabel3 = std::make_shared<ImageLabel>(Image("@camera.png"), "Camera");
        imagelabel3->set_boxtype(Theme::boxtype::blank);
        imagelabel3->set_border(1);
        grid0->add(expand(imagelabel3));
        imagelabel3->set_text_align(alignmask::center | alignmask::top);
        imagelabel3->set_image_align(alignmask::bottom);

        auto imagelabel4 = std::make_shared<ImageLabel>(Image("@camera.png"), "Camera");
        imagelabel4->set_boxtype(Theme::boxtype::blank);
        imagelabel4->set_border(1);
        grid0->add(expand(imagelabel4));
        imagelabel4->set_text_align(alignmask::center | alignmask::bottom);

        auto imagelabel5 = std::make_shared<ImageLabel>(Image("@camera.png"));
        imagelabel5->set_boxtype(Theme::boxtype::blank);
        imagelabel5->set_border(1);
        grid0->add(expand(imagelabel5));
    }
};

struct TextPage : public NotebookTab
{
    TextPage()
    {
        auto grid1 = std::make_shared<StaticGrid>(Tuple(2, 1), 5);
        add(expand(grid1));

        auto grid0 = std::make_shared<StaticGrid>(Tuple(1, 10), 5);
        grid1->add(expand(grid0));

        auto text1 = std::make_shared<TextBox>("text 1");
        grid0->add(expand(text1));

        auto text2 = std::make_shared<TextBox>("text 2 disabled");
        text2->disable();
        grid0->add(expand(text2));

        auto text3 = std::make_shared<TextBox>("right aligned text");
        text3->set_boxtype(Theme::boxtype::fill | Theme::boxtype::border_bottom);
        text3->set_text_align(alignmask::right);
        grid0->add(expand(text3));

        auto text4 = std::make_shared<TextBox>("text 4");
        text4->set_boxtype(Theme::boxtype::fill | Theme::boxtype::border_bottom);
        text4->disable();
        grid0->add(expand(text4));

        auto text5 = std::make_shared<TextBox>("all characters allowed");
        text5->add_validator_function([this](std::string s) { return true; });
        text5->set_input_validation_enabled(true);
        grid0->add(expand(text5));

        auto text6 = std::make_shared<TextBox>("abc123 only");
        text6->add_validator_function([this](std::string s)
        {
            return std::regex_match(s, regex("[abc123]*"));
        });
        text6->set_input_validation_enabled(true);
        grid0->add(expand(text6));

        auto text7 = std::make_shared<TextBox>(
                         u8"The Ensemble Graphics Toolkit (EGT) is a free and open-source C++ GUI widget "
                         " toolkit for Microchip AT91/SAMA5 microprocessors.  It is used to develop"
                         " graphical applications for Linux.  EGT provides modern and complete GUI"
                         " functionality, look-and-feel, and performance.\n\nThis multi-line TextBox fully"
                         " supports UTF-8 encoding.  See: \u2190\u2191\u2192\u2193", Rect());
        text7->set_selection(4, 25);
        text7->text_flags().set({TextBox::flag::multiline, TextBox::flag::word_wrap});
        grid1->add(expand(text7));
    }
};

template<class T>
static unique_ptr<AnimationSequence> demo_up_down_animator(std::shared_ptr<T> widget)
{
    auto animationup = std::make_shared<PropertyAnimator>(0, 100, std::chrono::seconds(5), easing_circular_easein);
    animationup->on_change(std::bind(&T::set_value, std::ref(*widget), std::placeholders::_1));

    auto animationdown = std::make_shared<PropertyAnimator>(100, 0, std::chrono::seconds(5), easing_circular_easeout);
    animationdown->on_change(std::bind(&T::set_value, std::ref(*widget), std::placeholders::_1));

    auto sequence = detail::make_unique<AnimationSequence>(true);
    sequence->add(animationup);
    sequence->add(animationdown);
    sequence->start();
    return sequence;
}

struct ProgressPage : public NotebookTab
{
    ProgressPage()
    {
        auto grid0 = std::make_shared<StaticGrid>(Tuple(2, 8), 5);
        add(expand(grid0));

        auto spinprogress = std::make_shared<SpinProgress>();
        grid0->add(expand(spinprogress));

        auto progressbar = std::make_shared<ProgressBar>();
        grid0->add(expand(progressbar));

        auto progressbar1 = std::make_shared<ProgressBar>();
        progressbar1->set_show_label(false);
        grid0->add(expand(progressbar1));

        m_animators.push_back(demo_up_down_animator(spinprogress));
        m_animators.push_back(demo_up_down_animator(progressbar));
        m_animators.push_back(demo_up_down_animator(progressbar1));
    }

    vector<unique_ptr<AnimationSequence>> m_animators;
};

struct SliderPage : public NotebookTab
{
    SliderPage()
    {
        auto hsizer1 = std::make_shared<BoxSizer>(orientation::flex);
        add(expand(hsizer1));

        auto slider1 = std::make_shared<Slider>(Rect(0, 0, 200, 80));
        slider1->set_value(50);
        hsizer1->add(slider1);

        auto slider2 = std::make_shared<Slider>(Rect(0, 0, 80, 200), 0, 100, 0, orientation::vertical);
        slider2->set_value(75);
        hsizer1->add(slider2);

        auto slider3 = std::make_shared<Slider>(Rect(0, 0, 200, 80));
        slider3->set_value(50);
        slider3->slider_flags().set({Slider::flag::round_handle, Slider::flag::show_label});
        hsizer1->add(slider3);

        auto slider4 = std::make_shared<Slider>(Rect(0, 0, 80, 200), 0, 100, 0, orientation::vertical);
        slider4->set_value(75);
        slider4->slider_flags().set(Slider::flag::round_handle);
        slider4->disable();
        hsizer1->add(slider4);

        auto slider5 = std::make_shared<Slider>(Rect(0, 0, 80, 200), 100, 200, 150, orientation::vertical);
        slider5->set_value(180);
        slider5->slider_flags().set({Slider::flag::square_handle, Slider::flag::show_labels});
        hsizer1->add(slider5);

        auto slider6 = std::make_shared<Slider>(Rect(0, 0, 200, 80), 100, 200, 150);
        slider6->slider_flags().set({Slider::flag::rectangle_handle, Slider::flag::show_labels});
        hsizer1->add(slider6);
    }
};

struct MeterPage : public NotebookTab
{
    MeterPage()
    {
        auto grid0 = std::make_shared<StaticGrid>(Tuple(2, 2), 10);
        add(expand(grid0));

        auto lp1 = std::make_shared<LevelMeter>();
        lp1->set_num_bars(10);
        grid0->add(expand(lp1));

        auto am1 = std::make_shared<AnalogMeter>();
        grid0->add(expand(am1));

        using namespace egt::experimental;

        auto r1 = std::make_shared<Radial>(Rect(), 0, 100, 0);
        r1->radial_flags().set({Radial::flag::primary_value,
                                Radial::flag::text_value,
                                Radial::flag::primary_handle});
        r1->set_readonly(true);
        grid0->add(expand(r1));

        m_animators.push_back(demo_up_down_animator(lp1));
        m_animators.push_back(demo_up_down_animator(am1));
        m_animators.push_back(demo_up_down_animator(r1));
    }

    vector<unique_ptr<AnimationSequence>> m_animators;
};

struct ComboPage : public NotebookTab
{
    ComboPage()
    {
        auto vsizer =
            std::make_shared<BoxSizer>(orientation::vertical);
        add(expand(vsizer));

        {
            ComboBox::item_array combo_items;
            for (auto x = 0; x < 5; x++)
                combo_items.push_back("item " + std::to_string(x));
            auto combo1 = std::make_shared<ComboBox>(combo_items);
            combo1->set_margin(10);
            vsizer->add(combo1);

            auto combo2 = std::make_shared<ComboBox>(combo_items);
            combo2->set_margin(10);
            combo2->disable();
            vsizer->add(combo2);
        }

        {
            ComboBox::item_array combo_items;
            for (auto x = 0; x < 25; x++)
                combo_items.push_back("item " + std::to_string(x));
            auto combo3 = std::make_shared<ComboBox>(combo_items);
            combo3->set_margin(10);
            vsizer->add(combo3);
        }
    }
};

struct ListPage : public NotebookTab
{
    ListPage()
    {
        auto hsizer1 = std::make_shared<BoxSizer>(orientation::horizontal, justification::justify);
        hsizer1->set_padding(20);
        add(expand(hsizer1));

        auto list0 = std::make_shared<ListBox>(Rect(0, 0, 200, 0));
        for (auto x = 0; x < 25; x++)
            list0->add_item(std::make_shared<StringItem>("item " + std::to_string(x)));
        list0->set_align(alignmask::expand_vertical);
        hsizer1->add(list0);

        auto list1 = std::make_shared<ListBox>(Rect(0, 0, 200, 300));
        for (auto x = 0; x < 5; x++)
            list1->add_item(std::make_shared<StringItem>("item " + std::to_string(x), Image("@ok.png")));
        hsizer1->add(list1);

        auto list2 = std::make_shared<ListBox>(Rect(0, 0, 200, 0));
        list2->add_item(std::make_shared<StringItem>("Help", Image("@help.png")));
        list2->add_item(std::make_shared<StringItem>("Info", Image("@info.png")));
        list2->add_item(std::make_shared<StringItem>("Warning", Image("@warning.png")));
        list2->set_height((list2->border() * 2) + (40 * list2->item_count()));
        hsizer1->add(list2);
    }
};

struct ScrollwheelPage : public NotebookTab
{
    ScrollwheelPage()
    {
        auto hsizer1 =
            std::make_shared<BoxSizer>(orientation::horizontal);
        add(expand(hsizer1));

        auto scrollwheel_day =
            std::make_shared<Scrollwheel>(Rect(0, 0, 50, 100), 1, 31, 1);

        std::vector<std::string> months = { "January", "February", "March",
                                            "April", "May", "June", "July",
                                            "August", "September", "October",
                                            "November", "December"
                                          };
        auto scrollwheel_month =
            std::make_shared<Scrollwheel>(Rect(0, 0, 200, 100), months);
        scrollwheel_month->add_item("");

        auto scrollwheel_year =
            std::make_shared<Scrollwheel>(Rect(0, 0, 100, 100),
                                          1900, 2019, 1, true);

        auto label_day =
            std::make_shared<Label>(scrollwheel_day->value(),
                                    Rect(0, 0, 50, 30));

        auto label_month =
            std::make_shared<Label>(scrollwheel_month->value(),
                                    Rect(0, 0, 100, 30));

        auto label_year =
            std::make_shared<Label>(scrollwheel_year->value(),
                                    Rect(0, 0, 75, 30));

        scrollwheel_day->on_event([label_day, scrollwheel_day](Event&)
        {
            label_day->set_text(scrollwheel_day->value());
        }, {eventid::property_changed});

        scrollwheel_month->on_event([label_month, scrollwheel_month](Event&)
        {
            label_month->set_text(scrollwheel_month->value());
        }, {eventid::property_changed});

        scrollwheel_year->on_event([label_year, scrollwheel_year](Event&)
        {
            label_year->set_text(scrollwheel_year->value());
        }, {eventid::property_changed});

        hsizer1->add(scrollwheel_day);
        hsizer1->add(scrollwheel_month);
        hsizer1->add(scrollwheel_year);

        hsizer1->add(label_day);
        hsizer1->add(label_month);
        hsizer1->add(label_year);

        scrollwheel_month->set_selected(4);
    }
};

struct FormPage : public NotebookTab
{
    FormPage()
    {
        auto form = make_shared<experimental::Form>("Information");
        form->set_align(alignmask::expand_vertical);
        form->set_horizontal_ratio(50);
        add(form);

        form->add_group("Name");
        form->add_option("First Name", make_shared<TextBox>());
        form->add_option("Last Name", make_shared<TextBox>());
        form->add_group("Settings");
        form->add_option("Admin", make_shared<CheckBox>());
        auto toggle1 = std::make_shared<ToggleBox>();
        toggle1->set_toggle_text("On", "Off");
        form->add_option("Active", toggle1);
        form->add_option(make_shared<Button>("Save"));
    }
};

struct ShapesPage : public NotebookTab
{
    ShapesPage()
    {
        auto hsizer1 = std::make_shared<BoxSizer>(orientation::flex);
        add(expand(hsizer1));

        auto circle = std::make_shared<CircleWidget>(Circle(Point(), 100));
        circle->set_margin(10);
        hsizer1->add(circle);

        auto hline = std::make_shared<LineWidget>(Size(100, 100));
        hline->set_margin(10);
        hsizer1->add(hline);

        auto vline = std::make_shared<LineWidget>(Size(100, 100));
        vline->set_margin(10);
        vline->set_horizontal(false);
        hsizer1->add(vline);

        auto rect = std::make_shared<RectangleWidget>(Size(100, 100));
        rect->set_margin(10);
        hsizer1->add(rect);
    }
};

int main(int argc, const char** argv)
{
    Application app(argc, argv, "widgets");

    TopWindow win;

    VerticalBoxSizer vsizer(win);
    expand(vsizer);

    auto frame = make_shared<Frame>(Size(0, 60));
    vsizer.add(expand_horizontal(frame));

    auto logo = make_shared<ImageLabel>(Image("@128px/egt_logo_black.png"));
    logo->set_align(alignmask::center);
    frame->add(logo);

    vector<std::pair<std::string, std::function<std::unique_ptr<Theme>()>>> combo_items =
    {
        {"Default Theme", []{ return detail::make_unique<Theme>(); }},
        {"Lapis", []{ return detail::make_unique<LapisTheme>(); }},
        {"Midnight", []{ return detail::make_unique<MidnightTheme>(); }},
        {"Sky", []{ return detail::make_unique<SkyTheme>(); }},
        {"Shamrock", []{ return detail::make_unique<ShamrockTheme>(); }},
        {"Coconut", []{ return detail::make_unique<CoconutTheme>(); }},
        {"Ultra Violet", []{ return detail::make_unique<UltraVioletTheme>(); }}
    };

    auto combo = make_shared<ComboBox>();
    for (auto& i : combo_items)
        combo->add_item(i.first);
    combo->set_align(alignmask::center_vertical | alignmask::right);
    combo->set_margin(5);
    frame->add(combo);

    combo->on_event([&combo_items, combo, &win](Event&)
    {
        auto s = combo->item_at(combo->selected());
        for (auto& i : combo_items)
        {
            if (s == i.first)
            {
                set_global_theme(i.second());
                break;
            }
        }

        win.damage();

    }, {eventid::property_changed});

    BoxSizer hsizer(orientation::horizontal);
    vsizer.add(expand(hsizer));

    auto list = make_shared<ListBox>();
    list->resize(Size(150, 0));

    auto notebook = make_shared<Notebook>();

    vector<std::pair<string, shared_ptr<NotebookTab>>> pages =
    {
        {"Buttons", make_shared<ButtonPage>()},
        {"Text", make_shared<TextPage>()},
        {"CheckBox", make_shared<CheckBoxPage>()},
        {"Label", make_shared<LabelPage>()},
        {"Progress", make_shared<ProgressPage>()},
        {"Sliders", make_shared<SliderPage>()},
        {"Meters", make_shared<MeterPage>()},
        {"ComboBox", make_shared<ComboPage>()},
        {"ListBox", make_shared<ListPage>()},
        {"Scrollwheel", make_shared<ScrollwheelPage>()},
        {"Form", make_shared<FormPage>()},
        {"Shapes", make_shared<ShapesPage>()},
    };

    for (auto& i : pages)
    {
        list->add_item(make_shared<StringItem>(i.first));
        notebook->add(i.second);
    }

    hsizer.add(expand_vertical(list));
    hsizer.add(expand(notebook));

    list->on_event([&notebook, &list](Event&)
    {
        notebook->set_selected(list->selected());
    }, {eventid::property_changed});

    auto default_keyboard = make_shared<VirtualKeyboard>();
    PopupVirtualKeyboard popup_keyboard {default_keyboard};

    win.add(popup_keyboard);

    win.show();

    return app.run();
}
