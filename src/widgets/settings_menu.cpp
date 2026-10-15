#include "settings_menu.hpp"

#include "state.hpp"

#include <QBoxLayout>
#include <QFormLayout>
#include <QKeySequenceEdit>
#include <QLabel>
#include <QPushButton>
#include <QSettings>
#include <QSpinBox>

#define SHORTCUT(KEY) "settings/shortcuts/tools/" #KEY

namespace veles
{
settings_menu::settings_menu(QWidget *_parent)
    : QDialog(_parent),
      max_recent_files_(state::instance()->recent_count()),
      max_recent_files_edit_(new QSpinBox)
#define X(NAME, KEY) , KEY##_(QSettings().value(SHORTCUT(KEY)).toString())
          X_TOOLS
#undef X
{
    setFocusPolicy(Qt::FocusPolicy::NoFocus);
    max_recent_files_edit_->setValue(QSettings().value("settings/maxRecentFiles", 0).toInt());

    auto *lyt_general = new QFormLayout;
    max_recent_files_edit_->setMinimum(2);
    max_recent_files_edit_->setMaximum(20);
    lyt_general->addRow("Max. recent files", max_recent_files_edit_);

    auto *lyt_shortcuts = new QFormLayout;
#define X(NAME, KEY) lyt_shortcuts->addRow(NAME, KEY##_edit_);
    X_TOOLS
#undef X

    auto *lyt_btns = new QHBoxLayout;
    auto *btn_ok = new QPushButton("Ok");
    btn_reset_ = new QPushButton("Reset");
    auto *btn_cancel = new QPushButton("Cancel");
    lyt_btns->addWidget(btn_ok);
    lyt_btns->addWidget(btn_reset_);
    lyt_btns->addWidget(btn_cancel);

    auto *lyt = new QVBoxLayout;
    lyt->addWidget(new QLabel("<b>General</b>"));
    lyt->addLayout(lyt_general);
    lyt->addWidget(new QLabel("<b>Shortcuts</b>"));
    lyt->addLayout(lyt_shortcuts);
    lyt->addLayout(lyt_btns);
    setLayout(lyt);

    connect(max_recent_files_edit_, &QSpinBox::valueChanged, this, &settings_menu::validate);
#define X(NAME, KEY) connect(KEY##_edit_, &QKeySequenceEdit::keySequenceChanged, this, &settings_menu::validate);
    X_TOOLS
#undef X
    connect(btn_cancel, &QPushButton::pressed, this, &QDialog::close);
    connect(btn_cancel, &QPushButton::pressed, btn_reset_, &QPushButton::click);
    connect(btn_reset_, &QPushButton::pressed,
        [this]
        {
            max_recent_files_edit_->setValue(max_recent_files_);
#define X(NAME, KEY) KEY##_edit_->setKeySequence(KEY##_);
            X_TOOLS
#undef X
        });
    connect(btn_ok, &QPushButton::pressed, this,
        [this]
        {
            max_recent_files_ = max_recent_files_edit_->value();
#define X(NAME, KEY) KEY##_ = KEY##_edit_->keySequence();
            X_TOOLS
#undef X
            emit updated();
            validate();
            close();
        });

    btn_reset_->click();
    validate();
}

void settings_menu::validate()
{
    bool is_edited = false;
    if (max_recent_files_edit_->value() != max_recent_files_)
        is_edited = true;
#define X(NAME, KEY)                          \
    if (KEY##_edit_->keySequence() != KEY##_) \
        is_edited = true;
    X_TOOLS
#undef X
    btn_reset_->setEnabled(is_edited);
}

settings_menu::~settings_menu()
{
    QSettings settings;
    settings.setValue("settings/maxRecentFiles", max_recent_files_);
#define X(NAME, KEY) settings.setValue(SHORTCUT(KEY), KEY##_edit_->keySequence());
    X_TOOLS
#undef X
}

[[nodiscard]] int settings_menu::max_recent_files() const noexcept { return max_recent_files_; }

#define X(NAME, KEY) \
    QKeySequence settings_menu::shortcut_##KEY() const noexcept { return KEY##_; }
X_TOOLS
#undef X
} // namespace veles
