#include "canvas.hpp"

#include "convert_palette_dialog.hpp"
#include "create_file_dialog.hpp"
#include "dialogs.hpp"
#include "editor.hpp"
#include "moc_canvas.cpp"
#include "resize_dialog.hpp"
#include "unsaved_file_dialog.hpp"

#include <QDir>
#include <QFileDialog>
#include <QSettings>
#include <QUndoStack>

namespace
{
const auto err_could_not_save = "Could not save the image.";
const auto err_file_already_open = "File is already open in the editor.";
} // namespace

#include <QMessageBox>

namespace veles
{
canvas::canvas(QWidget *_parent) : QTabWidget(_parent)
{
    tool::current = tool::make(tool::type::view);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setContentsMargins({});
    connect(this, &canvas::changed,
        [this](editor *_ed)
        {
            auto *t = tool::current;
            if (last_editor_ != _ed)
            {
                if (last_editor_)
                    t->exit_editor(*last_editor_);
                last_editor_ = _ed;
                if (_ed)
                    t->enter_editor(*_ed);
            }
            if (!_ed)
                return;
            const auto i = currentIndex();
            setTabText(i, _ed->name() + "* "[_ed->history()->isClean()]);
            setTabToolTip(i, _ed->path());
        });
    connect(this, &QTabWidget::currentChanged, [this] { emit changed(current_editor()); });
}

canvas::~canvas() { delete tool::current; }

void canvas::create_new_file() { (new create_file_dialog(this))->open(); }

void canvas::create_image(const QString &_name, QSize _size, palette::type _palette)
{ add_editor(new editor(QString{_name}, _size, _palette)); }

void canvas::open_image(const QString &_filepath)
{
    for (int i = 0, c = count(); i < c; ++i)
    {
        if (qobject_cast<editor *>(widget(i))->path() == _filepath)
        {
            setCurrentIndex(i);
            return;
        }
    }
    try
    {
        add_editor(new editor(_filepath));
    }
    catch (std::runtime_error &err)
    {
        show_error(err.what());
    }
}

void canvas::save()
{
    auto &ed = *current_editor();
    if (ed.history()->isClean() && ed.exists())
        return;
    if (!ed.exists())
        save_as(&ed);
    else if (!ed.save())
        show_error(err_could_not_save);
}

void canvas::save_as(editor *_ed)
{
    if (!_ed)
        _ed = current_editor();
    const auto &path = _ed->path();
    QString dir = path.isEmpty() ? QSettings().value("canvas/saveAs/directory", QDir::homePath()).toString() : path;
    dir += "/" + _ed->name();

    auto *const dialog = create_save_file_dialog(dir, this);
    connect(dialog, &QFileDialog::accepted, [this, _ed, dialog] { save_file(*_ed, *dialog->selectedFiles().data()); });
    dialog->open();
}

void canvas::close_image()
{
    auto &ed = *current_editor();
    if (ed.history()->isClean())
    {
        close_editor(ed);
        return;
    }
    auto *const dialog = new unsaved_file_dialog(ed, this);
    connect(dialog, &unsaved_file_dialog::accepted,
        [this, &ed, dialog](const QString *_path)
        {
            if (_path && !save_file(ed, *_path))
                return;
            dialog->close();
            close_editor(ed);
        });
    dialog->open();
}

bool canvas::try_exit()
{
    while (count() > 0)
    {
        auto &ed = *qobject_cast<editor *>(widget(count() - 1));
        if (ed.history()->isClean())
        {
            close_editor(ed);
            continue;
        }
        auto *const dialog = new unsaved_file_dialog(ed, this);
        connect(dialog, &unsaved_file_dialog::accepted,
            [this, &ed, dialog](const QString *_path)
            {
                if (_path && !save_file(ed, *_path))
                    return;
                dialog->close();
                close_editor(ed);
                if (try_exit())
                    emit exit_prepared();
            });
        dialog->open();
        return false;
    }
    return true;
}

void canvas::resize_image() { (new resize_dialog(current_editor(), this))->show(); }

void canvas::choose_tool(const tool::type _type)
{
    auto *t = tool::current;
    if (t->type_ == _type)
        return;
    auto *next = tool::make(_type);
    auto *ed = current_editor();
    if (ed)
        t->exit_editor(*ed);
    delete t;
    t = tool::current = next;
    if (ed)
        t->enter_editor(*ed);
    emit changed(ed);
}

void canvas::convert_palette() { (new convert_palette_dialog(this, current_editor()))->show(); }

#define ED current_editor()
#define X(FUNC, IMPL) \
    void canvas::FUNC() const { IMPL; }

X_VELES_CANVAS_USED_EDITOR_MEMBERS

#undef X
#undef ED

void canvas::add_editor(editor *const _ed)
{
    connect(_ed, &editor::changed, [this, _ed] { emit changed(_ed); });
    setCurrentIndex(addTab(_ed, _ed->name()));
}

editor *canvas::current_editor() const noexcept { return qobject_cast<editor *>(currentWidget()); }

void canvas::close_editor(editor &_ed)
{
    removeTab(currentIndex());
    _ed.close();
}

bool canvas::save_file(editor &_ed, const QString &_path)
{
    for (int i = 0, mx = count(); i < mx; ++i)
    {
        const auto *ed = qobject_cast<editor *>(widget(i));
        if (ed->path() == _path && ed != &_ed)
        {
            show_error(err_file_already_open);
            return false;
        }
    }
    if (!_ed.save(_path))
    {
        show_error(err_could_not_save);
        return false;
    }
    return true;
}

void canvas::show_error(const QString &_text)
{
    auto *const msg = new QMessageBox(QMessageBox::Critical, "Error", _text, QMessageBox::Ok, this);
    msg->setAttribute(Qt::WA_DeleteOnClose);
    msg->show();
}
} // namespace veles