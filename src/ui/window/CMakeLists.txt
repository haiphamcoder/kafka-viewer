target_sources(kafka-viewer PRIVATE
    MainWindow.cpp
    MainWindow.h
)

target_include_directories(kafka-viewer PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

