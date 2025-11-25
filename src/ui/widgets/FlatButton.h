#pragma once

#include <QPushButton>

class FlatButton : public QPushButton
{
    Q_OBJECT

public:
    explicit FlatButton(QWidget *parent = nullptr);
    explicit FlatButton(const QString &text, QWidget *parent = nullptr);

private:
    void initStyle();
};
