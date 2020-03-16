/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_FILEDIALOG_H
#define EGT_FILEDIALOG_H

/**
 * @file
 * @brief Working with file dialog pop-ups.
 */

#include <egt/button.h>
#include <egt/detail/meta.h>
#include <egt/detail/signal.h>
#include <egt/popup.h>
#include <memory>
#include <string>

namespace egt
{
inline namespace v1
{
class BoxSizer;
class ImageLabel;
class ListBox;
class StaticGrid;
class TextBox;

/**
 * A FileDialog is a widget that allows user to:
 *
 * 1. Choose a file from the file system.
 * 2. View the contents of file system directories.
 * 3. Select a location for saving a file.
 *
 * @note FileDialog widget is using a std::experimental::filesystem
 * library
 *
 */
class EGT_API FileDialog : public Popup
{
public:

    /**
     * Event signal.
     * @{
     */
    /**
     * Invoked when a selection is made.
     */
    detail::Signal<> on_selected;
    /** @} */

    /**
     * Create a file open dialog window and list the contents
     * of current directory.
     *
     * @param[in] rect is a size of a dialog window.
     *
     */
    explicit FileDialog(const Rect& rect = {});

    /**
     * Create a file dialog window.
     *
     * @param[in] rect is a size of a dialog window.
     * @param[in] filepath is to list the contents filepath directory.
     *
     * @note filepath should be a directory. If the file path is regular file,
     * then content of its parent directory will be listed.
     */
    explicit FileDialog(const std::string& filepath, const Rect& rect = {});

    virtual ~FileDialog() = default;

protected:
    /**
     * Vertical BoxSizer
     */
    std::shared_ptr<BoxSizer> m_vsizer;

    /**
     * Title of a FileDialog.
     */
    std::shared_ptr<ImageLabel> m_title;

    /**
     * List Box for file listing.
     */
    std::shared_ptr<ListBox> m_flist;

    /**
     * string containing the file path of a directory.
     */
    std::string m_filepath;

    /**
     * List the contents of file path directory.
     */
    bool list_files(const std::string& filepath);

    /**
     * Get the List Item selected index.
     */
    void list_item_selected(int index);

    /**
     * File selected.
     *
     * @param[in] fselect is a selected filename.
     */
    virtual void selected(const std::string& fselect) = 0;
};

/**
 * A FileOpenDialog is a widget which inherits from FileDialog
 * and that allows user to:
 *
 * 1. Choose a file from the file system.
 * 2. View the contents of file system directories.
 *
 */
class EGT_API FileOpenDialog : public FileDialog
{
public:
    /**
     * Create a file open dialog window and list the contents
     * of current directory.
     *
     * @param[in] rect is a size of file open dialog window.
     *
     */
    explicit FileOpenDialog(const Rect& rect = {});

    /**
     * Create a file open dialog window.
     *
     * @param[in] rect is a size of a dialog window.
     * @param[in] filepath is to list the contents of filepath directory.
     *
     * @note filepath should be a directory. If the file path is regular file,
     * then content of its parent directory will be listed.
     */
    explicit FileOpenDialog(const std::string& filepath, const Rect& rect = {});

    virtual void show() override;

    virtual void show_centered() override;
    /**
     * return file selected in file open dialog window.
     *
     * @return full path of the selected file.
     */
    virtual std::string selected() const;

    virtual ~FileOpenDialog() = default;

protected:
    /**
     * grid for organizing okay & cancel Buttons.
     */
    std::shared_ptr<StaticGrid> m_grid;

    /**
     * okay Button.
     */
    std::shared_ptr<Button> m_okay;

    /**
     * cancel Button.
     */
    std::shared_ptr<Button> m_cancel;

    /**
     * File path of a selected file.
     */
    std::string m_fselected;

    virtual void selected(const std::string& fselect) override;

};

/**
 * A FileSaveDialog is a widget which inherits from FileDialog
 * and that allows user to:
 *
 * 1. View the contents of file system directories.
 * 2. Select a location to save a file or select a
 *    file to override a existing file.
 */
class EGT_API FileSaveDialog : public FileDialog
{
public:
    /**
     * Create a file save dialog window and list the contents
     * of current directory.
     *
     * @param[in] rect is a size of file open dialog window.
     */
    explicit FileSaveDialog(const Rect& rect = {});

    /**
     * Create a file save dialog window.
     *
     * @param[in] rect is a size of a dialog window.
     * @param[in] filepath is to list the contents of filepath directory.
     *
     * @note filepath should be a directory. If the file path is regular file,
     * then content of its parent directory will be listed.
     */
    explicit FileSaveDialog(const std::string& filepath, const Rect& rect = {});

    virtual void show() override;

    virtual void show_centered() override;

    /**
     * Selected a location for saving a file.
     *
     * @return file path of the file save location.
     */
    virtual std::string selected() const;

    virtual ~FileSaveDialog() = default;

protected:

    /**
     * m_fsave_box to input a filename or file path.
     */
    std::shared_ptr<TextBox> m_fsave_box;

    /**
     * grid for organizing okay & cancel Buttons.
     */
    std::shared_ptr<StaticGrid> m_grid;

    /**
     * okay Button.
     */
    std::shared_ptr<Button> m_okay;

    /**
     * cancel Button.
     */
    std::shared_ptr<Button> m_cancel;

    /**
     * File path of a file save location.
     */
    std::string m_fsave;

    virtual void selected(const std::string& fselect) override;
};

}
}

#endif
