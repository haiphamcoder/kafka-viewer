#include "ui/window/decoration/WindowResizeHandle.h"

#include <QCursor>
#include <QMouseEvent>
#include <QWindow>

namespace
{
Qt::CursorShape cursorForEdges(Qt::Edges edges)
{
    switch (edges) {
    case Qt::LeftEdge:
    case Qt::RightEdge:
        return Qt::SizeHorCursor;
    case Qt::TopEdge:
    case Qt::BottomEdge:
        return Qt::SizeVerCursor;
    case Qt::TopEdge | Qt::LeftEdge:
    case Qt::BottomEdge | Qt::RightEdge:
        return Qt::SizeFDiagCursor;
    case Qt::TopEdge | Qt::RightEdge:
    case Qt::BottomEdge | Qt::LeftEdge:
        return Qt::SizeBDiagCursor;
    default:
        return Qt::SizeAllCursor;
    }
}
} // namespace

WindowResizeHandle::WindowResizeHandle(Qt::Edges edges, QWidget *parent)
    : QWidget(parent)
    , m_edges(edges)
{
    setCursor(cursorForEdges(edges));
    setAttribute(Qt::WA_NoMousePropagation);
    setMouseTracking(true);

    auto policy = sizePolicy();
    if (edges.testFlag(Qt::TopEdge) || edges.testFlag(Qt::BottomEdge))
        policy.setHorizontalPolicy(QSizePolicy::Expanding);
    else
        policy.setHorizontalPolicy(QSizePolicy::Fixed);

    if (edges.testFlag(Qt::LeftEdge) || edges.testFlag(Qt::RightEdge))
        policy.setVerticalPolicy(QSizePolicy::Expanding);
    else
        policy.setVerticalPolicy(QSizePolicy::Fixed);

    setSizePolicy(policy);
}

void WindowResizeHandle::mousePressEvent(QMouseEvent *event)
{
    beginSystemResize(event);
    QWidget::mousePressEvent(event);
}

void WindowResizeHandle::mouseMoveEvent(QMouseEvent *event)
{
    if (!m_resizing && (event->buttons() & Qt::LeftButton))
        beginSystemResize(event);

    QWidget::mouseMoveEvent(event);
}

void WindowResizeHandle::beginSystemResize(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton && !(event->buttons() & Qt::LeftButton))
        return;

    if (auto *handle = window() ? window()->windowHandle() : nullptr) {
        if (handle->startSystemResize(m_edges)) {
            event->accept();
            m_resizing = true;
        }
    }
}
