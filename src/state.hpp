#ifndef VELES_STATE_HPP
#define VELES_STATE_HPP

#include <QColor>
#include <QObject>

namespace veles
{
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

public slots:
    void swap_colors();

signals:
    void color_updated(QColor _value, bool _primary);

public:
    static state *instance();

private:
    QColor primary_;
    QColor secondary_;
};
} // namespace veles

#endif // VELES_STATE_HPP
