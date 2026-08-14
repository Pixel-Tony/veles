#include "widgets/main_window.hpp"

#include <QApplication>

namespace
{
using namespace veles;

int veles_main(int _argc, char **_argv)
{
    QApplication::setOrganizationName("Tony Pixel");
    QApplication::setApplicationName("Veles");
    QApplication::setDesktopFileName("Veles");
    QApplication::setApplicationDisplayName("Veles");
    QApplication app{_argc, _argv};
    main_window win;

    win.showMaximized();

    return QApplication::exec();
}
} // namespace

int main(int argc, char **argv) { return veles_main(argc, argv); }