#ifndef VELES_CANVAS_HPP
#define VELES_CANVAS_HPP

#include "editor.hpp"
#include "palette.hpp"

#include <QTabWidget>

class QString;

namespace veles
{
class canvas : public QTabWidget
{
    Q_OBJECT

public:
    explicit canvas(QWidget *_parent = nullptr);

signals:
    void changed(editor *_editor);

public slots:
    void create_new_file();
    void create_image(const QString &_name, QSize _size, palette::type _palette);
    void open_image(const QString &_filepath);
    void close_image();
    void exit();

private:
    void add_editor(editor *_ed);
    [[nodiscard]] editor *current_editor() const noexcept;
    void close_editor(editor &_ed);
    void show_error(const QString &_text);
};
} // namespace veles

#endif // VELES_CANVAS_HPP