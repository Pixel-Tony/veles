#include "palette_tab.hpp"

#include "moc_palette_tab.cpp"

#include <QPaintEvent>
#include <QPainter>
#include <QVBoxLayout>

namespace
{
const QSize cell_size = {20, 28};
const auto gap = 1;
const auto wg = cell_size.width() + gap;
const auto hg = cell_size.height() + gap;

QPixmap draw_palette_sprite(const veles::palette *const _palette)
{
    QPixmap result = {gap + 16 * wg, gap + 16 * hg};
    result.fill(qRgb(40, 40, 40));
    QPainter painter{&result};
    for (int y = 0; y < 16; ++y)
        for (int x = 0; x < 16; ++x)
            painter.fillRect(QRect{{gap + wg * x, gap + hg * y}, cell_size}, _palette->get(x, y));
    return result;
}
} // namespace

namespace veles
{
namespace priv
{
plt_color_frame::plt_color_frame(QWidget *_parent) : QFrame(_parent), color_box_(new QFrame), color_id_(new QLabel)
{
    color_box_->setFrameShape(Shape::Panel);
    color_box_->setLineWidth(2);
    color_box_->setFrameShadow(QFrame::Raised);
    color_box_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    color_box_->setFixedHeight(24);
    auto *lyt = new QVBoxLayout;
    lyt->setContentsMargins({});
    lyt->setSpacing(8);
    lyt->addWidget(color_box_);
    lyt->addWidget(color_id_);
    setLayout(lyt);
}

void plt_color_frame::set_color(QColor _new)
{
    color_box_->setStyleSheet(tr("background: %0").arg(_new.name()));
    color_id_->setText(_new == Qt::transparent ? " * * * " : _new.name());
}

plt_sprite::plt_sprite(const class palette *_palette, QWidget *_parent)
    : QWidget(_parent),
      palette_(_palette),
      sprite_(draw_palette_sprite(palette_))
{ setMouseTracking(true); }

void plt_sprite::set_color(QColor _color, bool _primary)
{
    (_primary ? primary_ : secondary_) = _color;
    update();
}

void plt_sprite::paintEvent(QPaintEvent *_ev)
{
    QPainter painter{this};
    painter.drawPixmap(_ev->rect(), sprite_, _ev->rect());
    for (auto [color, outer] : {QPair{primary_, Qt::white}, {secondary_, Qt::white}, {hovered_color_, Qt::green}})
    {
        const auto ind = palette_->index(color);
        if (ind == -1)
            continue;
        const auto top_left = QPoint{wg * int(ind % 16), hg * int(ind / 16)};
        painter.setPen(outer);
        painter.drawRect(QRect{top_left + QPoint(1, 1), cell_size - QSize{1, 1}});
        painter.setPen(Qt::black);
        painter.drawRect(QRect{top_left + QPoint(2, 2), cell_size - QSize{3, 3}});
    }
}

QSize plt_sprite::sizeHint() const { return sprite_.size(); }

void plt_sprite::enterEvent(QEnterEvent *_ev) { update_hovered(get_hovered_color(_ev->position())); }

void plt_sprite::leaveEvent(QEvent *) { update_hovered(Qt::transparent); }

void plt_sprite::mouseReleaseEvent(QMouseEvent *_ev)
{
    const auto color = get_hovered_color(_ev->position());
    if (color == Qt::transparent || pressed_color_ != color || !(_ev->button() & (Qt::LeftButton | Qt::RightButton)))
        return;
    emit color_selected(color, _ev->button() == Qt::LeftButton);
}

void plt_sprite::mouseMoveEvent(QMouseEvent *_ev) { update_hovered(get_hovered_color(_ev->position())); }

void plt_sprite::mousePressEvent(QMouseEvent *_ev) { pressed_color_ = get_hovered_color(_ev->position()); }

QColor plt_sprite::get_hovered_color(QPointF _p)
{
    const int px = _p.x();
    const int py = _p.y();
    const int xd = px / wg;
    const int yd = py / hg;
    return (qMax(xd, yd) > 15 || qMin(px % wg, py % hg) < gap) ? Qt::transparent : palette_->get(xd, yd);
}

void plt_sprite::update_hovered(const QColor _color)
{
    if (_color != hovered_color_)
        emit color_hovered(hovered_color_ = _color, palette_->color_group(_color));
}
} // namespace priv

palette_tab::palette_tab(QWidget *_parent) : QDockWidget("Palette", _parent), body_(new QTabWidget)
{
    using namespace priv;
    setFeatures(QDockWidget::NoDockWidgetFeatures);
    body_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    body_->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    for (const auto [type, name] : {QPair{palette::dos, "DOS"}, {palette::windows, "Windows (Legacy)"}})
    {
        const auto *const palette = palette::make(type);
        auto *const sprite = sprites_.emplace_back(new plt_sprite(palette));
        connect(sprite, &plt_sprite::color_hovered, this, &palette_tab::on_color_hovered);
        connect(sprite, &plt_sprite::color_selected, this, &palette_tab::color_selected);

        auto *const primary = primary_frames_.emplace_back(new plt_color_frame);
        auto *const hover = new plt_color_frame;
        auto *const secondary = secondary_frames_.emplace_back(new plt_color_frame);
        connect(this, &palette_tab::color_hovered, hover, &plt_color_frame::set_color);

        auto *const group_label = group_labels_.emplace_back(new QLabel);
        group_label->setObjectName("groupLabel");

        auto *const lyt = new QGridLayout;
        lyt->setContentsMargins(4, 6, 4, 6);
        lyt->setAlignment(Qt::AlignTop | Qt::AlignCenter);
        lyt->setSpacing(8);
        lyt->setVerticalSpacing(20);
        lyt->setColumnStretch(0, 3);
        lyt->setColumnStretch(1, 2);
        lyt->setColumnStretch(2, 3);
        lyt->addWidget(sprite, 0, 0, 1, 3);
        lyt->addWidget(primary, 1, 0);
        lyt->addWidget(hover, 1, 1);
        lyt->addWidget(secondary, 1, 2);
        lyt->addWidget(group_label, 2, 0, 1, 3);

        auto *const tab = new QWidget;
        tab->setLayout(lyt);
        body_->addTab(tab, name);
    }
    setWidget(body_);
    emit color_hovered(Qt::transparent);
}

void palette_tab::set_palette(palette::type _type)
{
    if (_type != palette::none)
        body_->setCurrentIndex(_type - palette::dos);
}

void palette_tab::set_color(QColor _color, bool _primary)
{
    for (auto *frame : (_primary ? primary_frames_ : secondary_frames_))
        frame->set_color(_color);
    for (auto *sprite : sprites_)
        sprite->set_color(_color, _primary);
}

void palette_tab::on_color_hovered(QColor _color, const QString &_group)
{
    for (auto *label : group_labels_)
        label->setText(_group);
    emit color_hovered(_color);
}
} // namespace veles