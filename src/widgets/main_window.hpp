#ifndef VELES_MAIN_WINDOW_HPP
#define VELES_MAIN_WINDOW_HPP

#include <QCloseEvent>
#include <QMainWindow>

namespace veles
{
class main_window : public QMainWindow
{
    Q_OBJECT

public:
    main_window();

protected:
    void closeEvent(QCloseEvent *_ev) override;
};
} // namespace veles

#endif // VELES_MAIN_WINDOW_HPP
