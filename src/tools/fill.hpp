#ifndef VELES_FILL_HPP
#define VELES_FILL_HPP

#include "tool.hpp"

#include <Qt>

namespace veles
{
class image;

struct fill : tool
{
    fill();

    void editor_event(QEvent &_ev, editor &_ed) override;

private:
    Qt::MouseButtons mouse_buttons_down_;
};

} // namespace veles

#endif // VELES_FILL_HPP