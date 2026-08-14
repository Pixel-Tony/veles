#include "color_picker.hpp"

#include "state.hpp"
#include "widgets/editor.hpp"
#include "widgets/image.hpp"

#include <QEvent>
#include <QGraphicsSceneMouseEvent>

namespace veles
{
color_picker::color_picker() : tool(type::color_picker) {}

void color_picker::editor_event(QEvent &_ev, editor &_ed)
{
    _ev.ignore();
    auto *mev = static_cast<QGraphicsSceneMouseEvent *>(&_ev);
    switch (_ev.type())
    {
    case QEvent::GraphicsSceneMousePress:
    {
        _ev.accept();
        auto &img = _ed.get_image();
        const auto pos = img.mapFromScene(mev->scenePos());
        const auto btn = mev->button();
        if (img.contains(pos) && !mouse_buttons_down_ && (btn == Qt::LeftButton || btn == Qt::RightButton))
            state::instance()->set_color(img.pixelColor(pos.x(), pos.y()), btn == Qt::LeftButton);
        mouse_buttons_down_ |= btn;
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