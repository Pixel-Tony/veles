#include "state.hpp"

#include "moc_state.cpp"

#include <QApplication>
#include <QClipboard>

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
} // namespace veles