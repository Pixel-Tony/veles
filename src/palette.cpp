#include "palette.hpp"

#include <QHash>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QResource>
#include <QString>
#include <array>

namespace
{
using namespace veles;

std::array<palette *, 3> palettes = {};
auto **p_none = &palettes[palette::none];
auto **p_dos = &palettes[palette::dos];
auto **p_win = &palettes[palette::windows];

auto load_palette_file(const QString &_filename)
{
    auto result = QList<QRgb>(256);
    const uchar *const ptr = QResource{_filename}.data();
    for (int i = 0; i < 256; ++i)
        result[i] = qRgb(ptr[3 * i], ptr[3 * i + 1], ptr[3 * i + 2]);
    return result;
}

auto calc_color_list(const QList<QRgb> &_colors)
{
    QHash<QColor, uint> result;
    for (int i = 0; auto c : _colors)
        result[c] = i++;
    return result;
}

auto load_color_groups(const QString &_filename)
{
    QList<QPair<QString, QPair<uint8_t, uint8_t>>> groups;
    const QJsonDocument doc = QJsonDocument::fromJson(QResource(_filename).uncompressedData());
    for (const auto &key : doc.object().keys())
    {
        auto arr = doc[key].toArray();
        if (arr.count() != 2)
            throw QJsonParseError();
        uint8_t begin = arr[0].toString().toUInt(nullptr, 16);
        uint8_t end = arr[1].toString().toUInt(nullptr, 16);
        groups.push_back({QString("Group: %0").arg(key), {begin, end}});
    }
    return groups;
}
} // namespace

static size_t qHash(const QColor &_col) { return _col.rgba(); }

namespace veles
{
palette::palette(const type _type, const QString &_filename, const QString &_groups_filename, QString &&_name)
    : type_(_type),
      name_(_name),
      contents_(load_palette_file(_filename)),
      colors_(calc_color_list(contents_)),
      color_groups_(load_color_groups(_groups_filename))
{
}

const palette *palette::make(palette::type _type)
{
    if (*p_dos == nullptr)
    {
        *p_dos = new palette(type::dos, ":/assets/palette-dos.act", ":/assets/palette-dos-groups.json", "DOS");
        *p_win = new palette(
            type::windows, ":/assets/palette-win.act", ":/assets/palette-win-groups.json", "Windows (Legacy)");
    }
    return palettes[_type];
}

uint palette::index(QColor _color) const noexcept { return colors_.value(_color, -1); }

const QString &palette::color_group(QColor _color) const noexcept
{
    static const QString none = "Group: none";
    static const QString transparent = "Groups: Full Transparent";
    static const QString empty = "";
    if (_color == Qt::transparent)
        return type_ == palette::none ? transparent : empty;
    if (!colors_.contains(_color))
        return none;
    const auto ind = colors_.value(_color);
    for (const auto &[name, bounds] : color_groups_)
    {
        if (bounds.first <= ind && ind <= bounds.second)
            return name;
    }
    return none;
}

QColor palette::get(int _x, int _y) const noexcept { return contents_[_y * 16 + _x]; }
} // namespace veles