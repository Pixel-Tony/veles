#include "unsaved_file_dialog.hpp"

#include "dialogs.hpp"
#include "editor.hpp"
#include "moc_unsaved_file_dialog.cpp"

#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QStandardPaths>

namespace veles
{
unsaved_file_dialog::unsaved_file_dialog(const editor &_ed, QWidget *_parent)
    : QDialog(_parent),
      info_(new QLabel(tr(R"(Image "%0" is unsaved. Save?)").arg(_ed.name())))
{
    setWindowTitle("Unsaved file");
    setAttribute(Qt::WA_DeleteOnClose);
    setModal(true);

    auto *save = new QPushButton("Save");
    auto *close = new QPushButton("Close");
    auto *cancel = new QPushButton("Cancel");

    connect(save, &QPushButton::clicked, [this, &_ed] { save_btn_clicked(_ed); });
    connect(close, &QPushButton::clicked, [this] { emit accepted(nullptr); });
    connect(cancel, &QPushButton::clicked, this, &unsaved_file_dialog::reject);

    auto *btn_box = new QHBoxLayout;
    btn_box->addWidget(save);
    btn_box->addWidget(close);
    btn_box->addWidget(cancel);

    auto *lyt = new QVBoxLayout;
    lyt->addWidget(info_);
    lyt->addLayout(btn_box);
    setLayout(lyt);
}

void unsaved_file_dialog::save_btn_clicked(const editor &_ed)
{
    const auto &path = _ed.path();
    if (!path.isEmpty())
    {
        emit accepted(&path);
        return;
    }
    const auto dir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/" + _ed.name();
    auto *const fd = create_save_file_dialog(dir, this);
    connect(fd, &QFileDialog::accepted, [this, fd] { emit accepted(fd->selectedFiles().data()); });
    fd->open();
}
} // namespace veles