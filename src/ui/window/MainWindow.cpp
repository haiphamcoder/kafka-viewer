#include "ui/window/MainWindow.h"

#include <QVBoxLayout>

#include "ui/window/decoration/TitleBar.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setObjectName(QStringLiteral("MainWindow"));
    setWindowTitle(QStringLiteral("Kafka Viewer"));
    resize(1024, 640);
    setupUi();
}

void MainWindow::setupUi()
{
    auto *central = new QWidget(this);
    auto *layout = new QVBoxLayout(central);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    m_titleBar = new TitleBar(central);
    layout->addWidget(m_titleBar);

    auto *content = new QWidget(central);
    content->setObjectName(QStringLiteral("MainContent"));
    layout->addWidget(content, /*stretch=*/1);

    setCentralWidget(central);
}
