#include "main_window.hpp"

#include "moc_main_window.cpp"
#include "palette_action.hpp"
#include "palette_tab.hpp"
#include "state.hpp"

#include <QIcon>
#include <QMenuBar>
#include <QSettings>
#include <QToolBar>

namespace veles
{
main_window::main_window() : palette_tab_(new palette_tab(this)), palette_action_(new palette_action(this))
{
    setWindowTitle("Veles");
    setMinimumSize(800, 600);
    setWindowIcon(QIcon{":/assets/logo-128.png"});

    auto *edit_menu = menuBar()->addMenu("Edit");
    edit_menu->addAction("Swap Pen Colors", QKeySequence{"X"}, state::instance(), &state::swap_colors);

    connect(palette_tab_, &palette_tab::color_selected, state::instance(), &state::set_color);
    connect(state::instance(), &state::color_updated, palette_tab_, &palette_tab::set_color);

    connect(palette_action_, &palette_action::toggled, palette_tab_, &palette_tab::setVisible);

    auto *tool_bar_ = new QToolBar("Tool Bar");
    tool_bar_->setMovable(false);
    tool_bar_->setIconSize({24, 24});
    tool_bar_->addAction(palette_action_);
    addToolBar(tool_bar_);

    addDockWidget(Qt::DockWidgetArea::RightDockWidgetArea, palette_tab_);

    palette_tab_->setVisible(QSettings().value("paletteTab/visible", false).toBool());
    palette_tab_->set_palette(palette::none);
}

void main_window::closeEvent(QCloseEvent *_ev)
{
    _ev->accept();
    QSettings().setValue("paletteTab/visible", palette_tab_->isVisible());
}
} // namespace veles
