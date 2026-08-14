#include "editor.hpp"

#include "image.hpp"
#include "moc_editor.cpp"
#include "tools/tool.hpp"

#include <QEvent>
#include <QFileInfo>
#include <QOpenGLWidget>
#include <QUndoStack>

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

} // namespace veles
