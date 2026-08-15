#include "resize_dialog.hpp"

#include "editor.hpp"
#include "image.hpp"
#include "moc_resize_dialog.cpp"

#include <QButtonGroup>
#include <QFormLayout>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QToolButton>

namespace veles
{
resize_dialog::resize_dialog(editor *_ed, QWidget *_parent)
    : QDialog(_parent),
      old_size_(_ed->get_image().size()),
      width_(new QSpinBox),
      height_(new QSpinBox),
      anchors_(new QButtonGroup(this)),
      confirm_(new QPushButton("Confirm"))
{
    setWindowTitle(QString("Resize %0").arg(_ed->name()));
    setModal(true);
    setAttribute(Qt::WA_DeleteOnClose);

    auto *const lyt = new QFormLayout;

    for (auto *edit : {width_, height_})
    {
        edit->setRange(1, 5000);
        connect(edit, &QSpinBox::textChanged, this, &resize_dialog::validate);
    }
    width_->setValue(old_size_.width());
    height_->setValue(old_size_.height());
    lyt->addRow("New width", width_);
    lyt->addRow("New height", height_);

    auto *const anchor_lyt = new QGridLayout;
    anchor_lyt->setSpacing(2);
    for (int i = 0; i < 9; ++i)
    {
        static const QList icons = {"🢄", "🢁", "🢅", "🢀", "+", "🢂", "🢇", "🢃", "🢆"};
        auto *const btn = new QToolButton;
        btn->setText(icons[i]);
        btn->setFont(QFont("monospace"));
        btn->setToolButtonStyle(Qt::ToolButtonTextOnly);
        btn->setFixedSize(32, 32);
        btn->setCheckable(true);
        btn->setChecked(i == 4);
        anchors_->addButton(btn, i);
        anchor_lyt->addWidget(btn, i / 3, i % 3);
    }
    lyt->addRow("Anchor", anchor_lyt);

    auto *warning_label = new QLabel;
    warning_label->setPixmap(QIcon::fromTheme(QIcon::ThemeIcon::DialogInformation).pixmap({32, 32}));
    lyt->addRow(warning_label, new QLabel("If the new size has a smaller dimension,\n"
                                          "contents will be cropped"));

    connect(confirm_, &QPushButton::clicked, this, &resize_dialog::accept);
    connect(this, &QDialog::accepted,
        [this, _ed]
        {
            auto new_size = QSize(width_->value(), height_->value());
            const auto i = anchors_->checkedId();
            _ed->resize_image(new_size, {static_cast<resize_anchor>(i % 3), static_cast<resize_anchor>(i / 3)});
        });
    auto *cancel = new QPushButton("Cancel");
    connect(cancel, &QPushButton::clicked, this, &resize_dialog::reject);
    lyt->addRow(confirm_, cancel);

    setLayout(lyt);
    validate();
}

void resize_dialog::validate()
{
    const auto new_size = QSize(width_->value(), height_->value());
    QString tooltip;
    if (!width_->hasAcceptableInput())
        tooltip = "Invalid width";
    else if (!height_->hasAcceptableInput())
        tooltip = "Invalid height";
    else if (old_size_ == new_size)
        tooltip = "Choose a size different from the current";
    confirm_->setEnabled(tooltip.isEmpty());
    confirm_->setToolTip(tooltip);
}
} // namespace veles