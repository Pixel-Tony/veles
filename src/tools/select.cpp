#include "select.hpp"

#include "state.hpp"
#include "widgets/editor.hpp"
#include "widgets/image.hpp"
#include "widgets/selection.hpp"

#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include <QUndoStack>

namespace
{
using namespace veles;

struct selection_lift_command : QUndoCommand
{
    selection_lift_command(image *_image, selection *const _sel)
        : image_(_image),
          rect_(_sel->rect()),
          bg_(state::instance()->secondary()),
          source_(image_->copy(rect_))
    { _sel->set_image(source_); }

    void undo() override
    {
        QPainter painter{image_};
        painter.drawImage(rect_, source_);
        image_->update();
    }

    void redo() override
    {
        QPainter painter{image_};
        painter.setCompositionMode(QPainter::CompositionMode_Source);
        painter.fillRect(rect_, bg_);
        image_->update();
    }

private:
    image *const image_;
    const QRect rect_;
    const QColor bg_;
    const QImage source_;
};
} // namespace

namespace veles
{
select::select() : tool(type::select), action_(std::nullopt) {}

void select::editor_event(QEvent &_ev, editor &_ed)
{
    _ev.ignore();
    auto *mev = static_cast<QGraphicsSceneMouseEvent *>(&_ev);
    switch (_ev.type())
    {
    case QEvent::GraphicsSceneMousePress:
    {
        _ev.accept();
        const auto btn = mev->button();
        mouse_buttons_down_ |= btn;
        if (mouse_buttons_down_ != Qt::LeftButton)
            break;
        const auto origin = mev->scenePos();
        if (auto *const sel = _ed.get_selection())
        {
            if (sel->sceneBoundingRect().contains(origin))
            {
                sel_ = sel;
                action_ = point_to_pixel(sel_->scenePos() - origin);
                if (mev->modifiers().testFlag(Qt::ControlModifier) && sel_->get_image().isNull())
                    _ed.history()->push(new selection_lift_command(&_ed.get_image(), sel_));
                break;
            }
            _ed.drop_selection();
        }
        if (mev->modifiers().testFlag(Qt::ControlModifier))
        {
            const auto rect = _ed.get_image().sprite_bounds(point_to_pixel(origin));
            if (!rect.isNull())
            {
                sel_ = _ed.add_selection(rect);
                action_ = QPair{rect, rect};
                break;
            }
        }
        action_ = origin;
        break;
    }
    case QEvent::GraphicsSceneMouseMove:
    {
        _ev.setAccepted(mouse_buttons_down_);
        if (std::holds_alternative<std::nullopt_t>(action_))
            break;
        auto pixel = point_to_pixel(mev->scenePos());
        if (const auto *move_origin_shift = std::get_if<QPoint>(&action_))
        {
            sel_->setPos(pixel + *move_origin_shift);
            break;
        }
        if (const auto *cast_origin = std::get_if<QPointF>(&action_))
        {
            const auto rect = QRect::span(point_to_pixel(*cast_origin), pixel).intersected(_ed.get_image().rect());
            if (sel_)
                sel_->set_rect(rect);
            else if (qMax(rect.width(), rect.height()) > 1)
                sel_ = _ed.add_selection(rect);
            break;
        }
        auto [r_first, r_second] = std::get<QPair<QRect, QRect>>(action_);
        if (r_second.contains(pixel))
            break;
        auto candidate = _ed.get_image().sprite_bounds(pixel);
        if (candidate.isNull())
            candidate = r_first;
        sel_->set_rect(r_first.united(candidate));
        action_ = QPair{r_first, candidate};
    }
    case QEvent::KeyPress:
    {
        if (mouse_buttons_down_)
        {
            _ev.setAccepted(true);
            break;
        }
        selection *sel = nullptr;
        if (!(sel = _ed.get_selection()))
            break;
        auto *kev = static_cast<QKeyEvent *>(&_ev);
        QPoint shift;
        switch (kev->key())
        {
        case Qt::Key_Escape:
        case Qt::Key_Return:
            _ed.drop_selection();
            _ev.setAccepted(true);
            return;
        case Qt::Key::Key_Up:
            shift = {0, -1};
            break;
        case Qt::Key::Key_Down:
            shift = {0, 1};
            break;
        case Qt::Key::Key_Left:
            shift = {-1, 0};
            break;
        case Qt::Key::Key_Right:
            shift = {1, 0};
            break;
        default:
            _ev.setAccepted(false);
            return;
        }
        sel->setPos(sel->pos() + shift);
        _ev.setAccepted(true);
        break;
    }
    case QEvent::KeyRelease:
    case QEvent::ShortcutOverride:
        _ev.setAccepted(mouse_buttons_down_);
        break;
    case QEvent::Wheel:
    case QEvent::GraphicsSceneWheel:
        handle_zoom(_ev, _ed, !mouse_buttons_down_);
        break;
    case QEvent::GraphicsSceneMouseRelease:
    {
        _ev.accept();
        const auto btn = mev->button();
        if (btn == Qt::LeftButton)
            action_ = std::nullopt;
        sel_ = nullptr;
        mouse_buttons_down_ &= ~btn;
        break;
    }
    default:
        break;
    }
}
} // namespace veles
