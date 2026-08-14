#ifndef VELES_PEN_HPP
#define VELES_PEN_HPP

#include "tool.hpp"

#include <QColor>
#include <QList>
#include <QPair>
#include <QPoint>
#include <QUndoCommand>

namespace veles
{
class image;

struct pen : tool
{
private:
    struct command : QUndoCommand
    {
        command(QColor _color, image *_image);

        [[nodiscard]] bool empty() const noexcept;

        void undo() override;
        void redo() override;
        void add_color(QPointF _pos);

    private:
        const QColor color_;
        image *const image_;
        QList<QPair<QPoint, QColor>> pixels_;
    };

public:
    pen();

    ~pen() override;

    void editor_event(QEvent &_ev, editor &_ed) override;

private:
    command *command_ = nullptr;
};
} // namespace veles

#endif // VELES_PEN_HPP