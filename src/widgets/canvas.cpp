#include "canvas.hpp"

#include "create_file_dialog.hpp"
#include "editor.hpp"
#include "moc_canvas.cpp"

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
            setTabText(i, _ed->name());
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

void canvas::close_image() { close_editor(*current_editor()); }

void canvas::exit()
{
    while (count() > 0)
        close_editor(*qobject_cast<editor *>(widget(count() - 1)));
}

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

void canvas::show_error(const QString &_text)
{
    auto *const msg = new QMessageBox(QMessageBox::Critical, "Error", _text, QMessageBox::Ok, this);
    msg->setAttribute(Qt::WA_DeleteOnClose);
    msg->show();
}
} // namespace veles