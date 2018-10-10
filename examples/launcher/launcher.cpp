/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#include <array>
#include <cmath>
#include <cstdio>
#include <iostream>
#include <map>
#include <memory>
#include <mui/ui>
#include <rapidxml.hpp>
#include <rapidxml_utils.hpp>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;
using namespace mui;

static float sliding_scale(int win_w, int item_w, int item_pos,
                           float min = 0.5, float max = 2.0)
{
    float range = win_w / 2;
    float delta = std::fabs(range - (item_pos + (item_w / 2)));
    float scale = 1.0 - (delta / range);
    if (scale < min || scale > max)
        return min;
    return scale;
}

#if 0
static std::string exec(const char* cmd)
{
    std::array<char, 128> buffer;
    std::string result;
    std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
    if (!pipe)
        throw std::runtime_error("popen() failed!");
    while (!feof(pipe.get()))
    {
        if (fgets(buffer.data(), 128, pipe.get()) != nullptr)
            result += buffer.data();
    }
    return result;
}
#endif

#define SPACE 250

/*
 *
 */
class LauncherItem : public ImageText
{
public:
    LauncherItem(int num, const string& name, const string& description,
                 const string& image, const string& exec, int x = 0, int y = 0)
        : ImageText(image, name, Point(x, y)),
          m_num(num),
          m_fd(-1),
          m_animation(0, 600, LauncherItem::animate, 1000, easing_snap, this),
          m_name(name),
          m_description(description),
          m_exec(exec)
    {
        palette().set(Palette::TEXT, Palette::GROUP_NORMAL, Color::WHITE);
        Font newfont(24, Font::WEIGHT_BOLD);
        font(newfont);
    }

    static void animate(float value, void* data)
    {
        LauncherItem* item = reinterpret_cast<LauncherItem*>(data);
        assert(item);
        item->scale(value, value);
    }

    int handle(int event)
    {
        switch (event)
        {
        case EVT_MOUSE_DOWN:
        {
            if (!m_animation.running())
            {
#if 0
                m_animation.set_easing_func(easing_snap);
                m_animation.starting(hscale());
                m_animation.ending(hscale() + 0.2);
                m_animation.duration(500);
                m_animation.start();
                m_fd = EventLoop::start_periodic_timer(1, LauncherItem::timer_callback, this);
#endif
                return 1;
            }

            break;
        }
        }

        return Image::handle(event);
    }

    void scale_box(int pos)
    {
        auto c = center();
        float s = sliding_scale(parent()->w(), w(), pos);
        label_enabled(s > 0.9);
        scale(s, s, true);
        move_to_center(c);
    }

    static void timer_callback(int fd, void* data)
    {
        LauncherItem* item = reinterpret_cast<LauncherItem*>(data);
        assert(item);

        if (!item->m_animation.next())
        {
            item->scale_box(item->x());
            main_app().event().cancel_periodic_timer(item->m_fd);

            // TODO: exec
        }
    }

    inline int num() const { return m_num; }

private:
    int m_num;
    int m_fd;
    Animation m_animation;
    string m_name;
    string m_description;
    string m_exec;
};

#if 0
static bool debounce_mouse(int delta)
{
    static Point pos;
    bool res = false;

    if (delta)
    {
        if (std::abs(pos.x - mouse_position().x) > delta ||
            std::abs(pos.y - mouse_position().y) > delta)
        {
            res = true;
        }
    }

    pos = mouse_position();

    return res;;
}
#endif

/**
 *
 */
class LauncherWindow : public Window
{
public:
    LauncherWindow()
    {
        add(new Image("background.jpg"));

        auto logo = new Image("logo.png");
        add(logo);
        logo->align(ALIGN_LEFT | ALIGN_TOP, 10);

        auto settings = new Image("settings.png");
        add(settings);
        settings->align(ALIGN_RIGHT | ALIGN_TOP, 10);

        add(&m_plane);
        m_plane.resize(size());
        m_plane.show();
    }

