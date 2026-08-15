#ifndef VELES_WIDGETS_RESIZE_DIALOG_HPP
#define VELES_WIDGETS_RESIZE_DIALOG_HPP

#include <QDialog>

class QSpinBox;
class QButtonGroup;

namespace veles
{
class editor;

enum class resize_anchor : uint8_t
{
    start,
    center,
    end
};

class resize_dialog : public QDialog
{
    Q_OBJECT

public:
    resize_dialog(editor *_ed, QWidget *_parent = nullptr);

private:
    void validate();

    const QSize old_size_;
    QSpinBox *width_;
    QSpinBox *height_;
    QButtonGroup *anchors_;
    QPushButton *confirm_;
};
} // namespace veles

#endif // VELES_WIDGETS_RESIZE_DIALOG_HPP