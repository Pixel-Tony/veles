#ifndef CONVERT_PALETTE_DIALOG_HPP
#define CONVERT_PALETTE_DIALOG_HPP

#include "editor.hpp"

#include <QComboBox>
#include <QDialog>
#include <QPushButton>

namespace veles
{
class editor;

struct convert_palette_dialog : QDialog
{
    Q_OBJECT

public:
    convert_palette_dialog(QWidget *_parent, editor *_ed);

private:
    void validate();
    void convert();

    int initial_index_;
    editor *const ed_;
    QComboBox *const combobox_;
    QPushButton *const convert_;
};
} // namespace veles

#endif // CONVERT_PALETTE_DIALOG_HPP