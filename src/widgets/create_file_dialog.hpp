#ifndef VELES_CREATE_FILE_DIALOG_HPP
#define VELES_CREATE_FILE_DIALOG_HPP

#include <QDialog>

class QComboBox;
class QLineEdit;
class QPushButton;

namespace veles
{
class canvas;

class create_file_dialog : public QDialog
{
    Q_OBJECT

public:
    create_file_dialog(canvas *_canv);

private slots:
    void validate();

private:
    canvas *const canv_;
    QLineEdit *const file_name_;
    QLineEdit *const width_;
    QLineEdit *const height_;
    QComboBox *const palette_;

    QPushButton *const create_;
    QPushButton *const cancel_;
};
} // namespace veles

#endif // VELES_CREATE_FILE_DIALOG_HPP