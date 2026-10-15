#ifndef VELES_TOOL_HPP
#define VELES_TOOL_HPP

class QEvent;

#define X_TOOLS                     \
    X("View", view)                 \
    X("Pen", pen)                   \
    X("Color Picker", color_picker) \
    X("Flood Fill", fill)           \
    X("Select", select)

namespace veles
{
class editor;

struct tool
{
    enum class type
    {
#define X(NAME, KEY) KEY,
        X_TOOLS
#undef X
    };

    static tool *current;

protected:
    tool(type _type);

public:
    virtual ~tool() = default;

    static tool *make(tool::type _type);

    virtual void editor_event(QEvent &_ev, editor &_ed);
    virtual void enter_editor(editor &_ed);
    virtual void exit_editor(editor &_ed);

protected:
    void handle_zoom(QEvent &_ev, editor &_ed, bool _when);

public:
    const type type_;
};
} // namespace veles

#endif // VELES_TOOL_HPP