    virtual int load(const string& file)
    {
        rapidxml::file<> xml_file(file.c_str());
        rapidxml::xml_document<> doc;
        doc.parse<0>(xml_file.data());

        int num = 0;
        rapidxml::xml_node<>* root_node = doc.first_node("menu");
        for (rapidxml::xml_node<>* node = root_node->first_node("item"); node; node = node->next_sibling())
        {
            string name = node->first_attribute("name")->value();
            string description = node->first_node("description")->value();
            string image = node->first_node("image")->value();
            string exec = node->first_node("exec")->value();

            LauncherItem* box = new LauncherItem(num++, name, description, image, exec);
            m_plane.add(box);

            box->move_to_center(Point(m_boxes.size() * SPACE, h() / 2));

            // pre-seed the image cache
            for (auto s = 0.5; s <= 2.0; s+=0.01)
                box->scale_box(s);

            box->scale_box(m_boxes.size() * SPACE - box->w() / 2);

            m_boxes.push_back(box);
        }

        start_snap();

        return 0;
    }

    int handle(int event)
    {
        int ret = Window::handle(event);
        if (ret)
            return 1;

        switch (event)
        {
        case EVT_MOUSE_DOWN:
            if (!m_moving)
            {
                m_moving = true;
                m_moving_x = mouse_position().x;
                m_offset = m_boxes[0]->center().x;
                //debounce_mouse(2);
            }

            return 1;
            break;
        case EVT_MOUSE_UP:
            m_moving = false;
            start_snap();
            //debounce_mouse(0);
            return 1;
            break;
        case EVT_MOUSE_MOVE:
            //if (debounce_mouse(2))
            //{
            if (m_moving)
            {
                move_boxes(mouse_position().x);
                return 1;
            }
            //}
            break;
        }

        return 0;
    }

    void move_boxes(int x)
    {
        auto diff = x - m_moving_x;

        for (unsigned int t = 0; t < m_boxes.size(); t++)
        {
            auto pos = m_offset + (m_boxes[t]->num() * SPACE) + diff;

            Rect to(m_boxes[t]->box());
            to.x = pos;

            bool visible = Rect::is_intersect(Rect::merge(to, m_boxes[t]->box()), this->box());
            if (visible)
            {
                m_boxes[t]->move_to_center(Point(pos, m_boxes[t]->center().y));
                m_boxes[t]->scale_box(pos - m_boxes[t]->w() / 2);
            }
            else
            {
                m_boxes[t]->move_to_center(Point(pos, m_boxes[t]->center().y));
            }
        }
    }

    void start_snap()
    {
        if (m_animation)
            delete m_animation;

        auto center = box().center();
        auto distance = w();

        for (auto& box : m_boxes)
        {
            if (center.distance_to<int>(box->box().center()) < std::abs(distance))
            {
                distance = center.distance_to<int>(box->box().center());
                if (center.x < box->box().center().x)
                    distance *= -1;
            }
        }

        m_animation = new Animation(0, distance, LauncherWindow::animate, 200,
                                    easing_snap, this);
        m_animation->start();
        m_fd = main_app().event().start_periodic_timer(1, LauncherWindow::timer_callback, this);

        m_moving_x = 0;
        m_offset = m_boxes[0]->center().x;
    }

    static void animate(float value, void* data)
    {
        LauncherWindow* item = reinterpret_cast<LauncherWindow*>(data);
        assert(item);
        item->move_boxes(value);
    }

    static void timer_callback(int fd, void* data)
    {
        LauncherWindow* item = reinterpret_cast<LauncherWindow*>(data);
        assert(item);

        if (!item->m_animation->next())
        {
            main_app().event().cancel_periodic_timer(item->m_fd);

            // TODO: exec
        }
    }

private:
    PlaneWindow m_plane;
    bool m_moving {false};
    int m_moving_x {0};
    int m_offset {0};
    vector<LauncherItem*> m_boxes;
    Animation* m_animation {nullptr};
    int m_fd {-1};
};

#define SHARED_PATH "/root/mui/share/mui/examples/launcher/"

int main()
{
    Application app;

    set_image_path(SHARED_PATH);

    LauncherWindow win;
    win.load(SHARED_PATH "menu.xml");
    win.show();

    return app.run();
}
