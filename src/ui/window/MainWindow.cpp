#include "ui/window/MainWindow.h"

#include <QApplication>
#include <QEvent>
#include <QGridLayout>
#include <QMenuBar>
#include <QVBoxLayout>
#include <QWindow>

#include "app/Application.h"
#include "ui/dialogs/AboutDialog.h"
#include "ui/window/decoration/TitleBar.h"
#include "ui/window/decoration/WindowResizeHandle.h"

namespace
{
constexpr int kResizeHandleThickness = 6;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setObjectName(QStringLiteral("MainWindow"));
    setWindowTitle(QStringLiteral("Kafka Viewer"));
    setWindowFlag(Qt::FramelessWindowHint);
    resize(1024, 640);
    setupUi();
    updateWindowUiState();
}

void MainWindow::setupUi()
{
    auto *root = new QWidget(this);
    auto *grid = new QGridLayout(root);
    grid->setContentsMargins(0, 0, 0, 0);
    grid->setSpacing(0);
    grid->setRowStretch(1, 1);
    grid->setColumnStretch(1, 1);
    grid->setRowMinimumHeight(0, kResizeHandleThickness);
    grid->setRowMinimumHeight(2, kResizeHandleThickness);
    grid->setColumnMinimumWidth(0, kResizeHandleThickness);
    grid->setColumnMinimumWidth(2, kResizeHandleThickness);

    setupResizeHandles(root, grid);

    auto *contentContainer = new QWidget(root);
    auto *contentLayout = new QVBoxLayout(contentContainer);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(0);

    m_titleBar = new TitleBar(contentContainer);
    contentLayout->addWidget(m_titleBar);

    auto *content = new QWidget(contentContainer);
    content->setObjectName(QStringLiteral("MainContent"));
    contentLayout->addWidget(content, /*stretch=*/1);

    grid->addWidget(contentContainer, 1, 1);

    setCentralWidget(root);

    connectTitleBarSignals();
}

void MainWindow::setupResizeHandles(QWidget *rootWidget, QGridLayout *gridLayout)
{
    auto addHandle = [&](int row, int column, int rowSpan, int columnSpan, Qt::Edges edges,
                         int fixedWidth, int fixedHeight) {
        auto *handle = new WindowResizeHandle(edges, rootWidget);
        if (fixedWidth > 0)
            handle->setFixedWidth(fixedWidth);
        if (fixedHeight > 0)
            handle->setFixedHeight(fixedHeight);
        gridLayout->addWidget(handle, row, column, rowSpan, columnSpan);
        m_resizeHandles.append(handle);
    };

    addHandle(0, 0, 1, 1, Qt::TopEdge | Qt::LeftEdge, kResizeHandleThickness, kResizeHandleThickness);
    addHandle(0, 1, 1, 1, Qt::TopEdge, 0, kResizeHandleThickness);
    addHandle(0, 2, 1, 1, Qt::TopEdge | Qt::RightEdge, kResizeHandleThickness, kResizeHandleThickness);

    addHandle(1, 0, 1, 1, Qt::LeftEdge, kResizeHandleThickness, 0);
    addHandle(1, 2, 1, 1, Qt::RightEdge, kResizeHandleThickness, 0);

    addHandle(2, 0, 1, 1, Qt::BottomEdge | Qt::LeftEdge, kResizeHandleThickness, kResizeHandleThickness);
    addHandle(2, 1, 1, 1, Qt::BottomEdge, 0, kResizeHandleThickness);
    addHandle(2, 2, 1, 1, Qt::BottomEdge | Qt::RightEdge, kResizeHandleThickness, kResizeHandleThickness);
}

void MainWindow::connectTitleBarSignals()
{
    QObject::connect(m_titleBar, &TitleBar::minimizeRequested, this, &QWidget::showMinimized);
    QObject::connect(m_titleBar, &TitleBar::maximizeRequested, this, [this]() {
        toggleMaximizeRestore();
    });
    QObject::connect(m_titleBar, &TitleBar::restoreRequested, this, [this]() {
        restoreWindow();
    });
    QObject::connect(m_titleBar, &TitleBar::closeRequested, this, &QWidget::close);
    QObject::connect(m_titleBar, &TitleBar::systemMoveRequested, this, [this]() {
        if (auto *handle = windowHandle())
            handle->startSystemMove();
    });
    QObject::connect(m_titleBar, &TitleBar::aboutRequested, this, [this]() {
        AboutDialog dialog(this);
        dialog.exec();
    });
    QObject::connect(m_titleBar, &TitleBar::useSystemFrameRequested, this,
                    &MainWindow::setUseSystemFrame);
    QObject::connect(m_titleBar, &TitleBar::themeChanged, this, [](const QString &themeName) {
        if (auto *app = qobject_cast<Application *>(QApplication::instance())) {
            app->onThemeChanged(themeName);
        }
    });
}

void MainWindow::toggleMaximizeRestore()
{
    if (isMaximized()) {
        showNormal();
    } else {
        showMaximized();
    }
    updateWindowUiState();
}

void MainWindow::restoreWindow()
{
    if (isMaximized())
        showNormal();
    updateWindowUiState();
}

void MainWindow::updateWindowUiState()
{
    const bool maximized = isMaximized();
    if (m_titleBar)
        m_titleBar->setMaximized(maximized);

    for (auto *handle : std::as_const(m_resizeHandles)) {
        if (handle)
            handle->setVisible(!maximized);
    }
}

void MainWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::WindowStateChange)
        updateWindowUiState();

    QMainWindow::changeEvent(event);
}

void MainWindow::setUseSystemFrame(bool useSystemFrame)
{
    if (m_useSystemFrame == useSystemFrame)
        return;

    m_useSystemFrame = useSystemFrame;

    if (useSystemFrame) {
        // Switch to system window frame
        setWindowFlag(Qt::FramelessWindowHint, false);
        if (m_titleBar) {
            // Hide logo and control buttons
            m_titleBar->setUseSystemFrame(true);
            // Set menubar as the window's menu bar (it will appear in system title bar)
            if (m_titleBar->menuBar()) {
                setMenuBar(m_titleBar->menuBar());
            }
            // Hide the TitleBar widget itself (menubar is now in system frame)
            m_titleBar->setVisible(false);
        }
        for (auto *handle : std::as_const(m_resizeHandles)) {
            if (handle)
                handle->setVisible(false);
        }
    } else {
        // Switch to custom frameless window
        setWindowFlag(Qt::FramelessWindowHint, true);
        // Remove menubar from window (restore it to TitleBar)
        if (m_titleBar && m_titleBar->menuBar()) {
            setMenuBar(nullptr);
            // Restore menubar parent to TitleBar
            m_titleBar->menuBar()->setParent(m_titleBar);
            // Restore menubar to TitleBar's layout
            m_titleBar->restoreMenuBar();
        }
        if (m_titleBar) {
            // Show TitleBar again
            m_titleBar->setVisible(true);
            // Show logo and control buttons again
            m_titleBar->setUseSystemFrame(false);
        }
        updateWindowUiState();
    }

    show(); // Required to apply window flag changes
}
