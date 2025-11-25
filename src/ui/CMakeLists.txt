target_include_directories(kafka-viewer PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_subdirectory(window)
add_subdirectory(dialogs)
