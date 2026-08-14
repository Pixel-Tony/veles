#ifndef VELES_PALETTE_HPP
#define VELES_PALETTE_HPP

#include <QColor>
#include <QHash>
#include <QList>
#include <QPair>
#include <QRgb>
#include <QString>

namespace veles
{
class palette
{
public:
    enum type
    {
        none,
        dos,
        windows,
        _count
    };

private:
    explicit palette(type _type, const QString &_filename, const QString &_groups_filename, QString &&_name);

public:
    static const palette *make(type _type);
    static const palette *find(const QList<QRgb> &_color_table);

    [[nodiscard]] uint index(QColor _color) const noexcept;
    [[nodiscard]] const QString &color_group(QColor _color) const noexcept;
    [[nodiscard]] QColor get(int _x, int _y) const noexcept;

    const type type_;
    const QString name_;
    const QList<QRgb> contents_;

private:
    const QHash<QColor, uint> colors_;
    const QList<QPair<QString, QPair<uint8_t, uint8_t>>> color_groups_;
};
} // namespace veles

#endif // VELES_PALETTE_HPP