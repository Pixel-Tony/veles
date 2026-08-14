#include "main_window.hpp"

#include "moc_main_window.cpp"

#include <QIcon>

namespace veles
{
main_window::main_window()
{
    setWindowTitle("Veles");
    setMinimumSize(800, 600);
    setWindowIcon(QIcon{":/assets/logo-128.png"});
}

void main_window::closeEvent(QCloseEvent *_ev)
{
    _ev->accept();
}
} // namespace veles