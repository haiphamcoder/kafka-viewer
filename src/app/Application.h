#pragma once

#include <QApplication>
#include <QMainWindow>

/**
 * @brief Thin wrapper around QApplication that owns the main window.
 */
class Application final : public QApplication {
  Q_OBJECT

public:
  Application(int &argc, char **argv);

  /**
   * @brief Starts the event loop after showing the main window.
   */
  int run();

private:
  QMainWindow m_mainWindow;
};
