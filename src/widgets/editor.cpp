#include "editor.hpp"

#include "image.hpp"
#include "moc_editor.cpp"

#include <QFileInfo>
#include <QOpenGLWidget>

namespace
{
QString to_png_filename(const QString &_filepath)
{
    const auto info = QFileInfo(_filepath);
    return _filepath.endsWith(".png") ? info.fileName() : (info.baseName() + ".png");
}
} // namespace

namespace veles
{
editor::editor(QString &&_name, QSize _image_size, palette::type _palette, QWidget *_parent)
    : editor(new image(_image_size, _palette), {}, std::move(_name), _parent)
{
}

editor::editor(const QString &_filepath, QWidget *_parent)
    : editor(new image(_filepath), _filepath.endsWith(".png") ? QString{_filepath} : QString{},
          std::move(to_png_filename(_filepath)), _parent)
{
}

editor::editor(image *_image, QString &&_path, QString &&_name, QWidget *_parent)
    : QGraphicsView(new QGraphicsScene(_parent), _parent),
      image_(_image),
      image_padding_(new QGraphicsRectItem(image_)),
      name_(std::move(_name)),
      path_(std::move(_path))
{
    image_->setParent(this);
    setAttribute(Qt::WA_DeleteOnClose);
    setBackgroundBrush(palette().window());
    setContentsMargins(2, 2, 2, 2);
    setFocusPolicy(Qt::WheelFocus);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setTransformationAnchor(ViewportAnchor::AnchorUnderMouse);
    setViewport(new QOpenGLWidget(this));
    image_padding_->setPen(Qt::NoPen);
    scene()->addItem(image_);
    center_on_image();
}

void editor::center_on_image()
{
    const auto pad_size = 40;
    image_padding_->setRect({-pad_size * QPoint{1, 1}, image_->size() + 2 * pad_size * QSize{1, 1}});
    const auto rect = image_padding_->sceneBoundingRect();
    scene()->setSceneRect(rect);
    setSceneRect(rect);
    centerOn(rect.center());
}

image &editor::get_image() noexcept { return *image_; }

const image &editor::get_image() const noexcept { return *image_; }

const QString &editor::name() const noexcept { return name_; }

const QString &editor::path() const noexcept { return path_; }
} // namespace veles
