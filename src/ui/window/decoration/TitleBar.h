#pragma once

#include <QWidget>
#include <QString>

class QLabel;
class QToolButton;

/**
 * @brief Simple custom title bar with window control buttons.
 */
class TitleBar final : public QWidget
{
    Q_OBJECT

public:
    explicit TitleBar(QWidget *parent = nullptr);

    void setTitle(const QString &title);
    void setMaximized(bool maximized);

signals:
    void minimizeRequested();
    void maximizeRequested();
    void restoreRequested();
    void closeRequested();

private:
    void setupUi();
    QToolButton *createButton(const QString &iconPath, const QString &tooltip);
    void updateMaximizeButton();

    QLabel *m_titleLabel = nullptr;
    QToolButton *m_minimizeButton = nullptr;
    QToolButton *m_maximizeButton = nullptr;
    QToolButton *m_closeButton = nullptr;
    bool m_isMaximized = false;
};
