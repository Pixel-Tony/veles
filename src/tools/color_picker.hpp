#ifndef VELES_COLOR_PICKER_HPP
#define VELES_COLOR_PICKER_HPP

#include "tool.hpp"

#include <Qt>

namespace veles
{
struct color_picker : tool
{
    color_picker();

    void editor_event(QEvent &_ev, editor &_ed) override;

private:
    Qt::MouseButtons mouse_buttons_down_;
};
} // namespace veles

#endif // VELES_COLOR_PICKER_HPP