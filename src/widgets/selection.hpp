#ifndef VELES_SELECTION_HPP
#define VELES_SELECTION_HPP

#include <QGraphicsObject>
#include <QImage>

namespace veles
{
class image;

class selection : public QGraphicsObject
{
    Q_OBJECT

public:
    selection(QRect _rect, QGraphicsObject *_parent);

signals:
    void size_changed();

public:
    [[nodiscard]] QRect rect() const noexcept;
    void set_rect(QRect _rect);
    [[nodiscard]] const QImage &get_image() const noexcept;
    void set_image(QImage &&_other);
    void set_image(QImage &_other) = delete;
    void set_image(const QImage &_other);

private:
    void set_size(QSize _size);

protected:
    void paint(QPainter *_painter, const QStyleOptionGraphicsItem *_opt, QWidget *_widget = nullptr) override;
    [[nodiscard]] QRectF boundingRect() const override;

private:
    int offset_;
    QImage contents_;
    QSize size_;
};
} // namespace veles

#endif // VELES_SELECTION_HPP