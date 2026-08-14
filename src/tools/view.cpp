#include "view.hpp"

#include "tools/tool.hpp"
#include "widgets/editor.hpp"

#include <QEvent>

namespace veles
{
view::view() : tool(type::view) {}

void view::enter_editor(editor &_ed) { _ed.setDragMode(QGraphicsView::ScrollHandDrag); }

void view::exit_editor(editor &_ed) { _ed.setDragMode(QGraphicsView::NoDrag); }

void view::editor_event(QEvent &_ev, editor &_ed)
{
    _ev.ignore(); // leave for QGraphicsView to handle
    switch (_ev.type())
    {
    case QEvent::GraphicsSceneMousePress:
        is_dragging_ = true;
        break;
    case QEvent::GraphicsSceneMouseRelease:
        is_dragging_ = false;
        break;
    case QEvent::KeyPress:
    case QEvent::KeyRelease:
    case QEvent::ShortcutOverride:
        _ev.setAccepted(is_dragging_);
        break;
    case QEvent::Wheel:
    case QEvent::GraphicsSceneWheel:
        handle_zoom(_ev, _ed, !is_dragging_);
        break;
    default:
        break;
    }
}
} // namespace veles