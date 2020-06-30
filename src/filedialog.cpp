/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "detail/egtlog.h"
#include "egt/embed.h"
#include "egt/filedialog.h"
#include <experimental/filesystem>

#ifdef SRCDIR
EGT_EMBED(internal_folder, SRCDIR "/icons/32px/folder.png")
#endif

namespace fs = std::experimental::filesystem;

namespace egt
{
inline namespace v1
{

FileDialog::FileDialog(const std::string& filepath, const Rect& rect) noexcept
    : Popup(rect.size(), rect.point()),
      m_vsizer(Orientation::vertical),
      m_title(Image("res:internal_folder"), filepath),
      m_filepath(filepath)
{
    name("FileDialog" + std::to_string(m_widgetid));

    border(theme().default_border());
    padding(5);

    add(expand(m_vsizer));

    m_title.text_align(AlignFlag::left | AlignFlag::center);
    m_vsizer.add(expand_horizontal(m_title));

    m_vsizer.add(expand(m_flist));

    m_flist.on_selected([this](size_t index)
    {
        list_item_selected(index);
    });

    if (m_filepath.empty())
        m_filepath = fs::current_path().string();
}

FileDialog::FileDialog(const Rect& rect) noexcept
    : FileDialog( {}, rect)
{

}

bool FileDialog::list_files(const std::string& filepath)
{
    if (fs::is_directory(filepath))
    {
        m_filepath = filepath;
    }
    else
    {
        /* if filepath is a regular file then list
         * files of parent directory.
         */
        fs::path p = filepath;
        m_filepath = p.parent_path().string();
    }

    m_title.text(m_filepath);

    EGTLOG_DEBUG("FileDialog : file path is {}", m_filepath);

    m_flist.clear();

    if (filepath != "/")
    {
        m_flist.add_item(std::make_shared<StringItem>("./", Rect(), AlignFlag::left | AlignFlag::center));

        m_flist.add_item(std::make_shared<StringItem>("../", Rect(), AlignFlag::left | AlignFlag::center));
    }

    try
    {
        for (auto& dir : fs::directory_iterator(m_filepath))
        {
            m_flist.add_item(std::make_shared<StringItem>(dir.path().filename().string(), Rect(), AlignFlag::left | AlignFlag::center));
        }
    }
    catch (const fs::filesystem_error& ex)
    {
        EGTLOG_DEBUG("FileDialog : Error: {}", ex.what());
        return false;
    }

    damage();

    return true;
}

void FileDialog::list_item_selected(int index)
{
    auto item = dynamic_cast<StringItem*>(m_flist.item_at(index).get());
    if (!item)
        return;

    auto fselect = item->text();

    EGTLOG_DEBUG("FileDialog : File Selected is : {}", fselect);

    if (fselect == "./")
    {
        return;
    }
    else if (fselect == "../")
    {
        fs::path p = m_filepath;
        m_filepath = p.parent_path().string();
        EGTLOG_DEBUG("FileDialog : parent dir {}", m_filepath);
        selected("");
        list_files(m_filepath);
    }
    else if (fs::is_directory(m_filepath + "/" + fselect))
    {
        EGTLOG_DEBUG("FileDialog : {} is a directory", fselect);
        selected("");
        if (m_filepath == "/")
            m_filepath += fselect;
        else
            m_filepath =  m_filepath + "/" + fselect;
        list_files(m_filepath);
    }
    else if (fs::is_regular_file(m_filepath + "/" + fselect))
    {
        EGTLOG_DEBUG("FileDialog : {} is a regular file", fselect);
        selected(fselect);
    }
}

FileOpenDialog::FileOpenDialog(const std::string& filepath, const Rect& rect) noexcept
    : FileDialog(filepath, rect),
      m_grid(Size(0, (rect.height() * 0.15)), StaticGrid::GridSize(2, 1), 5),
      m_okay("OK"),
      m_cancel("Cancel")
{
    name("FileOpenDialog" + std::to_string(m_widgetid));

    m_vsizer.add(expand_horizontal(m_grid));
    m_grid.add(expand(m_okay));
    m_grid.add(expand(m_cancel));

    m_okay.on_event([this](Event&)
    {
        list_item_selected(this->m_flist.selected());
    }, {EventId::pointer_click});

    m_cancel.on_event([this](Event&)
    {
        this->m_fselected = std::string();
        this->m_flist.clear();
        this->hide();
    }, {EventId::pointer_click});
}

FileOpenDialog::FileOpenDialog(const Rect& rect) noexcept
    : FileOpenDialog( {}, rect)
{}

void FileOpenDialog::show()
{
    list_files(m_filepath);
    Popup::show();
}

void FileOpenDialog::show_centered()
{
    list_files(m_filepath);
    Popup::show_centered();
}

void FileOpenDialog::selected(const std::string& fselect)
{
    m_fselected = fselect;
    if (!m_fselected.empty())
        on_selected.invoke();
}

std::string FileOpenDialog::selected() const
{
    return (m_filepath + "/" + m_fselected);
}

FileSaveDialog::FileSaveDialog(const std::string& filepath, const Rect& rect) noexcept
    : FileDialog(filepath, rect),
      m_fsave_box("", Size(0, rect.height() * 0.15)),
      m_grid(Size(rect.width() * 0.30, rect.height() * 0.15), StaticGrid::GridSize(2, 1), 5),
      m_okay("OK"),
      m_cancel("Cancel")
{
    auto hpositioner = std::make_shared<HorizontalBoxSizer>();
    m_vsizer.add(expand_horizontal(hpositioner));

    m_fsave_box.margin(5);
    hpositioner->add(expand_horizontal(m_fsave_box));
    hpositioner->add(m_grid);

    m_grid.add(expand(m_okay));
    m_grid.add(expand(m_cancel));

    m_okay.on_event([this](Event&)
    {
        if (!m_fsave.empty())
        {
            on_selected.invoke();
        }
        else if (!m_fsave_box.text().empty())
        {
            m_fsave = m_fsave_box.text();
            on_selected.invoke();
        }
    }, {EventId::pointer_click});

    m_cancel.on_event([this](Event&)
    {
        this->m_fsave = std::string();
        this->m_flist.clear();
        this->m_fsave_box.text(std::string());
        this->hide();
    }, {EventId::pointer_click});
}

FileSaveDialog::FileSaveDialog(const Rect& rect) noexcept
    : FileSaveDialog(std::string(), rect)
{

}

void FileSaveDialog::show()
{
    m_fsave_box.text("");
    list_files(m_filepath);
    Popup::show();
}

void FileSaveDialog::show_centered()
{
    m_fsave_box.text("");
    list_files(m_filepath);
    Popup::show_centered();
}

void FileSaveDialog::selected(const std::string& fselect)
{
    m_fsave = fselect;
    m_fsave_box.text(m_fsave);
}

std::string FileSaveDialog::selected() const
{
    return (m_filepath + "/" + m_fsave);
}

}
}
