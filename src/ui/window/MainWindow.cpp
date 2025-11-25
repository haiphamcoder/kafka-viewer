#include "ui/window/MainWindow.h"

#include <QLabel>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  setObjectName(QStringLiteral("MainWindow"));
  setWindowTitle(QStringLiteral("Kafka Viewer"));
  resize(1024, 640);
  setupUi();
}

void MainWindow::setupUi() {
  auto *label = new QLabel(tr("Kafka Viewer"), this);
  label->setAlignment(Qt::AlignCenter);
  m_titleLabel = label;
  setCentralWidget(label);
}
