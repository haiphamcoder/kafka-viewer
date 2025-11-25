add_executable(kafka-viewer main.cpp)

target_link_libraries(kafka-viewer PRIVATE Qt5::Widgets Qt5::Svg)