#include "state.hpp"
#include "widgets/main_window.hpp"

#include <QApplication>
#include <QFile>
#include <QLocalServer>
#include <QLocalSocket>
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

    const auto *const server_name = "veles-server-instance";

    auto *sock = new QLocalSocket(&app);
    sock->connectToServer(server_name);
    if (sock->waitForConnected(1000))
    {
        QString s;
        for (const auto &arg : QApplication::arguments())
            if (!arg.endsWith("veles"))
                s.append(';').append(arg);
        const auto data = s.toUtf8();
        QDataStream writer{sock};
        writer << qint32(data.size());
        sock->write(data);
        sock->flush();
        sock->waitForBytesWritten(3000);
        sock->disconnectFromServer();
        return 0;
    }
    sock->deleteLater();

    auto *const server = new QLocalServer(&app);
    while (!server->listen(server_name))
    {
        if (server->serverError() != QAbstractSocket::AddressInUseError)
            return -1;
        QLocalServer::removeServer(server_name);
    }
    QObject::connect(server, &QLocalServer::newConnection,
        [&server, &win]
        {
            auto *const sock = server->nextPendingConnection();
            auto *const message = new QByteArray;
            QObject::connect(sock, &QLocalSocket::readyRead,
                [sock, &win, message]
                {
                    message->append(sock->readAll());
                    if (message->size() < 4)
                        return;
                    QDataStream stream(*message);
                    qint32 size = 0;
                    stream >> size;
                    if (message->size() < size + 4)
                        return;
                    for (const QString &item : QString::fromUtf8(message->mid(4, size)).split(';', Qt::SkipEmptyParts))
                        win.try_open_image(item);
                    win.activateWindow();
                });

            QObject::connect(sock, &QLocalSocket::disconnected,
                [sock, message]
                {
                    delete message;
                    sock->deleteLater();
                });
        });

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
