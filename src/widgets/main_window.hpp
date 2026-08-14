#ifndef VELES_MAIN_WINDOW_HPP
#define VELES_MAIN_WINDOW_HPP

#include "editor.hpp"

#include <QAction>
#include <QCloseEvent>
#include <QLabel>
#include <QMainWindow>
#include <QOpenGLWidget>

class QString;

namespace veles
{
class canvas;
class palette_tab;
class palette_action;

class main_window : public QMainWindow
{
    Q_OBJECT

public:
    main_window();

    void try_open_image(const QString &_filepath);

protected:
    void closeEvent(QCloseEvent *_ev) override;

private slots:
    void on_active_editor_changed(const editor *_editor);

private:
    canvas *canv_;
    QLabel *status_bar_;
    palette_tab *palette_tab_;

    QAction *close_;

    palette_action *palette_action_;

    // A workaround to avoid initializing context on editor creation
    QOpenGLWidget *init_opengl_context_early_ = new QOpenGLWidget(this);
    const editor *old_editor_;
};
} // namespace veles

#endif // VELES_MAIN_WINDOW_HPP
