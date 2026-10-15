#include "main_window.hpp"

#include "canvas.hpp"
#include "dialogs.hpp"
#include "editor.hpp"
#include "image.hpp"
#include "moc_main_window.cpp"
#include "palette_action.hpp"
#include "palette_tab.hpp"
#include "settings_menu.hpp"
#include "state.hpp"
#include "tools/tool.hpp"

#include <QFileDialog>
#include <QIcon>
#include <QMenuBar>
#include <QSettings>
#include <QStatusBar>
#include <QToolBar>
#include <QUndoStack>

namespace
{
QString ellipsis(const QString &_input, const qsizetype _length)
{ return _input.length() <= _length ? _input : _input.sliced(0, _length).replace(_length - 3, 3, "..."); }

QAction *make_tool_action(QActionGroup *_group, const QString &_name, const QKeySequence &_shortcut,
    const QString &_atlas, veles::tool::type _type)
{
    QIcon icon;
    {
        const auto atlas = QPixmap{_atlas};
        const auto h = atlas.height();
        icon.addPixmap(atlas.copy(0, 0, h, h), QIcon::Normal, QIcon::Off);
        icon.addPixmap(atlas.copy(h, 0, h, h), QIcon::Normal, QIcon::On);
    }
    auto *const _target = _group->addAction(icon, _name);
    _target->setIconVisibleInMenu(false);
    _target->setShortcut(_shortcut);
    _target->setCheckable(true);
    _target->setData(QVariant::fromValue(_type));
    return _target;
}
} // namespace

