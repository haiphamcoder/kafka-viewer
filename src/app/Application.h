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

  /**
   * @brief Loads and applies a theme.
   * @param themeName Theme name: "light" or "dark"
   */
  void loadTheme(const QString &themeName);

public slots:
  void onThemeChanged(const QString &themeName);

private:
  std::unique_ptr<MainWindow> m_mainWindow;
  QString m_currentTheme;
};
