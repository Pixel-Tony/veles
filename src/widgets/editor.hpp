#ifndef VELES_CANVAS_SCENE_HPP
#define VELES_CANVAS_SCENE_HPP

#include "palette.hpp"

#include <QGraphicsView>
#include <QPair>
#include <QSize>
#include <QString>

namespace veles
{
class image;

class editor : public QGraphicsView
{
    Q_OBJECT

public:
    editor(QString &&_name, QSize _image_size, palette::type _palette, QWidget *_parent = nullptr);

    explicit editor(const QString &_filepath, QWidget *_parent = nullptr) noexcept(false);

private:
    editor(image *_image, QString &&_path, QString &&_name, QWidget *_parent);

public:
    [[nodiscard]] image &get_image() noexcept;
    [[nodiscard]] const image &get_image() const noexcept;
    [[nodiscard]] const QString &name() const noexcept;
    [[nodiscard]] const QString &path() const noexcept;

private slots:
    void center_on_image();

private:
    image *image_;
    QGraphicsRectItem *image_padding_;
    QString name_;
    QString path_;
};
} // namespace veles

#endif // VELES_CANVAS_SCENE_HPP