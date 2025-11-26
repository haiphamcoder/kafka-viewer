#pragma once

#include <QEvent>
#include <QString>
#include <QWidget>

class QAction;
class QLabel;
class QMenu;
class QMenuBar;
class QMouseEvent;
class QToolButton;

/**
 * @brief Simple custom title bar with window control buttons.
 */
class TitleBar final : public QWidget
{
    Q_OBJECT

public:
    explicit TitleBar(QWidget *parent = nullptr);

    void setMaximized(bool maximized);
    void setUseSystemFrame(bool useSystemFrame);
    void setMenuBar(QMenuBar *menuBar);
    void setTheme(const QString &themeName);
    QMenuBar *menuBar() const { return m_menuBar; }

signals:
    void minimizeRequested();
    void maximizeRequested();
    void restoreRequested();
    void closeRequested();
    void systemMoveRequested();
    void aboutRequested();
    void useSystemFrameRequested(bool useSystemFrame);
    void themeChanged(const QString &themeName);

private:
    void setupUi();
    void createMenus();
    QToolButton *createButton(const QString &iconPath, const QString &tooltip);
    void updateMaximizeButton();
    bool eventFilter(QObject *watched, QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;

    QLabel *m_logoLabel = nullptr;
    QMenuBar *m_menuBar = nullptr;
    QToolButton *m_minimizeButton = nullptr;
    QToolButton *m_maximizeButton = nullptr;
    QToolButton *m_closeButton = nullptr;
    QAction *m_useSystemFrameAction = nullptr;
    QAction *m_lightThemeAction = nullptr;
    QAction *m_darkThemeAction = nullptr;
    bool m_isMaximized = false;
};
