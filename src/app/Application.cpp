#include "Application.h"

#include "ui/window/MainWindow.h"

Application::Application(int &argc, char **argv)
    : QApplication(argc, argv), m_mainWindow(std::make_unique<MainWindow>()) {
  setApplicationName(QStringLiteral("kafka-viewer"));
  setApplicationDisplayName(QStringLiteral("Kafka Viewer"));
  setOrganizationName(QStringLiteral("Kafka Viewer"));
}

Application::~Application() = default;

int Application::run() {
  m_mainWindow->show();
  return exec();
}
