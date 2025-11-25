#pragma once

#include <QMainWindow>

class TitleBar;


class MainWindow final : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);

private:
  void setupUi();

  TitleBar *m_titleBar = nullptr;
};
