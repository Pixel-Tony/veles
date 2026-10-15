#include "state.hpp"

#include "moc_state.cpp"

#include <QApplication>
#include <QClipboard>
#include <QFileInfo>
#include <QSettings>

namespace
{
veles::state *instance_ = nullptr;
}

namespace veles
{
state::state() : QObject(qApp), primary_(Qt::white), secondary_(Qt::blue) { instance_ = this; }

[[nodiscard]] QColor state::primary() const noexcept { return primary_; }

void state::set_primary(QColor _color) { set_color(_color, true); }

[[nodiscard]] QColor state::secondary() const noexcept { return secondary_; }

void state::set_secondary(QColor _color) { set_color(_color, false); }

void state::set_color(QColor _color, bool _primary)
{
    (_primary ? primary_ : secondary_) = _color;
    emit color_updated(_color, _primary);
}

void state::push_recent(const QString &_filepath) noexcept
{
    auto recent = recent_files();
    if (recent.contains(_filepath))
        recent.removeAll(_filepath);
    recent.prepend(_filepath);
    store_recent(recent);
}

void state::replace_recent(const QString &_filepath, const QString &_new_filepath) noexcept
{
    auto recent = recent_files();
    recent.removeAll(_filepath);
    recent.prepend(_new_filepath);
    store_recent(recent);
}

[[nodiscard]] QStringList state::recent_files() const noexcept
{ return QSettings().value("recentFiles", QStringList{}).toStringList(); }

void state::set_recent_count(int _count) noexcept
{
    if (_count == max_recent_files_)
        return;
    max_recent_files_ = _count;
    store_recent(recent_files());
}

[[nodiscard]] int state::recent_count() const noexcept { return max_recent_files_; }

void state::refresh_recent_files() noexcept { store_recent(recent_files()); }

void state::clear_recent_files() noexcept { store_recent({}); }

void state::swap_colors()
{
    qSwap(primary_, secondary_);
    emit color_updated(primary_, true);
    emit color_updated(secondary_, false);
}

[[nodiscard]] QColor state::color(bool _primary) const noexcept { return _primary ? primary_ : secondary_; }

void state::set_clipboard(const QImage &_image, const palette *_palette)
{
    qApp->clipboard()->setImage(_image);
    clipboard_ = {_image, _palette};
}

QPair<const QImage &, const palette *> state::clipboard() { return clipboard_; }

state *state::instance() { return instance_ ? instance_ : new state; }

void state::store_recent(QStringList _recent) noexcept
{
    _recent.removeDuplicates();
    for (int i = 0; i < _recent.length();)
    {
        const auto fileinfo = QFileInfo{_recent[i]};
        if (fileinfo.isSymLink() || !fileinfo.isFile())
        {
            _recent.remove(i);
            continue;
        }
        ++i;
    }
    QSettings().setValue("recentFiles", _recent.sliced(0, qMin(_recent.length(), max_recent_files_)));
    emit recent_files_changed();
}
} // namespace veles
