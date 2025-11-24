# Fallback finder
find_path(RDKAFKA_INCLUDE_DIR NAMES rdkafka.h PATH_SUFFIXES librdkafka)
find_library(RDKAFKA_LIBRARY NAMES rdkafka++ rdkafka)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(RdKafka DEFAULT_MSG
    RDKAFKA_LIBRARY RDKAFKA_INCLUDE_DIR)

if(RDKAFKA_FOUND)
    add_library(RdKafka::librdkafka UNKNOWN IMPORTED)
    set_target_properties(RdKafka::librdkafka PROPERTIES
        IMPORTED_LOCATION "${RDKAFKA_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${RDKAFKA_INCLUDE_DIR}"
    )
endif()