#pragma once

#include <QEvent>
#include <QString>
#include <QWidget>

class QLabel;
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

signals:
    void minimizeRequested();
    void maximizeRequested();
    void restoreRequested();
    void closeRequested();
    void systemMoveRequested();

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
    bool m_isMaximized = false;
};
