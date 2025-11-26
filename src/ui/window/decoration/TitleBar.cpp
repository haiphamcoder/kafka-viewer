#include "ui/window/decoration/TitleBar.h"

#include <QAction>
#include <QActionGroup>
#include <QDebug>
#include <QEvent>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QMouseEvent>
#include <QPixmap>
#include <QSizePolicy>
#include <QStyle>
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
  
  auto *settingsMenu = m_menuBar->addMenu(tr("Settings"));
  m_useSystemFrameAction = settingsMenu->addAction(tr("Use system window frame"));
  m_useSystemFrameAction->setCheckable(true);
  m_useSystemFrameAction->setChecked(false);
  connect(m_useSystemFrameAction, &QAction::toggled, this,
          &TitleBar::useSystemFrameRequested);

  settingsMenu->addSeparator();

  // Theme selection
  auto *themeMenu = settingsMenu->addMenu(tr("Theme"));
  auto *themeGroup = new QActionGroup(this);
  
  m_lightThemeAction = themeMenu->addAction(tr("Light"));
  m_lightThemeAction->setCheckable(true);
  m_lightThemeAction->setChecked(true);
  m_lightThemeAction->setActionGroup(themeGroup);
  connect(m_lightThemeAction, &QAction::triggered, this, [this]() {
    emit themeChanged(QStringLiteral("light"));
  });

  m_darkThemeAction = themeMenu->addAction(tr("Dark"));
  m_darkThemeAction->setCheckable(true);
  m_darkThemeAction->setChecked(false);
  m_darkThemeAction->setActionGroup(themeGroup);
  connect(m_darkThemeAction, &QAction::triggered, this, [this]() {
    emit themeChanged(QStringLiteral("dark"));
  });

  auto *helpMenu = m_menuBar->addMenu(tr("Help"));
  auto *aboutAction = helpMenu->addAction(tr("About"));
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

void TitleBar::setUseSystemFrame(bool useSystemFrame) {
  if (m_logoLabel)
    m_logoLabel->setVisible(!useSystemFrame);
  if (m_minimizeButton)
    m_minimizeButton->setVisible(!useSystemFrame);
  if (m_maximizeButton)
    m_maximizeButton->setVisible(!useSystemFrame);
  if (m_closeButton)
    m_closeButton->setVisible(!useSystemFrame);
}

// The menu bar can be hosted either inside the custom title bar or the regular
// content area, so this helper keeps the layout bookkeeping in one place.
void TitleBar::setMenuBar(QMenuBar *bar) {
  if (m_menuBar == bar)
    return;

  auto *layout = qobject_cast<QHBoxLayout *>(this->layout());
  if (!layout)
    return;

  if (m_menuBar) {
    layout->removeWidget(m_menuBar);
    m_menuBar->removeEventFilter(this);
    if (m_menuBar != bar)
      m_menuBar->setParent(nullptr);
  }

  m_menuBar = bar;
  if (!m_menuBar)
    return;

  if (layout->indexOf(m_logoLabel) < 0 && m_logoLabel)
    layout->insertWidget(0, m_logoLabel);

  m_menuBar->setParent(this);
  m_menuBar->setObjectName(QStringLiteral("TitleBarMenu"));
  m_menuBar->setNativeMenuBar(false);
  m_menuBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  m_menuBar->installEventFilter(this);
  m_menuBar->setVisible(true);

  const int logoIndex = layout->indexOf(m_logoLabel);
  const int insertIndex = logoIndex >= 0 ? logoIndex + 1 : 0;
  layout->insertWidget(insertIndex, m_menuBar, /*stretch=*/1);
  layout->setAlignment(m_menuBar, Qt::AlignVCenter);
}

void TitleBar::setTheme(const QString &themeName) {
  if (themeName == QStringLiteral("light")) {
    if (m_lightThemeAction)
      m_lightThemeAction->setChecked(true);
    if (m_darkThemeAction)
      m_darkThemeAction->setChecked(false);
  } else if (themeName == QStringLiteral("dark")) {
    if (m_lightThemeAction)
      m_lightThemeAction->setChecked(false);
    if (m_darkThemeAction)
      m_darkThemeAction->setChecked(true);
  }
}
