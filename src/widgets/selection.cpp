#include "selection.hpp"

#include "moc_selection.cpp"

#include <QPainter>
#include <QTimer>

namespace veles
{
selection::selection(QRect _rect, QGraphicsObject *_parent)
    : QGraphicsObject(_parent),
      size_(_rect.size()),
      contents_({})
{
    setFlags(QGraphicsItem::ItemIsMovable);
    setPos(_rect.topLeft());
    auto *timer = new QTimer(this);
    timer->setInterval(50);
    connect(timer, &QTimer::timeout,
        [this]
        {
            offset_ = (offset_ + 1) % 10;
            update();
        });
    timer->start();
}

QRect selection::rect() const noexcept { return QRect{QPoint(pos().x(), pos().y()), size_}; }

void selection::set_rect(QRect _rect)
{
    setPos(_rect.topLeft());
    set_size(_rect.size());
}

[[nodiscard]] const QImage &selection::get_image() const noexcept { return contents_; }

void selection::set_image(QImage &&_other)
{
    contents_ = std::move(_other);
    set_size(contents_.size());
    update();
}

void selection::set_image(const QImage &_other) { set_image(_other.copy()); }

void selection::set_size(QSize _size)
{
    if (_size != size_)
    {
        prepareGeometryChange();
        size_ = _size;
        emit size_changed();
        update();
    }
}

void selection::paint(QPainter *_painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    const auto rect = QRectF{{}, size_};
    if (!contents_.isNull())
        _painter->drawImage(rect, contents_);

    QPen boundary{Qt::BrushStyle::SolidPattern, 3, Qt::PenStyle::SolidLine, Qt::PenCapStyle::SquareCap,
        Qt::PenJoinStyle::MiterJoin};
    boundary.setCosmetic(true);
    _painter->setPen(boundary);
    _painter->drawRect(rect);

    boundary.setWidth(1);
    boundary.setStyle(Qt::PenStyle::DashLine);
    boundary.setDashOffset(offset_);
    boundary.setDashPattern({5, 5});
    boundary.setColor(contents_.isNull() ? qRgb(250, 250, 250) : qRgb(130, 250, 130));
    _painter->setPen(boundary);
    _painter->drawRect(rect);
}

QRectF selection::boundingRect() const { return QRectF{{}, size_}; }
} // namespace veles