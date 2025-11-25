add_executable(kafka-viewer)

target_sources(kafka-viewer PRIVATE
    ${PROJECT_SOURCE_DIR}/resources/resources.qrc
)

target_include_directories(kafka-viewer PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_subdirectory(app)
add_subdirectory(ui)

target_link_libraries(kafka-viewer PRIVATE Qt5::Widgets Qt5::Svg)