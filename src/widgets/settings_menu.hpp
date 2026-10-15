#ifndef SETTINGS_MENU_HPP
#define SETTINGS_MENU_HPP

#include "tools/tool.hpp"

#include <QDialog>
#include <QKeySequence>
#include <QKeySequenceEdit>

class QPushButton;
class QSpinBox;

namespace veles
{
class settings_menu : public QDialog
{
    Q_OBJECT

public:
    explicit settings_menu(QWidget *_parent);

    ~settings_menu() override;

signals:
    void updated();

private:
    void validate();

public:
    [[nodiscard]] int max_recent_files() const noexcept;
#define X(NAME, KEY) [[nodiscard]] QKeySequence shortcut_##KEY() const noexcept;
    X_TOOLS
#undef X

private:
    QPushButton *btn_reset_;

    QSpinBox *max_recent_files_edit_;
    int max_recent_files_;

#define X(NAME, KEY)                                            \
    QKeySequenceEdit *const KEY##_edit_ = new QKeySequenceEdit; \
    QKeySequence KEY##_;
    X_TOOLS
#undef X
};
} // namespace veles

#endif // SETTINGS_MENU_HPP
