#ifndef VELES_SELECT_HPP
#define VELES_SELECT_HPP

#include "tool.hpp"

#include <QPoint>
#include <QRect>
#include <Qt>
#include <optional>
#include <variant>

namespace veles
{
class image;
class selection;

struct select : tool
{
private:
    using action = std::variant<std::nullopt_t, QPoint, QPointF, QPair<QRect, QRect>>;

public:
    select();

protected:
    void editor_event(QEvent &_ev, editor &_ed) override;

private:
    selection *sel_ = nullptr;
    action action_;
    Qt::MouseButtons mouse_buttons_down_;
};
} // namespace veles

#endif // VELES_SELECT_HPP
