#include "ui/window/decoration/TitleBar.h"

#include <QAction>
#include <QEvent>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QMouseEvent>
#include <QPixmap>
#include <QSizePolicy>
#include <QToolButton>

namespace {
constexpr auto kIconPrefix = ":/icons/titlebar/";
constexpr auto kLogoIcon = ":/icons/titlebar/logo.svg";
} // namespace

TitleBar::TitleBar(QWidget *parent) : QWidget(parent) {
  setObjectName(QStringLiteral("TitleBar"));
  setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
  setupUi();
}

void TitleBar::setupUi() {
  auto *layout = new QHBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(6);

  m_logoLabel = new QLabel(this);
  m_logoLabel->setObjectName(QStringLiteral("TitleBarLogo"));
  m_logoLabel->setAlignment(Qt::AlignCenter);
  m_logoLabel->setFixedSize(24, 24);
  m_logoLabel->setPixmap(QIcon(QString::fromLatin1(kLogoIcon)).pixmap(24, 24));

  m_menuBar = new QMenuBar(this);
  m_menuBar->setObjectName(QStringLiteral("TitleBarMenu"));
  m_menuBar->setNativeMenuBar(false);
  m_menuBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  m_menuBar->installEventFilter(this);
  createMenus();

  m_minimizeButton =
      createButton(QStringLiteral("btn_minimize.svg"), tr("Minimize"));
  connect(m_minimizeButton, &QToolButton::clicked, this,
          &TitleBar::minimizeRequested);

  m_maximizeButton =
      createButton(QStringLiteral("btn_maximize.svg"), tr("Maximize"));
  connect(m_maximizeButton, &QToolButton::clicked, this, [this]() {
    if (m_isMaximized) {
      emit restoreRequested();
    } else {
      emit maximizeRequested();
    }
  });

  m_closeButton = createButton(QStringLiteral("btn_close.svg"), tr("Close"));
  connect(m_closeButton, &QToolButton::clicked, this,
          &TitleBar::closeRequested);

  layout->addWidget(m_logoLabel);
  layout->addWidget(m_menuBar, /*stretch=*/1);
  layout->setAlignment(m_menuBar, Qt::AlignVCenter);
  layout->addStretch();
  layout->addWidget(m_minimizeButton);
  layout->addWidget(m_maximizeButton);
  layout->addWidget(m_closeButton);
}

QToolButton *TitleBar::createButton(const QString &iconPath,
                                    const QString &tooltip) {
  auto *button = new QToolButton(this);
  button->setIcon(QIcon(
      QString::fromLatin1("%1%2").arg(QLatin1String(kIconPrefix), iconPath)));
  button->setToolTip(tooltip);
  button->setCursor(Qt::PointingHandCursor);
  button->setAutoRaise(true);
  return button;
}

void TitleBar::createMenus() {
  m_menuBar->addMenu(tr("File"));
  m_menuBar->addMenu(tr("Edit"));
  m_menuBar->addMenu(tr("View"));
  m_menuBar->addMenu(tr("Settings"));

  auto *helpMenu = m_menuBar->addMenu(tr("Help"));
  auto *aboutAction = helpMenu->addAction(tr("About Kafka Viewer..."));
  connect(aboutAction, &QAction::triggered, this, &TitleBar::aboutRequested);
}

void TitleBar::setMaximized(bool maximized) {
  if (m_isMaximized == maximized)
    return;

  m_isMaximized = maximized;
  updateMaximizeButton();
}

void TitleBar::updateMaximizeButton() {
  if (!m_maximizeButton)
    return;

  const QString iconName = m_isMaximized ? QStringLiteral("btn_restore.svg")
                                         : QStringLiteral("btn_maximize.svg");
  m_maximizeButton->setIcon(QIcon(
      QString::fromLatin1("%1%2").arg(QLatin1String(kIconPrefix), iconName)));
  m_maximizeButton->setToolTip(m_isMaximized ? tr("Restore") : tr("Maximize"));
}

bool TitleBar::eventFilter(QObject *watched, QEvent *event) {
  if (watched == m_menuBar && event->type() == QEvent::MouseButtonPress) {
    auto *mouseEvent = static_cast<QMouseEvent *>(event);
    if (mouseEvent->button() == Qt::LeftButton &&
        !m_menuBar->actionAt(mouseEvent->pos())) {
      emit systemMoveRequested();
      return true;
    }
  }
  return QWidget::eventFilter(watched, event);
}

void TitleBar::mousePressEvent(QMouseEvent *event) {
  if (event->button() == Qt::LeftButton) {
    emit systemMoveRequested();
    event->accept();
    return;
  }
  QWidget::mousePressEvent(event);
}

void TitleBar::mouseDoubleClickEvent(QMouseEvent *event) {
  if (event->button() == Qt::LeftButton) {
    if (m_isMaximized) {
      emit restoreRequested();
    } else {
      emit maximizeRequested();
    }
    event->accept();
    return;
  }
  QWidget::mouseDoubleClickEvent(event);
}
