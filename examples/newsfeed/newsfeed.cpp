/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#include <cmath>
#include <cstdio>
#include <glob.h>
#include <iostream>
#include <map>
#include <memory>
#include <mui/ui>
#include <sstream>
#include <stdexcept>
#include <string.h>
#include <string>
#include <vector>
//#include <curl/curl.h>

#ifdef HAVE_RAPIDXML_RAPIDXML_HPP
#include <rapidxml/rapidxml.hpp>
#include <rapidxml/rapidxml_utils.hpp>
#else
#include <rapidxml.hpp>
#include <rapidxml_utils.hpp>
#endif

using namespace std;
using namespace mui;

// pull feed from http://feeds.reuters.com/reuters/technologyNews

#if 1
class NewsItem : public ListBoxItem
{
public:

    NewsItem(const std::string& title, const std::string& desc, const std::string& date)
        : m_title(title),
          m_desc(desc),
          m_date(date),
          m_grid(Rect(), 1, 2)
    {
        m_grid.add(&m_title, 0, 0);
        m_grid.add(&m_desc, 0, 1);
    }

    virtual void draw(Painter& painter, const Rect& rect, bool selected) override
    {
        ListBoxItem::draw(painter, rect, selected);

        m_grid.set_box(rect);
        m_grid.reposition();
        m_grid.draw(painter, m_grid.box());

        //m_title.draw(painter, m_title.box());
        //m_description.draw(painter, m_desc.box());
    }

    virtual ~NewsItem()
    {}

protected:

    Label m_title;
    Label m_desc;
    Label m_date;
    StaticGrid m_grid;
};
#endif

#if 0
static void download()
{
    CURL* curl;
    CURLcode res;

    curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, "https://example.com");
        /* example.com is redirected, so we tell libcurl to follow redirection */
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

        /* Perform the request, res will get the return code */
        res = curl_easy_perform(curl);
        /* Check for errors */
        if (res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(res));

        /* always cleanup */
        curl_easy_cleanup(curl);
    }
}
#endif

static int load(const string& file, ListBox& list)
{
    rapidxml::file<> xml_file(file.c_str());
    rapidxml::xml_document<> doc;
    doc.parse<0>(xml_file.data());

    auto rss = doc.first_node("rss");
    auto channel = rss->first_node("channel");
    for (auto node = channel->first_node("item"); node; node = node->next_sibling())
    {
        string title;
        string description;
        string link;
        string date;

        auto t = node->first_node("title");
        if (t)
        {
            title = t->first_node()->value();
        }

        auto d = node->first_node("description");
        if (d)
        {
            description = d->first_node()->value();
        }

        auto p = node->first_node("pubDate");
        if (p)
        {
            date = p->first_node()->value();
        }

        auto l = node->first_node("link");
        if (l)
        {
            link = l->first_node()->value();
        }

        //list.add_item(new StringItem(title));
        list.add_item(new NewsItem(title, description, date));
    }

    return 0;
}

#define SHARED_PATH "../share/mui/examples/newsfeed/"

int main()
{
    Application app;

    Window win;

    ListBox list(win, Rect(Point(), Size(win.w(), win.h())));

    load(SHARED_PATH "feed.xml", list);

    win.show();

    return app.run();
}
