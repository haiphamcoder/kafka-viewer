target_sources(kafka-viewer PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/TitleBar.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/TitleBar.h
)

target_include_directories(kafka-viewer PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

