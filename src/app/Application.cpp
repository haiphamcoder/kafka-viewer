#include "Application.h"

#include <QFile>
#include <QTextStream>

#include "ui/window/MainWindow.h"

Application::Application(int &argc, char **argv)
    : QApplication(argc, argv), m_mainWindow(std::make_unique<MainWindow>()), m_currentTheme(QStringLiteral("light")) {
  setApplicationName(QStringLiteral("kafka-viewer"));
  setApplicationDisplayName(QStringLiteral("Kafka Viewer"));
  setOrganizationName(QStringLiteral("Kafka Viewer"));
  
  // Load default theme
  loadTheme(m_currentTheme);
}

Application::~Application() = default;

int Application::run() {
  m_mainWindow->show();
  return exec();
}

void Application::loadTheme(const QString &themeName) {
  if (m_currentTheme == themeName)
    return;

  m_currentTheme = themeName;

  // Clear existing stylesheet
  QString combinedStylesheet;

  // Load base app stylesheet
  QFile appFile(QStringLiteral(":/styles/app.qss"));
  if (appFile.open(QFile::ReadOnly | QFile::Text)) {
    QTextStream appStream(&appFile);
    combinedStylesheet += appStream.readAll();
    combinedStylesheet += QStringLiteral("\n");
    appFile.close();
  }

  // Load theme stylesheet
  const QString themePath = QStringLiteral(":/themes/%1.qss").arg(themeName);
  QFile themeFile(themePath);
  if (themeFile.open(QFile::ReadOnly | QFile::Text)) {
    QTextStream themeStream(&themeFile);
    combinedStylesheet += themeStream.readAll();
    themeFile.close();
  }

  // Apply combined stylesheet
  setStyleSheet(combinedStylesheet);
}

void Application::onThemeChanged(const QString &themeName) {
  loadTheme(themeName);
}