namespace veles
{
main_window::main_window()
    : canv_(new canvas),
      status_bar_(new QLabel),
      palette_tab_(new palette_tab(this)),
      settings_menu_(new settings_menu(this)),
      palette_action_(new palette_action(this)),
      tool_group_(new QActionGroup(this)),
#define X(NAME, KEY) KEY##_(nullptr),
      X_TOOLS
#undef X
      old_editor_()
{
    init_opengl_context_early_->hide();
    setWindowTitle("Veles");
    setMinimumSize(800, 600);
    setWindowIcon(QIcon{":/assets/logo-128.png"});

#define ICON(VAL) QIcon::fromTheme(QIcon::ThemeIcon::VAL)
#define KEY(VAL) QKeySequence::VAL

#define X(NAME, KEY) KEY##_->setShortcut(settings_menu_->shortcut_##KEY());
    connect(settings_menu_, &settings_menu::updated, [this] {
        state::instance()->set_recent_count(settings_menu_->max_recent_files());
         X_TOOLS
        });
#undef X

    auto *file_menu = menuBar()->addMenu("File");
    auto *new_file = file_menu->addAction(ICON(DocumentNew), "New...", KEY(New), canv_, &canvas::create_new_file);
    file_menu->addSeparator();
    auto *open = file_menu->addAction(
        ICON(DocumentOpen), "Open...", KEY(Open), [this] { open_file_dialog(canv_, false, this)->show(); });
    auto *open_recent = file_menu->addMenu(ICON(DocumentOpenRecent), "Open recent");
    connect(state::instance(), &state::recent_files_changed,
        [open_recent]
        {
            open_recent->clear();
            auto recent = state::instance()->recent_files();
            if (recent.isEmpty())
            {
                open_recent->addAction("No recent files yet")->setEnabled(false);
                return;
            }
            for (const auto &path : recent)
            {
                auto *const act = open_recent->addAction(QFileInfo{path}.fileName());
                act->setToolTip(path);
                act->setData(path);
            }
            open_recent->addSeparator();
            open_recent->addAction(ICON(ListRemove), "Clear recent");
        });
    connect(open_recent, &QMenu::triggered,
        [this](QAction *_action)
        {
            if (auto data = _action->data().toString(); !data.isEmpty())
                canv_->open_image(_action->data().toString());
            else
                state::instance()->clear_recent_files();
        });

    file_menu->addAction(ICON(DocumentOpen), "Import...", [this] { open_file_dialog(canv_, true, this)->show(); });
    file_menu->addSeparator();
    save_ = file_menu->addAction(ICON(DocumentSave), "Save", KEY(Save), canv_, &canvas::save);
    save_as_ = file_menu->addAction(ICON(DocumentSaveAs), "Save As...", KEY(SaveAs), [this] { canv_->save_as(); });
    file_menu->addSeparator();
    close_ = file_menu->addAction(ICON(WindowClose), "Close", KEY(Close), canv_, &canvas::close_image);
    file_menu->addSeparator();
    file_menu->addAction(ICON(DocumentProperties), "Settings...", settings_menu_, &settings_menu::open);
    file_menu->addSeparator();
    file_menu->addAction(ICON(ApplicationExit), "Exit", KEY(Quit), this, &main_window::close);

    auto *edit_menu = menuBar()->addMenu("Edit");
    undo_ = edit_menu->addAction(ICON(EditUndo), "Undo", KEY(Undo));
    redo_ = edit_menu->addAction(ICON(EditRedo), "Redo", KEY(Redo));
    edit_menu->addSeparator();
    edit_menu->addAction("Swap Pen Colors", QKeySequence{"X"}, state::instance(), &state::swap_colors);
    edit_menu->addSeparator();
    select_all_ = edit_menu->addAction(ICON(EditSelectAll), "Select All", KEY(SelectAll));
    copy_selection_ = edit_menu->addAction(ICON(EditCopy), "Copy", KEY(Copy));
    paste_selection_ = edit_menu->addAction(ICON(EditPaste), "Paste", KEY(Paste));

    auto *image_menu = menuBar()->addMenu("Image");
    hflip_ = image_menu->addAction("Flip horizontally");
    vflip_ = image_menu->addAction("Flip vertically");
    image_menu->addSeparator();
    rotate_cw_ = image_menu->addAction(ICON(ObjectRotateRight), "Rotate clockwise");
    rotate_ccw_ = image_menu->addAction(ICON(ObjectRotateLeft), "Rotate counter-clockwise");
    image_menu->addSeparator();
    resize_ = image_menu->addAction("Resize...", canv_, &canvas::resize_image);
    image_menu->addSeparator();
    crop_to_selection_ = image_menu->addAction("Crop to selection");
    image_menu->addSeparator();
    convert_palette_ = image_menu->addAction("Convert palette...", canv_, &canvas::convert_palette);

    auto *view_menu = menuBar()->addMenu("View");
    zoom_in_ = view_menu->addAction(ICON(ZoomIn), "Zoom In", KEY(ZoomIn));
    zoom_out_ = view_menu->addAction(ICON(ZoomOut), "Zoom Out", KEY(ZoomOut));
    view_menu->addSeparator();
    default_zoom_ = view_menu->addAction("Default Zoom");

#undef ICON
#undef KEY

#define X(FILENAME, IGNORED) connect(FILENAME##_, &QAction::triggered, canv_, &canvas::FILENAME);
    X_VELES_CANVAS_USED_EDITOR_MEMBERS
#undef X

#define X(NAME, KEY)           \
    KEY##_ = make_tool_action( \
        tool_group_, NAME, settings_menu_->shortcut_##KEY(), ":/assets/icons/" #KEY ".png", tool::type::KEY);
    X_TOOLS
#undef X

    connect(tool_group_, &QActionGroup::triggered,
        [this](QAction *_act) { canv_->choose_tool(qvariant_cast<tool::type>(_act->data())); });
    view_->trigger();

    connect(palette_tab_, &palette_tab::color_selected, state::instance(), &state::set_color);
    connect(state::instance(), &state::color_updated, palette_tab_, &palette_tab::set_color);

    connect(palette_action_, &palette_action::toggled, palette_tab_, &palette_tab::setVisible);

    menuBar()->addMenu("Tool")->addActions(tool_group_->actions());

    auto *tool_bar_ = new QToolBar("Tool Bar");
    tool_bar_->setMovable(false);
    tool_bar_->setIconSize(view_->icon().availableSizes()[0]);
    tool_bar_->addActions({undo_, redo_});
    tool_bar_->addSeparator();
    tool_bar_->addActions({new_file, open, save_, save_as_});
    tool_bar_->addSeparator();
    tool_bar_->addAction(select_all_);
    tool_bar_->addAction(copy_selection_);
    tool_bar_->addSeparator();
    tool_bar_->addActions(tool_group_->actions());
    tool_bar_->addSeparator();
    tool_bar_->addAction(palette_action_);
    addToolBar(tool_bar_);

    setCentralWidget(canv_);

    addDockWidget(Qt::DockWidgetArea::RightDockWidgetArea, palette_tab_);

    connect(canv_, &canvas::changed, this, &main_window::on_active_editor_changed);
    connect(canv_, &canvas::exit_prepared, this, &main_window::close);
    statusBar()->addWidget(status_bar_);

    palette_tab_->setVisible(QSettings().value("paletteTab/visible", false).toBool());
    palette_tab_->set_palette(palette::none);

    on_active_editor_changed(nullptr);
}

void main_window::try_open_image(const QString &_filepath) { canv_->open_image(_filepath); }

void main_window::closeEvent(QCloseEvent *_ev)
{
    bool will_exit = canv_->try_exit();
    if (will_exit)
        QSettings().setValue("paletteTab/visible", palette_tab_->isVisible());
    _ev->setAccepted(will_exit);
}

void main_window::on_active_editor_changed(const editor *const _editor)
{
    const bool enabled = _editor;
    for (auto *a : {save_as_, close_, hflip_, vflip_, rotate_cw_, rotate_ccw_, resize_, convert_palette_})
        a->setEnabled(enabled);
    save_->setEnabled(enabled && (!_editor->exists() || !_editor->history()->isClean()));
    zoom_out_->setEnabled(enabled && _editor->zoom() != editor::zoom_bounds.first);
    zoom_in_->setEnabled(enabled && _editor->zoom() != editor::zoom_bounds.second);
    default_zoom_->setEnabled(enabled && _editor->zoom() != editor::zoom_bounds.first);
    undo_->setEnabled(enabled && _editor->history()->canUndo());
    redo_->setEnabled(enabled && _editor->history()->canRedo());
    select_all_->setEnabled(enabled && tool::current->type_ == tool::type::select);
    for (auto *a : {copy_selection_, crop_to_selection_})
        a->setEnabled(enabled && _editor->get_selection());

    QString status_bar_text;
    QString title_text = "Veles";
    if (enabled)
    {
        const auto &img = _editor->get_image();
        const auto [w, h] = img.size();
        const char dirty_asterisk = "* "[_editor->history()->isClean()];
        const auto *const palette = img.get_palette();
        const auto name = ellipsis(_editor->name(), 50);
        status_bar_text = tr(" %0%1 (%2) | %3x%4 | %5% |")
                              .arg(name, dirty_asterisk, palette ? palette->name_ : "32bpp", QString::number(w),
                                  QString::number(h), QString::number(static_cast<long>(_editor->zoom() * 100)));
        if (selection *sel = nullptr; _editor && (sel = _editor->get_selection()))
        {
            auto [w, h] = sel->rect().size();
            status_bar_text.append(tr(" %0x%1").arg(QString::number(w), QString::number(h)));
        }
        title_text = tr("%0%1 - %2").arg(_editor->name(), dirty_asterisk, title_text);
        if (old_editor_ != _editor)
            palette_tab_->set_palette(palette ? palette->type_ : palette::none);
    }
    setWindowTitle(title_text);
    status_bar_->setText(status_bar_text);
    old_editor_ = _editor;
}
} // namespace veles
