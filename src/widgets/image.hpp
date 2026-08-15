#ifndef VELES_IMAGE_HPP
#define VELES_IMAGE_HPP

#include "palette.hpp"

#include <QGraphicsObject>
#include <QImage>

namespace veles
{
inline QPoint point_to_pixel(QPointF _point) { return {static_cast<int>(_point.x()), static_cast<int>(_point.y())}; }

class image : public QGraphicsObject, public QImage
{
    Q_OBJECT

public:
    image(QSize _size, palette::type _type);

    explicit image(const QString &_filepath) noexcept(false);

    [[nodiscard]] const palette *get_palette() const noexcept;

    [[nodiscard]] bool set_pixel_color(QPoint _point, QColor _color, QColor *_old);
    [[nodiscard]] bool save_with_palette(const QString &_filepath, const char *_format = nullptr) const;
    void fill_points(const QList<QPair<QPoint, QColor>> &_positions_colors);
    void fill_points(const QList<QPoint> &_positions, QColor _fill_color);
    void fill_points(const QList<QPair<QPoint, QColor>> &_positions, QColor _fill_color);
    [[nodiscard]] QList<QPoint> flood_fill(QPoint _origin, QColor _color, bool _is_star) const;
    void update_contents(QImage &&_source);
    void update_contents(const QImage &_source);
    void update_contents(QImage &_source) = delete;
    bool convert_palette(palette::type _type, bool _validate = true);

    [[nodiscard]] QRect sprite_bounds(QPoint _pos) const noexcept;

signals:
    void size_changed();

protected:
    void paint(QPainter *_painter, const QStyleOptionGraphicsItem *_opt, QWidget *_widget = nullptr) override;
    [[nodiscard]] QRectF boundingRect() const override;

private:
    const palette *palette_;
};
} // namespace veles

#endif // VELES_IMAGE_HPP