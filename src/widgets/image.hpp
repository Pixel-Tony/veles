#ifndef VELES_IMAGE_HPP
#define VELES_IMAGE_HPP

#include "palette.hpp"

#include <QGraphicsObject>
#include <QImage>

namespace veles
{
class image : public QGraphicsObject, public QImage
{
    Q_OBJECT

public:
    image(QSize _size, palette::type _type);

    explicit image(const QString &_filepath) noexcept(false);

    [[nodiscard]] const palette *get_palette() const noexcept;

protected:
    void paint(QPainter *_painter, const QStyleOptionGraphicsItem *_opt, QWidget *_widget = nullptr) override;
    [[nodiscard]] QRectF boundingRect() const override;

private:
    const palette *const palette_;
};
} // namespace veles

#endif // VELES_IMAGE_HPP