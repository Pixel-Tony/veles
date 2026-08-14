#include "tool.hpp"

#include "color_picker.hpp"
#include "pen.hpp"
#include "view.hpp"
#include "widgets/editor.hpp"

#include <QApplication>
#include <QEvent>
#include <QGraphicsSceneWheelEvent>
#include <QWheelEvent>
#include <stdexcept>

namespace veles
{
tool *tool::current = nullptr;

tool::tool(tool::type _type) : type_(_type) {}

tool *tool::make(tool::type _type)
{
    switch (_type)
    {
    case type::view:
        return new view();
    case type::pen:
        return new pen();
    case type::color_picker:
        return new color_picker();
    default:
        throw std::runtime_error{"Unknown tool type"};
    }
}

void tool::editor_event(QEvent &, editor &) {}

void tool::enter_editor(editor &) {}

void tool::exit_editor(editor &) {}

void tool::handle_zoom(QEvent &_ev, editor &_ed, bool _when)
{
    _ev.accept();
    if (_when)
    {
        const auto dy = _ev.type() == QEvent::Wheel ? static_cast<QWheelEvent *>(&_ev)->angleDelta().y()
                                                    : static_cast<QGraphicsSceneWheelEvent *>(&_ev)->pixelDelta().y();
        const auto sign = 2 * (dy > 0) - 1;
        _ed.apply_zoom_delta(sign * (qApp->keyboardModifiers().testFlag(Qt::ControlModifier) + 1));
    }
}
} // namespace veles