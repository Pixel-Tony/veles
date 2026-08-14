#include "dialogs.hpp"

#include "canvas.hpp"

#include <QFileDialog>
#include <QSettings>

namespace veles
{
QFileDialog *open_file_dialog(canvas *_canv, bool _allow_custom_ext, QWidget *_parent)
{
    static const auto *const directory_key = "openFileDialog/dir";
    auto *const fd = new QFileDialog(
        _parent, "Choose file to open", {}, _allow_custom_ext ? "Image files (*.bmp)" : "Image files (*.png)");
    fd->setAcceptMode(QFileDialog::AcceptOpen);
    fd->setModal(true);
    fd->setDirectory(QSettings().value(directory_key, QString()).toString());
    QFileDialog::connect(fd, &QFileDialog::accepted,
        [fd, _canv]
        {
            QSettings().setValue(directory_key, fd->directory().absolutePath());
            _canv->open_image(fd->selectedFiles()[0]);
            fd->close();
        });
    return fd;
}
} // namespace veles