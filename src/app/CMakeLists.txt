target_sources(kafka-viewer PRIVATE
    main.cpp
    Application.h
    Application.cpp
)

target_include_directories(kafka-viewer PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)