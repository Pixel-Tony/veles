#include "main_window.hpp"

#include "canvas.hpp"
#include "dialogs.hpp"
#include "editor.hpp"
#include "image.hpp"
#include "moc_main_window.cpp"
#include "palette_action.hpp"
#include "palette_tab.hpp"
#include "state.hpp"

#include <QFileDialog>
#include <QIcon>
#include <QMenuBar>
#include <QSettings>
#include <QStatusBar>
#include <QToolBar>

namespace
{
QString ellipsis(const QString &_input, const qsizetype _length)
{ return _input.length() <= _length ? _input : _input.sliced(0, _length).replace(_length - 3, 3, "..."); }
} // namespace

namespace veles
{
main_window::main_window()
    : canv_(new canvas),
      status_bar_(new QLabel),
      palette_tab_(new palette_tab(this)),
      palette_action_(new palette_action(this)),
      old_editor_()
{
    init_opengl_context_early_->hide();
    setWindowTitle("Veles");
    setMinimumSize(800, 600);
    setWindowIcon(QIcon{":/assets/logo-128.png"});

#define ICON(VAL) QIcon::fromTheme(QIcon::ThemeIcon::VAL)
#define KEY(VAL) QKeySequence::VAL

    auto *file_menu = menuBar()->addMenu("File");
    auto *new_file = file_menu->addAction(ICON(DocumentNew), "New...", KEY(New), canv_, &canvas::create_new_file);
    file_menu->addSeparator();
    auto *open = file_menu->addAction(
        ICON(DocumentOpen), "Open...", KEY(Open), [this] { open_file_dialog(canv_, false, this)->show(); });
    file_menu->addAction(ICON(DocumentOpen), "Import...", [this] { open_file_dialog(canv_, true, this)->show(); });
    file_menu->addSeparator();
    close_ = file_menu->addAction(ICON(WindowClose), "Close", KEY(Close), canv_, &canvas::close_image);
    file_menu->addSeparator();
    file_menu->addAction(ICON(ApplicationExit), "Exit", KEY(Quit), this, &main_window::close);

    auto *edit_menu = menuBar()->addMenu("Edit");
    edit_menu->addAction("Swap Pen Colors", QKeySequence{"X"}, state::instance(), &state::swap_colors);

#undef ICON
#undef KEY

    connect(palette_tab_, &palette_tab::color_selected, state::instance(), &state::set_color);
    connect(state::instance(), &state::color_updated, palette_tab_, &palette_tab::set_color);

    connect(palette_action_, &palette_action::toggled, palette_tab_, &palette_tab::setVisible);

    auto *tool_bar_ = new QToolBar("Tool Bar");
    tool_bar_->setMovable(false);
    tool_bar_->setIconSize({24, 24});
    tool_bar_->addActions({new_file, open});
    tool_bar_->addSeparator();
    tool_bar_->addAction(palette_action_);
    addToolBar(tool_bar_);

    setCentralWidget(canv_);

    addDockWidget(Qt::DockWidgetArea::RightDockWidgetArea, palette_tab_);

    connect(canv_, &canvas::changed, this, &main_window::on_active_editor_changed);
    statusBar()->addWidget(status_bar_);

    palette_tab_->setVisible(QSettings().value("paletteTab/visible", false).toBool());
    palette_tab_->set_palette(palette::none);

    on_active_editor_changed(nullptr);
}

void main_window::try_open_image(const QString &_filepath) { canv_->open_image(_filepath); }

void main_window::closeEvent(QCloseEvent *_ev)
{
    _ev->accept();
    canv_->exit();
    QSettings().setValue("paletteTab/visible", palette_tab_->isVisible());
}

void main_window::on_active_editor_changed(const editor *const _editor)
{
    const bool enabled = _editor;
    close_->setEnabled(enabled);
    QString status_bar_text;
    QString title_text = "Veles";
    if (enabled)
    {
        const auto &img = _editor->get_image();
        const auto [w, h] = img.size();
        const auto *const palette = img.get_palette();
        const auto name = ellipsis(_editor->name(), 50);
        status_bar_text = tr("%0 (%1) | %2x%3")
                              .arg(name, palette ? palette->name_ : "32bpp", QString::number(w), QString::number(h));
        title_text = tr("%0 - %1").arg(_editor->name(), title_text);
        if (old_editor_ != _editor)
            palette_tab_->set_palette(palette ? palette->type_ : palette::none);
    }
    setWindowTitle(title_text);
    status_bar_->setText(status_bar_text);
    old_editor_ = _editor;
}
} // namespace veles
