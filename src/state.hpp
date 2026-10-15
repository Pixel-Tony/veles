#ifndef VELES_STATE_HPP
#define VELES_STATE_HPP

#include <QColor>
#include <QImage>
#include <QObject>

namespace veles
{
class palette;

class state : public QObject
{
    Q_OBJECT

private:
    explicit state();

public:
    [[nodiscard]] QColor primary() const noexcept;
    [[nodiscard]] QColor secondary() const noexcept;
    [[nodiscard]] QColor color(bool _primary) const noexcept;
    void set_primary(QColor _color);
    void set_secondary(QColor _color);
    void set_color(QColor _color, bool _primary);
    void set_clipboard(const QImage &_image, const palette *_palette);
    QPair<const QImage &, const palette *> clipboard();
    void push_recent(const QString &_filepath) noexcept;
    void replace_recent(const QString &_filepath, const QString &_new_filepath) noexcept;
    [[nodiscard]] QStringList recent_files() const noexcept;
    [[nodiscard]] int recent_count() const noexcept;
    void set_recent_count(int _count) noexcept;
    void refresh_recent_files() noexcept;
    void clear_recent_files() noexcept;

public slots:
    void swap_colors();

signals:
    void color_updated(QColor _value, bool _primary);
    void recent_files_changed();

public:
    static state *instance();

private:
    void store_recent(QStringList _recent) noexcept;

    QColor primary_;
    QColor secondary_;
    QPair<QImage, const palette *> clipboard_;
    qint32 max_recent_files_ = 8;
};
} // namespace veles

#endif // VELES_STATE_HPP
