#pragma once

#include <QMainWindow>
#include <QVector>

class QEvent;
class QGridLayout;
class QWidget;

class TitleBar;
class WindowResizeHandle;


class MainWindow final : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);

protected:
  void changeEvent(QEvent *event) override;

private:
  void setupUi();
  void setupResizeHandles(QWidget *rootWidget, QGridLayout *gridLayout);
  void connectTitleBarSignals();
  void updateWindowUiState();
  void toggleMaximizeRestore();
  void restoreWindow();

  TitleBar *m_titleBar = nullptr;
  QVector<WindowResizeHandle *> m_resizeHandles;
};
