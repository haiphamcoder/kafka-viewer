#pragma once

#include <QApplication>
#include <memory>

class MainWindow;

/**
 * @brief Thin wrapper around QApplication that owns the main window.
 */
class Application final : public QApplication {
  Q_OBJECT

public:
  Application(int &argc, char **argv);
  ~Application();

  /**
   * @brief Starts the event loop after showing the main window.
   */
  int run();

private:
  std::unique_ptr<MainWindow> m_mainWindow;
};
