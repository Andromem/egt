/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <egt/ui>
#include <chrono>
#include <memory>

using namespace std;
using namespace egt;
using namespace egt::experimental;

template<class T>
static auto demo_up_down_animator(std::shared_ptr<T> widget, int min, int max,
                                  std::chrono::milliseconds duration = std::chrono::seconds(10),
                                  EasingFunc easingin = easing_circular_easein,
                                  EasingFunc easingout = easing_circular_easeout)
{
    auto animationup = std::make_shared<PropertyAnimator>(min, max, duration, easingin);
    animationup->on_change([widget](PropertyAnimator::Value value) { widget->value(value); });

    auto animationdown = std::make_shared<PropertyAnimator>(max, min, duration, easingout);
    animationdown->on_change([widget](PropertyAnimator::Value value) { widget->value(value); });

    auto sequence = std::make_unique<AnimationSequence>(true);
    sequence->add(animationup);
    sequence->add(animationdown);
    sequence->start();
    return sequence;
}

static shared_ptr<NeedleLayer> create_needle(Gauge& gauge, SvgImage& svg,
        const std::string& id, const std::string& point_id,
        int min, int max, int min_angle, int max_angle,
        std::chrono::milliseconds duration,
        vector<std::unique_ptr<AnimationSequence>>& animations,
        EasingFunc easing = easing_circular_easein)

{
    if (!svg.id_exists(id) || !svg.id_exists(point_id))
        return nullptr;

    auto needle_box = svg.id_box(id);
    auto needle = make_shared<NeedleLayer>(svg.render(id, needle_box),
                                           min, max, min_angle, max_angle);
    auto needle_point = svg.id_box(point_id).center();
    needle->needle_point(needle_point);
    needle->needle_center(needle_point - needle_box.point());
    gauge.add_layer(needle);

    animations.push_back(demo_up_down_animator(needle, min, max, duration, easing));

    return needle;
}

static shared_ptr<GaugeLayer> create_layer(Gauge& gauge, SvgImage& svg,
        const std::string& id,
        std::chrono::milliseconds duration)
{
    if (!svg.id_exists(id))
        return nullptr;

    auto box = svg.id_box(id);
    auto layer = make_shared<GaugeLayer>(svg.render(id, box));
    layer->box(Rect(std::floor(box.x()),
                    std::floor(box.y()),
                    std::ceil(box.width()),
                    std::ceil(box.height())));
    layer->hide();
    gauge.add_layer(layer);

    /// @todo leak
    auto timer = new PeriodicTimer(duration);
    timer->on_timeout([layer]() { layer->visible_toggle(); });
    timer->start();

    return layer;
}

int main(int argc, const char** argv)
{
    Application app(argc, argv, "dash");

    TopWindow win;
    win.padding(10);
    win.color(Palette::ColorId::bg, Color::css("#1b1d43"));

    auto logo = std::make_shared<ImageLabel>(Image("@128px/egt_logo_white.png"));
    win.add(top(left(logo)));

    // the gauge
    Gauge gauge;
    center(gauge);

    auto dash_background = detail::make_unique<SvgImage>("dash_background.svg", SizeF(win.content_area().width(), 0));

    // create a background layer
    auto gauge_background = make_shared<GaugeLayer>(dash_background->render("#background"));
    gauge.add_layer(gauge_background);

    // pick out some other layers
    auto right_blink = create_layer(gauge, *dash_background, "#right_blink", std::chrono::milliseconds(1500));
    auto left_blink = create_layer(gauge, *dash_background, "#left_blink", std::chrono::seconds(1));
    auto brights = create_layer(gauge, *dash_background, "#brights", std::chrono::seconds(5));
    auto high_brights = create_layer(gauge, *dash_background, "#highbrights", std::chrono::seconds(4));
    auto hazards = create_layer(gauge, *dash_background, "#hazards", std::chrono::seconds(2));
    auto heat = create_layer(gauge, *dash_background, "#heat", std::chrono::seconds(3));

    vector<std::unique_ptr<AnimationSequence>> animations;

    // pick out the needles
    auto rpm_needle = create_needle(gauge, *dash_background, "#rpmneedle", "#rpmpoint",
                                    0, 6000, -20, 190, std::chrono::seconds(8), animations, easing_bounce);
    auto mph_needle = create_needle(gauge, *dash_background, "#mphneedle", "#mphpoint",
                                    0, 220, -20, 190, std::chrono::seconds(8), animations);
    auto fuel_needle = create_needle(gauge, *dash_background, "#fuelneedle", "#fuelpoint",
                                     0, 100, 0, 90, std::chrono::seconds(3), animations);

    win.add(gauge);

    // add some labels to show updating text at specific places

    auto rpm_box = dash_background->id_box("#rpm");
    auto rpm_text = make_shared<Label>();
    rpm_text->text_align(AlignFlag::center);
    rpm_text->box(Rect(rpm_box.x(), rpm_box.y(), rpm_box.width(), rpm_box.height()));
    rpm_text->color(Palette::ColorId::label_text, Palette::cyan);
    rpm_text->text("Trip 1: 100.5 miles");
    gauge.add(rpm_text);

    auto speed_box = dash_background->id_box("#speed");
    auto speed_text = make_shared<Label>();
    speed_text->text_align(AlignFlag::center);
    speed_text->box(Rect(speed_box.x(), speed_box.y(), speed_box.width(), speed_box.height()));
    speed_text->color(Palette::ColorId::label_text, Palette::white);
    speed_text->font(Font(28, Font::Weight::bold));
    speed_text->text("0 mph");
    gauge.add(speed_text);

    mph_needle->on_value_changed([speed_text, mph_needle]()
    {
        ostringstream ss;
        ss << mph_needle->value() << " mph";
        speed_text->text(ss.str());
    });

    auto middle_box = dash_background->id_box("#middle");
    auto middle_text = make_shared<Label>();
    middle_text->text_align(AlignFlag::center);
    middle_text->box(Rect(middle_box.x(), middle_box.y(), middle_box.width(), middle_box.height()));
    middle_text->color(Palette::ColorId::label_text, Palette::aquamarine);
    middle_text->text("98.7 FM");
    gauge.add(middle_text);

    auto console_box = dash_background->id_box("#console");
    auto console_text = make_shared<Label>();
    console_text->text_align(AlignFlag::center);
    console_text->box(Rect(console_box.x(), console_box.y(), console_box.width(), console_box.height()));
    console_text->color(Palette::ColorId::label_text, Palette::orange);
    console_text->font(Font(55, Font::Weight::bold));
    console_text->text("D");
    gauge.add(console_text);

    dash_background.release();

    win.show();

    return app.run();
}
