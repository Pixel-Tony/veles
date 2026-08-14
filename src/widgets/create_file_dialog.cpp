#include "create_file_dialog.hpp"

#include "canvas.hpp"
#include "moc_create_file_dialog.cpp"

#include <QComboBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QPushButton>

namespace
{
const auto dim_min = 1;
const auto dim_max = 5000;
const auto file_name_regex = QRegularExpression{R"([^<>:'"/\\|?*]+)"};
const auto palette_options = QStringList{"None (32bpp)", "DOS", "Windows (Legacy)"};
} // namespace

namespace veles
{
create_file_dialog::create_file_dialog(canvas *_canv)
    : QDialog(_canv),
      canv_(_canv),
      file_name_(new QLineEdit("Unnamed.png")),
      width_(new QLineEdit("100")),
      height_(new QLineEdit("100")),
      palette_(new QComboBox),
      create_(new QPushButton("Create")),
      cancel_(new QPushButton("Cancel"))
{
    setWindowTitle("Create File");
    setAttribute(Qt::WA_DeleteOnClose);
    setModal(true);

    for (auto *input : QList<QWidget *>{file_name_, width_, height_, palette_})
        input->setFixedWidth(240);

    file_name_->setValidator(new QRegularExpressionValidator(file_name_regex, file_name_));
    width_->setValidator(new QIntValidator(dim_min, dim_max, width_));
    height_->setValidator(new QIntValidator(dim_min, dim_max, height_));
    palette_->addItems(palette_options);
    palette_->setCurrentIndex(1);

    auto *table = new QFormLayout;
    table->addRow("File Name", file_name_);
    table->addRow("Width", width_);
    table->addRow("Height", height_);
    table->addRow("Palette", palette_);
    table->addRow(create_, cancel_);
    setLayout(table);

    for (auto *edit : {file_name_, width_, height_})
        connect(edit, &QLineEdit::textChanged, this, &create_file_dialog::validate);
    connect(create_, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancel_, &QPushButton::clicked, this, &QDialog::reject);
    connect(this, &QDialog::accepted,
        [this]
        {
            canv_->create_image(file_name_->text(), QSize{width_->text().toInt(), height_->text().toInt()},
                static_cast<palette::type>(palette_->currentIndex()));
        });
    validate();
}

void create_file_dialog::validate()
{
    QString tooltip;
    if (!file_name_->hasAcceptableInput())
        tooltip = "Invalid file name";
    else if (!width_->hasAcceptableInput())
        tooltip = "Invalid width";
    else if (!height_->hasAcceptableInput())
        tooltip = "Invalid height";

    create_->setEnabled(tooltip.isEmpty());
    create_->setToolTip(tooltip);
}
} // namespace veles
