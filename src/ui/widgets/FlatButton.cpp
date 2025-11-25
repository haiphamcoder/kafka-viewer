#include "ui/widgets/FlatButton.h"

FlatButton::FlatButton(QWidget *parent)
    : QPushButton(parent)
{
    initStyle();
}

FlatButton::FlatButton(const QString &text, QWidget *parent)
    : QPushButton(text, parent)
{
    initStyle();
}

void FlatButton::initStyle()
{
    setCursor(Qt::PointingHandCursor);
    setFlat(true);
    setMinimumHeight(34);
    setStyleSheet(
        "FlatButton {"
        "  border: 1px solid rgba(255,255,255,0.2);"
        "  border-radius: 6px;"
        "  padding: 6px 18px;"
        "  color: #ffffff;"
        "  background-color: #0078D4"
        "}"
        "FlatButton:hover {"
        "  border-color: rgba(255,255,255,0.4);"
        "  background-color: #1590f0"
        "}"
        "FlatButton:pressed {"
        "  background-color: #005a9e"
        "}"
        "FlatButton:disabled {"
        "  color: rgba(255,255,255,0.4);"
        "  border-color: rgba(255,255,255,0.1);"
        "  background-color: rgba(255,255,255,0.05);"
        "}");
}
