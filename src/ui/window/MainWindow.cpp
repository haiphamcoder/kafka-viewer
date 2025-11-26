#include "ui/window/MainWindow.h"

#include <QApplication>
#include <QDebug>
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

    m_contentContainer = new QWidget(root);
    auto *contentLayout = new QVBoxLayout(m_contentContainer);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(0);

    m_titleBar = new TitleBar(m_contentContainer);
    contentLayout->addWidget(m_titleBar);

    auto *content = new QWidget(m_contentContainer);
    content->setObjectName(QStringLiteral("MainContent"));
    contentLayout->addWidget(content, /*stretch=*/1);

    grid->addWidget(m_contentContainer, 1, 1);

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

    // Moving the menu bar needs to happen before the title bar visibility
    // changes so we do not briefly lose the menu during the transition.
    if (useSystemFrame)
        moveMenuBarToContent();
    else
        restoreMenuBarToTitleBar();

    applyWindowFrameFlags(useSystemFrame);
    updateTitleBarVisibilityForFrameMode(useSystemFrame);

    for (auto *handle : std::as_const(m_resizeHandles)) {
        if (handle)
            handle->setVisible(!useSystemFrame);
    }

    show(); // Required to apply window flag changes

    if (auto *app = qobject_cast<Application *>(QApplication::instance())) {
        app->loadTheme(app->currentTheme());
    }
}

// When the window uses the system frame we no longer need a custom title bar,
// so migrate the menu bar into the content area to keep it visible.
void MainWindow::moveMenuBarToContent()
{
    if (!m_titleBar || !m_contentContainer)
        return;

    auto *menuBar = m_titleBar->menuBar();
    auto *layout = contentLayout();
    if (!menuBar || !layout)
        return;

    m_titleBar->setMenuBar(nullptr);

    menuBar->setParent(m_contentContainer);
    const int titleIndex = layout->indexOf(m_titleBar);
    const int insertPosition = titleIndex >= 0 ? titleIndex + 1 : 0;
    layout->insertWidget(insertPosition, menuBar);
}

// Restore the menu bar to the custom title bar when returning to the frameless
// window mode.
void MainWindow::restoreMenuBarToTitleBar()
{
    if (!m_titleBar)
        return;

    QMenuBar *menuBar = takeMenuBarFromContentLayout();
    if (!menuBar)
        menuBar = m_titleBar->menuBar();

    if (!menuBar)
        return;

    menuBar->setParent(m_titleBar);
    m_titleBar->setMenuBar(menuBar);
}

// Finds the menu bar widget inside the content layout, detaches it from the
// layout, and returns it to the caller for reparenting.
QMenuBar *MainWindow::takeMenuBarFromContentLayout()
{
    auto *layout = contentLayout();
    if (!layout)
        return nullptr;

    for (int i = 0; i < layout->count(); ++i) {
        auto *item = layout->itemAt(i);
        if (!item)
            continue;
        if (auto *widget = item->widget()) {
            if (auto *menuBar = qobject_cast<QMenuBar *>(widget)) {
                layout->removeWidget(menuBar);
                menuBar->setParent(nullptr);
                return menuBar;
            }
        }
    }
    return nullptr;
}

// Convenience accessor that guarantees we only work with QVBoxLayout when
// manipulating the stacked widgets in the window content container.
QVBoxLayout *MainWindow::contentLayout() const
{
    return qobject_cast<QVBoxLayout *>(m_contentContainer ? m_contentContainer->layout() : nullptr);
}

// Applies the correct set of Qt window flags for the selected frame mode.
void MainWindow::applyWindowFrameFlags(bool useSystemFrame)
{
    auto flags = windowFlags();
    if (useSystemFrame) {
        flags &= ~Qt::FramelessWindowHint;
        flags |= Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint;
    } else {
        flags |= Qt::FramelessWindowHint | Qt::WindowSystemMenuHint;
        flags &= ~(Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
    }
    setWindowFlags(flags);
}

// Keeps the title bar visibility and control state in sync with the active
// frame mode so the user never sees duplicate chrome.
void MainWindow::updateTitleBarVisibilityForFrameMode(bool useSystemFrame)
{
    if (!m_titleBar)
        return;

    m_titleBar->setUseSystemFrame(useSystemFrame);
    m_titleBar->setVisible(!useSystemFrame);
    if (!useSystemFrame)
        updateWindowUiState();
}
