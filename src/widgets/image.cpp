#include "image.hpp"

#include "moc_image.cpp"

#include <QCursor>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QToolTip>

namespace
{
template <typename TImage>
struct raster
{
    using type = std::conditional_t<std::is_const_v<TImage>, const QRgb, QRgb>;

    raster(TImage *_image) : width_(_image->width()), contents_(reinterpret_cast<type *>(_image->bits())) {}

    type &operator[](QPoint _point) { return contents_[_point.y() * width_ + _point.x()]; }

private:
    const uint width_;
    type *const contents_;
};
} // namespace

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

bool image::set_pixel_color(const QPoint _point, const QColor _color, QColor *_old)
{
    if (palette_ && palette_->index(_color) == -1)
    {
        QToolTip::showText(QCursor::pos(), "Color not in the image palette");
        return false;
    }
    if ((*_old = pixelColor(_point)) == _color)
        return false;
    setPixelColor(_point, _color);
    update();
    return true;
}

bool image::save_with_palette(const QString &_filepath, const char *_format) const
{
    if (palette_)
        return convertToFormat(Format_Indexed8, palette_->contents_).save(_filepath, _format);
    return save(_filepath, _format);
}

void image::fill_points(const QList<QPair<QPoint, QColor>> &_positions_colors)
{
    auto r = raster(this);
    for (const auto &[pos, color] : _positions_colors)
        r[pos] = color.rgba();
    update();
}

void image::fill_points(const QList<QPair<QPoint, QColor>> &_positions, QColor _fill_color)
{
    auto r = raster(this);
    for (const auto &[pos, _] : _positions)
        r[pos] = _fill_color.rgba();
    update();
}

void image::update_contents(QImage &&_source)
{
    const auto is_size_changed = _source.size() != size();
    if (is_size_changed)
        prepareGeometryChange();
    *static_cast<QImage *>(this) = std::move(_source);
    if (is_size_changed)
        emit size_changed();
    update();
}

void image::update_contents(const QImage &_source) { update_contents(_source.copy()); }

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
