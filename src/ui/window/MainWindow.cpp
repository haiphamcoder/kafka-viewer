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

    // Move menu bar between title bar and content container based on frame mode
    auto *contentLayout = qobject_cast<QVBoxLayout *>(m_contentContainer->layout());
    QMenuBar *menuBar = nullptr;
    
    if (useSystemFrame) {
        // System frame: get menubar from TitleBar before removing it
        if (m_titleBar) {
            menuBar = m_titleBar->menuBar();
            if (menuBar) {
                // Remove from TitleBar first
                m_titleBar->setMenuBar(nullptr);
                // Add to content layout between titleBar and content
                if (contentLayout && m_contentContainer) {
                    menuBar->setParent(m_contentContainer);
                    int contentIndex = contentLayout->indexOf(m_titleBar);
                    if (contentIndex >= 0) {
                        contentLayout->insertWidget(contentIndex + 1, menuBar);
                    } else {
                        contentLayout->insertWidget(0, menuBar);
                    }
                }
            }
        }
    } else {
        // Custom frame: get menubar from contentLayout
        if (contentLayout && m_contentContainer) {
            // Find menubar in contentLayout
            for (int i = 0; i < contentLayout->count(); ++i) {
                auto *item = contentLayout->itemAt(i);
                if (item) {
                    if (auto *widget = item->widget()) {
                        if (qobject_cast<QMenuBar *>(widget)) {
                            menuBar = qobject_cast<QMenuBar *>(widget);
                            qDebug() << "Found menubar in contentLayout at index" << i;
                            break;
                        }
                    }
                }
            }
        }
        
        // If not found in contentLayout, try to get from TitleBar (fallback)
        if (!menuBar && m_titleBar) {
            menuBar = m_titleBar->menuBar();
            qDebug() << "Menubar from TitleBar:" << menuBar;
        }
        
        if (menuBar) {
            qDebug() << "Switching to custom frame - restoring menubar to TitleBar";
            // Ensure TitleBar is visible first
            if (m_titleBar) {
                qDebug() << "TitleBar visible:" << m_titleBar->isVisible();
                m_titleBar->setVisible(true);
                qDebug() << "TitleBar visible after setVisible(true):" << m_titleBar->isVisible();
            }
            // Remove from contentLayout if it's there
            if (contentLayout) {
                qDebug() << "Removing menubar from contentLayout";
                contentLayout->removeWidget(menuBar);
                qDebug() << "ContentLayout count after remove:" << contentLayout->count();
            }
            qDebug() << "Setting menubar parent to MainWindow";
            menuBar->setParent(this);
            qDebug() << "Menubar parent:" << menuBar->parent();
            if (m_titleBar) {
                qDebug() << "Calling titleBar->setMenuBar(menuBar)";
                m_titleBar->setMenuBar(menuBar);
                qDebug() << "TitleBar menubar:" << m_titleBar->menuBar();
                qDebug() << "TitleBar layout count:" << (m_titleBar->layout() ? m_titleBar->layout()->count() : 0);
            }
        } else {
            qDebug() << "ERROR: No menubar found to restore!";
        }
    }

    // Set window flags first (before moving widgets)
    auto flags = windowFlags();
    if (useSystemFrame) {
        // Switch to system window frame
        flags &= ~Qt::FramelessWindowHint;
        flags |= Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint;
    } else {
        // Switch to custom frameless window
        flags |= Qt::FramelessWindowHint | Qt::WindowSystemMenuHint;
        flags &= ~(Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
    }
    setWindowFlags(flags);
    
    // Update UI state after window flags are set
    if (useSystemFrame) {
        if (m_titleBar) {
            // Hide logo and control buttons
            m_titleBar->setUseSystemFrame(true);
            // Hide the TitleBar widget itself
            m_titleBar->setVisible(false);
        }
    } else {
        if (m_titleBar) {
            // Show TitleBar again
            m_titleBar->setVisible(true);
            // Show logo and control buttons again
            m_titleBar->setUseSystemFrame(false);
        }
        updateWindowUiState();
    }
    
    for (auto *handle : std::as_const(m_resizeHandles)) {
        if (handle)
            handle->setVisible(!useSystemFrame);
    }

    // Show window and force style update
    show(); // Required to apply window flag changes
    
    // Reapply stylesheet to ensure styles are correct after window flag change
    if (auto *app = qobject_cast<Application *>(QApplication::instance())) {
        // Trigger style update by reapplying current theme
        app->loadTheme(app->currentTheme());
    }
}
