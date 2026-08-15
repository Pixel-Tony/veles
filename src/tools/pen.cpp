#include "pen.hpp"

#include "state.hpp"
#include "widgets/editor.hpp"
#include "widgets/image.hpp"

#include <QEvent>
#include <QGraphicsSceneMouseEvent>

namespace
{
inline bool covered_by_selection(const veles::editor &_ed, const QPointF &_point)
{
    auto *sel = _ed.get_selection();
    return sel && !sel->get_image().isNull() && sel->sceneBoundingRect().contains(_point);
}
} // namespace

namespace veles
{
pen::command::command(QColor _color, image *_image) : color_(_color), image_(_image) {}

bool pen::command::empty() const noexcept { return pixels_.empty(); }

void pen::command::undo() { image_->fill_points(pixels_); }

void pen::command::redo() { image_->fill_points(pixels_, color_); }

void pen::command::add_color(QPointF _pos)
{
    if (!image_->contains(_pos))
        return;
    const auto pixel_pos = QPoint(_pos.x(), _pos.y());
    QColor old;
    if (image_->set_pixel_color(pixel_pos, color_, &old))
        pixels_.emplace_back(pixel_pos, old);
}

pen::pen() : tool(type::pen) {}

pen::~pen() { delete command_; }

void pen::editor_event(QEvent &_ev, editor &_ed)
{
    _ev.ignore();
    const auto *const mev = static_cast<QGraphicsSceneMouseEvent *>(&_ev);
    switch (_ev.type())
    {
    case QEvent::GraphicsSceneMousePress:
    {
        _ev.accept();
        auto *const st = state::instance();
        auto &img = _ed.get_image();
        const auto pos = mev->scenePos();
        QColor color;
        switch (mev->button())
        {
        case Qt::MiddleButton:
            if (img.contains(pos))
                st->set_primary(img.pixelColor(point_to_pixel(pos)));
            return;
        case Qt::LeftButton:
            color = st->primary();
            break;
        case Qt::RightButton:
            color = st->secondary();
            break;
        default:
            _ev.ignore();
            return;
        }
        command_ = new command(color, &img);
        if (!covered_by_selection(_ed, pos))
            command_->add_color(pos);
        break;
    }
    case QEvent::KeyPress:
    case QEvent::KeyRelease:
    case QEvent::ShortcutOverride:
        _ev.setAccepted(command_);
        break;
    case QEvent::Wheel:
    case QEvent::GraphicsSceneWheel:
        handle_zoom(_ev, _ed, !command_);
        break;
    case QEvent::GraphicsSceneMouseMove:
        _ev.setAccepted(command_);
        if (const auto point = mev->scenePos(); command_ && !covered_by_selection(_ed, point))
            command_->add_color(point);
        break;
    case QEvent::GraphicsSceneMouseRelease:
        _ev.setAccepted(command_);
        if (command_ && !command_->empty())
            _ed.history()->push(command_);
        else
            delete command_;
        command_ = nullptr;
        break;
    default:
        break;
    }
}
} // namespace veles