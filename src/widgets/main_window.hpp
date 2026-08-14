#ifndef VELES_MAIN_WINDOW_HPP
#define VELES_MAIN_WINDOW_HPP

#include <QCloseEvent>
#include <QMainWindow>

namespace veles
{
class palette_tab;
class palette_action;

class main_window : public QMainWindow
{
    Q_OBJECT

public:
    main_window();

protected:
    void closeEvent(QCloseEvent *_ev) override;
private:
    palette_tab *palette_tab_;

    palette_action *palette_action_;
};
} // namespace veles

#endif // VELES_MAIN_WINDOW_HPP
