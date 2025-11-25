#pragma once

#include <QWidget>
#include <Qt>

class QMouseEvent;

/**
 * @brief Invisible widget positioned around the window edges that triggers system resize.
 */
class WindowResizeHandle final : public QWidget
{
    Q_OBJECT

public:
    explicit WindowResizeHandle(Qt::Edges edges, QWidget *parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    void beginSystemResize(QMouseEvent *event);

    Qt::Edges m_edges;
    bool m_resizing = false;
};
