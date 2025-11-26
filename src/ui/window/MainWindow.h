#pragma once

#include <QMainWindow>
#include <QVector>

class QEvent;
class QGridLayout;
class QWidget;
class QMenuBar;
class QVBoxLayout;

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
  void setUseSystemFrame(bool useSystemFrame);
  void moveMenuBarToContent();
  void restoreMenuBarToTitleBar();
  QMenuBar *takeMenuBarFromContentLayout();
  QVBoxLayout *contentLayout() const;
  void applyWindowFrameFlags(bool useSystemFrame);
  void updateTitleBarVisibilityForFrameMode(bool useSystemFrame);

  TitleBar *m_titleBar = nullptr;
  QVector<WindowResizeHandle *> m_resizeHandles;
  QWidget *m_contentContainer = nullptr;
  bool m_useSystemFrame = false;
};
