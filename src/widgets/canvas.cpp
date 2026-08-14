#include "canvas.hpp"

#include "create_file_dialog.hpp"
#include "editor.hpp"
#include "moc_canvas.cpp"

#include <QMessageBox>

namespace veles
{
canvas::canvas(QWidget *_parent) : QTabWidget(_parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setContentsMargins({});
    connect(this, &QTabWidget::currentChanged, [this] { emit changed(current_editor()); });
}

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

void canvas::close_image() { close_editor(*current_editor()); }

void canvas::exit()
{
    while (count() > 0)
        close_editor(*qobject_cast<editor *>(widget(count() - 1)));
}

void canvas::add_editor(editor *const _ed) { setCurrentIndex(addTab(_ed, _ed->name())); }

editor *canvas::current_editor() const noexcept { return qobject_cast<editor *>(currentWidget()); }

void canvas::close_editor(editor &_ed)
{
    removeTab(currentIndex());
    _ed.close();
}

void canvas::show_error(const QString &_text)
{
    auto *const msg = new QMessageBox(QMessageBox::Critical, "Error", _text, QMessageBox::Ok, this);
    msg->setAttribute(Qt::WA_DeleteOnClose);
    msg->show();
}
} // namespace veles