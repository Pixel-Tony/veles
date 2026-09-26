#include "convert_palette_dialog.hpp"

#include "editor.hpp"
#include "image.hpp"
#include "palette.hpp"

#include <QComboBox>
#include <QFormLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>

namespace veles
{
convert_palette_dialog::convert_palette_dialog(QWidget *_parent, editor *_ed)
    : QDialog(_parent),
      ed_(_ed),
      combobox_(new QComboBox),
      convert_(new QPushButton("Convert"))
{
    setWindowTitle("Convert image palette");
    setMinimumWidth(320);
    setAttribute(Qt::WA_DeleteOnClose);
    setModal(true);

    const auto *palette_ = _ed->get_image().get_palette();
    initial_index_ = static_cast<int>(palette_ ? palette_->type_ : palette::none);
    combobox_->insertItems(0, {"None (32bpp)", "DOS", "Windows (Legacy)"});
    combobox_->setCurrentIndex(initial_index_);
    auto *close = new QPushButton("Cancel");
    auto *lyt = new QFormLayout;
    lyt->addRow(new QLabel(tr("Converting image \"%0\" to a new palette.").arg(_ed->name())));
    lyt->addRow("New palette:", combobox_);
    lyt->addRow(convert_, close);
    setLayout(lyt);
    connect(combobox_, &QComboBox::currentIndexChanged, this, &convert_palette_dialog::validate);
    connect(convert_, &QPushButton::clicked, this, &QDialog::accept);
    connect(close, &QPushButton::clicked, this, &QDialog::close);
    connect(this, &QDialog::accepted, this, &convert_palette_dialog::convert);
    validate();
}

void convert_palette_dialog::validate()
{
    const bool different = combobox_->currentIndex() != initial_index_;
    convert_->setEnabled(different);
    convert_->setToolTip(different ? "" : "Choose a different palette to convert to");
}

void convert_palette_dialog::convert()
{
    const auto new_type = static_cast<palette::type>(combobox_->currentIndex());
    if (ed_->convert_palette(new_type))
    {
        close();
        return;
    }
    auto *const msg = new QMessageBox(QMessageBox::Critical, "Error",
        "Cannot convert image;\n"
        "one or more pixels have colors not on the new palette.",
        QMessageBox::Ok);
    msg->setAttribute(Qt::WA_DeleteOnClose);
    msg->show();
}

} // namespace veles
