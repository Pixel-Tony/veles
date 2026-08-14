#ifndef VELES_VIEW_HPP
#define VELES_VIEW_HPP

#include "tool.hpp"

namespace veles
{
struct view : tool
{
    view();

    void editor_event(QEvent &_ev, editor &) override;

    void enter_editor(editor &_ed) override;
    void exit_editor(editor &_ed) override;

private:
    bool is_dragging_ = false;
};
} // namespace veles

#endif // VELES_VIEW_HPP