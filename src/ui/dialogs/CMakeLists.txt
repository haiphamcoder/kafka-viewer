
target_sources(kafka-viewer PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/AboutDialog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AboutDialog.h
)

target_include_directories(kafka-viewer PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

