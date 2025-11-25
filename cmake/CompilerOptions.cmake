# ============================================================
# Compiler options (warnings, features, optimizations)
# ============================================================

include(CheckIPOSupported)

# ====================
# Global compile flags
# ====================
if (MSVC)
    # MSVC warnings
    add_compile_options(
        /W4
        /permissive-
    )
else()
    # GCC / Clang warnings
    add_compile_options(
        -Wall            # All common warnings
        -Wextra          # Extra warnings
        -Wpedantic       # ISO C++ strict
        -Wconversion     # implicit conversions
        -Wsign-conversion
        -Wshadow
        -Wformat=2
        -Wnull-dereference
        -Wdouble-promotion
        -Wnon-virtual-dtor
    )
endif()

# ================
# Link time optimize
# ================
check_ipo_supported(RESULT ipo_supported OUTPUT ipo_error)
if (ipo_supported)
    message(STATUS "IPO / LTO enabled")
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)
endif()

# ======================
# Sanitizers (optional)
# ======================
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Sanitizers enabled (Debug Mode)")
    add_compile_options(-fsanitize=address,undefined)
    add_link_options(-fsanitize=address,undefined)
endif()
