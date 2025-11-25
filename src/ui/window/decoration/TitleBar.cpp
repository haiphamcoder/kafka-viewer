#include "ui/window/decoration/TitleBar.h"

#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QSizePolicy>
#include <QToolButton>

namespace
{
constexpr auto kIconPrefix = ":/icons/titlebar/";
}

TitleBar::TitleBar(QWidget *parent)
    : QWidget(parent)
{
    setObjectName(QStringLiteral("TitleBar"));
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    setupUi();
}

void TitleBar::setupUi()
{
    auto *layout = new QHBoxLayout(this);
    layout->setContentsMargins(8, 0, 8, 0);
    layout->setSpacing(6);

    m_titleLabel = new QLabel(tr("Kafka Viewer"), this);
    m_titleLabel->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);

    m_minimizeButton = createButton(QStringLiteral("btn_minimize.svg"), tr("Minimize"));
    connect(m_minimizeButton, &QToolButton::clicked, this, &TitleBar::minimizeRequested);

    m_maximizeButton = createButton(QStringLiteral("btn_maximize.svg"), tr("Maximize"));
    connect(m_maximizeButton, &QToolButton::clicked, this, [this]() {
        if (m_isMaximized) {
            emit restoreRequested();
        } else {
            emit maximizeRequested();
        }
    });

    m_closeButton = createButton(QStringLiteral("btn_close.svg"), tr("Close"));
    connect(m_closeButton, &QToolButton::clicked, this, &TitleBar::closeRequested);

    layout->addWidget(m_titleLabel, /*stretch=*/1);
    layout->addWidget(m_minimizeButton);
    layout->addWidget(m_maximizeButton);
    layout->addWidget(m_closeButton);
}

QToolButton *TitleBar::createButton(const QString &iconPath, const QString &tooltip)
{
    auto *button = new QToolButton(this);
    button->setIcon(QIcon(QString::fromLatin1("%1%2").arg(QLatin1String(kIconPrefix), iconPath)));
    button->setToolTip(tooltip);
    button->setCursor(Qt::PointingHandCursor);
    button->setAutoRaise(true);
    return button;
}

void TitleBar::setTitle(const QString &title)
{
    if (m_titleLabel) {
        m_titleLabel->setText(title);
    }
}

void TitleBar::setMaximized(bool maximized)
{
    if (m_isMaximized == maximized)
        return;

    m_isMaximized = maximized;
    updateMaximizeButton();
}

void TitleBar::updateMaximizeButton()
{
    if (!m_maximizeButton)
        return;

    const QString iconName = m_isMaximized ? QStringLiteral("btn_restore.svg")
                                           : QStringLiteral("btn_maximize.svg");
    m_maximizeButton->setIcon(
        QIcon(QString::fromLatin1("%1%2").arg(QLatin1String(kIconPrefix), iconName)));
    m_maximizeButton->setToolTip(m_isMaximized ? tr("Restore") : tr("Maximize"));
}
