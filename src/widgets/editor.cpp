#include "editor.hpp"

#include "image.hpp"
#include "moc_editor.cpp"
#include "resize_dialog.hpp"
#include "state.hpp"
#include "tools/tool.hpp"

#include <QEvent>
#include <QFileInfo>
#include <QOpenGLWidget>
#include <QUndoStack>

namespace
{
using namespace veles;

QString to_png_filename(const QString &_filepath)
{
    const auto info = QFileInfo(_filepath);
    return _filepath.endsWith(".png") ? info.fileName() : (info.baseName() + ".png");
}

struct flip_command : QUndoCommand
{
    flip_command(image *_img, Qt::Orientations _orient) : img_(_img), orient_(_orient) {}

    void undo() override { redo(); }

    void redo() override { img_->update_contents(img_->flipped(orient_)); }

private:
    image *const img_;
    const Qt::Orientations orient_;
};

struct rotate_command : QUndoCommand
{
    rotate_command(image *_img, bool _clockwise) : img_(_img), angle_(_clockwise ? 90 : -90) {}

    void undo() override { img_->update_contents(img_->transformed(QTransform().rotate(-angle_))); }

    void redo() override { img_->update_contents(img_->transformed(QTransform().rotate(angle_))); }

private:
    image *const img_;
    const int angle_;
};

struct resize_command : QUndoCommand
{
    resize_command(image *_img, QSize _new_size, QPair<resize_anchor, resize_anchor> _anchors)
        : img_(_img),
          old_image_(*static_cast<const QImage *const>(img_)),
          new_size_(_new_size),
          anchors_(_anchors),
          bg_(state::instance()->secondary())
    {
    }

    void undo() override { img_->update_contents(old_image_); }

    void redo() override
    {
        auto new_img = QImage(new_size_, QImage::Format_ARGB32);
        new_img.fill(bg_);

        const auto old_size = img_->size();
        const auto source_dims = old_size.boundedTo(new_size_);
        const auto in_delta = old_size - source_dims;
        const auto out_delta = new_size_ - source_dims;

        QPoint source_shift;
        QPoint pos;
        switch (anchors_.first)
        {
        case resize_anchor::start:
            break;
        case resize_anchor::center:
            source_shift.setX(in_delta.width() / 2);
            pos.setX(out_delta.width() / 2);
            break;
        case resize_anchor::end:
            source_shift.setX(in_delta.width());
            pos.setX(out_delta.width());
            break;
        }
        switch (anchors_.second)
        {
        case resize_anchor::start:
            break;
        case resize_anchor::center:
            source_shift.setY(in_delta.height() / 2);
            pos.setY(out_delta.height() / 2);
            break;
        case resize_anchor::end:
            source_shift.setY(in_delta.height());
            pos.setY(out_delta.height());
            break;
        }
        QPainter painter{&new_img};
        painter.drawImage(pos, old_image_, {source_shift, source_dims});
        img_->update_contents(std::move(new_img));
    }

private:
    image *const img_;
    const QPair<resize_anchor, resize_anchor> anchors_;
    const QSize new_size_;
    const QImage old_image_;
    const QColor bg_;
};
} // namespace

namespace veles
{
priv::tool_handler::tool_handler(editor *_parent) : QObject(_parent), ed_(_parent) {}

bool priv::tool_handler::eventFilter(QObject *, QEvent *_ev)
{
    tool::current->editor_event(*_ev, *ed_);
    return _ev->isAccepted();
}

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
      tool_handler_(new priv::tool_handler(this)),
      image_(_image),
      image_padding_(new QGraphicsRectItem(image_)),
      name_(std::move(_name)),
      path_(std::move(_path)),
      history_(new QUndoStack(this))
{
    image_->setParent(this);
    installEventFilter(tool_handler_);
    setAttribute(Qt::WA_DeleteOnClose);
    setBackgroundBrush(palette().window());
    setContentsMargins(2, 2, 2, 2);
    setFocusPolicy(Qt::WheelFocus);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setTransformationAnchor(ViewportAnchor::AnchorUnderMouse);
    setViewport(new QOpenGLWidget(this));

    image_->installEventFilter(tool_handler_);
    image_padding_->setPen(Qt::NoPen);
    history_->setUndoLimit(200);

    connect(image_, &image::size_changed, this, &editor::center_on_image);
    connect(this, &editor::changed, [this] { setTransform(QTransform::fromScale(zoom_, zoom_)); });
    connect(history_, &QUndoStack::indexChanged, this, &editor::changed);
    connect(history_, &QUndoStack::cleanChanged, this, &editor::changed);

    scene()->addItem(image_);
    scene()->installEventFilter(tool_handler_);
    center_on_image();
}

editor::~editor() { history_->disconnect(this); }

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

QUndoStack *editor::history() noexcept { return history_; }

const QUndoStack *editor::history() const noexcept { return history_; }

bool editor::exists() const noexcept { return !path_.isEmpty(); }

float editor::zoom() const noexcept { return zoom_; }

void editor::default_zoom()
{
    zoom_ = 1.f;
    emit changed();
}

bool editor::save() { return save(path_); }

bool editor::save(const QString &_path)
{
    if (!image_->save_with_palette(_path, "png"))
        return false;
    if (path_ != _path)
    {
        path_ = _path;
        name_ = QFileInfo(path_).fileName();
    }
    if (!history_->isClean())
        history_->setClean();
    else
        emit changed();
    return true;
}

void editor::apply_zoom_delta(const float _delta)
{
    const auto zoom_factor = 1 + (zoom_ >= zoom_bounds.second / 3);
    const auto new_zoom = qBound(zoom_bounds.first, zoom_ + _delta * zoom_factor, zoom_bounds.second);
    if (new_zoom == zoom_)
        return;
    zoom_ = new_zoom;
    emit changed();
}

void editor::flip_image(Qt::Orientations _orient) { history_->push(new flip_command(image_, _orient)); }

void editor::rotate_image(bool _clockwise) { history_->push(new rotate_command(image_, _clockwise)); }

void editor::resize_image(QSize _new_size, QPair<resize_anchor, resize_anchor> _anchors)
{ history_->push(new resize_command(image_, _new_size, _anchors)); }

} // namespace veles
