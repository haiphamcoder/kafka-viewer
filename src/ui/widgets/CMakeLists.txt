target_sources(kafka-viewer PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/FlatButton.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/FlatButton.h
)

target_include_directories(kafka-viewer PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)
