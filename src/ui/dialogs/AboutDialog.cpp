#include "ui/dialogs/AboutDialog.h"

#include "ui/widgets/FlatButton.h"

#include <QLabel>
#include <QVBoxLayout>

AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("About Kafka Viewer"));
    setModal(true);
    setFixedSize(360, 220);

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(24, 24, 24, 16);
    layout->setSpacing(12);

    auto *title = new QLabel(tr("<h2>Kafka Viewer</h2>"), this);
    title->setAlignment(Qt::AlignCenter);

    auto *description = new QLabel(
        tr("Kafka Viewer is a modern Qt-based GUI tool for exploring Kafka clusters,\n"
           "inspecting topics, and debugging payloads faster."), this);
    description->setWordWrap(true);
    description->setAlignment(Qt::AlignCenter);

    auto *version = new QLabel(tr("Version 0.1.0"), this);
    version->setAlignment(Qt::AlignCenter);

    auto *closeButton = new FlatButton(tr("Close"), this);
    closeButton->setFixedWidth(100);
    connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);

    layout->addWidget(title);
    layout->addWidget(description);
    layout->addWidget(version);
    layout->addStretch();
    layout->addWidget(closeButton, 0, Qt::AlignCenter);
}
