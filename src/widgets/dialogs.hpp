#ifndef VELES_DIALOGS_HPP
#define VELES_DIALOGS_HPP

class QFileDialog;
class QString;
class QWidget;

namespace veles
{
class canvas;

QFileDialog *open_file_dialog(canvas *_canv, bool _allow_custom_ext, QWidget *_parent);

QFileDialog *create_save_file_dialog(const QString &_directory, QWidget *_parent);
} // namespace veles

#endif // VELES_DIALOGS_HPP