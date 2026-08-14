#ifndef VELES_SAVE_FILE_DIALOG_HPP
#define VELES_SAVE_FILE_DIALOG_HPP

#include <QDialog>

class QWidget;
class QLabel;

namespace veles
{
class editor;

class unsaved_file_dialog : public QDialog
{
    Q_OBJECT

public:
    unsaved_file_dialog(const editor &_ed, QWidget *_parent = nullptr);

signals:
    void accepted(const QString *_path);

private:
    void save_btn_clicked(const editor &_ed);

    QLabel *const info_;
};
} // namespace veles

#endif // VELES_SAVE_FILE_DIALOG_HPP