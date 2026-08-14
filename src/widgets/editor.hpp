#ifndef VELES_CANVAS_SCENE_HPP
#define VELES_CANVAS_SCENE_HPP

#include "palette.hpp"

#include <QGraphicsView>
#include <QPair>
#include <QSize>
#include <QString>

class QUndoStack;

namespace veles
{
class editor;
class image;

namespace priv
{
class tool_handler : public QObject
{
    Q_OBJECT

public:
    explicit tool_handler(editor *_parent = nullptr);

    bool eventFilter(QObject *, QEvent *_ev) override;

private:
    editor *const ed_;
};
} // namespace priv

class editor : public QGraphicsView
{
    Q_OBJECT

public:
    editor(QString &&_name, QSize _image_size, palette::type _palette, QWidget *_parent = nullptr);

    explicit editor(const QString &_filepath, QWidget *_parent = nullptr) noexcept(false);

private:
    editor(image *_image, QString &&_path, QString &&_name, QWidget *_parent);

public:
    ~editor() override;

    static constexpr QPair<float, float> zoom_bounds = {1, 32};

signals:
    void changed();

public:
    [[nodiscard]] image &get_image() noexcept;
    [[nodiscard]] const image &get_image() const noexcept;
    [[nodiscard]] const QString &name() const noexcept;
    [[nodiscard]] const QString &path() const noexcept;
    [[nodiscard]] QUndoStack *history() noexcept;
    [[nodiscard]] const QUndoStack *history() const noexcept;
    [[nodiscard]] bool exists() const noexcept;
    [[nodiscard]] float zoom() const noexcept;

    void apply_zoom_delta(float _delta);
    void default_zoom();

    bool save();
    bool save(const QString &_path);

private slots:
    void center_on_image();

private:
    priv::tool_handler *tool_handler_;
    image *image_;
    QGraphicsRectItem *image_padding_;
    QString name_;
    QString path_;
    float zoom_ = 1;

    QUndoStack *history_;
};
} // namespace veles

#endif // VELES_CANVAS_SCENE_HPP