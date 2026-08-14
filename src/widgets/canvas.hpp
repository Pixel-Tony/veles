#ifndef VELES_CANVAS_HPP
#define VELES_CANVAS_HPP

#include "editor.hpp"
#include "palette.hpp"
#include "tools/tool.hpp"

#include <QTabWidget>

class QString;

#define X_VELES_CANVAS_USED_EDITOR_MEMBERS \
    X(zoom_in, ED->apply_zoom_delta(1))    \
    X(zoom_out, ED->apply_zoom_delta(-1))  \
    X(default_zoom, ED->default_zoom())

namespace veles
{
class canvas : public QTabWidget
{
    Q_OBJECT

public:
    explicit canvas(QWidget *_parent = nullptr);

    ~canvas() override;

signals:
    void changed(editor *_editor);

public slots:
    void create_new_file();
    void create_image(const QString &_name, QSize _size, palette::type _palette);
    void open_image(const QString &_filepath);
    void close_image();
    void exit();
    void choose_tool(tool::type _type);

#define X(FUNC, IGNORED) void FUNC() const;
    X_VELES_CANVAS_USED_EDITOR_MEMBERS
#undef X

private:
    void add_editor(editor *_ed);
    [[nodiscard]] editor *current_editor() const noexcept;
    void close_editor(editor &_ed);
    void show_error(const QString &_text);

    editor *last_editor_ = nullptr;
};
} // namespace veles

#endif // VELES_CANVAS_HPP