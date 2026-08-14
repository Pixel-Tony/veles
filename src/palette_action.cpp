#include "palette_action.hpp"

#include "moc_palette_action.cpp"
#include "state.hpp"

#include <QPainter>
#include <QPixmap>

namespace veles
{
palette_action::palette_action(QObject *_parent) : QWidgetAction(_parent)
{
    setCheckable(true);
    setText("Show/Hide Palette");
    connect(state::instance(), &state::color_updated, this, &palette_action::update_colors);
}

void palette_action::update_colors()
{
    auto pix = QPixmap{24, 24};
    pix.fill(Qt::transparent);
    QPainter painter{&pix};
    painter.setPen(qRgba(0, 0, 0, 255));
    painter.setBrush(state::instance()->secondary());
    painter.drawRect(1, 0, 16, 16);
    painter.setBrush(state::instance()->primary());
    painter.drawRect(6, 7, 16, 16);
    setIcon(pix);
}
} // namespace veles
