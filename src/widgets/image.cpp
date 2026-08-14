#include "image.hpp"

#include "moc_image.cpp"

#include <QPainter>
#include <QStyleOptionGraphicsItem>

namespace veles
{
image::image(QSize _size, palette::type _type) : QImage(_size, QImage::Format_ARGB32), palette_(palette::make(_type))
{ fill(Qt::white); }

image::image(const QString &_filepath) : QImage(_filepath), palette_(palette::find(colorTable()))
{
    if (isNull())
        throw std::runtime_error{"File is not an image."};
    if (format() != QImage::Format_ARGB32)
        convertTo(QImage::Format_ARGB32);
}

const palette *image::get_palette() const noexcept { return palette_; }

void image::paint(QPainter *_painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    const auto rect_ = QRectF({}, size());
    if (!palette_)
    {
        static QPixmap tex;
        if (tex.isNull())
        {
            const auto gray = qRgb(220, 220, 220);
            tex = QPixmap(4, 4);
            tex.fill(Qt::white);
            QPainter p(&tex);
            p.fillRect(QRect(0, 0, 2, 2), gray);
            p.fillRect(QRect(2, 2, 2, 2), gray);
        }
        _painter->drawTiledPixmap(rect_, tex);
    }
    _painter->drawImage(rect_, *this);
}

QRectF image::boundingRect() const { return {{}, size()}; }
} // namespace veles
