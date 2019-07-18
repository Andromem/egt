/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <egt/ui>
#include <gtest/gtest.h>

using namespace egt;

TEST(Color, Basic)
{
    Color c1(0x11223344);
    EXPECT_EQ(c1.red(), 0x11U);
    EXPECT_EQ(c1.green(), 0x22U);
    EXPECT_EQ(c1.blue(), 0x33U);
    EXPECT_EQ(c1.alpha(), 0x44U);

    Color c2(c1,0xff);
    EXPECT_EQ(c2.red(), 0x11U);
    EXPECT_EQ(c2.green(), 0x22U);
    EXPECT_EQ(c2.blue(), 0x33U);
    EXPECT_EQ(c2.alpha(), 0xffU);

    Color c3(11,22,33,44);
    EXPECT_EQ(c3.red(), 11U);
    EXPECT_EQ(c3.green(), 22U);
    EXPECT_EQ(c3.blue(), 33U);
    EXPECT_EQ(c3.alpha(), 44U);

    Color c4 = Color::rgb(0x112233);
    EXPECT_EQ(c4.red(), 0x11U);
    EXPECT_EQ(c4.green(), 0x22U);
    EXPECT_EQ(c4.blue(), 0x33U);
    EXPECT_EQ(c4.alpha(), 0xffU);

    Color c5 = Color::rgbaf(0.0,1.0,0.5,0.75);
    EXPECT_EQ(c5.red(), 0U);
    EXPECT_EQ(c5.green(), 255U);
    EXPECT_FLOAT_EQ(c5.blue(), 127);
    EXPECT_FLOAT_EQ(c5.alpha(), 191);
}

TEST(TextBox, Basic)
{
    Application app;

    std::string str1 = "hello world\nthis is EGT";
    std::string str2 = "howdy";

    TextBox text1(str1);
    ASSERT_EQ(str1, text1.text());
    text1.set_text(str2);
    ASSERT_EQ(str2, text1.text());
    ASSERT_EQ(str2.size(), text1.len());
    text1.append(str1);
    ASSERT_EQ(str2 + str1, text1.text());
    text1.cursor_set_begin();
    text1.insert(str1);
    ASSERT_EQ(str1 + str2 + str1, text1.text());
    text1.cursor_set_end();
    text1.insert(str1);
    ASSERT_EQ(str1 + str2 + str1 + str1, text1.text());
    text1.set_selection_all();
    ASSERT_EQ(str1 + str2 + str1 + str1, text1.selected_text());
    text1.set_selection(str1.size(), 5);
    ASSERT_EQ(str2, text1.selected_text());
    text1.clear_selection();
    ASSERT_EQ("", text1.selected_text());
    text1.set_selection_all();
    text1.delete_selection();
    ASSERT_EQ("", text1.text());
}

TEST(Screen, DamageAlgorithm)
{
    Screen::damage_array damage;
    Screen::damage_algorithm(damage,Rect(0,0,100,100));
    EXPECT_EQ(damage.front(),Rect(0,0,100,100));
    Screen::damage_algorithm(damage,Rect(0,0,200,200));
    EXPECT_EQ(damage.size(),1U);
    EXPECT_EQ(damage.front(),Rect(0,0,200,200));
}

TEST(Canvas, Basic)
{
    Canvas canvas1(Size(100,100));
    EXPECT_EQ(canvas1.size(), Size(100,100));
    EXPECT_EQ(canvas1.format(), pixel_format::argb8888);
    Canvas canvas2(canvas1.surface());
    EXPECT_EQ(canvas2.size(), Size(100,100));
    EXPECT_EQ(canvas2.format(), pixel_format::argb8888);

    Canvas canvas3(Size(100,100), pixel_format::rgb565);
    EXPECT_EQ(canvas3.size(), Size(100,100));
    EXPECT_EQ(canvas3.format(), pixel_format::rgb565);

    Canvas canvas4(canvas3.surface());
    EXPECT_EQ(canvas4.size(), Size(100,100));
    EXPECT_EQ(canvas4.format(), pixel_format::rgb565);
}

TEST(Geometry, Points)
{
    Point p1;

    Point p2(10,0);
    EXPECT_EQ(p1.distance_to(p2), 10);
}

TEST(Geometry, Rects)
{
    Rect r1;
    EXPECT_TRUE(r1.empty());
}

TEST(Geometry, Sizes)
{
    Size s1;
    EXPECT_TRUE(s1.empty());
}

TEST(Geometry, Arcs)
{
    Arc a1;
    EXPECT_TRUE(a1.empty());
}

TEST(Geometry, Circless)
{
    Circle c1;
    EXPECT_TRUE(c1.empty());
}

template <class T>
class Widgets : public testing::Test
{
protected:
    Widgets() {}
    ~Widgets() override {}
    Application app;
};

using WidgetTypes = ::testing::Types<AnalogMeter,
				     Button,
				     CheckBox,
				     CircleWidget,
				     Label,
				     ListBox,
				     RadioBox,
				     TextBox>;
TYPED_TEST_SUITE(Widgets, WidgetTypes);
TYPED_TEST(Widgets, Properties)
{
    auto widget = std::make_shared<TypeParam>();

    widget->resize(Size(100,100));
    EXPECT_EQ(widget->size(), Size(100,100));
    widget->move(Point(100,100));
    EXPECT_EQ(widget->point(), Point(100,100));
    widget->set_width(10);
    widget->set_height(10);
    EXPECT_EQ(widget->size(), Size(10,10));
    widget->set_x(10);
    widget->set_y(10);
    EXPECT_EQ(widget->point(), Point(10,10));
    widget->move_to_center(Point(10,10));
    EXPECT_EQ(widget->box(), Rect(5,5,10,10));
    EXPECT_EQ(widget->center(), Point(10,10));
    widget->set_box(Rect(50,51,15,16));
    EXPECT_EQ(widget->box(), Rect(50,51,15,16));
    EXPECT_EQ(widget->width(), 15);
    EXPECT_EQ(widget->height(), 16);
    EXPECT_EQ(widget->x(), 50);
    EXPECT_EQ(widget->y(), 51);
    EXPECT_EQ(widget->visible(), true);
    widget->hide();
    EXPECT_EQ(widget->visible(), false);
    widget->show();
    EXPECT_EQ(widget->visible(), true);
    widget->visible_toggle();
    EXPECT_EQ(widget->visible(), false);
    widget->visible_toggle();
    EXPECT_EQ(widget->visible(), true);
    EXPECT_EQ(widget->readonly(), false);
    widget->set_readonly(true);
    EXPECT_EQ(widget->readonly(), true);
    widget->set_readonly(false);
    EXPECT_EQ(widget->readonly(), false);
    widget->set_align(alignmask::right | alignmask::center);
    EXPECT_EQ(widget->align(), alignmask::right | alignmask::center);
    widget->set_padding(10);
    widget->set_border(11);
    widget->set_margin(12);
    EXPECT_EQ(widget->moat(), 33U);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
