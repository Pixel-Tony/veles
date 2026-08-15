#include "fill.hpp"

#include "state.hpp"
#include "widgets/editor.hpp"
#include "widgets/image.hpp"

#include <QEvent>
#include <QGraphicsSceneMouseEvent>
#include <QUndoCommand>

namespace
{
struct command : QUndoCommand
{
    command(veles::image *_image, QColor _replaced, QColor _color, QPoint _point, bool _is_star)
        : img_(_image),
          replaced_(_replaced),
          color_(_color),
          point_(_point),
          is_star_(_is_star)
    {
    }

    void undo() override { img_->fill_points(colors_, replaced_); }

    void redo() override
    {
        if (colors_.empty())
            colors_ = img_->flood_fill(point_, color_, is_star_);
        img_->fill_points(colors_, color_);
    }

private:
    veles::image *const img_;
    const QColor replaced_;
    const QColor color_;
    const QPoint point_;
    const bool is_star_;
    QList<QPoint> colors_;
};
} // namespace

namespace veles
{
fill::fill() : tool(type::fill) {}

void fill::editor_event(QEvent &_ev, editor &_ed)
{
    _ev.ignore();
    auto *const mev = static_cast<QGraphicsSceneMouseEvent *>(&_ev);
    switch (_ev.type())
    {
    case QEvent::GraphicsSceneMousePress:
    {
        _ev.accept();
        const auto btn = mev->button();
        mouse_buttons_down_ |= btn;
        auto &img = _ed.get_image();
        const auto pos = img.mapFromScene(mev->scenePos());
        if (!img.contains(pos))
            break;
        auto *const st = state::instance();
        const auto point = QPoint(pos.x(), pos.y());
        if (btn == Qt::MiddleButton && mouse_buttons_down_ == btn)
        {
            st->set_primary(img.pixelColor(point));
            break;
        }
        if (btn == Qt::LeftButton || btn == Qt::RightButton)
        {
            const auto color = st->color(btn == Qt::LeftButton);
            if (mouse_buttons_down_ != btn)
                break;
            if (const auto replaced = img.pixelColor(point); replaced != color)
                _ed.history()->push(
                    new command(&img, replaced, color, point, mev->modifiers().testFlag(Qt::ControlModifier)));
        }
        break;
    }
    case QEvent::GraphicsSceneMouseMove:
    case QEvent::KeyPress:
    case QEvent::KeyRelease:
    case QEvent::ShortcutOverride:
        _ev.setAccepted(mouse_buttons_down_);
        break;
    case QEvent::Wheel:
    case QEvent::GraphicsSceneWheel:
        handle_zoom(_ev, _ed, !mouse_buttons_down_);
        break;
    case QEvent::GraphicsSceneMouseRelease:
        _ev.accept();
        mouse_buttons_down_ &= ~mev->button();
        break;
    default:
        break;
    }
}
} // namespace veles