#include "Application.h"

Application::Application(int &argc, char **argv) : QApplication(argc, argv) {
  setApplicationName(QStringLiteral("kafka-viewer"));
  setApplicationDisplayName(QStringLiteral("Kafka Viewer"));
  setOrganizationName(QStringLiteral("Kafka Viewer"));
}

int Application::run() {
  m_mainWindow.show();
  return exec();
}
