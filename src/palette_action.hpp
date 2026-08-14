#ifndef VELES_PALETTE_BUTTON_HPP
#define VELES_PALETTE_BUTTON_HPP

#include <QObject>
#include <QWidgetAction>

namespace veles
{
class palette_action : public QWidgetAction
{
    Q_OBJECT

public:
    explicit palette_action(QObject *_parent = nullptr);

private:
    void update_colors();
};
} // namespace veles

#endif // VELES_PALETTE_BUTTON_HPP