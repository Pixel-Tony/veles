#include "state.hpp"
#include "widgets/main_window.hpp"

#include <QApplication>
#include <QFile>

#ifdef QT_DEBUG
#include <QFileSystemWatcher>
#include <QString>
#else
#include <QResource>
#endif

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

#ifdef QT_DEBUG
    {
        const auto *const path = "../assets/style.qss";
        auto *const watcher = new QFileSystemWatcher({path}, &win);
        const auto cb = [&win, &path]
        {
            qInfo("Reloading style file...");
            QFile style{path};
            if (style.open(QFile::ReadOnly))
                win.setStyleSheet(style.readAll());
            else
                qInfo("Couldn't open style file");
        };
        QObject::connect(watcher, &QFileSystemWatcher::fileChanged, cb);
        cb();
    }
#else
    win.setStyleSheet(QString{QResource{":/assets/style.qss"}.uncompressedData()});
#endif

    auto *const state_ = state::instance();
    state_->set_primary(Qt::white);
    state_->set_secondary(Qt::blue);

    for (const auto &arg : QApplication::arguments())
        if (!arg.endsWith("veles"))
            win.try_open_image(arg);
    win.showMaximized();

    return QApplication::exec();
}
} // namespace

int main(int argc, char **argv) { return veles_main(argc, argv); }
